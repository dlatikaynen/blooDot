#include <iostream>
#include <sstream>
#include <vector>

#include "dexassy.h"
#include "brotli/types.h"
#include "brotli/decode.h"
#include "../res/chunk-sizes.h"
#include "../res/chunk-constants.h"
#include "../src/util/bytefmt.h"

std::vector<size_t> chunkOffsets;
SDL_IOStream* cooked = nullptr;
static constexpr size_t bufferSize = 1 << 19;

void PrepareIndex()
{
	// skip the preamble
	size_t runningOffset = lenBom + lenSignature;

	for (const unsigned long long chunkSize : chunkSizes)
	{
		chunkOffsets.push_back(runningOffset);
		runningOffset += chunkSize;
	}
}

bool OpenCooked()
{
	if (cooked)
	{
		ReportError("FAIL Could not open wad", "already open");

		return false;
	}

	const auto basePath = SDL_GetBasePath();
	std::stringstream sourcePath;

	sourcePath << basePath << "\\" << XassyFile;

	const auto& strSourcePath = sourcePath.str();

	cooked = SDL_IOFromFile(strSourcePath.c_str(), "rb");
	if (!cooked)
	{
		const auto openError = SDL_GetError();

		ReportError("FAIL Could not read wad", openError);

		return false;
	}

	SDL_IOStream* dlgStream;
	const auto resMem = Retrieve(CHUNK_KEY_DLG_MANUFACTURER, &dlgStream);

	if (!resMem)
	{
		const auto retrieveError = SDL_GetError();

		ReportError("FAIL Could not retrieve from wad", retrieveError);

		return false;
	}

	char ch;
	auto readResult = SDL_ReadIO(dlgStream, &ch, sizeof(ch));

	while(readResult != 0)
	{
		if (ch == '*') {
			// because we can, and we know how
			std::cout << "\x1b[38;2;50;50;255mo\x1b[0m";
		} else if (ch != '\r') {
			std::cout << ch;
		}

		readResult = SDL_ReadIO(dlgStream, &ch, sizeof(ch));
	}

	auto result = true;

	if (!SDL_CloseIO(dlgStream)) {
		const auto closeError = SDL_GetError();

		ReportError("FAIL Could not close principal wad stream", closeError);

		result = false;
	}

	SDL_free(resMem);

	return result;
}

static BROTLI_BOOL WriteOutput(uint8_t** outBuffer, size_t* outBufferSize, const uint8_t* next_out, const uint8_t* output, size_t* total_out) {
	const auto out_size = static_cast<size_t>(next_out - output);
	const size_t offset = (*total_out);

	(*total_out) += out_size;
	if (out_size == 0) {
		// nothing to write, cool
		return BROTLI_TRUE;
	}

	if (out_size > bufferSize) {
		ReportError("FAIL Attempt to flush more than one buffer's worth of bytes per chunk", "Inflator");

		return BROTLI_FALSE;
	}

	if ((*total_out) > (*outBufferSize)) {
		(*outBufferSize) += bufferSize;
		(*outBuffer) = static_cast<uint8_t *>(SDL_realloc(*outBuffer, *outBufferSize));
	}

	memcpy((*outBuffer) + offset, output, out_size);

	return BROTLI_TRUE;
}

/// <summary>
/// Caller must free both
/// </summary>
void* Retrieve(int chunkKey, SDL_IOStream** const stream)
{
	size_t compressedSize = chunkSizes[chunkKey];
	size_t chunkOffset = chunkOffsets[chunkKey];

	*stream = nullptr;
	if (SDL_SeekIO(cooked, static_cast<Sint64>(chunkOffset), SDL_IO_SEEK_SET) == -1) {
		ReportError("FAIL Failed to seek", "Inflator");

		return nullptr;
	}

	size_t originalSize = 0;

	const auto preambleRead = SDL_ReadIO(cooked, &originalSize, sizeof(size_t));

	if (preambleRead <= 0) {
		const auto preambleError = SDL_GetError();

		ReportError("FAIL Wad preamble is corrupted", preambleError);

		return nullptr;
	} else if (originalSize <= 0) {
		const auto preambleError = SDL_GetError();

		ReportError("FAIL Wad layout is corrupted", preambleError);

		return nullptr;
	}

	compressedSize -= preambleRead;

	unsigned long long alreadyReadFromCompressedSource = 0;
	size_t actuallyExtracted = 0;
	const auto decoderState = BrotliDecoderCreateInstance(nullptr, nullptr, nullptr);

	if (decoderState == nullptr) {
		ReportError("FAIL Failed to allocate memory", "Inflator");

		return nullptr;
	}

	BrotliDecoderSetParameter(decoderState, BROTLI_DECODER_PARAM_LARGE_WINDOW, 1u);
	BrotliDecoderResult result = BROTLI_DECODER_RESULT_NEEDS_MORE_INPUT;

	const uint8_t* next_in = nullptr;
	auto extractedRaw = static_cast<uint8_t *>(SDL_malloc(bufferSize));
	size_t extractedRawSize = bufferSize;
	auto buffer = static_cast<uint8_t *>(malloc(bufferSize * 2));
	uint8_t* input = buffer;
	uint8_t* output = buffer + bufferSize;
	size_t available_in = 0;
	size_t available_out = bufferSize;
	uint8_t* next_out = output;

	for (;;) {
		if (result == BROTLI_DECODER_RESULT_NEEDS_MORE_INPUT) {
			if (alreadyReadFromCompressedSource >= compressedSize) {
				ReportError("FAIL Extra input", "Inflator");

				return nullptr;
			}

			const auto remainingIn = compressedSize - alreadyReadFromCompressedSource;

			if (remainingIn <= 0) {
				ReportError("FAIL Compressed source underflow", "Inflator");

				return nullptr;
			}

			if (remainingIn < bufferSize) {
				available_in = SDL_ReadIO(cooked, input, remainingIn);
			} else {
				available_in = SDL_ReadIO(cooked, input, bufferSize);
			}

			if (available_in == 0) {
				ReportError("FAIL Compressed source exhausted prematurely", "Inflator");

				return nullptr;
			}

			for (size_t i = 0; i < available_in; ++i) {
				input[i] ^= 0b10000010;
			}

			alreadyReadFromCompressedSource += available_in;
			next_in = input;
		} else if (result == BROTLI_DECODER_RESULT_NEEDS_MORE_OUTPUT) {
			// provide output
			if (!WriteOutput(&extractedRaw, &extractedRawSize, next_out, output, &actuallyExtracted)) {
				return BROTLI_FALSE;
			}

			available_out = bufferSize;
			next_out = output;
		} else if (result == BROTLI_DECODER_RESULT_SUCCESS) {
			// flush output
			if (!WriteOutput(&extractedRaw, &extractedRawSize, next_out, output, &actuallyExtracted)) {
				return BROTLI_FALSE;
			}

			available_out = 0;
			next_out = output;

			break;
		} else {
			/* result == BROTLI_DECODER_RESULT_ERROR */
			ReportError("FAIL Corrupted input", "Inflator");

			return nullptr;
		}

		// advance
		result = BrotliDecoderDecompressStream(
			decoderState,
			&available_in,
			&next_in,
			&available_out,
			&next_out,
			nullptr);
	}

	if (actuallyExtracted != originalSize)
	{
		std::stringstream readError;
		readError << originalSize << " bytes expected, " << actuallyExtracted << " bytes actually extracted";
		const auto& strReadError = readError.str();

		ReportError("FAIL Principal wad chunk corrupted", strReadError.c_str());

		return nullptr;
	}

#ifndef NDEBUG
	std::cout
		<< "Inflated chunk #"
		<< chunkKey
		<< " from stored "
		<< blooDot::bytesize(compressedSize)
		<< " to original "
		<< blooDot::bytesize(originalSize)
		<< "\n";
#endif

	const auto extractedStream = SDL_IOFromConstMem(extractedRaw, static_cast<int>(originalSize));

	if (!extractedStream)
	{
		const auto streamError = SDL_GetError();
		std::stringstream extractError;
		extractError << "Chunk #" << chunkKey << ", " << streamError;
		const auto& strExtractError = extractError.str();

		ReportError("FAIL Failed to extract from principal wad", strExtractError.c_str());

		return nullptr;
	}

	*stream = extractedStream;

	return extractedRaw;
}

void CloseCooked()
{
	if (cooked)
	{
		SDL_CloseIO(cooked);
		cooked = nullptr;
	}
}
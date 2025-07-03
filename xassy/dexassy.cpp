#include <iostream>
#include <sstream>
#include <vector>

#include "dexassy.h"
#include "../res/chunk-sizes.h"
#include "../res/chunk-constants.h"
#include "../src/util/bytefmt.h"

std::vector<size_t> chunkOffsets;
SDL_IOStream* cooked = nullptr;

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
		ReportError("Could not open wad", "already open");

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

		ReportError("Could not read wad", openError);

		return false;
	}

	SDL_IOStream* dlgStream;
	const auto resMem = Retrieve(CHUNK_KEY_DLG_MANUFACTURER, &dlgStream);

	if (!resMem)
	{
		const auto retrieveError = SDL_GetError();

		ReportError("Could not retrieve from wad", retrieveError);

		return false;
	}

	char ch;
	auto readResult = SDL_ReadIO(dlgStream, &ch, sizeof(ch));

	while(readResult != 0)
	{
		std::cout << ch;
		readResult = SDL_ReadIO(dlgStream, &ch, sizeof(ch));
	}

	SDL_CloseIO(dlgStream);
	SDL_free(resMem);

	return true;
}

/// <summary>
/// Caller must free both
/// </summary>
void* Retrieve(int chunkKey, SDL_IOStream** const stream)
{
	size_t compressedSize = chunkSizes[chunkKey];
	size_t chunkOffset = chunkOffsets[chunkKey];
	*stream = nullptr;

	void* compressedMem = SDL_malloc(compressedSize);
	if (!compressedMem)
	{
		std::stringstream prepError;
		const auto allocError = SDL_GetError();

		prepError << "Chunk #" << chunkKey << " asks for " << compressedSize << " bytes, " << allocError;

		const auto& strPrepError = prepError.str();

		ReportError("Failed to allocate memory for a chunk", strPrepError.c_str());

		return nullptr;
	}

	SDL_SeekIO(cooked, static_cast<Sint64>(chunkOffset), SDL_IO_SEEK_SET);

	long long originalSize = 0;
	long long actuallyExtracted = 0;
	const auto extractedRaw = (void *)(static_cast<const char *>("0")); // HuffInflate(cooked, compressedSize, &originalSize, &actuallyExtracted);

	SDL_free(compressedMem);
	if (actuallyExtracted != originalSize)
	{
		std::stringstream readError;
		readError << originalSize << " bytes expected, " << actuallyExtracted << " bytes actually extracted";
		const auto& strReadError = readError.str();

		ReportError("Principal wad chunk corrupted", strReadError.c_str());

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

		ReportError("Failed to extract from principal wad", strExtractError.c_str());

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
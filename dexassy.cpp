#include "pch.h"
#include "dexassy.h"
#include "chunk-sizes.h"
#include <iostream>

constexpr char const* XassyFile = "xassy.cooked1.ngld";

std::vector<size_t> chunkOffsets;
SDL_RWops* cooked = NULL;

void PrepareIndex()
{
	const auto numChunks = sizeof(chunkSizes) / sizeof(size_t);
	size_t runningOffset = 0;

	for (auto i = 0; i < numChunks; ++i)
	{
		chunkOffsets.push_back(runningOffset);
		runningOffset += chunkSizes[i];
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
	cooked = SDL_RWFromFile(sourcePath.str().c_str(), "rb");
	if (!cooked)
	{
		const auto openError = SDL_GetError();
		ReportError("Could not read wad", openError);
		return false;
	}

	return true;
}

/// <summary>
/// Caller must free both
/// </summary>
void* Retrieve(int chunkKey, __out SDL_RWops** const stream)
{
	size_t compressedSize = chunkSizes[chunkKey];
	size_t chunkOffset = chunkOffsets[chunkKey];
	*stream = NULL;

	void* compressedMem = SDL_malloc(compressedSize);
	if (!compressedMem)
	{
		std::stringstream prepError;
		const auto allocError = SDL_GetError();
		prepError << "Chunk #" << chunkKey << " asks for " << compressedSize << " bytes, " << allocError;
		ReportError("Failed to allocate memory for a chunk", prepError.str().c_str());
		return NULL;
	}

	cooked->seek(cooked, chunkOffset, RW_SEEK_SET);

	long long originalSize = 0;
	long long actuallyExtracted = 0;
	const auto extractedRaw = HuffInflate(cooked, compressedSize, &originalSize, &actuallyExtracted);
	SDL_free(compressedMem);
	if (actuallyExtracted != originalSize)
	{
		std::stringstream readError;
		readError << originalSize << " bytes expected, " << actuallyExtracted << " bytes actually extracted";
		ReportError("Principal wad chunk corrupted", readError.str().c_str());
		return NULL;
	}

#ifndef NDEBUG
	std::cout
		<< "Inflated chunk #"
		<< chunkKey
		<< " from stored "
		<< compressedSize
		<< " to original "
		<< originalSize
		<< " bytes\n";
#endif

	const auto extractedStream = SDL_RWFromConstMem(extractedRaw, static_cast<int>(originalSize));
	if (!extractedStream)
	{
		const auto streamError = SDL_GetError();
		std::stringstream extractError;
		extractError << "Chunk #" << chunkKey << ", " << streamError;
		ReportError("Failed to extract from principal wad", extractError.str().c_str());
		return NULL;
	}

	*stream = extractedStream;
	return extractedRaw;
}

void CloseCooked()
{
	if (cooked)
	{
		cooked->close(cooked);
		SDL_free(cooked);
		cooked = NULL;
	}
}
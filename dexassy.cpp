#include "pch.h"
#include "dexassy.h"

size_t chunkSizes[] = {
	1072649,
	42452,
	107112,
	37776,
	3662
};

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
		return false;
	}
	const auto basePath = SDL_GetBasePath();
	std::stringstream sourcePath;
	sourcePath << basePath << "\\xassy.cooked1.ngld";

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
	size_t expectedSíze = chunkSizes[chunkKey];
	size_t chunkOffset = chunkOffsets[chunkKey];
	void* chunkMem = SDL_malloc(expectedSíze);
	*stream = NULL;

	if (!chunkMem)
	{
		std::stringstream prepError;
		const auto allocError = SDL_GetError();
		prepError << "Chunk #" << chunkKey << " asks for " << expectedSíze << " bytes, " << allocError;
		ReportError("Failed to allocate memory for a chunk", prepError.str().c_str());
		return NULL;
	}

	cooked->seek(cooked, chunkOffset, RW_SEEK_SET);
	const auto chunkRead = cooked->read(cooked, chunkMem, 1, expectedSíze);
	if (chunkRead != expectedSíze)
	{
		std::stringstream readError;
		readError << expectedSíze << " bytes expected, " << chunkRead << " bytes actual";
		ReportError("Principal wad chunk corrupted", readError.str().c_str());
		return NULL;
	}

	const auto extractedStream = SDL_RWFromConstMem(chunkMem, static_cast<int>(chunkRead));
	if (!extractedStream)
	{
		const auto streamError = SDL_GetError();
		std::stringstream extractError;
		extractError << "Chunk #" << chunkKey << ", " << streamError;
		ReportError("Failed to extract from principal wad", extractError.str().c_str());
		return NULL;
	}

	*stream = extractedStream;
	return chunkMem;
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
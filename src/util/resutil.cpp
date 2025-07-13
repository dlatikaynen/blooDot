#include "resutil.h"
#include "../../main.h"
#include "SDL3_image/SDL_image.h"
#include "../../xassy/dexassy.h"
#include "../../res/chunk-constants.h"

#ifndef NDEBUG
#include <fstream>
#include <iostream>
#endif

namespace blooDot::Res
{
	/// <summary>
	/// Leaves the stream open
	/// </summary>
	SDL_Texture* LoadPicture(SDL_Renderer* renderer, SDL_IOStream* reader, SDL_FRect* dimensions)
	{
		const auto& surface = IMG_LoadPNG_IO(reader);
		if (!surface)
		{
			const auto loadError = SDL_GetError();

			ReportError("Failed to load picture from stream", loadError);

			return nullptr;
		}

		return LoadPictureInternal(renderer, surface, dimensions);
	}

	SDL_Texture* LoadPicture(SDL_Renderer* renderer, const int chunkKey, SDL_FRect* dimensions)
	{
		SDL_IOStream* resStream;
		const auto resMem = Retrieve(chunkKey, &resStream);

		if (!resMem)
		{
			return nullptr;
		}

		const auto& resPicture = IMG_LoadPNG_IO(resStream);

		if (!resPicture)
		{
			const auto resLoadError = SDL_GetError();

			SDL_CloseIO(resStream);
			SDL_free(resMem);
			ReportError("Failed to load picture from resource", resLoadError);

			return nullptr;
		}

		SDL_CloseIO(resStream);
		SDL_free(resMem);

		return LoadPictureInternal(renderer, resPicture, dimensions);
	}

	bool LoadText(const int chunkKey, std::string& text)
	{
#ifndef NDEBUG
		// ReSharper disable once CppDFAConstantConditions
		if (auto const& relFile = GetUncookedRelPath(chunkKey); relFile != nullptr)
		{
			auto const&& basePath = SDL_GetBasePath();
			std::stringstream rawPath;
			rawPath << basePath << "../../res/" << relFile;
			std::ifstream rawFile(rawPath.str());

			if (rawFile.is_open() && rawFile.good())
			{
				std::string loadedLine;
				std::stringstream loadedLines;

				while (std::getline(rawFile, loadedLine))
				{
					loadedLines
						<< loadedLine
						<< "\n";
				}

				text.assign(loadedLines.str());
				std::cout
					<< "Debug-loaded text resource #"
					<< chunkKey
					<< " uncooked from \""
					<< relFile
					<< "\" ("
					<< text.length()
					<< " chars)\n";

				return true;
			}
		}
#endif

		SDL_IOStream* resStream;
		const auto resMem = Retrieve(chunkKey, &resStream);

		if (!resMem)
		{
			std::cerr
				<< "Could not load text #"
				<< chunkKey
				<< ", retrieve failed\n";

			return false;
		}

		const auto numBytes = SDL_GetIOSize(resStream);
		auto rawText = static_cast<char *>(SDL_malloc(numBytes));

		if (rawText == nullptr)
		{
			const auto mallocError = SDL_GetError();

			SDL_CloseIO(resStream);
			SDL_free(resMem);
			std::stringstream allocationMsg;
			allocationMsg
				<< "Could not load text #"
				<< chunkKey
				<< ", allocation failed\n";

			const auto& strAllocationMsg = allocationMsg.str();

			ReportError(strAllocationMsg.c_str(), mallocError);

			return false;
		}

		if (SDL_ReadIO(resStream, static_cast<void *>(rawText), numBytes) == 0)
		{
			const auto readError = SDL_GetError();

			SDL_CloseIO(resStream);
			SDL_free(resMem);
			std::stringstream readMsg;
			readMsg
				<< "Could not load text #"
				<< chunkKey
				<< ", read failed\n";

			const auto& strReadMsg = readMsg.str();

			ReportError(strReadMsg.c_str(), readError);

			return false;
		}

		SDL_CloseIO(resStream);
		SDL_free(resMem);
		text.assign(rawText, numBytes);
		SDL_free((void*)rawText);

		return true;
	}

	SDL_Texture* LoadPictureInternal(SDL_Renderer* renderer, SDL_Surface* surface, SDL_FRect* dimensions)
	{
		const auto& resTexture = SDL_CreateTextureFromSurface(renderer, surface);

		if (!resTexture)
		{
			const auto textureError = SDL_GetError();

			SDL_free(surface);
			ReportError("Failed to create texture from picture", textureError);
			
			return nullptr;
		}

		auto& layoutRect = *dimensions;

		layoutRect.x = 0;
		layoutRect.y = 0;
		layoutRect.w = static_cast<float>(surface->w);
		layoutRect.h = static_cast<float>(surface->h);
		SDL_free(surface);

		return resTexture;
	}
}
#include "pch.h"
#include "resutil.h"
#include <iostream>

namespace blooDot::Res
{
	/// <summary>
	/// Leaves the stream open
	/// </summary>
	SDL_Texture* LoadPicture(SDL_Renderer* renderer, SDL_RWops* reader, SDL_Rect* dimensions)
	{
		const auto& surface = IMG_LoadPNG_RW(reader);
		if (!surface)
		{
			const auto loadError = IMG_GetError();
			ReportError("Failed to load picture from stream", loadError);
			return nullptr;
		}

		return _LoadPicture(renderer, surface, dimensions);
	}

	SDL_Texture* LoadPicture(SDL_Renderer* renderer, const int chunkKey, SDL_Rect* dimensions)
	{
		SDL_RWops* resStream;
		const auto resMem = Retrieve(chunkKey, &resStream);
		if (!resMem)
		{
			return nullptr;
		}

		const auto& resPicture = IMG_LoadPNG_RW(resStream);
		if (!resPicture)
		{
			const auto resLoadError = IMG_GetError();
			resStream->close(resStream);
			SDL_free(resMem);
			ReportError("Failed to load picture from resource", resLoadError);
			return nullptr;
		}

		resStream->close(resStream);
		SDL_free(resMem);

		return _LoadPicture(renderer, resPicture, dimensions);
	}

	bool LoadText(const int chunkKey, std::string& text)
	{
#ifndef NDEBUG
		auto const& relFile = GetUncookedRelPath(chunkKey);

		if (relFile != nullptr)
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

		SDL_RWops* resStream;
		const auto resMem = Retrieve(chunkKey, &resStream);
		if (!resMem)
		{
			std::cerr
				<< "Could not load text #"
				<< chunkKey
				<< ", retrieve failed\n";

			return false;
		}

		const auto numBytes = resStream->size(resStream);
		const char* rawText = (const char*)SDL_malloc(numBytes);
		if (rawText == nullptr)
		{
			const auto mallocError = SDL_GetError();
			resStream->close(resStream);
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

		if (resStream->read(resStream, (void*)rawText, numBytes, 1) == 0)
		{
			const auto readError = SDL_GetError();
			resStream->close(resStream);
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

		resStream->close(resStream);
		SDL_free(resMem);
		text.assign(rawText, numBytes);
		SDL_free((void*)rawText);

		return true;
	}

	SDL_Texture* _LoadPicture(SDL_Renderer* renderer, SDL_Surface* surface, SDL_Rect* dimensions)
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
		layoutRect.w = surface->w;
		layoutRect.h = surface->h;
		SDL_free(surface);

		return resTexture;
	}
}
#include "pch.h"
#include "enxassy.h"
#include <iostream>

int Cook()
{
	std::stringstream chunkConstants;
	std::stringstream chunkSizes;
	chunkConstants
		<< "#pragma once"
		<< "\n\n";

	chunkSizes
		<< "#pragma once"
		<< "\n\n"
		<< "size_t chunkSizes[] = {\n";

	auto const&& basePath = SDL_GetBasePath();
	std::stringstream recipePath;
	recipePath << basePath << "..\\..\\" << "gameres.xassy.recipe";
	std::ifstream recipeFile(recipePath.str());
	std::stringstream cookedPath;
	cookedPath << basePath << "xassy.cooked1.ngld";
	int resNumber = 0;

	if (recipeFile.is_open() && recipeFile.good())
	{
		std::ofstream cookedFile(cookedPath.str(), std::ios::binary);
		std::string loadedLine;
		std::string previousIdentifier;
		while (std::getline(recipeFile, loadedLine))
		{
			std::stringstream recipeLine;
			recipeLine << loadedLine;
			std::string fileName;
			std::string identifier;
			if (std::getline(recipeLine, fileName, '\t')) 
			{
				if (std::getline(recipeLine, identifier, '\t'))
				{
					std::cout
						<< "About to add \""
						<< fileName
						<< "\" as "
						<< identifier
						<< "\n";

					std::stringstream rawPath;
					rawPath << basePath << "..\\..\\res\\" << fileName;
					std::ifstream rawFile(rawPath.str(), std::ios::binary);
					if (rawFile.is_open() && rawFile.good())
					{
						const long long&& chunkStart = resNumber == 0
							? (std::streampos)0
							: cookedFile.tellp();

						HuffCompress(rawFile, cookedFile);
						const long long&& posAfter = cookedFile.tellp();
						const long long&& compressedSize = posAfter - chunkStart;
						if (previousIdentifier.empty())
						{
							chunkSizes
								<< "\t"
								<< compressedSize;
						}
						else
						{
							chunkSizes
								<< ",\t\t// "
								<< previousIdentifier
								<< "\n\t"
								<< compressedSize;
						}

						chunkConstants
							<< "constexpr int const "
							<< identifier
							<< " = "
							<< resNumber
							<< ";\n";

						std::cout
							<< compressedSize
							<< " bytes written.\n";

						rawFile.close();
						++resNumber;
					}

					previousIdentifier = identifier;
					identifier.clear();
				}

				fileName.clear();
			}

			loadedLine.clear();
			recipeLine.clear();
		}
		
		if (!previousIdentifier.empty())
		{
			chunkSizes
				<< "\t\t// "
				<< previousIdentifier
				<< "\n";
		}

		previousIdentifier.clear();
		cookedFile.close();
		recipeFile.close();

		/* write the chunk constants header */
		std::stringstream chunkConstantsPath;
		chunkConstantsPath << basePath << "..\\..\\" << "chunk-constants.h";
		auto const&& chunkConstantsFile = SDL_RWFromFile(chunkConstantsPath.str().c_str(), "wt");
		auto const&& chunkConstantsStr = chunkConstants.str();
		auto const&& chunkConstantsCStr = chunkConstantsStr.c_str();
		auto const&& chunkConstantsBlocksWritten = chunkConstantsFile->write(chunkConstantsFile, chunkConstantsCStr, chunkConstantsStr.length(), 1);
		chunkConstantsFile->close(chunkConstantsFile);
		std::cout
			<< chunkConstantsBlocksWritten
			<< " block of "
			<< chunkConstantsStr.length()
			<< " chars written to chunk constants header file\n";

		/* write the chunk sizes header */
		chunkSizes << "};\n";
		std::stringstream chunkSizesPath;
		chunkSizesPath << basePath << "..\\..\\" << "chunk-sizes.h";
		auto const&& chunkSizesFile = SDL_RWFromFile(chunkSizesPath.str().c_str(), "wt");
		auto const&& chunkSizesStr = chunkSizes.str();
		auto const&& chunkSizesCStr = chunkSizesStr.c_str();
		auto const&& chunkSizesBlocksWritten = chunkSizesFile->write(chunkSizesFile, chunkSizesCStr, chunkSizesStr.length(), 1);
		chunkSizesFile->close(chunkSizesFile);
		std::cout
			<< chunkSizesBlocksWritten
			<< " block of "
			<< chunkSizesStr.length()
			<< " chars written to chunk sizes header file\n";

		return CookReturnCodeSuccess;
	}

	return -1;
}
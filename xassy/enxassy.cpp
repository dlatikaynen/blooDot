#include "brotli/types.h"
#include "brotli/encode.h"
#include <fstream>
#include <iostream>
#include <filesystem>

#include "enxassy.h"
#include "dexassy.h"
#include "../src/util/bytefmt.h"

static BROTLI_BOOL WriteOutput(std::ofstream* fOut, const uint8_t* next_out, uint8_t* output) {
	const auto out_size = next_out - output;

	if (out_size == 0) {
		return BROTLI_TRUE;
	}

	for (auto i = 0; i < out_size; ++i) {
		output[i] ^= 0b10000010;
	}

	fOut->write(reinterpret_cast<char*>(output), out_size);
	if (fOut->bad()) {
		return BROTLI_FALSE;
	}

	return BROTLI_TRUE;
}

int Cook(XassyCookInfo *cookStats)
{
	std::stringstream chunkConstants;
	std::stringstream chunkSizes;
	chunkConstants
		<< "#pragma once"
		<< "\n\n";

	chunkSizes
		<< "#pragma once"
		<< "\n\n"
		<< "size_t inline chunkSizes[] = {\n";

	cookStats->numFiles = 0;
	cookStats->numBytesBefore = 0;
	cookStats->numBytesAfter = 0;

	auto const& basePath = SDL_GetBasePath();

	std::stringstream recipePath;
	recipePath << basePath << R"(..\res\)" << "gameres.xassy.recipe";
	std::ifstream recipeFile(recipePath.str());
	std::stringstream cookedPath;
	cookedPath << basePath << XassyFile;

	if (recipeFile.is_open() && recipeFile.good())
	{
		int resNumber = 0;
		std::stringstream debugLocations;
		std::ofstream cookedFile(cookedPath.str(), std::ios::binary);
		std::string loadedLine;
		std::string previousIdentifier;

		cookedFile.write(bom, lenBom);
		cookedFile.write(signature, lenSignature);
		while (std::getline(recipeFile, loadedLine))
		{
			std::stringstream recipeLine;
			recipeLine << loadedLine;
			if (std::string fileName; std::getline(recipeLine, fileName, '\t'))
			{
				if (recipeLine.str()[0] == '#')
				{
					std::cout << "Skipping commented-out entry\n";

					continue;
				}

				if (std::string identifier; std::getline(recipeLine, identifier, '\t'))
				{
					std::cout
						<< "About to add \""
						<< fileName
						<< "\" as "
						<< identifier
						<< "\n";

					std::stringstream rawPath;
					size_t uncompressedSize;
					rawPath << basePath << R"(..\res\)" << fileName;
					try
					{
						uncompressedSize = std::filesystem::file_size(rawPath.str());
					}
					catch(...)
					{
						std::cerr
							<< "Failed to open \""
							<< fileName
							<< "\" for "
							<< identifier
							<< "\n";

						return CookReturnCodeInputFileNotFound;
					}

					if (std::ifstream rawFile(rawPath.str(), std::ios::binary); rawFile.is_open() && rawFile.good())
					{
						const long long&& chunkStart = cookedFile.tellp();

						cookedFile.write (reinterpret_cast<const char *>(&uncompressedSize), sizeof (uncompressedSize));

						auto const& encoderState = BrotliEncoderCreateInstance(nullptr,nullptr,nullptr);

						if (!encoderState) {
							return CookReturnCodeRecipeFail;
						}

						BrotliEncoderSetParameter(encoderState, BROTLI_PARAM_QUALITY, BROTLI_MAX_QUALITY);
						BrotliEncoderSetParameter(encoderState, BROTLI_PARAM_SIZE_HINT, uncompressedSize);

						BROTLI_BOOL is_eof = BROTLI_FALSE;
						static constexpr size_t bufferSize = 1 << 19;
						auto buffer = static_cast<uint8_t *>(malloc(bufferSize * 2));
						uint8_t* input = buffer;
						uint8_t* output = buffer + bufferSize;
						size_t available_in = 0;
						const uint8_t* next_in = nullptr;
						size_t available_out = bufferSize;
						uint8_t* next_out = output;

						while (!is_eof) {
							if (available_in == 0) {
								// provide_input
								rawFile.read(reinterpret_cast<char*>(input), bufferSize);
								available_in = rawFile.gcount();
								next_in = input;
								is_eof = rawFile.eof();
							}

							if (!BrotliEncoderCompressStream(
								encoderState,
								is_eof ? BROTLI_OPERATION_FINISH : BROTLI_OPERATION_PROCESS,
								&available_in,
								&next_in,
								&available_out,
								&next_out,
								nullptr
							)) {
								free(buffer);
								rawFile.close();

								return 1212;
							}

							if (available_out == 0) {
								// provide output
								if (!WriteOutput(&cookedFile, next_out, output)) {
									free(buffer);
									rawFile.close();

									return CookReturnCodeRecipeFail;
								}

								available_out = bufferSize;
								next_out = output;
							}

							if (BrotliEncoderIsFinished(encoderState)) {
								// flush output
								if (!WriteOutput(&cookedFile, next_out, output)) {
									free(buffer);
									rawFile.close();

									return CookReturnCodeRecipeFail;
								}

								available_out = 0;
								next_out = output;

								break;
							}
						}

						free(buffer);
						BrotliEncoderDestroyInstance(encoderState);
						rawFile.close();

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
							<< "constexpr int "
							<< identifier
							<< " = "
							<< resNumber
							<< ";\n";

						// with this, we can access uncooked stuff
						// while we're running in debug environment
						debugLocations
							<< "\tcase "
							<< resNumber
							<< ": return \""
							<< fileName
							<< "\"; // "
							<< identifier
							<< "\n";

						std::cout
							<< blooDot::bytesize(compressedSize)
							<< " written, original was "
							<< blooDot::bytesize(uncompressedSize)
							<< "\n";

						++resNumber;
						++cookStats->numFiles;
						cookStats->numBytesBefore += uncompressedSize;
						cookStats->numBytesAfter += compressedSize;
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

		auto const&& debugLocationsStr = debugLocations.str();
		chunkConstants
			<< "\n"
			<< "#ifndef NDEBUG\n"
			<< "inline const char* GetUncookedRelPath(int chunkIndex)\n"
			<< "{\n"
			<< "\tswitch(chunkIndex) {\n"
			<< debugLocationsStr
			<< "\tdefault: return \"~MISSING~\";\n"
			<< "\t}\n"
			<< "}\n"
			<< "#endif\n";

		/* write the chunk constants header */
		std::stringstream chunkConstantsPath;
		chunkConstantsPath << basePath << R"(..\res\)" << "chunk-constants.h";

		const auto& strChunkConstantsPath = chunkConstantsPath.str();
		auto const&& chunkConstantsFile = SDL_IOFromFile(strChunkConstantsPath.c_str(), "wt");
		auto const&& chunkConstantsStr = chunkConstants.str();
		auto const&& chunkConstantsCStr = chunkConstantsStr.c_str();

		SDL_WriteIO(chunkConstantsFile, chunkConstantsCStr, chunkConstantsStr.length());
		SDL_CloseIO(chunkConstantsFile);
		std::cout
			<< chunkConstantsStr.length()
			<< " chars written to chunk constants header file\n";

		/* write the chunk sizes header */
		chunkSizes << "};\n";
		std::stringstream chunkSizesPath;
		chunkSizesPath << basePath << R"(..\res\)" << "chunk-sizes.h";

		const auto& strChunkSizesPath = chunkSizesPath.str();
		auto const&& chunkSizesFile = SDL_IOFromFile(strChunkSizesPath.c_str(), "wt");
		auto const&& chunkSizesStr = chunkSizes.str();
		auto const&& chunkSizesCStr = chunkSizesStr.c_str();

		SDL_WriteIO(chunkSizesFile, chunkSizesCStr, chunkSizesStr.length());
		SDL_CloseIO(chunkSizesFile);
		std::cout
			<< chunkSizesStr.length()
			<< " chars written to chunk sizes header file\n";

		return CookReturnCodeSuccess;
	}

	return CookReturnCodeRecipeFail;
}

#include "enxlate.h"
#include <iostream>
#include <vector>
#include "SDL3/SDL.h"

int Xlate(XassyXlatInfo* xlatInfo)
{
	std::stringstream pathHeader;
	std::stringstream pathSource;

	xlatInfo->numFiles = 0;
	xlatInfo->numLiterals = 0;

	auto const&& basePath = SDL_GetBasePath();
	std::stringstream recipePath;
	recipePath << basePath << R"(..\res\)" << "gameres.xlation.recipe";
	std::ifstream recipeFile;
	//auto const&& locale = std::locale("en-US");
	std::locale locale;
	recipeFile.imbue(locale);
	recipeFile.open(recipePath.str());
	pathHeader << basePath << R"(..\res\)" << "xlations.h";
	pathSource << basePath << R"(..\res\)" << "xlations.cpp";

	if (recipeFile.is_open() && recipeFile.good())
	{
		std::vector<XlatableIdentifier> identifiers;
		XlatableIdentifier currentIdentifier;
		bool hasCurrentIdentifier = false;
		std::string loadedLine;
		int lineNumber = 0;

		while (std::getline(recipeFile, loadedLine))
		{
			++lineNumber;
			std::stringstream rawLine;
			rawLine << loadedLine;
			if (rawLine.str()[0] == '\t')
			{
				std::stringstream xlationLine;
				xlationLine << rawLine.str();
				std::string indent;
				std::string lcid;
				std::string literal;
				std::getline(xlationLine, indent, '\t');
				std::getline(xlationLine, lcid, '\t');
				std::getline(xlationLine, literal, '\t');
				if (hasCurrentIdentifier == false)
				{
					std::cerr
						<< "Line "
						<< lineNumber
						<< ": No current identifier for literal";
				}
				else
				{
					if (lcid == "[**]" || lcid == "[am]")
					{
						currentIdentifier.HasAm = true;
						currentIdentifier.am = literal;
					}
					if (lcid == "[de]")
					{
						currentIdentifier.HasDe = true;
						currentIdentifier.de = literal;
					}
					if (lcid == "[fi]")
					{
						currentIdentifier.HasFi = true;
						currentIdentifier.fi = literal;
					}
					else if (lcid == "[ua]")
					{
						currentIdentifier.HasUa = true;
						currentIdentifier.ua = literal;
					}
				}
			}
			else if (!rawLine.str().empty() && rawLine.str()[0] != '#')
			{
				if (hasCurrentIdentifier)
				{
					identifiers.push_back(currentIdentifier);
				}

				XlatableIdentifier newIdentifier;
				newIdentifier.Identifier = rawLine.str();
				newIdentifier.HasAm = false;
				newIdentifier.HasDe = false;
				newIdentifier.HasFi = false;
				newIdentifier.HasUa = false;
				currentIdentifier = newIdentifier;
				hasCurrentIdentifier = true;
			}

			rawLine.clear();
		}

		if (hasCurrentIdentifier)
		{
			identifiers.push_back(currentIdentifier);
		}

		recipeFile.close();
		++xlatInfo->numFiles;

		std::ofstream headerFile(pathHeader.str());
		std::ofstream sourceFile(pathSource.str());

		headerFile
			<< "#pragma once"
			<< "\n\n";

		sourceFile
			<< "#include \"xlations.h\"\n"
			<< "#include \"xlations-en.h\"\n"
			<< "#include \"xlations-de.h\"\n"
			<< "#include \"xlations-fi.h\"\n"
			<< "#include \"xlations-ua.h\"\n"
			<< "\n";

		for (auto& identifier : identifiers)
		{
			headerFile
				<< "extern const char* literal"
				<< identifier.Identifier
				<< ";\n";

			sourceFile
				<< "const char* literal"
				<< identifier.Identifier
				<< " = \"\";\n";

			++xlatInfo->numLiterals;
		}

		for (auto i = 1; i <= 4; ++i)
		{
			std::string lcid;
			switch (i) {
			case 1:
				lcid = "en";
				break;

			case 2:
				lcid = "de";
				break;

			case 3:
				lcid = "fi";
				break;

			case 4:
				lcid = "ua";
				break;

			default:
				break;
			}

			std::stringstream pathHeaderLang;
			pathHeaderLang
				<< basePath
				<< R"(..\res\)"
				<< "xlations-"
				<< lcid
				<< ".h";

			std::ofstream headerLangFile;
			headerLangFile.open(pathHeaderLang.str(), std::ios::out | std::ios::trunc);
			std::locale utf8_locale;
			headerLangFile.imbue(utf8_locale);
			headerLangFile
				<< "#pragma once"
				<< "\n\n"
				<< "namespace blooDot::"
				<< lcid
				<< "\n{\n";

			sourceFile
				<< "\nnamespace blooDot::"
				<< lcid
				<< "\n{\n"
				<< "\tvoid Set()\n"
				<< "\t{\n";

			for (auto& identifier : identifiers)
			{
				headerLangFile
					<< "\tconstexpr auto const* _literal_"
					<< lcid
					<< "_"
					<< identifier.Identifier
					<< " = \"";
				
				std::string literal;
				switch(i)
				{ 
				case 1:
					literal = identifier.HasAm ? identifier.am : "";
					break;

				case 2:
					literal = identifier.HasDe ? identifier.de : identifier.am;
					break;

				case 3:
					literal = identifier.HasFi ? identifier.fi : identifier.am;
					break;

				case 4:
					literal = identifier.HasUa ? identifier.ua : identifier.am;
					break;

				default:
					break;
				}

				for(const auto& ch: literal)
				{
					if (ch == '\"')
					{
						headerLangFile << "\\\"";
					}
					else
					{
						headerLangFile << ch;
					}
				}

				headerLangFile
					<< "\";\n";

				sourceFile
					<< "\t\tliteral"
					<< identifier.Identifier
					<< " = _literal_"
					<< lcid
					<< "_"
					<< identifier.Identifier
					<< ";\n";
			}

			sourceFile << "\t}\n}\n";
			headerLangFile << "}\n";
			headerLangFile.close();

			headerFile
				<< "\nnamespace blooDot::"
				<< lcid
				<< "\n{\n"
				<< "\tvoid Set();\n"
				<< "}\n";
		}

		headerFile.close();
		sourceFile.close();

		return XlateReturnCodeSuccess;
	}

	return -1;
}

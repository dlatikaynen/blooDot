#include "pch.h"
#include "enxlate.h"
#ifndef NDEBUG

int Xlate()
{
	std::stringstream pathHeader;
	std::stringstream pathSource;

	auto const&& basePath = SDL_GetBasePath();
	std::stringstream recipePath;
	recipePath << basePath << "..\\..\\" << "gameres.xlation.recipe";
	std::ifstream recipeFile;
	recipeFile.imbue(std::locale("en_US.UTF8"));
	recipeFile.open(recipePath.str());
	pathHeader << basePath << "..\\..\\xlations.h";
	pathSource << basePath << "..\\..\\xlations.cpp";

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
						currentIdentifier.HasEn = true;
						currentIdentifier.en = literal;
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
			else if (rawLine.str().length() > 0 && rawLine.str()[0] != '#')
			{
				if (hasCurrentIdentifier)
				{
					identifiers.push_back(currentIdentifier);
					hasCurrentIdentifier = false;
				}

				XlatableIdentifier newIdentifier;
				newIdentifier.Identifier = rawLine.str();
				newIdentifier.HasEn = false;
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
		std::ofstream headerFile(pathHeader.str());
		std::ofstream sourceFile(pathSource.str());

		headerFile
			<< "#pragma once"
			<< "\n\n";

		sourceFile
			<< "#include \"pch.h\"\n"
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
			}

			std::stringstream pathHeaderLang;
			pathHeaderLang
				<< basePath
				<< "..\\..\\"
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
					<< "\tconstexpr char const* _literal_"
					<< lcid
					<< "_"
					<< identifier.Identifier
					<< " = \"";
					
				switch(i)
				{ 
				case 1:
					headerLangFile << (identifier.HasEn ? identifier.en : "");
					break;

				case 2:
					headerLangFile << (identifier.HasDe ? identifier.de : identifier.en);
					break;

				case 3:
					headerLangFile << (identifier.HasFi ? identifier.fi : identifier.en);
					break;

				case 4:
					headerLangFile << (identifier.HasUa ? identifier.ua : identifier.en);
					break;
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

#endif
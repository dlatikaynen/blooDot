#include "pch.h"
#include "enxlate.h"
#ifndef NDEBUG

int Xlate()
{
	std::stringstream xlations_h;
	std::stringstream xlations_de_h;
	std::stringstream xlations_fi_h;
	std::stringstream xlations_ua_h;
	
	std::stringstream enPath;
	std::stringstream dePath;
	std::stringstream fiPath;
	std::stringstream uaPath;

	xlations_h
		<< "#pragma once"
		<< "\n\n";

	xlations_de_h
		<< "#pragma once"
		<< "\n\n";

	xlations_fi_h
		<< "#pragma once"
		<< "\n\n";

	xlations_ua_h
		<< "#pragma once"
		<< "\n\n";

	auto const&& basePath = SDL_GetBasePath();
	std::stringstream recipePath;
	recipePath << basePath << "..\\..\\" << "gameres.xlation.recipe";
	std::ifstream recipeFile(recipePath.str());
	enPath << basePath << "..\\..\\" << "xlations-en.h";
	dePath << basePath << "..\\..\\" << "xlations-de.h";
	fiPath << basePath << "..\\..\\" << "xlations-fi.h";
	uaPath << basePath << "..\\..\\" << "xlations-ua.h";

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
					if (lcid == "[**]" || lcid == "[en]")
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
		std::ofstream enFile(enPath.str(), std::ios::binary);
		std::ofstream deFile(dePath.str(), std::ios::binary);
		std::ofstream fiFile(fiPath.str(), std::ios::binary);
		std::ofstream uaFile(uaPath.str(), std::ios::binary);

		for (auto& identifier : identifiers)
		{
			enFile
				<< "const char* literal"
				<< identifier.Identifier
				<< " = \"\";\n";
		}

		enFile
			<< "\nnamespace blooDot::en\n"
			<< "{\n";

		deFile
			<< "#include \"xlations-en.h\"\n\n"
			<< "namespace blooDot::de\n"
			<< "{\n";

		fiFile
			<< "#include \"xlations-en.h\"\n\n"
			<< "namespace blooDot::fi\n"
			<< "{\n";

		uaFile
			<< "#include \"xlations-en.h\"\n\n"
			<< "namespace blooDot::ua\n"
			<< "{\n";

		for (auto& identifier : identifiers)
		{
			if (identifier.HasEn)
			{
				enFile
					<< "\tconstexpr char const* _literal_en_"
					<< identifier.Identifier
					<< " = \""
					<< identifier.en
					<< "\";\n";
			}

			if (identifier.HasDe)
			{
				deFile
					<< "\tconstexpr char const* _literal_de_"
					<< identifier.Identifier
					<< " = \""
					<< identifier.de
					<< "\";\n";
			}

			if (identifier.HasFi)
			{
				fiFile
					<< "\tconstexpr char const* _literal_fi_"
					<< identifier.Identifier
					<< " = \""
					<< identifier.fi
					<< "\";\n";
			}

			if (identifier.HasUa)
			{
				uaFile
					<< "\tconstexpr char const* _literal_ua_"
					<< identifier.Identifier
					<< " = \""
					<< identifier.ua
					<< "\";\n";
			}
		}

		enFile
			<< "\n\tvoid Set()\n"
			<< "\t{\n";

		deFile
			<< "\n\tvoid Set()\n"
			<< "\t{\n";

		fiFile
			<< "\n\tvoid Set()\n"
			<< "\t{\n";

		uaFile
			<< "\n\tvoid Set()\n"
			<< "\t{\n";

		for (auto& identifier : identifiers)
		{
			enFile
				<< "\t\tliteral"
				<< identifier.Identifier
				<< " = _literal_en_"
				<< identifier.Identifier
				<< ";\n";

			deFile
				<< "\t\tliteral"
				<< identifier.Identifier
				<< (identifier.HasDe ? " = _literal_de_" : " = blooDot::en::_literal_en_")
				<< identifier.Identifier
				<< ";\n";

			fiFile
				<< "\t\tliteral"
				<< identifier.Identifier
				<< (identifier.HasFi ? " = _literal_fi_" : " = blooDot::en::_literal_en_")
				<< identifier.Identifier
				<< ";\n";

			uaFile
				<< "\t\tliteral"
				<< identifier.Identifier
				<< (identifier.HasUa ? " = _literal_ua_" : " = blooDot::en::_literal_en_")
				<< identifier.Identifier
				<< ";\n";
		}

		enFile << "\t}\n}\n";
		deFile << "\t}\n}\n";
		fiFile << "\t}\n}\n";
		uaFile << "\t}\n}\n";

		enFile.close();
		deFile.close();
		fiFile.close();
		uaFile.close();

		return XlateReturnCodeSuccess;
	}

	return -1;
}

#endif
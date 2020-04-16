#include "..\PreCompiledHeaders.h"
#include "Utility.h"

using namespace std;

wchar_t* blooDot::Utility::CharToWChar(const char * inputString)
{
	/* what could be more straighforward than that? */
	size_t cSize = strlen(inputString) + 1;
	auto outputString = new wchar_t[cSize];
	mbstowcs_s(&cSize, outputString, cSize, inputString, cSize);
	return outputString;
}

wchar_t* blooDot::Utility::ExceptionToWChar(const std::exception& exceptionObject)
{
	auto rawMessage = exceptionObject.what();
	return blooDot::Utility::CharToWChar(rawMessage);
}

void blooDot::Utility::DebugOutputException(const std::exception& exceptionObject)
{
	auto errorMessage = blooDot::Utility::ExceptionToWChar(exceptionObject);
	OutputDebugStringW(errorMessage);
}

#pragma once

namespace blooDot
{
	class Utility sealed
	{
	public:
		static wchar_t* CharToWChar(const char * inputString);
		static wchar_t* ExceptionToWChar(const std::exception& exceptionObject);
		static void DebugOutputException(const std::exception& exceptionObject);
	};
}
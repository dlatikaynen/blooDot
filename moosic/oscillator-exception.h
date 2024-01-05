#pragma once

#include <string>
#include <cstring>
#include <iostream>
#include <sstream>
#include <vector>

namespace blooDot::Oscillator
{
	enum class OrchestratorExceptionType
	{
		GENERIC,
		VERBOSE_DEBUG,
		STATUS_INFO,
		WARNING_MESSAGE,
		MEMORY_ALLOCATION,
		MEMORY_ACCESS,
		FUNCTION_ARGUMENT,
		FILE_NOT_FOUND,
		FILE_FORMAT,
		FILE_ERROR,
		PROCESS_THREAD,
		SUBSYSTEM_AUDIO,
		SUBSYSTEM_MIDI
	};

	class OrchestratorException
	{
	public:
		OrchestratorException(const std::string& message, OrchestratorExceptionType type = OrchestratorExceptionType::GENERIC) : m_Message(message), m_Type(type) { }
		virtual ~OrchestratorException(void) { };

		virtual void Report(void) { std::cerr << std::endl << this->m_Message << std::endl; }
		virtual const OrchestratorExceptionType& GetType(void) { return this->m_Type; }
		virtual const std::string& GetMessage(void) { return this->m_Message; }
		virtual const char* GetMessageAsCStr(void) { return this->m_Message.c_str(); }

	private:
		OrchestratorExceptionType m_Type;
		std::string m_Message;
	};
}
#include "pch.h"
#include "oscillator-exception.h"
#include "oscillator-base.h"
#include "frame.h"

#include <stdlib.h>
#include <windows.h>

namespace blooDot::Oscillator
{
	const OscillatorBase::MemoryLayout OscillatorBase::SHORT_SIGNED_INT_8 = 0x1;
	const OscillatorBase::MemoryLayout OscillatorBase::SHORT_SIGNED_INT_16 = 0x2;
	const OscillatorBase::MemoryLayout OscillatorBase::SIGNED_INT_24 = 0x4;
	const OscillatorBase::MemoryLayout OscillatorBase::SIGNED_INT_32 = 0x8;
	const OscillatorBase::MemoryLayout OscillatorBase::SYMMETRIC_FLOAT_32 = 0x10;
	const OscillatorBase::MemoryLayout OscillatorBase::SYMMETRIC_FLOAT_64 = 0x20;

	double OscillatorBase::m_sRate = blooDot::Oscillator::SAMPLE_RATE;
	std::string OscillatorBase::m_samplesPath = "";
	bool OscillatorBase::m_showWarnings = true;
	bool OscillatorBase::m_printErrors = true;
	std::vector<OscillatorBase*> OscillatorBase::m_alertList;
	std::ostringstream OscillatorBase::m_oStream;

	OscillatorBase::OscillatorBase(void) : m_ignoreSampleRateChange(false)
	{
	}

	OscillatorBase::~OscillatorBase(void)
	{
	}

	void OscillatorBase::SetSampleRate(double rate)
	{
		if (rate > 0.0 && rate != m_sRate)
		{
			double oldRate = m_sRate;
			m_sRate = rate;
			for (unsigned int i = 0; i < m_alertList.size(); i++)
			{
				m_alertList[i]->SampleRateChanged(m_sRate, oldRate);
			}
		}
	}

	void OscillatorBase::SampleRateChanged(double, double)
	{
	}

	void OscillatorBase::AddSampleRateAlert(OscillatorBase* ptr)
	{
		for (unsigned int i = 0; i < this->m_alertList.size(); i++)
		{
			if (this->m_alertList[i] == ptr) return;
		}

		this->m_alertList.push_back(ptr);
	}

	void OscillatorBase::RemoveSampleRateAlert(OscillatorBase* ptr)
	{
		for (unsigned int i = 0; i < m_alertList.size(); i++)
		{
			if (m_alertList[i] == ptr)
			{
				m_alertList.erase(m_alertList.begin() + i);
				return;
			}
		}
	}

	void OscillatorBase::SetSamplesPath(std::string path)
	{
		if (!path.empty())
		{
			m_samplesPath = path;
		}

		if (m_samplesPath[m_samplesPath.length() - 1] != '\\')
		{
			m_samplesPath += "\\";
		}
	}

	void OscillatorBase::Swap16(unsigned char* ptr)
	{
		unsigned char val = *(ptr);
		*(ptr) = *(ptr + 1);
		*(ptr + 1) = val;
	}

	void OscillatorBase::Swap32(unsigned char* ptr)
	{
		unsigned char val = *(ptr);

		*(ptr) = *(ptr + 3);
		*(ptr + 3) = val;
		++ptr;
		val = *(ptr);
		*(ptr) = *(ptr + 1);
		*(ptr + 1) = val;
	}

	void OscillatorBase::Swap64(unsigned char* ptr)
	{
		unsigned char val = *(ptr);

		*(ptr) = *(ptr + 7);
		*(ptr + 7) = val;
		++ptr;
		val = *(ptr);
		*(ptr) = *(ptr + 5);
		*(ptr + 5) = val;
		++ptr;
		val = *(ptr);
		*(ptr) = *(ptr + 3);
		*(ptr + 3) = val;
		++ptr;
		val = *(ptr);
		*(ptr) = *(ptr + 1);
		*(ptr + 1) = val;
	}

	void OscillatorBase::Sleep(unsigned long milliseconds)
	{
		::Sleep((DWORD)milliseconds);
	}

	void OscillatorBase::HandleException(OrchestratorExceptionType type) const
	{
		HandleException(m_oStream.str(), type);
		m_oStream.str(std::string());
	}

	void OscillatorBase::HandleException(const char* message, OrchestratorExceptionType type)
	{
		std::string msg(message);
		HandleException(msg, type);
	}

	void OscillatorBase::HandleException(std::string message, OrchestratorExceptionType type)
	{
		if (type == OrchestratorExceptionType::WARNING_MESSAGE || type == OrchestratorExceptionType::STATUS_INFO)
		{
			if (!m_showWarnings)
			{
				return;
			}

			std::cerr << '\n' << message << '\n' << std::endl;
		}
		else if (type == OrchestratorExceptionType::VERBOSE_DEBUG)
		{
#ifdef DEBUG
			std::cerr << '\n' << message << '\n' << std::endl;
#endif
		}
		else
		{
			if (m_printErrors)
			{
				std::cerr << '\n' << message << '\n' << std::endl;
			}

			throw OrchestratorException(message, type);
		}
	}
}
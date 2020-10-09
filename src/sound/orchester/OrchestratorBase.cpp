#include "OrchestratorException.h"
#include "OrchestratorBase.h"
#include "AudioFrames.h"

#include <stdlib.h>
#include <windows.h>

namespace blooDot
{
	namespace Orchestrator
	{
		const OrchestratorBase::MemoryLayout OrchestratorBase::SHORT_SIGNED_INT_8 = 0x1;
		const OrchestratorBase::MemoryLayout OrchestratorBase::SHORT_SIGNED_INT_16 = 0x2;
		const OrchestratorBase::MemoryLayout OrchestratorBase::SIGNED_INT_24 = 0x4;
		const OrchestratorBase::MemoryLayout OrchestratorBase::SIGNED_INT_32 = 0x8;
		const OrchestratorBase::MemoryLayout OrchestratorBase::SYMMETRIC_FLOAT_32 = 0x10;
		const OrchestratorBase::MemoryLayout OrchestratorBase::SYMMETRIC_FLOAT_64 = 0x20;

		double OrchestratorBase::m_sRate = blooDot::Orchestrator::SAMPLE_RATE;
		std::string OrchestratorBase::m_samplesPath = "";
		bool OrchestratorBase::m_showWarnings = true;
		bool OrchestratorBase::m_printErrors = true;
		std::vector<OrchestratorBase *> OrchestratorBase::m_alertList;
		std::ostringstream OrchestratorBase::m_oStream;

		OrchestratorBase::OrchestratorBase(void) : m_ignoreSampleRateChange(false)
		{
		}

		OrchestratorBase::~OrchestratorBase(void)
		{
		}

		void OrchestratorBase::SetSampleRate(double rate)
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

		void OrchestratorBase::SampleRateChanged(double, double)
		{
		}

		void OrchestratorBase::AddSampleRateAlert(OrchestratorBase* ptr)
		{
			for (unsigned int i = 0; i < this->m_alertList.size(); i++)
			{
				if (this->m_alertList[i] == ptr) return;
			}

			this->m_alertList.push_back(ptr);
		}

		void OrchestratorBase::RemoveSampleRateAlert(OrchestratorBase* ptr)
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

		void OrchestratorBase::SetSamplesPath(std::string path)
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

		void OrchestratorBase::Swap16(unsigned char *ptr)
		{
			unsigned char val = *(ptr);
			*(ptr) = *(ptr + 1);
			*(ptr + 1) = val;
		}

		void OrchestratorBase::Swap32(unsigned char *ptr)
		{
			unsigned char val = *(ptr);
			*(ptr) = *(ptr + 3);
			*(ptr + 3) = val;
			++ptr;
			val = *(ptr);
			*(ptr) = *(ptr + 1);
			*(ptr + 1) = val;
		}

		void OrchestratorBase::Swap64(unsigned char *ptr)
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

		void OrchestratorBase::Sleep(unsigned long milliseconds)
		{
			::Sleep((DWORD)milliseconds);
		}

		void OrchestratorBase::HandleException(OrchestratorExceptionType type) const
		{
			HandleException(m_oStream.str(), type);
			m_oStream.str(std::string());
		}

		void OrchestratorBase::HandleException(const char *message, OrchestratorExceptionType type)
		{
			std::string msg(message);
			HandleException(msg, type);
		}

		void OrchestratorBase::HandleException(std::string message, OrchestratorExceptionType type)
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
}
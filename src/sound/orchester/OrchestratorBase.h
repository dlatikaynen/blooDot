#pragma once

#include <string>
#include <cstring>
#include <iostream>
#include <sstream>
#include <vector>

namespace blooDot
{
	namespace Orchestrator
	{
		class OrchestratorBase
		{
		public:
			typedef unsigned long MemoryLayout;

			static const MemoryLayout SHORT_SIGNED_INT_8;
			static const MemoryLayout SHORT_SIGNED_INT_16;
			static const MemoryLayout SIGNED_INT_24;
			static const MemoryLayout SIGNED_INT_32;
			static const MemoryLayout SYMMETRIC_FLOAT_32;
			static const MemoryLayout SYMMETRIC_FLOAT_64;

			static double SampleRate(void) { return m_sRate; }
			static void SetSampleRate(double rate);
			void ignoreSampleRateChange(bool ignore = true) { this->m_ignoreSampleRateChange = ignore; };
			static void ClearAlertList() { std::vector<OrchestratorBase *>().swap(m_alertList); };
			static std::string SamplesPath(void) { return m_samplesPath; }
			static void SetSamplesPath(std::string path);
			static void Swap16(unsigned char *ptr);
			static void Swap32(unsigned char *ptr);
			static void Swap64(unsigned char *ptr);
			static void Sleep(unsigned long milliseconds);

			static bool inRange(double value, double min, double max)
			{
				if (value < min) return false;
				else if (value > max) return false;
				else return true;
			}

			static void HandleException(const char *message, OrchestratorExceptionType type);
			static void HandleException(std::string message, OrchestratorExceptionType type);
			static void showWarnings(bool status) { m_showWarnings = status; }
			static void printErrors(bool status) { m_printErrors = status; }

		protected:
			OrchestratorBase(void);
			virtual ~OrchestratorBase(void);

			virtual void SampleRateChanged(double newRate, double oldRate);
			void AddSampleRateAlert(OrchestratorBase *ptr);
			void RemoveSampleRateAlert(OrchestratorBase *ptr);
			void HandleException(OrchestratorExceptionType type) const;
			static std::ostringstream m_oStream;
			bool m_ignoreSampleRateChange;

		private:
			static double m_sRate;
			static std::string m_samplesPath;
			static bool m_showWarnings;
			static bool m_printErrors;
			static std::vector<OrchestratorBase *> m_alertList;
		};

		typedef unsigned short UINT16;
		typedef unsigned int UINT32;
		typedef signed short SINT16;
		typedef signed int SINT32;
		typedef float FLOAT32;
		typedef double FLOAT64;

		const double SAMPLE_RATE = 22050.0;
		const double INVERSE_HALFBYTE = 0.0078125;
	}
}
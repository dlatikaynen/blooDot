#pragma once

#include <string>
#include <cstring>
#include <iostream>
#include <sstream>
#include <vector>

namespace blooDot::Oscillator
{
	enum MIDIControlCode
	{
		BankSelectMSB = 0,
		ModulationWheel = 1,
		BreathController = 2,
		PedalMSB = 4,
		PortamentoTimeMSB = 5,
		DataEntryMSB = 6,
		VolumeMSB = 7,
		BalanceMSB = 8,
		PanPositionMSB = 10,
		ExpressionMSB = 11,
		EffectControl1MSB = 12,
		EffectControl2MSB = 13,
		Controller0 = 32,
		Controller1 = 33,
		Controller2 = 34,
		Controller3 = 35,
		Controller4 = 36,
		Controller5 = 37,
		Controller6 = 38,
		Controller7 = 39,
		Controller8 = 40,
		Controller9 = 41,
		Controller10 = 42,
		Controller11 = 43,
		Controller12 = 44,
		Controller13 = 45,
		Controller14 = 46,
		Controller15 = 47,
		Controller16 = 48,
		Controller17 = 49,
		Controller18 = 50,
		Controller19 = 51,
		Controller20 = 52,
		Controller21 = 53,
		Controller22 = 54,
		Controller23 = 55,
		Controller24 = 56,
		Controller25 = 57,
		Controller26 = 58,
		Controller27 = 59,
		Controller28 = 60,
		Controller29 = 61,
		Controller30 = 62,
		Controller31 = 63,
		HoldPedal = 64,
		Portamento = 65,
		SostenutoPedal = 66,
		SoftPedal = 67,
		LegatoPedal = 68,
		Hold2Pedal = 69,
		SoundVariation = 70,
		ResonanceTimbre = 71,
		SoundReleaseTime = 72,
		SoundAttackTime = 73,
		FrequencyCutoffBrightness = 74,
		SoundControl6 = 75,
		SoundControl7 = 76,
		SoundControl8 = 77,
		SoundControl9 = 78,
		SoundControl10 = 79,
		Decay = 80,
		HiPassFilterFreq = 81,
		GeneralPurposeButton3 = 82,
		GeneralPurposeButton4 = 83,
		PortamentoAmount = 84,
		ReverbLevel = 91,
		TremoloLevel = 92,
		ChorusLevel = 93,
		DetuneLevel = 94,
		PhaserLevel = 95,
		DataButtonIncrement = 96,
		DataButtonDecrement = 97,
		NonregisteredParameterLSB = 98,
		NonRegisteredParameterMSB = 99,
		RegisteredParameterLSB = 100,
		RegisteredParameterMSB = 101,
		AllSoundOff = 120,
		AllControllersOff = 121,
		LocalKeyboard = 122,
		AllNotesOff = 123,
		OmniModeOff = 124,
		OmniModeOn = 125,
		MonoOperation = 126,
		PolyMode = 127
	};

	class OscillatorBase
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
		static void ClearAlertList() { std::vector<OscillatorBase*>().swap(m_alertList); };
		static std::string SamplesPath(void) { return m_samplesPath; }
		static void SetSamplesPath(std::string path);
		static void Swap16(unsigned char* ptr);
		static void Swap32(unsigned char* ptr);
		static void Swap64(unsigned char* ptr);
		static void Sleep(unsigned long milliseconds);

		static bool inRange(double value, double min, double max)
		{
			if (value < min)
			{
				return false;
			}
			else if (value > max)
			{
				return false;
			}

			return true;
		}

		static void HandleException(const char* message, OrchestratorExceptionType type);
		static void HandleException(std::string message, OrchestratorExceptionType type);
		static void showWarnings(bool status) { m_showWarnings = status; }
		static void printErrors(bool status) { m_printErrors = status; }

	protected:
		OscillatorBase(void);
		virtual ~OscillatorBase(void);

		virtual void SampleRateChanged(double newRate, double oldRate);
		void AddSampleRateAlert(OscillatorBase* ptr);
		void RemoveSampleRateAlert(OscillatorBase* ptr);
		void HandleException(OrchestratorExceptionType type) const;
		static std::ostringstream m_oStream;
		bool m_ignoreSampleRateChange;

	private:
		static double m_sRate;
		static std::string m_samplesPath;
		static bool m_showWarnings;
		static bool m_printErrors;
		static std::vector<OscillatorBase*> m_alertList;
	};

	typedef unsigned short UINT16;
	typedef unsigned int UINT32;
	typedef signed short SINT16;
	typedef signed int SINT32;
	typedef float FLOAT32;
	typedef double FLOAT64;

	const double PI = 3.14159265358979323846;
	const double PI_2 = 6.28318530717958647692;
	const double SAMPLE_RATE = 22050.0;
	const double INVERSE_HALFBYTE = 0.0078125;
}
#include "..\PreCompiledHeaders.h"

#pragma once

namespace blooDot
{
	typedef int PITCH;

	enum class ENVELOPE
	{
		Constant = 0,
		Attack = 1,
		Decay = 2
	};

	class SynthPrimitive
	{
	public:
		PITCH Pitch;
		ENVELOPE Envelope;
		std::shared_ptr<XAUDIO2_BUFFER> Sample;
	};
}
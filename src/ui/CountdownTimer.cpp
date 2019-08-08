#include "..\PreCompiledHeaders.h"
#include "UserInterface.h"
#include "..\dx\DirectXHelper.h"

using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace Microsoft::WRL;
using namespace Windows::UI::ViewManagement;
using namespace D2D1;

CountdownTimer::CountdownTimer() :
	m_elapsedTime(0.0f),
	m_secondsRemaining(0)
{
}

void CountdownTimer::Initialize()
{
	TextElement::Initialize();
}

void CountdownTimer::Update(float timeTotal, float timeDelta)
{
	if (m_secondsRemaining > 0)
	{
		m_elapsedTime += timeDelta;
		if (m_elapsedTime >= 1.0f)
		{
			m_elapsedTime -= 1.0f;

			if (--m_secondsRemaining > 0)
			{
				WCHAR buffer[4];
				swprintf_s(buffer, L"%2d", m_secondsRemaining);
				SetText(buffer);
				SetTextOpacity(1.0f);
				FadeOut(1.0f);
			}
			else
			{
				SetText(L"Go!");
				SetTextOpacity(1.0f);
				FadeOut(1.0f);
			}
		}
	}

	TextElement::Update(timeTotal, timeDelta);
}

void CountdownTimer::Render()
{
	TextElement::Render();
}

void CountdownTimer::StartCountdown(int seconds)
{
	m_secondsRemaining = seconds;
	m_elapsedTime = 0.0f;

	WCHAR buffer[4];
	swprintf_s(buffer, L"%2d", m_secondsRemaining);
	SetText(buffer);
	SetTextOpacity(1.0f);
	FadeOut(1.0f);
}

bool CountdownTimer::IsCountdownComplete() const
{
	return (m_secondsRemaining == 0);
}
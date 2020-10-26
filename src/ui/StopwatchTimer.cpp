#include "..\PreCompiledHeaders.h"
#include "UserInterface.h"
#include "..\dx\DirectXHelper.h"

using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace Microsoft::WRL;
using namespace Windows::UI::ViewManagement;
using namespace D2D1;

StopwatchTimer::StopwatchTimer() :
	m_active(false),
	m_elapsedTime(0.0f)
{
}

void StopwatchTimer::Initialize(std::shared_ptr<BrushRegistry> brushRegistry)
{
	TextElement::Initialize(brushRegistry);
}

void StopwatchTimer::Update(float timeTotal, float timeDelta)
{
	if (this->m_active)
	{
		WCHAR buffer[16];
		this->m_elapsedTime += timeDelta;
		this->GetFormattedTime(buffer);
		this->SetText(buffer);
	}

	TextElement::Update(timeTotal, timeDelta);
}

void StopwatchTimer::Render()
{
	TextElement::Render();
}

void StopwatchTimer::Start()
{
	m_active = true;
}

void StopwatchTimer::Stop()
{
	m_active = false;
}

void StopwatchTimer::Reset()
{
	m_elapsedTime = 0.0f;
}

void StopwatchTimer::GetFormattedTime(WCHAR* buffer, int length) const
{
	GetFormattedTime(buffer, length, m_elapsedTime);
}

void StopwatchTimer::GetFormattedTime(WCHAR* buffer, int length, float time)
{
	int partial = (int)floor(fmodf(time * 10.0f, 10.0f));
	int seconds = (int)floor(fmodf(time, 60.0f));
	int minutes = (int)floor(time / 60.0f);
	swprintf_s(buffer, length, L"%02d:%02d.%01d", minutes, seconds, partial);
}
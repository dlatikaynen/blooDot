#include "..\PreCompiledHeaders.h"
#include "UserInterface.h"
#include "..\dx\DirectXHelper.h"

using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace Microsoft::WRL;
using namespace Windows::UI::ViewManagement;
using namespace D2D1;

NerdStatsDisplay::NerdStatsDisplay() :
	m_averageFPS(0),
	m_lastShownFPS(0)
{
}

void NerdStatsDisplay::Initialize()
{
	TextElement::Initialize();
}

void NerdStatsDisplay::Update(float timeTotal, float timeDelta)
{
	if (m_isFadingOut == false && m_lastShownFPS != m_averageFPS)
	{
		m_lastShownFPS = m_averageFPS;
		WCHAR buffer[10];
		swprintf_s(buffer, L"%d fps", m_averageFPS);
		this->m_Buffer = ref new Platform::String(buffer);
		SetText(this->m_Buffer);
		SetTextOpacity(1.0f);
		SetVisible(true);
		FadeOut(2.0f);
	}

	TextElement::Update(timeTotal, timeDelta);
}

void NerdStatsDisplay::UpdateFPS(int averageFPS)
{
	m_averageFPS = averageFPS;
}

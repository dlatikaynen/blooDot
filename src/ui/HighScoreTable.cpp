#include "..\PreCompiledHeaders.h"
#include "UserInterface.h"
#include "..\dx\DirectXHelper.h"

using namespace Windows::UI::Core;
using namespace Windows::Foundation;
using namespace Microsoft::WRL;
using namespace Windows::UI::ViewManagement;
using namespace D2D1;

HighScoreTable::HighScoreTable()
{
}

void HighScoreTable::Initialize()
{
	TextElement::Initialize();
	UpdateText();
}

void HighScoreTable::Reset()
{
	m_entries.clear();
	UpdateText();
}

void HighScoreTable::Update(float timeTotal, float timeDelta)
{
	TextElement::Update(timeTotal, timeDelta);
}

void HighScoreTable::Render()
{
	TextElement::Render();
}

void HighScoreTable::AddScoreToTable(HighScoreEntry& entry)
{
	for (auto iter = m_entries.begin(); iter != m_entries.end(); ++iter)
	{
		iter->wasJustAdded = false;
	}

	entry.wasJustAdded = false;

	for (auto iter = m_entries.begin(); iter != m_entries.end(); ++iter)
	{
		if (entry.elapsedTime < iter->elapsedTime)
		{
			m_entries.insert(iter, entry);
			while (m_entries.size() > MAX_HIGH_SCORES)
				m_entries.pop_back();

			entry.wasJustAdded = true;
			UpdateText();
			return;
		}
	}

	if (m_entries.size() < MAX_HIGH_SCORES)
	{
		m_entries.push_back(entry);
		UpdateText();
		entry.wasJustAdded = true;
	}
}

void HighScoreTable::UpdateText()
{
	WCHAR formattedTime[32];
	WCHAR lines[1024] = { 0, };
	WCHAR buffer[128];

	swprintf_s(lines, L"High Scores:");
	for (unsigned int i = 0; i < MAX_HIGH_SCORES; ++i)
	{
		if (i < m_entries.size())
		{
			StopwatchTimer::GetFormattedTime(formattedTime, m_entries[i].elapsedTime);
			swprintf_s(
				buffer,
				(m_entries[i].wasJustAdded ? L"\n>> %s\t%s <<" : L"\n%s\t%s"),
				m_entries[i].tag->Data(),
				formattedTime
			);
			wcscat_s(lines, buffer);
		}
		else
		{
			wcscat_s(lines, L"\n-");
		}
	}

	SetText(lines);
}
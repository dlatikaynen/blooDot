#pragma once

#include "TextElement.h"
#include "StopwatchTimer.h"

struct HighScoreEntry
{
	Platform::String^ tag;
	float elapsedTime;
	bool wasJustAdded;
};

#define MAX_HIGH_SCORES 5
typedef std::vector<HighScoreEntry> HighScoreEntries;

class HighScoreTable : public TextElement
{
public:
	HighScoreTable();

	virtual void Initialize(std::shared_ptr<BrushRegistry> brushRegistry);
	virtual void Update(float timeTotal, float timeDelta);
	virtual void Render();

	void AddScoreToTable(HighScoreEntry& entry);
	HighScoreEntries GetEntries() { return m_entries; };
	void Reset();

protected:
	HighScoreEntries m_entries;

	void UpdateText();
};
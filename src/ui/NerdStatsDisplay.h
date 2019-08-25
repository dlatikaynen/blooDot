#pragma once

class NerdStatsDisplay : public TextElement
{
public:
	NerdStatsDisplay();

	virtual void Initialize();
	virtual void Update(float timeTotal, float timeDelta);
	virtual void Render();

	void UpdateFPS(int averageFPS);

protected:
	int m_averageFPS;
	int m_lastShownFPS;
};
#pragma once

class NerdStatsDisplay : public TextElement
{
public:
	NerdStatsDisplay();
	void Initialize();
	void Update(float timeTotal, float timeDelta);
	//void Render();
	void UpdateFPS(int averageFPS);

protected:
	int m_averageFPS;
	int m_lastShownFPS;
};
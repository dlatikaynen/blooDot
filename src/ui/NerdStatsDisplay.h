#pragma once

class NerdStatsDisplay : public TextElement
{
public:
	NerdStatsDisplay();
	void Initialize() override;
	void Update(float timeTotal, float timeDelta) override;	
	void UpdateFPS(int averageFPS);

protected:
	int m_averageFPS;
	int m_lastShownFPS;

private:
	Platform::String^ m_Buffer;
};
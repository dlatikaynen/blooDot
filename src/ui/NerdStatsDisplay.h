#pragma once

class NerdStatsDisplay : public TextElement
{
public:
	NerdStatsDisplay();
	void Initialize(std::shared_ptr<BrushRegistry> brushRegistry) override;
	void Update(float timeTotal, float timeDelta) override;	
	void UpdateFPS(int averageFPS);

protected:
	int m_averageFPS;
	int m_lastShownFPS;

private:
	Platform::String^ m_Buffer;
};
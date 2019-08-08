#pragma once

class CountdownTimer : public TextElement
{
public:
	CountdownTimer();

	virtual void Initialize();
	virtual void Update(float timeTotal, float timeDelta);
	virtual void Render();

	void StartCountdown(int seconds);

	bool IsCountdownComplete() const;

protected:
	float   m_elapsedTime;
	int     m_secondsRemaining;
};
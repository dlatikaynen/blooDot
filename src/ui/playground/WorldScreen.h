#include "Player.h"

#pragma once

class WorldScreen : public WorldScreenBase
{
public:
	WorldScreen();
	~WorldScreen() override;
	
	void Initialize(_In_ std::shared_ptr<Audio> audioEngine, _In_ std::shared_ptr<DX::DeviceResources>&	deviceResources, _In_ std::shared_ptr<BrushRegistry> brushRegistry) override;
	void SetControl(DirectX::XMFLOAT2 pointerPosition, TouchMap* touchMap, bool shiftKeyActive, bool left, bool right, bool up, bool down, float scrollDeltaX, float scrollDeltaY) override;
	void SetControl(int detentCount, bool shiftKeyActive) override;
	void SetControl(bool triggershoot);
	void Update(float timeTotal, float timeDelta) override;
	void Render(D2D1::Matrix3x2F orientation2D, DirectX::XMFLOAT2 pointerPosition) override;

protected:
	bool OnHit(std::shared_ptr<BlockObject> hitTarget);

private:
	void RenderSprites();
	void UpdateParticles(float timeTotal, float timeDelta);

	std::vector<Sprite*> m_Sprites;

	float m_shootx;
	float m_shooty;
	bool m_shooting;
	float m_shoot_directionX;
	float m_shoot_directionY;
	float m_blockstravelled;







};




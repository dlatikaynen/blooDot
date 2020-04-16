#include "Player.h"

#pragma once

class WorldScreen : public WorldScreenBase
{
public:
	WorldScreen();
	~WorldScreen() override;
	
	void Initialize(_In_ std::shared_ptr<DX::DeviceResources>&	deviceResources) override;
	void SetControl(DirectX::XMFLOAT2 pointerPosition, TouchMap* touchMap, bool shiftKeyActive, bool left, bool right, bool up, bool down, float scrollDeltaX, float scrollDeltaY) override;
	void SetControl(int detentCount, bool shiftKeyActive) override;
	void Update(float timeTotal, float timeDelta) override;
	void Render(D2D1::Matrix3x2F orientation2D, DirectX::XMFLOAT2 pointerPosition) override;

private:
	void RenderSprites();

	std::vector<Player*> m_playerData;
};

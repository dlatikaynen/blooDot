#pragma once

class PlayerMomentum
{
public:
	D2D1_SIZE_F Speed;
	D2D1_SIZE_F Acceleration;
	D2D1_SIZE_F Gradient;
};

class Player 
{
public:
	Player(const Player& obj) { };
	Player& operator=(const Player& obj) { return *this; };
	Player() = default;
	Player(Player&& obj) {};
	~Player() {};

	Platform::String^ Name;
	D2D1_RECT_F Position;
	D2D1_POINT_2U PositionSquare;
	PlayerMomentum Momentum;
	Facings Facing;
	D2D1_RECT_F SpriteSourceRect;
};

class WorldScreen : public WorldScreenBase
{
public:
	WorldScreen();
	~WorldScreen() override;
	
	void Update(float timeTotal, float timeDelta) override;
	void Render(D2D1::Matrix3x2F orientation2D, DirectX::XMFLOAT2 pointerPosition) override;
	void Initialize(_In_ std::shared_ptr<DX::DeviceResources>&	deviceResources) override;

private:
	void RenderSprites();

	std::vector<Player*> m_playerData;
};

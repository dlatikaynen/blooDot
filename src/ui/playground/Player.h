#include "PlayerMomentum.h"
#include "..\src\dings\dings.h"

#pragma once

class Player
{
public:
	Player(const Player& obj) { };
	Player& operator=(const Player& obj) { return *this; };
	Player();
	Player(Player&& obj) {};
	~Player() {};

	void Update();

	Platform::String^ Name;
	D2D1_RECT_F Position;
	D2D1_POINT_2U PositionSquare;
	PlayerMomentum Momentum;
	Facings Facing;
	D2D1_RECT_F SpriteSourceRect;
};

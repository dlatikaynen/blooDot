#include "Player.h"

Player::Player()
{
	this->Momentum.Acceleration = D2D1::SizeF(0.0f, 0.0f);
	this->Momentum.Gradient = D2D1::SizeF(0.0f, 0.0f);
	this->Momentum.Speed = D2D1::SizeF(0.0f, 0.0f);
}

void Player::Update()
{
	this->Momentum.Speed.width += this->Momentum.Acceleration.width;
	this->Momentum.Speed.height += this->Momentum.Acceleration.height;
	this->Momentum.Acceleration.width += this->Momentum.Gradient.width;
	this->Momentum.Acceleration.height += this->Momentum.Gradient.height;
	auto deltaX = this->Momentum.Speed.width;
	auto deltaY = this->Momentum.Speed.height;
	this->Position.left += deltaX;
	this->Position.right += deltaX;
	this->Position.top += deltaY;
	this->Position.bottom += deltaY;
}
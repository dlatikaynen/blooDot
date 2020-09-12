#pragma once

#include "Dings.h"

class Mob : public Dings
{
public:
	Mob(Dings::DingIDs dingID, Platform::String^ dingName, std::shared_ptr<DX::DeviceResources> deviceResources, BrushRegistry* drawBrushes);
	bool IsMob() override;
};

class Player1 : public Mob
{
public:
	Player1(std::shared_ptr<DX::DeviceResources> deviceResources, BrushRegistry* drawBrushes);
protected:
	Platform::String^ ShouldLoadFromBitmap() override;
};

class Player2 : public Player1
{
public:
	Player2(std::shared_ptr<DX::DeviceResources> deviceResources, BrushRegistry* drawBrushes) : Player1(deviceResources, drawBrushes) { this->m_ID = Dings::DingIDs::Player2; m_Name = L"Player-2"; };
protected:
	Platform::String^ ShouldLoadFromBitmap() override;
};

class Player3 : public Player1
{
public:
	Player3(std::shared_ptr<DX::DeviceResources> deviceResources, BrushRegistry* drawBrushes) : Player1(deviceResources, drawBrushes) { this->m_ID = Dings::DingIDs::Player3; m_Name = L"Player-3"; };
protected:
	Platform::String^ ShouldLoadFromBitmap() override;
};

class Player4 : public Player1
{
public:
	Player4(std::shared_ptr<DX::DeviceResources> deviceResources, BrushRegistry* drawBrushes) : Player1(deviceResources, drawBrushes) { this->m_ID = Dings::DingIDs::Player4; m_Name = L"Player-4"; };
protected:
	Platform::String^ ShouldLoadFromBitmap() override;
};

class Dalek : public Mob
{
public:
	Dalek(std::shared_ptr<DX::DeviceResources> deviceResources, BrushRegistry* drawBrushes);
protected:
	Platform::String^ ShouldLoadFromBitmap() override;
};

class Schaedel : public Mob
{
public:
	Schaedel(std::shared_ptr<DX::DeviceResources> deviceResources, BrushRegistry* drawBrushes);
protected:
	Platform::String^ ShouldLoadFromBitmap() override;
};

class FlameGhost : public Mob
{
public:
	FlameGhost(std::shared_ptr<DX::DeviceResources> deviceResources, BrushRegistry* drawBrushes);
	void DrawInternal(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo, const D2D1_RECT_F *rect) override;

private:
	float outerRadius;
	float innerRadius;
	float rimRadius;
};
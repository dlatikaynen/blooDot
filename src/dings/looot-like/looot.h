#pragma once

#include "..\Dings.h"

class Coin : public Dings
{
public:
	Coin(std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<BrushRegistry> drawBrushes);
	void DrawInternal(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo) override;
	SoundEvent GetSoundOnTaken() override;
};

class Chest : public Dings
{
public:
	Chest(std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<BrushRegistry> drawBrushes);
	void DrawInternal(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo, const D2D1_RECT_F *rect) override;

protected:
	virtual int ColorVariation();
};

class SilverChest : public Chest
{
public:
	SilverChest(std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<BrushRegistry> drawBrushes);

protected:
	int ColorVariation() override;
};


class GoldChest : public Chest
{
public:
	GoldChest(std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<BrushRegistry> drawBrushes);

protected:
	int ColorVariation() override;
};

class Lettuce : public Dings
{
public:
	Lettuce(std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<BrushRegistry> drawBrushes);
protected:
	Platform::String^ ShouldLoadFromBitmap() override;
	SoundEvent GetSoundOnTaken() override;
};

class Radium : public Dings
{
public:
	Radium(std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<BrushRegistry> drawBrushes);
protected:
	Platform::String^ ShouldLoadFromBitmap() override;
};

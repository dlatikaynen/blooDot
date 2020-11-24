#pragma once

#include "..\Dings.h"

class Loot : public Dings
{
public:
	Loot(Dings::DingIDs dingID, Platform::String^ dingName, std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<BrushRegistry> drawBrushes);

protected:
	void InitChemicalElement();
};

class Coin : public Loot
{
public:
	Coin(std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<BrushRegistry> drawBrushes);
	void DrawInternal(Microsoft::WRL::ComPtr<ID2D1BitmapRenderTarget> drawTo) override;
	SoundEvent GetSoundOnTaken() override;
};

class Chest : public Loot
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

class Lettuce : public Loot
{
public:
	Lettuce(std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<BrushRegistry> drawBrushes);
protected:
	Platform::String^ ShouldLoadFromBitmap() override;
	SoundEvent GetSoundOnTaken() override;
};

class Radium : public Loot
{
public:
	Radium(std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<BrushRegistry> drawBrushes);
protected:
	Platform::String^ ShouldLoadFromBitmap() override;
};

class Fluorine : public Loot
{
public:
	Fluorine(std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<BrushRegistry> drawBrushes);
protected:
	Platform::String^ ShouldLoadFromBitmap() override;
};

class Copper : public Loot
{
public:
	Copper(std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<BrushRegistry> drawBrushes);
protected:
	Platform::String^ ShouldLoadFromBitmap() override;
};

class Chromium : public Loot
{
public:
	Chromium(std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<BrushRegistry> drawBrushes);
protected:
	Platform::String^ ShouldLoadFromBitmap() override;
};

class Silver : public Loot
{
public:
	Silver(std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<BrushRegistry> drawBrushes);
protected:
	Platform::String^ ShouldLoadFromBitmap() override;
};

class Gold : public Loot
{
public:
	Gold(std::shared_ptr<DX::DeviceResources> deviceResources, std::shared_ptr<BrushRegistry> drawBrushes);
protected:
	Platform::String^ ShouldLoadFromBitmap() override;
};

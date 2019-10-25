#pragma once

class LevelEditor : public WorldScreenBase
{
public:
	LevelEditor();
	~LevelEditor();
	void Render(D2D1::Matrix3x2F orientation2D, DirectX::XMFLOAT2 pointerPosition) override;

private:
	void DrawLevelEditorRaster();

};


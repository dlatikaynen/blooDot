#pragma once

class LevelEditor : public WorldScreenBase
{
public:
	LevelEditor();
	~LevelEditor();
	virtual void Update(float timeTotal, float timeDelta) override;
	void Render(D2D1::Matrix3x2F orientation2D, DirectX::XMFLOAT2 pointerPosition) override;

private:
	void DrawLevelEditorRaster();

};

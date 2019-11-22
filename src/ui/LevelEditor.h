#pragma once

class LevelEditor : public WorldScreenBase
{
public:
	LevelEditor();
	~LevelEditor() override;
	virtual void Update(float timeTotal, float timeDelta) override;
	void Render(D2D1::Matrix3x2F orientation2D, DirectX::XMFLOAT2 pointerPosition) override;

	void SelectDingForPlacement(unsigned dingID);
	void SelectNextDingForPlacement();
	void SelectPreviousDingForPlacement();

private:
	void DrawLevelEditorRaster();

	unsigned m_selectedDingID;
	bool m_IsErasing;
};


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
	Dings* SelectedDing();
	void DoPlaceDing();
	void DoRotateObject(bool inverse);
	void DoObliterateDing();
	void DoSetScrollLock(bool scrollLocked);
	void DoToggleGrid();
	void SetControl(int detentCount, bool shiftKeyActive) override;
	void DoRotate(bool affectPlacement, bool inverse);

private:
	void DrawLevelEditorRaster();
	void ClumsyPackNeighborhoodOf(ClumsyPacking::NeighborConfiguration neighborHood, unsigned aroundLevelX, unsigned aroundLevelY, Layers inLayer, unsigned dingID);
	void ClumsyPackNeighborhoodOf(unsigned aroundLevelX, unsigned aroundLevelY, Layers inLayer, unsigned dingID);
	void RedrawSingleSquare(unsigned levelX, unsigned levelY, Layers inLayer);

	unsigned m_selectedDingID;
	Facings m_selectedDingOrientation;
	bool m_IsErasing;
	bool m_IsOverwriting;
	bool m_isGridShown;
};


#include "..\dings\dings.h"

#pragma once

// An object is the runtime incarnation of a Ding on a level map, including actual position in level
class Object
{
public:
	Object::Object(unsigned posInLevelX, unsigned posInLevelY);
	void				Instantiate(Dings* templateDing);
	void				InstantiateInLayer(Layers inLayer, Dings* templateDing);
	void				Weed();
	Platform::String^	GetName();
	Layers				GetLayers();
	Dings*				GetDing(Layers ofLayer);
	Facings				PlacementFacing();

protected:

private:
	D2D1_POINT_2U		m_positionInLevel;
	Layers				m_Layers;
	Dings*				m_DingFloor;
	Dings*				m_DingWalls;
	Dings*				m_DingRooof;
	Facings				m_FacingFloor;
	Facings				m_FacingWalls;
	Facings				m_FacingRooof;
};


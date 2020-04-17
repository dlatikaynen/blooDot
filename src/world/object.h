#pragma once

#include "..\dings\dings.h"
#include "..\algo\ClumsyPacking.h"
#include "..\dings\ObjectProperties.h"

// An object is the runtime incarnation of a Ding on a level map, including actual position in level
class Object
{
public:
	Object::Object(unsigned posInLevelX, unsigned posInLevelY);
	void				Instantiate(Dings* templateDing, ClumsyPacking::NeighborConfiguration neighborHood);
	void				InstantiateInLayer(Layers inLayer, Dings* templateDing, ClumsyPacking::NeighborConfiguration neighborHood);
	void				InstantiateInLayerFacing(Layers inLayer, Dings* templateDing, Facings placementFacing);
	void				Weed();
	bool				WeedFromTop(Dings** dingWeeded, Layers* layerWeeded);
	void				SetupRuntimeState(Dings* floorDing, Dings* wallsDing, Dings* rooofDing);
	Layers				GetTopmostPopulatedLayer();
	Platform::String^	GetName();
	Layers				GetLayers();
	Dings*				GetDing(Layers ofLayer);
	Facings				PlacementFacing(Layers ofLayer);
	void				AdjustFacing(Layers inLayer, Facings shouldBeFacing);
	void				RotateInPlace(Layers inLayer, bool inverseDirection);
	void				DesignSaveToFile(std::ofstream* toFile, Layers ofLayer);

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
	ObjectBehaviors		m_BehaviorsFloor;
	ObjectBehaviors		m_BehaviorsWalls;
	ObjectBehaviors		m_BehaviorsRooof;
};


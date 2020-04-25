#pragma once

#include "..\dings\dings.h"
#include "..\algo\ClumsyPacking.h"

class ObjectProperties;
class Level;

/// An object is the design-time or runtime incarnation of a Ding on a level map,
/// including it's actual position in level
class Object
{
public:
	Object::Object(unsigned posInLevelX, unsigned posInLevelY);
	Object(const Object& obj) { };
	Object& operator=(const Object& obj) { return *this; };
	Object::Object() { };
	Object(Object&& obj) { };
	Object::~Object();

	void				Instantiate(std::shared_ptr<Level> placeInLevel, Dings* templateDing, ClumsyPacking::NeighborConfiguration neighborHood);
	void				InstantiateInLayer(std::shared_ptr<Level> placeInLevel, Layers inLayer, Dings* templateDing, ClumsyPacking::NeighborConfiguration neighborHood);
	void				InstantiateInLayerFacing(std::shared_ptr<Level> placeInLevel, Layers inLayer, Dings* templateDing, Facings placementFacing);
	void				PlaceInLevel(std::shared_ptr<Level> hostLevel);
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
	bool				GetBoundingBox(_Out_ D2D1_RECT_F* boundingBox);
	bool				GetBoundingBoxNext(_Out_ D2D1_RECT_F* boundingBox);
	
	Dings*								m_DingWalls;
	ObjectBehaviors						m_BehaviorsWalls;
	ObjectProperties*					m_objectProperties;
	Facings								m_FacingWalls;
	D2D1_POINT_2U						m_positionInLevel;
	D2D1_RECT_F							m_boundingBox;
	std::vector<D2D1_RECT_F>*			m_boundingBoxes;

private:
	std::vector<D2D1_RECT_F>::iterator	m_boundingBoxIter;
	Layers								m_Layers;
	Dings*								m_DingFloor;
	Dings*								m_DingRooof;
	Facings								m_FacingFloor;
	Facings								m_FacingRooof;
	ObjectBehaviors						m_BehaviorsFloor;
	ObjectBehaviors						m_BehaviorsRooof;
	std::shared_ptr<Level> m_Level;
};


#pragma once

#include "..\dings\mobs.h"
#include "..\algo\ClumsyPacking.h"

class ObjectProperties;
class Level;
class WorldScreenBase;

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

	void								Instantiate(std::shared_ptr<Level> placeInLevel, std::shared_ptr<Dings> templateDing, ClumsyPacking::NeighborConfiguration neighborHood);
	void								InstantiateInLayer(std::shared_ptr<Level> placeInLevel, Layers inLayer, std::shared_ptr<Dings> templateDing, ClumsyPacking::NeighborConfiguration neighborHood);
	void								InstantiateInLayerFacing(std::shared_ptr<Level> placeInLevel, Layers inLayer, std::shared_ptr<Dings> templateDing, Facings placementFacing);
	void								PlaceInLevel(std::shared_ptr<Level> hostLevel);
	void								Weed();
	bool								WeedFromTop(std::shared_ptr<Dings>* dingWeeded, Layers* layerWeeded);
	void								SetupRuntimeState(std::shared_ptr<Dings> floorDing, std::shared_ptr<Dings> wallsDing, std::shared_ptr<Dings> rooofDing);
	Layers								GetTopmostPopulatedLayer();
	Platform::String^					GetName();
	Layers								GetLayers();
	std::shared_ptr<Dings>				GetDing(Layers ofLayer);
	Facings								PlacementFacing(Layers ofLayer);
	void								AdjustFacing(Layers inLayer, Facings shouldBeFacing);
	void								RotateInPlace(Layers inLayer, bool inverseDirection);
	void								SetPosition(const D2D1_POINT_2U gridPosition);
	void								SetPosition(const D2D1_POINT_2F pixelPosition);
	void								SetPosition(const D2D1_RECT_F pixelPosition);
	void								DesignSaveToFile(std::ofstream* toFile, Layers ofLayer);

	D2D1_RECT_F							Position;
	D2D1_POINT_2U						PositionSquare;

	bool								GetBoundingBox(_Out_ D2D1_RECT_F* boundingBox);
	bool								GetBoundingBoxNext(_Out_ D2D1_RECT_F* boundingBox);

	ObjectBehaviors						m_BehaviorsWalls;
	D2D1_RECT_F							m_spriteSourceRect;
	std::shared_ptr<Object>				SymLink2Floor;
	std::shared_ptr<Object>				SymLink2Walls;
	std::shared_ptr<Object>				SymLink2Rooof;

protected:
	std::shared_ptr<Dings>				m_DingWalls;
	ObjectProperties*					m_objectProperties;
	Facings								m_FacingWalls;
	std::shared_ptr<Level>				m_Level;
	D2D1_RECT_F							m_boundingBox;
	std::vector<D2D1_RECT_F>*			m_boundingBoxes;

	void SetMobRotation(OrientabilityIndexRotatory rotationDent);

private:
	std::vector<D2D1_RECT_F>::iterator	m_boundingBoxIter;
	Layers								m_Layers;
	std::shared_ptr<Dings>				m_DingFloor;
	std::shared_ptr<Dings>				m_DingRooof;
	Facings								m_FacingFloor;
	Facings								m_FacingRooof;
	ObjectBehaviors						m_BehaviorsFloor;
	ObjectBehaviors						m_BehaviorsRooof;
	Facings								m_orientationSpinTo;
	Facings								m_orientationCurrent;
};

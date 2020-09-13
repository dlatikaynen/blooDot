#pragma once

#include "..\dings\mobs.h"
#include "..\algo\ClumsyPacking.h"

class Blocks;
class ObjectProperties;
class Level;
class WorldScreenBase;

/// An object is the design-time or runtime incarnation of a Ding on a level map,
/// including it's actual position in level
class BlockObject
{
public:
	BlockObject::BlockObject(std::shared_ptr<Blocks> container);
	BlockObject::BlockObject() { }
	BlockObject::BlockObject(const BlockObject& obj) { };
	BlockObject& operator=(const BlockObject& obj) { return *this; };
	BlockObject::BlockObject(BlockObject&& obj) { };
	BlockObject::~BlockObject();

	Layers								Layer;

	void								Instantiate(std::shared_ptr<Dings> templateDing, ClumsyPacking::NeighborConfiguration neighborHood);
	void								InstantiateFacing(std::shared_ptr<Dings> templateDing, Facings placementFacing);
	void								SetupRuntimeState();
	std::shared_ptr<Dings>				GetDing();
	Facings								PlacementFacing();
	void								PlaceInLevel();
	void								SetPosition(const D2D1_POINT_2U gridPosition);
	void								SetPosition(const D2D1_POINT_2F pixelPosition);
	void								SetPosition(const D2D1_RECT_F pixelPosition);
	void								AdjustFacing(Facings shouldBeFacing);
	void								RotateInPlace(bool inverseDirection);
	bool								GetBoundingBox(_Out_ D2D1_RECT_F* boundingBox);
	bool								GetBoundingBoxNext(_Out_ D2D1_RECT_F* boundingBox);

	D2D1_RECT_F							Position;
	D2D1_POINT_2U						PositionSquare;

	ObjectBehaviors						m_Behaviors;
	D2D1_RECT_F							m_spriteSourceRect;

protected:
	std::shared_ptr<Blocks>				m_blocksContainer;
	std::shared_ptr<Dings>				m_Ding;
	ObjectProperties*					m_objectProperties;
	Facings								m_Facing;
	std::shared_ptr<Level>				m_Level;
	D2D1_RECT_F							m_boundingBox;
	std::vector<D2D1_RECT_F>*			m_boundingBoxes;

	void SetMobRotation(OrientabilityIndexRotatory rotationDent);

private:
	std::vector<D2D1_RECT_F>::iterator	m_boundingBoxIter;
	Facings								m_orientationSpinTo;
	Facings								m_orientationCurrent;
};

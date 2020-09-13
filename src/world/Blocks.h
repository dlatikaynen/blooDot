#pragma once

#include "..\dings\mobs.h"
#include "..\algo\ClumsyPacking.h"

class Level;
class WorldScreenBase;

/// A vertical slice of surface
class Blocks : public std::enable_shared_from_this<Blocks>
{
public:
	Blocks::Blocks(unsigned posInLevelX, unsigned posInLevelY);
	Blocks::Blocks(const Blocks& obj) { };
	Blocks& operator=(const Blocks& obj) { return *this; };
	Blocks::Blocks() { };
	Blocks::Blocks(Blocks&& obj) { };
	Blocks::~Blocks();

	void								Instantiate(std::shared_ptr<Level> placeInLevel, std::shared_ptr<Dings> templateDing, ClumsyPacking::NeighborConfiguration neighborHood);
	void								InstantiateInLayer(std::shared_ptr<Level> placeInLevel, Layers inLayer, std::shared_ptr<Dings> templateDing, ClumsyPacking::NeighborConfiguration neighborHood);
	void								InstantiateInLayerFacing(std::shared_ptr<Level> placeInLevel, Layers inLayer, std::shared_ptr<Dings> templateDing, Facings placementFacing);
	void								PlaceInLevel(std::shared_ptr<Level> hostLevel);
	void								Weed();
	bool								WeedFromTop(std::shared_ptr<Dings>* dingWeeded, Layers* layerWeeded);
	Layers								GetTopmostPopulatedLayer();
	Platform::String^					GetName();
	Layers								GetLayers();
	std::shared_ptr<BlockObject>		GetObject(Layers ofLayer);
	void								SetPosition(const D2D1_POINT_2U gridPosition);
	void								SetPosition(const D2D1_POINT_2F pixelPosition);
	void								SetPosition(const D2D1_RECT_F pixelPosition);
	void								DesignSaveToFile(std::ofstream* toFile, Layers ofLayer);

	D2D1_RECT_F							Position;
	D2D1_POINT_2U						PositionSquare;

protected:
	std::shared_ptr<Level>				m_Level;

private:
	std::shared_ptr<BlockObject>		m_ObjectFloor;
	std::shared_ptr<BlockObject>		m_ObjectWalls;
	std::shared_ptr<BlockObject>		m_ObjectRooof;
};

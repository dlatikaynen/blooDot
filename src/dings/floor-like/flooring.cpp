#include "..\..\PreCompiledHeaders.h"
#include "..\dings.h"

FloorStoneTile::FloorStoneTile(std::shared_ptr<DX::DeviceResources> deviceResources, BrushRegistry* drawBrushes) : Dings(Dings::DingIDs::FloorTilingStone, "Floor stone tiling", deviceResources, drawBrushes)
{
	this->m_Facings = Facings::Shy;
	this->m_Coalescing = Facings::Shy;
	this->m_preferredLayer = Layers::Floor;
	this->m_possibleLayers = Layers::Floor;
	this->m_Behaviors = ObjectBehaviors::Solid;
}

Platform::String^ FloorStoneTile::ShouldLoadFromBitmap()
{
	return L"floorstonetile.png";
}

FloorStoneTilePurple::FloorStoneTilePurple(std::shared_ptr<DX::DeviceResources> deviceResources, BrushRegistry* drawBrushes) : Dings(Dings::DingIDs::FloorTilingStonePurple, "Floor tiling (purple)", deviceResources, drawBrushes)
{
	this->m_Facings = Facings::Shy;
	this->m_Coalescing = Facings::Shy;
	this->m_preferredLayer = Layers::Floor;
	this->m_possibleLayers = Layers::Floor;
	this->m_Behaviors = ObjectBehaviors::Solid;
}

Platform::String^ FloorStoneTilePurple::ShouldLoadFromBitmap()
{
	return L"floorstonetile-purple.png";
}

FloorStoneTileOchre::FloorStoneTileOchre(std::shared_ptr<DX::DeviceResources> deviceResources, BrushRegistry* drawBrushes) : Dings(Dings::DingIDs::FloorTilingStoneOchre, "Corridor tiling (ochre)", deviceResources, drawBrushes)
{
	this->m_Facings = Facings::Shy;
	this->m_Coalescing = Facings::Shy;
	this->m_preferredLayer = Layers::Floor;
	this->m_possibleLayers = Layers::Floor;
	this->m_Behaviors = ObjectBehaviors::Solid;
}

Platform::String^ FloorStoneTileOchre::ShouldLoadFromBitmap()
{
	return L"floorstonetile-ochre.png";
}

FloorStoneTileSlate::FloorStoneTileSlate(std::shared_ptr<DX::DeviceResources> deviceResources, BrushRegistry* drawBrushes) : Dings(Dings::DingIDs::FloorTilingStoneSlate, "Floor tiling (slate)", deviceResources, drawBrushes)
{
	this->m_Facings = Facings::Shy;
	this->m_Coalescing = Facings::Shy;
	this->m_preferredLayer = Layers::Floor;
	this->m_possibleLayers = Layers::Floor;
	this->m_Behaviors = ObjectBehaviors::Solid;
}

Platform::String^ FloorStoneTileSlate::ShouldLoadFromBitmap()
{
	return L"floorstonetile-slate.png";
}

FloorRockTile::FloorRockTile(std::shared_ptr<DX::DeviceResources> deviceResources, BrushRegistry* drawBrushes) : Dings(Dings::DingIDs::FloorTilingRock, "Floor rock tiling", deviceResources, drawBrushes)
{
	this->m_Facings = Facings::Shy;
	this->m_Coalescing = Facings::Shy;
	this->m_preferredLayer = Layers::Floor;
	this->m_possibleLayers = Layers::Floor;
	this->m_Behaviors = ObjectBehaviors::Solid;
}

Platform::String^ FloorRockTile::ShouldLoadFromBitmap()
{
	return L"floorrocktile.png";
}

FloorRockTileCracked::FloorRockTileCracked(std::shared_ptr<DX::DeviceResources> deviceResources, BrushRegistry* drawBrushes) : Dings(Dings::DingIDs::FloorTilingRockCracked, "Floor rock (cracked)", deviceResources, drawBrushes)
{
	this->m_Facings = Facings::Shy;
	this->m_Coalescing = Facings::Shy;
	this->m_preferredLayer = Layers::Floor;
	this->m_possibleLayers = Layers::Floor;
	this->m_Behaviors = ObjectBehaviors::Solid;
}

Platform::String^ FloorRockTileCracked::ShouldLoadFromBitmap()
{
	return L"floorrocktile-cracked.png";
}
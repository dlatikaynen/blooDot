#pragma once

#include "..\PreCompiledHeaders.h"
#include "Dings.h"

class BlockObject;

class ObjectProperties
{
public:
	ObjectProperties::ObjectProperties();
	void InventoryEmplace(BlockObject* objectToAdd);

private:
	int										m_countLives;
	int										m_lootedPoints;
	int										m_countHits;
	int										m_countEnergy;
	std::map<Dings::DingIDs, BlockObject*>	m_inventoryContent;
	BlockObject*							m_wieldingTool;
};
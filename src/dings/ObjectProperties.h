#pragma once

#include "..\PreCompiledHeaders.h"
#include "Dings.h"

class Object;

class ObjectProperties
{
public:
	ObjectProperties::ObjectProperties();
	void InventoryEmplace(Object* objectToAdd);

private:
	int									m_countLives;
	int									m_lootedPoints;
	int									m_countHits;
	int									m_countEnergy;
	std::map<Dings::DingIDs, Object*>	m_inventoryContent;
	Object*								m_wieldingTool;
};
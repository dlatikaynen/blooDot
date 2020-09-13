#include "ObjectProperties.h"
#include "..\world\BlockObject.h"

ObjectProperties::ObjectProperties()
{
	this->m_countEnergy = -1;
	this->m_countHits = -1;
	this->m_countLives = -1;
	this->m_lootedPoints = 0;
	this->m_wieldingTool = nullptr;
}

void ObjectProperties::InventoryEmplace(BlockObject* objectToAdd)
{
	if (objectToAdd != nullptr)
	{
		auto dingID = objectToAdd->GetDing()->ID();
		auto existingItem = this->m_inventoryContent.find(dingID);
		if (existingItem == this->m_inventoryContent.end())
		{
			this->m_inventoryContent.emplace(dingID, objectToAdd);
		}
	}
}
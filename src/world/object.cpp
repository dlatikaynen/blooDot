#include "Object.h"

Object::Object(unsigned posInLevelX, unsigned posInLevelY)
{
	this->m_Dings = nullptr;
	this->m_positionInLevel = D2D1::Point2U(posInLevelX, posInLevelY);
}

Platform::String^ Object::Name()
{
	return m_Dings->Name();
}

Layers Object::Layer()
{
	return m_Layer;
}

Dings* Object::GetDings()
{
	return this->m_Dings;
}

Facings	Object::PlacementFacing()
{
	return this->m_Facing;
}
#include "..\dings\dings.h"

#pragma once

// An object is the runtime incarnation of a Ding on a level map, including actual position in level
class Object
{
public:
	Object::Object(unsigned posInLevelX, unsigned posInLevelY);
	Platform::String^	Name();
	Layers				Layer();

protected:

private:
	D2D1_POINT_2U		m_positionInLevel;
	Dings*				m_Dings;
	Layers				m_Layer;	
};


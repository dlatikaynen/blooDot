#include "..\dings\dings.h"

#pragma once

// An object is the runtime incarnation of a Ding on a level map, including actual position in level
class Object
{
public:
	Platform::String^	Name();
	Layers				Layer();

protected:

private:
	Dings*				m_Dings;
	Layers				m_Layer;
};


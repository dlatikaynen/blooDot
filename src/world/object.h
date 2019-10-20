#include "..\dings\dings.h"

#pragma once

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


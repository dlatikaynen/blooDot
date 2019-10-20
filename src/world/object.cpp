#include "object.h"

Platform::String^ Object::Name()
{
	return m_Dings->Name();
}

Layers Object::Layer()
{
	return m_Layer;
}
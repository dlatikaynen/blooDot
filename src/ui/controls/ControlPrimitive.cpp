#include "..\..\PreCompiledHeaders.h"
#include "..\UserInterface.h"
#include "..\..\dx\DirectXHelper.h"

#include "ControlPrimitive.h"

ControlPrimitive::ControlPrimitive(std::shared_ptr<BrushRegistry> brushRegistry)
{
	this->m_brushRegistry = brushRegistry;
}
#pragma once

class ControlPrimitive
{
public:
	ControlPrimitive(std::shared_ptr<BrushRegistry> brushRegistry);

protected:

private:
	std::shared_ptr<BrushRegistry> m_brushRegistry;

};
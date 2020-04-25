#pragma once

class BoundingGeometry;

class Bounding
{
public:
	Bounding::Bounding();
	Bounding(const Bounding& obj) { };
	Bounding& operator=(const Bounding& obj) { return *this; };
	Bounding(Bounding&& obj) { };

	D2D1_RECT_F OuterRim;
	std::vector<BoundingGeometry> Geometries;
};

class BoundingGeometry
{
public:
	enum class Primitive : unsigned
	{
		Rectangle = 0,
		Circle = 1
	};	

	/// when Shape is Primitive::Circle, then
	/// go hit the in-circle of the Bounds rectangle
	BoundingGeometry::Primitive Shape;
	D2D1_RECT_F Bounds;
};
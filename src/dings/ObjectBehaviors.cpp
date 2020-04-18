#include "ObjectBehaviors.h"

ObjectBehaviors& operator |=(ObjectBehaviors& a, ObjectBehaviors b)
{
	return a = a | b;
}
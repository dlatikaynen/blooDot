#include "pch.h"
#include "physics-render-binding.h"
#include "ding.h"

extern int viewportOffsetX;
extern int viewportOffsetY;

namespace blooDot::Physics
{
	/* for screens where we render only a part (square) */

	void AttachWorldPhysics(b2World* world)
	{
		auto centerSheet = GetWorldSheet(0, 0);
		auto& entities = *centerSheet->stuff;
		auto flapInfo = GetFlapAwareness(BUNGHOLE);

		int flapOffsetX = -flapInfo.myGridLeftX * GRIDUNIT + flapInfo.localDrawingOffsetX + GRIDUNIT / 2 + viewportOffsetX;
		int flapOffsetY = -flapInfo.myGridToopY * GRIDUNIT + flapInfo.localDrawingOffsetY + GRIDUNIT / 2 + viewportOffsetY;

		for (const auto& entity : entities)
		{
			b2BodyDef wallDef;
			b2PolygonShape wallShape;
			b2FixtureDef wallFixtureDef;

			wallDef.type = b2_staticBody;
			wallDef.position.Set(
				static_cast<float>(flapOffsetX + entity->gridAnchorX * GRIDUNIT + entity->pixOffsetX) / static_cast<float>(GRIDUNIT),
				static_cast<float>(flapOffsetY + entity->gridAnchorY * GRIDUNIT + entity->pixOffsetY) / static_cast<float>(GRIDUNIT)
			);

			const auto& wallBody = world->CreateBody(&wallDef);

			wallShape.SetAsBox(.5f, .5f);
			wallFixtureDef.shape = &wallShape;
			wallFixtureDef.friction = 0.32f;
			wallFixtureDef.restitution = .02f;
			wallBody->CreateFixture(&wallFixtureDef);
		}
	}
}
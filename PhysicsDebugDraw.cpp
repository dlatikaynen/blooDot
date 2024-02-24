#include "pch.h"

#ifndef NDEBUG

#include "physicsdebugdraw.h"
#include "ding.h"
#pragma warning (push)
#pragma warning (disable: 4100)
void PhysicsDebugDraw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	std::vector<SDL_FPoint> points;
	points.reserve(static_cast<std::vector<SDL_FPoint, std::allocator<SDL_FPoint>>::size_type>(vertexCount) + 1);
	for (int32 i = 0; i < vertexCount; ++i)
	{
		SDL_FPoint point
		{
			vertices[i].x * static_cast<float>(GRIDUNIT),
			vertices[i].y * static_cast<float>(GRIDUNIT)
		};

		points.push_back(point);
	}

	points.push_back({
		vertices[0].x* static_cast<float>(GRIDUNIT),
		vertices[0].y* static_cast<float>(GRIDUNIT)
	});

	SDL_SetRenderDrawColor(
		debugRenderer,
		static_cast<uint8>(ceil(color.b * 255.f)),
		static_cast<uint8>(ceil(color.r * 255.f)),
		static_cast<uint8>(ceil(color.g * 255.f)),
		static_cast<uint8>(ceil(color.a * 255.f))
	);

	SDL_RenderDrawLinesF(debugRenderer, points.data(), vertexCount + 1);
}

void PhysicsDebugDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	std::vector<SDL_FPoint> points;
	points.reserve(static_cast<std::vector<SDL_FPoint, std::allocator<SDL_FPoint>>::size_type>(vertexCount) + 1);
	for (int32 i = 0; i < vertexCount; ++i)
	{
		SDL_FPoint point
		{
			vertices[i].x * static_cast<float>(GRIDUNIT),
			vertices[i].y * static_cast<float>(GRIDUNIT)
		};

		points.push_back(point);
	}

	points.push_back({
		vertices[0].x * static_cast<float>(GRIDUNIT),
		vertices[0].y * static_cast<float>(GRIDUNIT)
	});

	SDL_SetRenderDrawColor(
		debugRenderer,
		static_cast<uint8>(ceil(color.b * 255.f)),
		static_cast<uint8>(ceil(color.r * 255.f)),
		static_cast<uint8>(ceil(color.g * 255.f)),
		static_cast<uint8>(ceil(color.a * 255.f))
	);

	SDL_RenderDrawLinesF(debugRenderer, points.data(), vertexCount + 1);
}

void PhysicsDebugDraw::DrawCircle(const b2Vec2& center, float radius, const b2Color& color)
{
	const int midX = static_cast<int>(center.x * static_cast<float>(GRIDUNIT));
	const int midY = static_cast<int>(center.y * static_cast<float>(GRIDUNIT));
	const int radiusR = static_cast<int>(radius * static_cast<float>(GRIDUNIT));
	const int r2 = radiusR * 2;

	SDL_Point octants[8]{};
	int x = radiusR - 1;
	int dX = 1;
	int dev = (dX - r2);
	int y = 0;
	int dY = 1;

	SDL_SetRenderDrawColor(
		debugRenderer,
		static_cast<uint8>(ceil(color.b * 255.f)),
		static_cast<uint8>(ceil(color.r * 255.f)),
		static_cast<uint8>(ceil(color.g * 255.f)),
		static_cast<uint8>(ceil(color.a * 255.f))
	);

	while (x >= y)
	{
		octants[0].x = midX + x;
		octants[0].y = midY - y;
		octants[1].x = midX + x;
		octants[1].y = midY + y;
		octants[2].x = midX - x;
		octants[2].y = midY - y;
		octants[3].x = midX - x;
		octants[3].y = midY + y;
		octants[4].x = midX + y;
		octants[4].y = midY - x;
		octants[5].x = midX + y;
		octants[5].y = midY + x;
		octants[6].x = midX - y;
		octants[6].y = midY - x;
		octants[7].x = midX - y;
		octants[7].y = midY + x;

		SDL_RenderDrawPoints(debugRenderer, &octants[0], 8);
		if (dev <= 0)
		{
			++y;
			dev += dY;
			dY += 2;
		}

		if (dev > 0)
		{
			--x;
			dX += 2;
			dev += (dX - r2);
		}
	}
}

void PhysicsDebugDraw::DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color)
{
	const int midX = static_cast<int>(center.x * static_cast<float>(GRIDUNIT));
	const int midY = static_cast<int>(center.y * static_cast<float>(GRIDUNIT));
	const int radiusR = static_cast<int>(radius * static_cast<float>(GRIDUNIT));
	const int r2 = radiusR * 2;
	
	SDL_Point octants[8]{};
	int x = radiusR - 1;
	int dX = 1;
	int dev = (dX - r2);
	int y = 0;
	int dY = 1;

	SDL_SetRenderDrawColor(
		debugRenderer,
		static_cast<uint8>(ceil(color.b * 255.f)),
		static_cast<uint8>(ceil(color.r * 255.f)),
		static_cast<uint8>(ceil(color.g * 255.f)),
		static_cast<uint8>(ceil(color.a * 255.f))
	);

	while (x >= y)
	{
		octants[0].x = midX + x;
		octants[0].y = midY - y;
		octants[1].x = midX + x;
		octants[1].y = midY + y;
		octants[2].x = midX - x;
		octants[2].y = midY - y;
		octants[3].x = midX - x;
		octants[3].y = midY + y;
		octants[4].x = midX + y;
		octants[4].y = midY - x;
		octants[5].x = midX + y;
		octants[5].y = midY + x;
		octants[6].x = midX - y;
		octants[6].y = midY - x;
		octants[7].x = midX - y;
		octants[7].y = midY + x;

		SDL_RenderDrawLines(debugRenderer, &octants[0], 8);
		if (dev <= 0)
		{
			++y;
			dev += dY;
			dY += 2;
		}

		if (dev > 0)
		{
			--x;
			dX += 2;
			dev += (dX - r2);
		}
	}
}

void PhysicsDebugDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
}

void PhysicsDebugDraw::DrawTransform(const b2Transform& xf)
{
}

void PhysicsDebugDraw::DrawPoint(const b2Vec2& p, float size, const b2Color& color)
{
}

#pragma warning (pop)
#endif
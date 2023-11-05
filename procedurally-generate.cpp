#include "pch.h"
#include "procedurally-generate.h"
#include "mapregion.h"
#include "xlations.h"

#pragma once

namespace blooDot::Map
{
	bool ProcedurallyGenerateScullery(SDL_Renderer* renderer, std::unique_ptr<StaticMapRegionDescriptor>& descriptor)
	{
		descriptor->region.RegionId = 1;
		descriptor->region.RegionName = std::string(literalregionNameScullery);
		
		/* overall dimensioning */
		descriptor->region.polygon.push_back({ -3,-3 });
		descriptor->region.polygon.push_back({ 3,-3 });
		descriptor->region.polygon.push_back({ 3,3 });
		descriptor->region.polygon.push_back({ -3,3 });
		descriptor->region.polygon.push_back({ -3,3 });
		descriptor->region.polygon.push_back({ -3,3 });
		descriptor->region.polygon.push_back({ -3,3 });
		descriptor->region.polygon.push_back({ -3,3 });
		descriptor->region.polygon.push_back({ -3,3 });
		descriptor->region.polygon.push_back({ -3,3 });
		if (!_PrepareDesigntimeRepresentation(renderer, descriptor))
		{
			return false;
		}

		_Define(descriptor, { -7, -5, Ding::BarrelIndigo });
		for (auto y = -3; y < 3; ++y)
		{
			for (auto x = -4; x < 4; ++x)
			{
				_Define(descriptor, { x, y, Ding::FloorSlate });
			}
		}

		_Define(descriptor, { -4, 2, Ding::WallClassic, -4, 7, 33 });

		_Define(descriptor, { 1, 1, Ding::BarrelIndigo });
		_Define(descriptor, { -1, 1, Ding::BarrelIndigo });
		_Define(descriptor, { 1, -1, Ding::BarrelWood });
		_Define(descriptor, { 0, 0, Ding::ChelF });
		_Define(descriptor, { -1, -1, Ding::BarrelLoaded });
		_Define(descriptor, { -1, -1, Ding::RooofMesh });
		_Define(descriptor, { 2, 1, Ding::Grass });
		_Define(descriptor, { -2, 0, Ding::Lettuce });
		_Define(descriptor, { 2, 0, Ding::Schaed });
		_Define(descriptor, { 0, -2, Ding::Snow });
		_Define(descriptor, { 0, -3, Ding::WallClassic });
		_Define(descriptor, { 1, -3, Ding::WallClassic });
		_Define(descriptor, { 2, -3, Ding::WallClassic });
		_Define(descriptor, { 2, -4, Ding::WallClassic });
		_Define(descriptor, { 2, -5, Ding::WallClassic });

		_Define(descriptor, { 6, 4, Ding::Grass });
		_Define(descriptor, { 7, 4, Ding::Grass });
		_Define(descriptor, { 6, 5, Ding::Grass });
		_Define(descriptor, { 7, 5, Ding::Grass });

		_Define(descriptor, { -10, 0, Ding::Lettuce });
		_Define(descriptor, { 8, 0, Ding::Lettuce });

		auto y = 2;
		for (auto i = 10; i < 120; ++i)
		{
			if (i % 5 == 0)
			{
				++y;
			}

			_Define(descriptor, { i, y, Ding::Lettuce });
		}

		y = 2;
		for (auto i = -10; i > -120; --i)
		{
			if (i % 5 == 0)
			{
				--y;
			}

			_Define(descriptor, { i, y, Ding::Schaed });
		}

		_TeardownDesigntimeRepresentation(descriptor);

		return true;
	}

	bool ProcedurallyGenerateSnurch(SDL_Renderer* renderer, std::unique_ptr<StaticMapRegionDescriptor>& descriptor)
	{
		descriptor->region.RegionId = 3;
		descriptor->region.RegionName = std::string(literallandmarkNameSnurch);

		descriptor->region.polygon.push_back({ -3,-3 });
		descriptor->region.polygon.push_back({ 3,-3 });
		descriptor->region.polygon.push_back({ 3,3 });
		if (!_PrepareDesigntimeRepresentation(renderer, descriptor))
		{
			return false;
		}

		/* the momument is composite; its head is elevated
		 * so mobs can move underneath it */
		_Define(descriptor, { 0, 0, Ding::SnaviorMonumentFloor });
		_Define(descriptor, { 0, 0, Ding::SnaviorMonumentHead, 48, 147 });

		/* the snavior sits on the cathedral floor,
		 * omitting floor tiles where there are none needed */

		/* the snurch is otherwise layouted like a standard
		 * cathedral, a cross with a dome in the middle and
		 * the choir/altar part in the east and the entrance
		 * at the westend */

		/* surrounding the snurch there is the cementary, with
		 * a special section devoted to the victims of the garden
		 * of ridiculously poisonous plants */

		/* in the crypt bonehouse, there is the king's
		 * chapel with the sanctuary of the holy thigh */


		_TeardownDesigntimeRepresentation(descriptor);

		return true;
	}

	bool _PrepareDesigntimeRepresentation(SDL_Renderer* renderer, std::unique_ptr<StaticMapRegionDescriptor>& descriptor)
	{
		if (descriptor->designtimeRendition != nullptr)
		{
			_TeardownDesigntimeRepresentation(descriptor);
		}

		descriptor->designtimeRendition = SDL_CreateTexture(renderer, 0, 0, 100, 100);
		if (descriptor->designtimeRendition == nullptr)
		{
			const auto& errMsg = SDL_GetError();
			ReportError("Could not create design time representation texture", errMsg);

			return false;
		}

		return true;
	}

	void _TeardownDesigntimeRepresentation(std::unique_ptr<StaticMapRegionDescriptor>& descriptor)
	{
		if (descriptor->designtimeRendition != nullptr)
		{
			SDL_DestroyTexture(descriptor->designtimeRendition);
			descriptor->designtimeRendition = nullptr;
		}
	}
}
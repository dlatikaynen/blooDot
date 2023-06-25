#include "pch.h"
#include "procedurally-generate.h"
#include "mapregion.h"
#include "xlations.h"

#pragma once

namespace blooDot::Map
{
	void ProcedurallyGenerateScullery(std::unique_ptr<StaticMapRegionDescriptor>& descriptor)
	{
		descriptor->region.RegionId = 1;
		descriptor->region.RegionName = std::string(literalregionNameScullery);
		
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
		
		_Define(descriptor, -7, -5,  Ding::BarrelIndigo, DingProps::Default);
		for (auto y = -3; y < 3; ++y)
		{
			for (auto x = -4; x < 4; ++x)
			{
				_Define(descriptor, x, y, Ding::FloorSlate);
			}
		}

		_Define(descriptor, -4, 2, Ding::WallClassic, -4, 7, 33);

		_Define(descriptor, 1, 1, Ding::BarrelIndigo);
		_Define(descriptor, -1, 1, Ding::BarrelIndigo);
		_Define(descriptor, 1, -1, Ding::BarrelWood);
		_Define(descriptor, 0, 0, Ding::ChelF);
		_Define(descriptor, -1, -1, Ding::BarrelLoaded);
		_Define(descriptor, -1, -1, Ding::RooofMesh);
		_Define(descriptor, 2, 1, Ding::Grass);
		_Define(descriptor, -2, 0, Ding::Lettuce);
		_Define(descriptor, 2, 0, Ding::Schaed);
		_Define(descriptor, 0, -2, Ding::Snow);
		_Define(descriptor, 0, -3, Ding::WallClassic);
		_Define(descriptor, 1, -3, Ding::WallClassic);
		_Define(descriptor, 2, -3, Ding::WallClassic);
		_Define(descriptor, 2, -4, Ding::WallClassic);
		_Define(descriptor, 2, -5, Ding::WallClassic);

		_Define(descriptor, 6, 4, Ding::Grass);
		_Define(descriptor, 7, 4, Ding::Grass);
		_Define(descriptor, 6, 5, Ding::Grass);
		_Define(descriptor, 7, 5, Ding::Grass);

		_Define(descriptor, -10, 0, Ding::Lettuce);
		_Define(descriptor, 8, 0, Ding::Lettuce);

		auto y = 2;
		for (auto i = 10; i < 120; ++i)
		{
			if (i % 5 == 0)
			{
				++y;
			}

			_Define(descriptor, i, y, Ding::Lettuce);
		}

		y = 2;
		for (auto i = -10; i > -120; --i)
		{
			if (i % 5 == 0)
			{
				--y;
			}

			_Define(descriptor, i, y, Ding::Schaed);
		}
	}

	void ProcedurallyGenerateSnurch(std::unique_ptr<StaticMapRegionDescriptor>& descriptor)
	{
		descriptor->region.RegionId = 3;
		descriptor->region.RegionName = std::string(literallandmarkNameSnurch);

		descriptor->region.polygon.push_back({ -3,-3 });
		descriptor->region.polygon.push_back({ 3,-3 });
		descriptor->region.polygon.push_back({ 3,3 });

		_Define(descriptor, 0, 0, Ding::SnaviorMonumentFloor);
		_Define(descriptor, 0, 0, Ding::SnaviorMonumentHead);
	}
}
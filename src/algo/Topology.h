namespace blooDot
{
	class Topology
	{
	public:
		/* ask a set theory pro why this has 13 cases stacked 10-8-8-8-8-6-6-6-6-4-4-4-4 */
		static constexpr const int D2EdgePlenty[] = { 6, 0, 2, 4, 6, 7, 5, 3, 1, 7 };
		static constexpr const int D2TripletNES[] = { 2, 4, 6, 0, 1, 7, 5, 3 };
		static constexpr const int D2TripletNWS[] = { 6, 0, 2, 4, 5, 3, 1, 7 };
		static constexpr const int D2TripletWNE[] = { 4, 6, 0, 2, 3, 1, 7, 5 };
		static constexpr const int D2TripletWSE[] = { 0, 2, 4, 6, 7, 5, 3, 1 };
		static constexpr const int D2TwinSW[] = { 0, 2, 4, 5, 3, 1 };
		static constexpr const int D2TwinSE[] = { 6, 7, 5, 3, 2, 4 };
		static constexpr const int D2TwinNW[] = { 0, 2, 3, 1, 7, 6 };
		static constexpr const int D2TwinNE[] = { 0, 1, 7, 5, 4, 6 };
		static constexpr const int D2SingleEast[] = { 6, 7, 5, 4 };
		static constexpr const int D2SingleWest[] = { 0, 2, 3, 1 };
		static constexpr const int D2SingleSouth[] = { 2, 4, 5, 3 };
		static constexpr const int D2SingleNorth[] = { 0, 1, 7, 6 };
	};
};

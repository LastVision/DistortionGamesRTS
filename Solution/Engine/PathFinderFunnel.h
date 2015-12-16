#pragma once

namespace Prism
{
	namespace Navigation
	{
		class NavMesh;
		class PathFinderAStar;

		class PathFinderFunnel
		{
		public:
			PathFinderFunnel(NavMesh* aNavMesh);
			~PathFinderFunnel();

			bool FindPath(const CU::Vector3<float>& aStart
				, const CU::Vector3<float>& anEnd, CU::GrowingArray<CU::Vector3<float>>& someFunneledPoints);

			bool FindPath(const CU::Vector2<float>& aStart
				, const CU::Vector2<float>& anEnd, CU::GrowingArray<CU::Vector3<float>>& someFunneledPoints);

			PathFinderAStar* GetAStar() const;

		private:
			PathFinderAStar* myAStar;
		};

		inline PathFinderAStar* PathFinderFunnel::GetAStar() const
		{
			return myAStar;
		}
	}
}
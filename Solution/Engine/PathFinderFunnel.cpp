#include "stdafx.h"
#include "PathFinderAStar.h"
#include "PathFinderFunnel.h"


namespace Prism
{
	namespace Navigation
	{
		PathFinderFunnel::PathFinderFunnel(NavMesh* aNavMesh)
			: myAStar(new PathFinderAStar(aNavMesh))
		{
		}


		PathFinderFunnel::~PathFinderFunnel()
		{
			SAFE_DELETE(myAStar);
		}

		bool PathFinderFunnel::FindPath(const CU::Vector3<float>& aStart
			, const CU::Vector3<float>& anEnd, CU::GrowingArray<CU::Vector3<float>>& someFunneledPoints)
		{
			return FindPath({ aStart.x, aStart.z }, { anEnd.x, anEnd.z }, someFunneledPoints);
		}

		bool PathFinderFunnel::FindPath(const CU::Vector2<float>& aStart
			, const CU::Vector2<float>& anEnd, CU::GrowingArray<CU::Vector3<float>>& someFunneledPoints)
		{
			CU::GrowingArray<Prism::Navigation::Triangle*> path(16);
			if (myAStar->FindPath(aStart, anEnd, path) == true)
			{
				for (int i = 0; i < path.Size(); ++i)
				{
					someFunneledPoints.Add(path[i]->GetCenter());
				}
				return true;
			}
			return false;
		}
	}
}
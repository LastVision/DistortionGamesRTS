#pragma once

namespace Prism
{
	enum class eOctreeType
	{
		NOT_IN_OCTREE,
		STATIC,
		DYNAMIC,
		PLAYER,
	};

	enum ePostProcessing
	{
		NONE = 0,
		BLOOM = 1,
	};
}
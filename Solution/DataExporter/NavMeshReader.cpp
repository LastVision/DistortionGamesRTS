#include "NavMeshReader.h"

#include <XMLReader.h>

NavMeshReader::NavMeshReader()
{
}


NavMeshReader::~NavMeshReader()
{
}

void NavMeshReader::ReadFile(const std::string& aFilePath)
{
	if (aFilePath.compare(aFilePath.size() - 4, 4, ".fbx") == 0
		|| aFilePath.compare(aFilePath.size() - 4, 4, ".FBX") == 0)
	{
		
	}
}
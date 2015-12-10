#include <CommonHelper.h>
#include "DGFXReader.h"
#include <iostream>

DGFXReader::DGFXReader()
{
}


DGFXReader::~DGFXReader()
{
}

void DGFXReader::ReadFile(const std::string& aFilePath)
{
	if (aFilePath.compare(aFilePath.size() - 4, 4, ".fbx") == 0
		|| aFilePath.compare(aFilePath.size() - 4, 4, ".FBX") == 0)
	{
		std::cout << "Converting FBX: " + aFilePath << std::endl;

		std::string dgfxPath = CU::GetGeneratedDataFolderFilePath(aFilePath, "dgfx");

		myFactory.ConvertToDGFX(aFilePath.c_str(), dgfxPath.c_str());
	}
}

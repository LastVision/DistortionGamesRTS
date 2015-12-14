#include <CommonHelper.h>
#include "DGFXReader.h"
#include <iostream>

DGFXReader::DGFXReader()
	: myErrors(64)
{
}


DGFXReader::~DGFXReader()
{
	if (myErrors.Size() > 0)
	{
		std::cout << "\n-----------------" << std::endl;
		std::cout << "ERRORS !!!" << std::endl;
		std::cout << "-----------------\n" << std::endl;

		for (int i = 0; i < myErrors.Size(); ++i)
		{
			std::cout << myErrors[i] << std::endl;
		}

		system("PAUSE");
	}
}

void DGFXReader::ReadFile(const std::string& aFilePath)
{
	if (aFilePath.compare(aFilePath.size() - 4, 4, ".fbx") == 0
		|| aFilePath.compare(aFilePath.size() - 4, 4, ".FBX") == 0)
	{
		std::cout << "Converting FBX: " + aFilePath << std::endl;

		std::string dgfxPath = CU::GetGeneratedDataFolderFilePath(aFilePath, "dgfx");

		myFactory.ConvertToDGFX(aFilePath.c_str(), dgfxPath.c_str(), myErrors);
	}
}

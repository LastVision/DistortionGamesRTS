#include <CommonHelper.h>
#include "DGFXReader.h"
#include <iostream>

struct ModelData
{
	ModelData(): myEffectPath("NOT INITED"), myIsAnimated(false) {}
	std::string myEffectPath;
	bool myIsAnimated;
};

DGFXReader::DGFXReader()
	: myErrors(64)
	, myConvertedFiles(64)
{
}


DGFXReader::~DGFXReader()
{
	std::ofstream file;
	file.open("GeneratedData/modellist.bin");
	for (int i = 0; i < myConvertedFiles.Size(); ++i)
	{
		std::string dgfxPath = CU::GetGeneratedDataFolderFilePath(myConvertedFiles[i], "dgfx");
		std::fstream binFile;
		binFile.open(dgfxPath.c_str(), std::ios::in | std::ios::binary);

		int fileVersion = -1;
		binFile.read((char*)&fileVersion, sizeof(int));

		int isNullObject = -1;
		binFile.read((char*)&isNullObject, sizeof(int));

		int isLodGroup = 0;
		binFile.read((char*)&isLodGroup, sizeof(int));

		int isAnimated = 0;
		binFile.read((char*)&isAnimated, sizeof(int));
		binFile.close();

		
		if (isAnimated == 1)
		{
			file << "1 ";
		}
		else
		{
			file << "0 ";
		}

		file << myConvertedFiles[i];

		file << std::endl;

	}
	file.close();

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
		myConvertedFiles.Add(aFilePath);

		std::cout << "Converting FBX: " + aFilePath << std::endl;

		std::string dgfxPath = CU::GetGeneratedDataFolderFilePath(aFilePath, "dgfx");

		myFactory.ConvertToDGFX(aFilePath.c_str(), dgfxPath.c_str(), myErrors);
	}
}

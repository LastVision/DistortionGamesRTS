#pragma once
#include <string>
#include <FBXFactory.h>

class Reader
{
public:
	void ReadFile(const std::string& aFilePath);

private:
	float CalcMaxDistance(const CU::GrowingArray<CU::Vector3<float>>& someVertices) const;
	void WriteXml(const std::string& aFbxPath, float aRadius) const;
	Prism::FBXFactory myFactory;
};


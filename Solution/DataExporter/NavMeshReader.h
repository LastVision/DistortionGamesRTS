#pragma once
#include "IReader.h"

class NavMeshReader : public IReader
{
public:
	NavMeshReader();
	~NavMeshReader();

	void ReadFile(const std::string& aFilePath) override;
};


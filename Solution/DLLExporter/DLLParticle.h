#pragma once
#include <string>
#include <Matrix.h>

class DLLParticle
{
public:
	DLLParticle();
	~DLLParticle();

	void LoadParticle(std::string& aParticleFile);
	void Update(float aDeltaTime);
	void Render();
private:
	void ReLoadParticle();
	void WatchFile(std::string& aParticleFile);

	bool myIsLoaded;

	CU::Matrix44<float> myOrientation;

	std::string myParticleFile;
};


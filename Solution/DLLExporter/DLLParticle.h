#pragma once
#include <string>
#include <Matrix.h>
class Entity;

namespace Prism 
{
	class Scene;
}

class DLLParticle
{
public:
	DLLParticle(Prism::Scene& aScene);
	~DLLParticle();

	void LoadParticle(std::string& aParticleFile);
	void Update(float aDeltaTime);
	void Render();
private:
	void ReLoadParticle();
	void WatchFile(std::string& aParticleFile);

	bool myIsLoaded;

	Entity* myEntity;
	Prism::Scene* myScene;

	std::string myParticleFile;
};


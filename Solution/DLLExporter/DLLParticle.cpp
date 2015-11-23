#include <CommonHelper.h>
#include <DL_Debug.h>
#include "DLLParticle.h"
#include <Entity.h>
#include <Engine.h>
#include <EngineEnums.h>
#include <FileWatcher.h>
#include <GraphicsComponent.h>
#include <ParticleEmitterComponent.h>
#include <ParticleEmitterInstance.h>
#include <Scene.h>

DLLParticle::DLLParticle(Prism::Scene& aScene)
	: myIsLoaded(false)
{
	myEntity = new Entity(eEntityType::PROP, aScene, Prism::eOctreeType::DYNAMIC);
	myEntity->myOrientation.SetPos({0.f, 0.f, 80.f});
	myScene = &aScene;
}


DLLParticle::~DLLParticle()
{
	delete myEntity;
	myEntity = nullptr;
}


void DLLParticle::ReLoadParticle()
{
	LoadParticle(myParticleFile);
}

void DLLParticle::WatchFile(std::string& aParticleFile)
{
	Prism::Engine::GetInstance()->GetFileWatcher()->Clear();
	DL_DEBUG("Cleared filewatcher");
	Prism::Engine::GetInstance()->GetFileWatcher()->WatchFile(aParticleFile,
		std::bind(&DLLParticle::ReLoadParticle, this));
	DL_DEBUG("Rewatch the file");
}

void DLLParticle::LoadParticle(std::string& aParticleFile)
{
	myIsLoaded = false;
	
	DL_DEBUG(CU::Concatenate("Start Loading Particle %s", aParticleFile).c_str());
	WatchFile(aParticleFile);

	CU::Matrix44f currentOrientation = myEntity->myOrientation;
	DL_DEBUG("Store the old orientation");
	if (myEntity->GetComponent<ParticleEmitterComponent>() != nullptr) 
	{
		myEntity->GetComponent<ParticleEmitterComponent>()->GetEmitter()->ReleaseData();
	}
	delete myEntity;
	DL_DEBUG("delete the old entity");
	myEntity = new Entity(eEntityType::PROP, *myScene, Prism::eOctreeType::DYNAMIC);
	myEntity->myOrientation = currentOrientation;
	DL_DEBUG("Create a new entity");
	myEntity->AddComponent<ParticleEmitterComponent>();
	DL_DEBUG("Added a ParticleEmitterComponent");
	myEntity->GetComponent<ParticleEmitterComponent>()->Init(aParticleFile);
	DL_DEBUG("Readd particleEmitter with the updated file");
	myIsLoaded = true;
	DL_DEBUG("Loading done");
	myParticleFile = aParticleFile;
}

void DLLParticle::Update(float aDeltaTime)
{
	if (myIsLoaded == false) return;
	myEntity->GetComponent<ParticleEmitterComponent>()->Update(aDeltaTime);
}

void DLLParticle::Render()
{
	if (myIsLoaded == false) return;
	myEntity->GetComponent<ParticleEmitterComponent>()->Render();
}

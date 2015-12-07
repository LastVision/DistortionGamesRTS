#include <CommonHelper.h>
#include <DebugDrawer.h>
#include <DL_Debug.h>
#include "DLLParticle.h"
#include <Engine.h>
#include <EngineEnums.h>
#include <FileWatcher.h>
#include <ParticleEmitterData.h>
#include <ParticleEmitterInstance.h>

DLLParticle::DLLParticle()
	: myIsLoaded(false)
{
	myOrientation.SetPos({0.f, 0.f, 80.f});
}

DLLParticle::~DLLParticle()
{
}

void DLLParticle::ReLoadParticle()
{
	LoadParticle(myParticleFile);
}

void DLLParticle::WatchFile(std::string& aParticleFile)
{
	Prism::FileWatcher::GetInstance()->Clear();
	Prism::FileWatcher::GetInstance()->WatchFile(aParticleFile, std::bind(&DLLParticle::ReLoadParticle, this));
}

void DLLParticle::LoadParticle(std::string& aParticleFile)
{
	myIsLoaded = false;
	WatchFile(aParticleFile);
	
	if (myCurrentParticle != nullptr)
	{
		myCurrentParticle->ReleaseData();
		delete myCurrentParticle;
		myCurrentParticle = nullptr;
	}

	if (myParticleData != nullptr)
	{
		delete myParticleData;
		myParticleData = nullptr;
	}

	CU::Matrix44f currentOrientation = myOrientation;
	myParticleData = new Prism::ParticleEmitterData();
	myParticleData->LoadDataFile(aParticleFile.c_str());
	
	myCurrentParticle = new Prism::ParticleEmitterInstance(myParticleData, true);

	myIsLoaded = true;
	myParticleFile = aParticleFile;
}

void DLLParticle::Update(float aDeltaTime)
{
	DEBUG_PRINT(myIsLoaded);
	if (myIsLoaded == false) return;
	myCurrentParticle->Update(aDeltaTime, CU::Matrix44f());
}

void DLLParticle::Render(Prism::Camera* aCamera)
{
	if (myIsLoaded == false) return;
	myCurrentParticle->Render(aCamera);
}

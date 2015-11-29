#include <CommonHelper.h>
#include <DL_Debug.h>
#include "DLLParticle.h"
#include <Engine.h>
#include <EngineEnums.h>
#include <FileWatcher.h>

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

	CU::Matrix44f currentOrientation = myOrientation;

	
	myIsLoaded = true;
	myParticleFile = aParticleFile;
}

void DLLParticle::Update(float aDeltaTime)
{
	if (myIsLoaded == false) return;
}

void DLLParticle::Render()
{
	if (myIsLoaded == false) return;
}

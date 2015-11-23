#include <DL_Debug.h>
#include "DLLModel.h"
#include <Entity.h>
#include <Engine.h>
#include <EngineEnums.h>
#include <FileWatcher.h>
#include <GraphicsComponent.h>
#include <InputComponent.h>
#include <Instance.h>
#include <Scene.h>


DLLModel::DLLModel(CU::InputWrapper& aInput, Prism::Scene& aScene)
	: myInput(&aInput)
	, myScene(&aScene)
{
	myEntity = new Entity(eEntityType::PROP, aScene, Prism::eOctreeType::DYNAMIC);
	myEntity->AddComponent<InputComponent>()->Init(aInput);
}


DLLModel::~DLLModel()
{
	delete myEntity;
	myEntity = nullptr;
}

void DLLModel::Update(float aDeltaTime)
{
	AutoRotate(aDeltaTime);
}

void DLLModel::LoadModel(const char* aModelFile, const char* aShaderFile)
{
	if (aShaderFile == "")
	{
		aShaderFile = "Data/Resource/Shader/S_effect_basic.fx";
	}

	WatchFiles(aModelFile, aShaderFile);
	CU::Matrix44f currentOrientation = myEntity->myOrientation;
	delete myEntity;

	myEntity = new Entity(eEntityType::PROP, *myScene, Prism::eOctreeType::DYNAMIC);
	myEntity->AddComponent<InputComponent>()->Init(*myInput);

	myEntity->AddComponent<GraphicsComponent>()->InitDLL(aModelFile, aShaderFile);
	myEntity->myOrientation = currentOrientation;
	GraphicsComponent* gfxComponent = myEntity->GetComponent<GraphicsComponent>();
	gfxComponent->GetInstance()->SetEffect(aShaderFile);

	myScene->AddInstance(gfxComponent->GetInstance());
}

void DLLModel::WatchFiles(const char* aModelFile, const char* aShaderFile)
{
	Prism::Engine::GetInstance()->GetFileWatcher()->Clear();
	Prism::Engine::GetInstance()->GetFileWatcher()->WatchFile(aModelFile, std::bind(&DLLModel::LoadModel, this, aModelFile, aShaderFile));
}

void DLLModel::AutoRotate(float aDeltaTime)
{
	CU::Vector3f orginalPos(myEntity->myOrientation.GetPos());
	myEntity->myOrientation.SetPos(CU::Vector3f());
	myEntity->GetComponent<InputComponent>()->RotateX(myAutoRotationSpeed.myX * aDeltaTime);
	myEntity->GetComponent<InputComponent>()->RotateY(myAutoRotationSpeed.myY * aDeltaTime);
	myEntity->GetComponent<InputComponent>()->RotateZ(myAutoRotationSpeed.myZ * aDeltaTime);
	myEntity->myOrientation.SetPos(orginalPos);
}

void DLLModel::ManualRotate()
{
	myAutoRotationSpeed = { 0, 0, 0 };
	CU::Matrix44f staticRotation;
	CU::Vector3f orginalPos(myEntity->myOrientation.GetPos());
	myEntity->myOrientation.SetPos(CU::Vector3f());
	staticRotation = CU::Matrix44f::CreateRotateAroundX(myManualRotationAngle.myX / (3.14f * 180));
	staticRotation = CU::Matrix44f::CreateRotateAroundY(myManualRotationAngle.myY / (3.14f * 180));
	staticRotation = CU::Matrix44f::CreateRotateAroundZ(myManualRotationAngle.myZ / (3.14f * 180));
	myEntity->GetComponent<InputComponent>()->SetRotation(myEntity->myOrientation * staticRotation);
	myEntity->myOrientation.SetPos(orginalPos);
}

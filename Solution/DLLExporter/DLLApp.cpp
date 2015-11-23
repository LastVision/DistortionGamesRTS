
#include <AudioInterface.h>
#include <Camera.h>
#include <DirectionalLight.h>
#include <DL_Debug.h>
#include "DLLApp.h"
#include "DLLCamera.h"
#include "DLLModel.h"
#include "DLLParticle.h"
#include <EffectContainer.h>
#include <Engine.h>
#include <EngineEnums.h>
#include <Entity.h>
#include <FileWatcher.h>
#include <GraphicsComponent.h>
#include <Instance.h>
#include <InputComponent.h>
#include <InputWrapper.h>
#include <Matrix.h>
#include <PostMaster.h>
#include <Scene.h>
#include <SetupInfo.h>
#include <TimerManager.h>

#define AudioInstance Prism::Audio::AudioInterface::GetInstance()
#define EngineInstance Prism::Engine::GetInstance()

DLLApp::DLLApp(int* aHwnd, Prism::SetupInfo& aWindowSetup, WNDPROC aWindowProc)
{
	DL_Debug::Debug::Create();
	CU::TimerManager::Create();
	PostMaster::Create();
	Prism::Audio::AudioInterface::CreateInstance();
	AudioInstance->Init("Data/Resource/Sound/Init.bnk");
	AudioInstance->LoadBank("Data/Resource/Sound/SpaceShooterBank.bnk");

	myScene = new Prism::Scene();
	myPanelWindowHandler = (HWND)aHwnd;

	Prism::Engine::Create(myEngineWindowHandler, aWindowProc, aWindowSetup);

	SetupLight();
	SetupInput();
	SetParentWindow(aWindowSetup);
	SetupCubeMap();
	myCamera = new DLLCamera(myInput, aWindowSetup, *myScene, 1.0f, 1.0f, 1.0f);
	myModel = new DLLModel(myInput, *myScene);
	myParticle = new DLLParticle(*myScene);
}

DLLApp::~DLLApp()
{
	delete myScene;
	myScene = nullptr;

	delete myCamera;
	myCamera = nullptr;

	delete myModel;
	myModel = nullptr;
}

void DLLApp::Render()
{
	Prism::Engine::GetInstance()->Render();
	myScene->Render();
	myParticle->Render();
}

void DLLApp::Update()
{
	CU::TimerManager::GetInstance()->Update();
	float deltaTime = CU::TimerManager::GetInstance()->GetMasterTimer().GetTime().GetFrameTime();
	myInput.Update();
	LogicUpdate(deltaTime);
}

void DLLApp::LoadModel(const char* aModelFile, const char* aShaderFile)
{
	myModel->LoadModel(aModelFile, aShaderFile);
	myModelFile = aModelFile;
	myShaderFile = aShaderFile;
}

void DLLApp::LoadParticle(const char* aParticleFile)
{
	DL_ASSERT_EXP(myParticle != nullptr, "myParticle dont exists!");
	std::string particleFile = aParticleFile;
	myParticle->LoadParticle(particleFile);
}

void DLLApp::SetClearColor(CU::Vector4f& aClearColor)
{
	Prism::Engine::GetInstance()->SetClearColor({ aClearColor.myR, aClearColor.myG, aClearColor.myB, aClearColor.myA });
}

void DLLApp::SetCubeMap(const char* aCubeMapFile)
{
	DL_DEBUG("Stuff happend!");
	EngineInstance->GetEffectContainer()->SetCubeMap(aCubeMapFile);
	LoadModel(myModelFile.c_str(), myShaderFile.c_str());
}

void DLLApp::LogicUpdate(float aDeltaTime)
{
	if (GetActiveWindow()) {
		if (myInput.KeyIsPressed(DIK_LALT) && myInput.MouseIsPressed(0))
		{
			myCamera->Zoom(aDeltaTime, myMouseSensitivty);
		}
		if (myInput.KeyIsPressed(DIK_LALT) && myInput.MouseIsPressed(2))
		{
			myCamera->Pan(aDeltaTime, myMouseSensitivty);
		}
		if (myInput.KeyIsPressed(DIK_LALT) && myInput.MouseIsPressed(1))
		{
			myCamera->Rotate(aDeltaTime, myMouseSensitivty);
		}
	}
	myModel->Update(aDeltaTime);
	myParticle->Update(aDeltaTime);
}

void DLLApp::SetupCubeMap()
{
	EngineInstance->GetEffectContainer()->SetCubeMap("Data/Resource/Texture/CubeMap/T_cubemap_test.dds");
	EngineInstance->SetClearColor(CU::Vector4f(0.3f, 0.3f, 0.3f, 1.f));
}

void DLLApp::SetupInput()
{
	myInput.Init(myPanelWindowHandler, GetModuleHandle(NULL),
		DISCL_NONEXCLUSIVE | DISCL_FOREGROUND, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
}

void DLLApp::SetParentWindow(Prism::SetupInfo &aWindowSetup)
{
	SetParent(myEngineWindowHandler, myPanelWindowHandler);
	SetWindowLong(myEngineWindowHandler, GWL_STYLE, WS_POPUP);
	SetWindowPos(myEngineWindowHandler, HWND_TOP, 0, 0,
		aWindowSetup.myScreenWidth, aWindowSetup.myScreenHeight, SWP_SHOWWINDOW);
}

void DLLApp::SetupLight()
{
	myDirectionalLight = new Prism::DirectionalLight();
	myDirectionalLight->SetDir(CU::Vector3f( 0.f, 0.f, -1.f ));
	myDirectionalLight->SetColor(CU::Vector4f(1.f, 1.f, 1.f, 1.f));

	myScene->AddLight(myDirectionalLight);
}

void DLLApp::SetDirectionalLightRotation(CU::Vector3f aRotation)
{
	myDirectionalLightRotation = aRotation;
	myDirectionalLight->SetDir(myDirectionalLightRotation);
}

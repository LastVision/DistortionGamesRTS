#include "stdafx.h"

#include <Engine.h>
#include "Level.h"
#include <Scene.h>
#include <Terrain.h>

Level::Level(const Prism::Camera& aCamera)
{
	myTerrain = new Prism::Terrain("Data/Resource/Texture/Terrain/heightmap.tga"
		, "Data/Resource/Texture/Terrain/T_rock.dds", { 256.f, 256.f }, 25.5f, CU::Matrix44<float>());

	myScene = new Prism::Scene(aCamera, *myTerrain);
}

Level::~Level()
{
	SAFE_DELETE(myTerrain);
	SAFE_DELETE(myScene);
}

bool Level::LogicUpdate(float aDeltaTime)
{
	return true;
}

void Level::Render()
{
	Prism::Engine::GetInstance()->SetClearColor({ 1.0f, 0.f, 0.f, 1.f });

	myScene->Render();
}

void Level::OnResize(int aWidth, int aHeigth)
{
}

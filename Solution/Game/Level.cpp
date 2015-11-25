#include "stdafx.h"

#include <Engine.h>
#include "Level.h"
#include <Terrain.h>

Level::Level()
{
	//myTerrain = new Prism::Terrain("Data/Resource/Texture/Terrain/heightmap.tga"
	//	, "Data/Resource/Texture/Terrain/T_rock.dds", { 1000.f, 1000.f }, 100.f, CU::Matrix44<float>());
}

Level::~Level()
{
	//SAFE_DELETE(myTerrain);
}

bool Level::LogicUpdate(float aDeltaTime)
{
	aDeltaTime;
	return true;
}

void Level::Render(bool aIsActiveState, const Prism::Camera& aCamera)
{
	Prism::Engine::GetInstance()->SetClearColor({ 1.0f, 0.f, 0.f, 1.f });
	aIsActiveState;

	//myTerrain->Render(aCamera);
}

void Level::OnResize(int aWidth, int aHeigth)
{
	aWidth;
	aHeigth;
}

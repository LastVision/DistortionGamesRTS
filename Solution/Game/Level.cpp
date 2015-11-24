#include "stdafx.h"

#include <Engine.h>
#include "Level.h"

Level::Level()
{
}

Level::~Level()
{
}

bool Level::LogicUpdate(float aDeltaTime)
{
	aDeltaTime;
	return true;
}

void Level::Render(bool aIsActiveState)
{
	Prism::Engine::GetInstance()->SetClearColor({ 1.0f, 0.f, 0.f, 1.f });
	aIsActiveState;
}

void Level::OnResize(int aWidth, int aHeigth)
{
	aWidth;
	aHeigth;
}

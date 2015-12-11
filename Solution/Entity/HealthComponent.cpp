#include "stdafx.h"

#include "Camera.h"
#include "Entity.h"
#include "HealthComponent.h"
#include <Engine.h>
#include "HealthComponentData.h"
#include "../GUI/BarWidget.h"
#include "ModelLoader.h"

HealthComponent::HealthComponent(Entity& aEntity, HealthComponentData& aData)
	: Component(aEntity)
	, myMaxHealth(aData.myHealth)
	, myCurrentHealth(aData.myHealth)
{
	Prism::ModelLoader::GetInstance()->Pause();
	myHealthBar = new GUI::BarWidget(myMaxHealth, myCurrentHealth, { 50.f, 10.f });
	Prism::ModelLoader::GetInstance()->UnPause();
}

HealthComponent::~HealthComponent()
{
	SAFE_DELETE(myHealthBar);
}

void HealthComponent::RenderHealthBar(const Prism::Camera& aCamera)
{
	CU::Matrix44<float> renderPos;
	renderPos.SetPos(myEntity.GetOrientation().GetPos());
	renderPos = renderPos * CU::InverseSimple(aCamera.GetOrientation());
	renderPos = renderPos * aCamera.GetProjection();

	CU::Vector3<float> newRenderPos = renderPos.GetPos();
	CU::Vector2<float> windowSize = Prism::Engine::GetInstance()->GetWindowSize();

	newRenderPos /= renderPos.GetPos4().w;
	newRenderPos += 1.f;
	newRenderPos *= 0.5f;
	newRenderPos.x *= windowSize.x;
	newRenderPos.y *= windowSize.y;
	newRenderPos.y += 50.f;
	myHealthBar->Render({ newRenderPos.x, newRenderPos.y });
}

bool HealthComponent::TakeDamage(int aDamage)
{
	DL_ASSERT_EXP(aDamage >= 0, "Cant take negative damage, use Heal for healing if that was your intention");

	if (myEntity.GetAlive() == false)
	{
		return true; // is this funky?
	}

	myCurrentHealth -= aDamage;
	if (myCurrentHealth <= 0)
	{
		myCurrentHealth = 0;
		myEntity.Kill();
		return false;
	}

	myHealthBar->Update();

	return true;
}

void HealthComponent::Heal(int aHealing)
{
	DL_ASSERT_EXP(aHealing >= 0, "Cant heal negative damage.");

	myCurrentHealth += aHealing;
	if (myCurrentHealth > myMaxHealth)
	{
		myCurrentHealth = myMaxHealth;
	}
	myHealthBar->Update();
}

void HealthComponent::Reset()
{
	myCurrentHealth = myMaxHealth;
}
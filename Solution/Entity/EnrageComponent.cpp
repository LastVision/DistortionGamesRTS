#include "stdafx.h"
#include "EnrageComponent.h"


EnrageComponent::EnrageComponent(Entity& anEntity, EnrageComponentData& aData)
	: Component(anEntity)
	, myOriginalCooldown(aData.myCooldown)
	, myOriginalDuration(aData.myDuration)
	, myCurrentCooldown(0.f)
	, myCurrentDuration(0.f)
	, myIsActive(false)
{
}

EnrageComponent::~EnrageComponent()
{
}

void EnrageComponent::Update(float aDeltaTime)
{
	myCurrentCooldown -= aDeltaTime;
	if (myIsActive == true)
	{
		myCurrentDuration -= aDeltaTime;

		if (myCurrentDuration <= 0.f)
		{
			myIsActive = false;

			/*
				Revert the modifiers that were activated when enrage activated.
			*/

		}
	}
}

void EnrageComponent::Activate()
{
	if (myCurrentCooldown <= 0.f)
	{
		myIsActive = true;
		myCurrentDuration = myOriginalDuration;
		myCurrentCooldown = myOriginalCooldown;

		/*
			Change stuff on entity here
			increase movement speed, armor, health, attack speed, m.m


		*/
	}
}
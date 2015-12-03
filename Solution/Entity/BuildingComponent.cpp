#include "stdafx.h"
#include "BuildingComponent.h"
#include "BuildingComponentData.h"
#include "Entity.h"
#include <PostMaster.h>
#include <SpawnUnitMessage.h>
#include <InputWrapper.h>

BuildingComponent::BuildingComponent(Entity& aEntity, BuildingCompnentData& aData)
	: Component(aEntity)
	, myBuildTypes(aData.myBuildUnitTypes)
{
}

BuildingComponent::~BuildingComponent()
{
}

void BuildingComponent::Update(float aDeltaTime)
{
	//If selected && owner is player Show GUI Buttons
	//If not selected && owner is player Hide GUI Buttons

	//Watch on postmaster Button pressed
	//Check if button is connected to this building
	//Call BuildUnit with the type that the button Represents

	//Basic version of this
	//If Selected && owner is Player
	//Check for input 1, 2 or 3
	//Build Unit depending on number if there is so many units
	CU::InputWrapper* input = CU::InputWrapper::GetInstance();
	if (myEntity.IsSelected() == true && myEntity.GetOwner() == eOwnerType::PLAYER)
	{
		if (input->KeyDown(DIK_1))
		{
			if (myBuildTypes[0] != eEntityType::EMPTY)
			{
				BuildUnit(myBuildTypes[0]);
			}
		}
		else if (input->KeyDown(DIK_2))
		{
			if (myBuildTypes[1] != eEntityType::EMPTY)
			{
				BuildUnit(myBuildTypes[1]);
			}
		}
		else if (input->KeyDown(DIK_3))
		{
			if (myBuildTypes[2] != eEntityType::EMPTY)
			{
				BuildUnit(myBuildTypes[2]);
			}
		}
	}
}

void BuildingComponent::BuildUnit(eEntityType aUnitType)
{
	PostMaster::GetInstance()->SendMessage(SpawnUnitMessage(aUnitType, myEntity.GetOwner(), myEntity.GetScene()));
}

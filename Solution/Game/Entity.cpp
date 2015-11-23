#include "stdafx.h"

#include <AudioInterface.h>
#include "Component.h"
#include "EnemyKilledMessage.h"
#include "Entity.h"
#include "PostMaster.h"
#include "SoundNote.h"
#include "SpawnExplosionMessage.h"
#include "SpawnPowerUpMessage.h"

Entity::Entity(eEntityType aType, Prism::Scene& aScene, Prism::eOctreeType anOctreeType, const std::string& aName)
	: myAlive(true)
	, myType(aType)
	, myScene(&aScene)
	, myOctreeType(anOctreeType)
	, myName(aName)
	, myPowerUpType(ePowerUpType::NO_POWERUP)
	, myPowerUpName("")
	, myGUIMarkerStartRendering(false)
	, myGUIMarkerStartRenderingTime(0.2f)

{
	for (int i = 0; i < static_cast<int>(eComponentType::_COUNT); ++i)
	{
		myComponents[i] = nullptr;
	}
}

Entity::~Entity()
{
	if (myType == eEntityType::ENEMY)
	{
		PostMaster::GetInstance()->SendMessage<EnemyKilledMessage>(EnemyKilledMessage());
	}
	for (int i = 0; i < static_cast<int>(eComponentType::_COUNT); ++i)
	{
		delete myComponents[i];
		myComponents[i] = nullptr;
	}

}

void Entity::Update(float aDeltaTime)
{
	for (int i = 0; i < static_cast<int>(eComponentType::_COUNT); ++i)
	{
		if (myComponents[i] != nullptr)
		{
			myComponents[i]->Update(aDeltaTime);
		}
	}
	if (myGUIMarkerStartRendering == true)
	{
		if (myGUIMarkerStartRenderingTime > 0)
		{
			myGUIMarkerStartRenderingTime -= aDeltaTime;
		}
		else
		{
			myGUIMarkerStartRendering = false;
		}
	}
}

void Entity::Kill()
{
	DL_ASSERT_EXP(myAlive == true, "Tried to kill an entity thats allready dead.");
	if (myComponents[static_cast<int>(eComponentType::SOUND)] != nullptr)
	{
		SendNote<SoundNote>(SoundNote(eSoundNoteType::PLAY, "Play_ShipExplosion"));
	}
	if (myPowerUpName != "")
	{
		PostMaster::GetInstance()->SendMessage(SpawnPowerUpMessage(myPowerUpName, myOrientation));
	}

	if (myType == eEntityType::ENEMY || myType == eEntityType::ALLY)
	{
		PostMaster::GetInstance()->SendMessage(SpawnExplosionMessage(eMessageType::SPAWN_EXPLOSION_ON_UNIT_DEATH
				, myOrientation.GetPos()));
	}

	if (myType == eEntityType::PROP || myType == eEntityType::STRUCTURE)
	{
		if (GetName().find("asteroid") != std::string::npos || GetName().find("Asteroid") != std::string::npos)
		{
			PostMaster::GetInstance()->SendMessage(SpawnExplosionMessage(eMessageType::SPAWN_EXPLOSION_ON_ASTROID_DEATH
				, myOrientation.GetPos()));
		}
		else if ( GetName().find("space_station_layer1") != std::string::npos //detta är det sista eventet
			|| GetName().find("Space_Station_Layer1") != std::string::npos
			|| GetName().find("Space_station_layer1") != std::string::npos
			|| GetName().find("Space_Station_layer1") != std::string::npos
			)
		{
			PostMaster::GetInstance()->SendMessage(SpawnExplosionMessage(eMessageType::SPAWN_ON_FINAL_STRUCTURE_1
				, myOrientation.GetPos()));
		}
		else if (GetName().find("space_station_layer2") != std::string::npos
			|| GetName().find("Space_Station_Layer2") != std::string::npos
			|| GetName().find("Space_station_layer2") != std::string::npos
			|| GetName().find("Space_Station_layer2") != std::string::npos
			)
		{
			PostMaster::GetInstance()->SendMessage(SpawnExplosionMessage(eMessageType::SPAWN_ON_FINAL_STRUCTURE_2
				, myOrientation.GetPos()));
		}
		else if (GetName().find("space_station_layer3") != std::string::npos
			|| GetName().find("Space_Station_Layer3") != std::string::npos
			|| GetName().find("Space_station_layer3") != std::string::npos
			|| GetName().find("Space_Station_layer3") != std::string::npos
			)
		{
			PostMaster::GetInstance()->SendMessage(SpawnExplosionMessage(eMessageType::SPAWN_ON_FINAL_STRUCTURE_3
				, myOrientation.GetPos()));
		}
		else
		{
			PostMaster::GetInstance()->SendMessage(SpawnExplosionMessage(eMessageType::SPAWN_EXPLOSION_ON_PROP_DEATH
				, myOrientation.GetPos()));
		}
	}
	myAlive = false;
}

void Entity::Reset()
{
	myOrientation = myOriginalOrientation;
	myAlive = true;
	for (int i = 0; i < static_cast<int>(eComponentType::_COUNT); ++i)
	{
		if (myComponents[i] != nullptr)
		{
			myComponents[i]->Reset();
		}
	}
}

void Entity::SetPowerUpName(const std::string& aPowerUpName)
{
	myPowerUpName = aPowerUpName;
}

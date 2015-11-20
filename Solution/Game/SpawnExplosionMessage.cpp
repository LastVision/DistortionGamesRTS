#include "stdafx.h"
#include "SpawnExplosionMessage.h"


SpawnExplosionMessage::SpawnExplosionMessage(eMessageType anExplosionType, CU::Vector3f aPosition)
	: myPosition(aPosition)
	, Message(anExplosionType)
{
	DL_ASSERT_EXP((anExplosionType == eMessageType::SPAWN_EXPLOSION_ON_UNIT_DEATH 
		|| anExplosionType == eMessageType::SPAWN_EXPLOSION_ON_PROP_DEATH 
		|| anExplosionType == eMessageType::SPAWN_EXPLOSION_ON_ASTROID_DEATH 
		|| anExplosionType == eMessageType::SPAWN_EXPLOSION_ON_ROCKET_DEATH) 
		|| anExplosionType == eMessageType::SPAWN_EFFECT_ON_HIT
		|| anExplosionType == eMessageType::SPAWN_EFFECT_ON_ASTROID_HIT
		|| anExplosionType == eMessageType::SPAWN_ON_FINAL_STRUCTURE_1
		|| anExplosionType == eMessageType::SPAWN_ON_FINAL_STRUCTURE_2
		|| anExplosionType == eMessageType::SPAWN_ON_FINAL_STRUCTURE_3,
		"Wrong message type. Has to be an explosion type! Check enum for Explosion type or ask Linus Skold.");
}


SpawnExplosionMessage::~SpawnExplosionMessage()
{
}

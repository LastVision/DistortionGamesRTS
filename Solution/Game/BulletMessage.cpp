#include "stdafx.h"
#include "BulletMessage.h"

BulletMessage::BulletMessage(eBulletType aType, const CU::Matrix44<float>& anOrientation, eEntityType aEntityType
		, const CU::Vector3<float>& aEnitityVelocity, const CU::Vector3<float>& aDirection, Entity* aHomingTarget
		, float aHomingTurnRateModifier)
	: myType(aType)
	, myOrientation(anOrientation)
	, myEntityType(aEntityType)
	, myEntityVelocity(aEnitityVelocity)
	, myDirection(aDirection)
	, myHomingTarget(aHomingTarget)
	, Message(eMessageType::ACTIVATE_BULLET)
	, myHomingTurnRateModifier(aHomingTurnRateModifier)
{
}
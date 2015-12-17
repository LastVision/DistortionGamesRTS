#pragma once

struct EnrageComponentData
{
	float myDuration;
	float myCooldown;

	float myArmorModifier;
	float myHealthModifier;

	float myAttackDamageModifier;
	float myAttackSpeedModifier;
	float myAttackRangeModifier;

	float myMovementSpeedModifier;

	bool myShouldExistInEntity = false;
};
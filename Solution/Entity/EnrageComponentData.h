#pragma once

struct EnrageComponentData
{
	float myDuration;
	float myCooldown;

	float myArmorModifier;
	float myHealthModifier;

	float myAttackDamageModifier;
	float myAttackRange2Modifier;
	float myRechargeTimeModifier;

	float myMovementSpeedModifier;

	bool myExistsInEntity = false;
};
#pragma once

struct ControllerComponentData
{
	float myVisionRange;
	float myAttackRange;
	float myAttackDamage;
	float myAttackRechargeTime;
	float myChaseDistance;
	float myChaseDistanceNeutral;
	bool myExistsInEntity = false;
};
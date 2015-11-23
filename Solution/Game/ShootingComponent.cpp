#include "stdafx.h"
#include "BulletMessage.h"
#include <Camera.h>
#include "Constants.h"
#include "EMPMessage.h"
#include "EMPNote.h"
#include "Entity.h"
#include <FileWatcher.h>
#include "GraphicsComponent.h"
#include "GUIComponent.h"
#include "GUINote.h"
#include <Instance.h>
#include "InputNote.h"
#include "LevelScoreMessage.h"
#include <MathHelper.h>
#include <ModelLoader.h>
#include "PostMaster.h"
#include "PhysicsComponent.h"
#include "PowerUpNote.h"
#include "ShootingComponent.h"
#include "ShootNote.h"
#include "SoundNote.h"
#include "PowerUpMessage.h"
#include "WeaponFactory.h"
#include <XMLReader.h>

#define PI 3.14159265359f

ShootingComponent::ShootingComponent(Entity& aEntity)
	: myWeapons(8)
	, myCurrentWeaponID(0)
	, Component(aEntity)
	, myHasWeapon(false)
	, myPowerUps(8)
	, myHomingTarget(nullptr)
	, myFireRatePowerUp(false)
	, myEMPPowerUp(false)
	, myHomingPowerUp(false)
	, myFireRatePowerUpDuration(0.f)
	, myEMPPowerUpDuration(0.f)
	, myHomingPowerUpDuration(0.f)
	, myEMPTime(10.f)
	, myHasShotMachinegun(true)
	, myHasShotRocket(true)
	, myHasSwitchWeapon(true)
	, myShootSwitch(false)
{

}

ShootingComponent::~ShootingComponent()
{

}

void ShootingComponent::Update(float aDeltaTime)
{
	if (myHasWeapon == true)
	{
		if (myWeapons[myCurrentWeaponID].myCurrentTime >= (myWeapons[myCurrentWeaponID].myCoolDownTime))
		{
			myWeapons[myCurrentWeaponID].myCurrentTime = myWeapons[myCurrentWeaponID].myCoolDownTime;
		}
		else
		{
			float totalReducer = 1.f;
			for (int i = 0; i < myPowerUps.Size(); i++)
			{
				totalReducer += myPowerUps[i].myPowerUpCoolDownReducer;
			}
			myWeapons[myCurrentWeaponID].myCurrentTime += aDeltaTime * totalReducer;
		}

		if (myWeapons.Size() >= 3) // for rockets
		{
			if (myWeapons[2].myCurrentTime >= (myWeapons[2].myCoolDownTime))
			{
				myWeapons[2].myCurrentTime = myWeapons[2].myCoolDownTime;
			}
			else
			{
				float totalReducer = 1.f;
				for (int i = 0; i < myPowerUps.Size(); i++)
				{
					totalReducer += myPowerUps[i].myPowerUpCoolDownReducer;
				}
				myWeapons[2].myCurrentTime += aDeltaTime * totalReducer;
			}
		}
	}


	myFireRatePowerUpDuration = fmaxf(myFireRatePowerUpDuration - aDeltaTime, 0.f);
	myHomingPowerUpDuration = fmaxf(myHomingPowerUpDuration - aDeltaTime, 0.f);
	//Should ' count down?? WE dont know.
	//myEMPPowerUpDuration = fmaxf(myEMPPowerUpDuration - aDeltaTime, 0.f);

	for (int i = myPowerUps.Size() - 1; i >= 0; --i)
	{
		if (myPowerUps[i].myPowerUpType == ePowerUpType::FIRERATEBOOST && myFireRatePowerUpDuration <= 0.f)
		{
			SetActivatePowerUp(ePowerUpType::FIRERATEBOOST, false);
			myPowerUps.RemoveCyclicAtIndex(i);
			myEntity.SendNote(GUINote(myWeapons[myCurrentWeaponID].myIsHoming || HasPowerUp(ePowerUpType::HOMING), eGUINoteType::HOMING_TARGET));
		}
		else if (myPowerUps[i].myPowerUpType == ePowerUpType::HOMING && myHomingPowerUpDuration <= 0.f)
		{
			SetActivatePowerUp(ePowerUpType::HOMING, false);
			myPowerUps.RemoveCyclicAtIndex(i);
			myEntity.SendNote(GUINote(myWeapons[myCurrentWeaponID].myIsHoming || HasPowerUp(ePowerUpType::HOMING), eGUINoteType::HOMING_TARGET));
		}
	}
}

void ShootingComponent::ReceiveNote(const ShootNote& aShootNote)
{
	if (myHasWeapon == true)
	{
		WeaponData* currWepData = nullptr;
		if (aShootNote.myIsRocket == false)
		{
			if (myHasShotMachinegun == false)
			{
				myEntity.GetComponent<GUIComponent>()->RemoveTutorialMessage();
				myHasShotMachinegun = true;
			}
			currWepData = &myWeapons[myCurrentWeaponID];
		}
		else if (myWeapons.Size() >= 3)
		{
			if (myHasShotRocket == false)
			{
				myEntity.GetComponent<GUIComponent>()->RemoveTutorialMessage();
				myHasShotRocket = true;
			}
			currWepData = &myWeapons[2];
		}

		if (currWepData != nullptr && currWepData->myCurrentTime == currWepData->myCoolDownTime)
		{
			if (myEntity.GetType() == eEntityType::PLAYER)
			{
				if (currWepData->myBulletType == eBulletType::MACHINGUN_BULLET_LEVEL_1
					|| currWepData->myBulletType == eBulletType::MACHINGUN_BULLET_LEVEL_2
					|| currWepData->myBulletType == eBulletType::MACHINGUN_BULLET_LEVEL_3)
				{
					myEntity.SendNote<SoundNote>(SoundNote(eSoundNoteType::PLAY, "Play_PlayerMachineGun"));

				}
				if (currWepData->myBulletType == eBulletType::SHOTGUN_BULLET_LEVEL_1
					|| currWepData->myBulletType == eBulletType::SHOTGUN_BULLET_LEVEL_2
					|| currWepData->myBulletType == eBulletType::SHOTGUN_BULLET_LEVEL_3)
				{
					if (myEntity.GetType() == eEntityType::PLAYER)
					{
						myEntity.SendNote<SoundNote>(SoundNote(eSoundNoteType::PLAY, "Play_Shotgun"));
					}
				}
			}


			for (int i = 0; i < currWepData->myBulletsPerShot; ++i)
			{
				CU::Matrix44<float> orientation = myEntity.myOrientation;
				orientation.SetPos(orientation.GetPos() + (orientation.GetForward() * 2.f)
					+ (currWepData->myPosition * myEntity.myOrientation));
				CU::Vector3<float> dir = aShootNote.myDirection;
				if (currWepData->mySpread > 0)
				{
					float max = float(currWepData->mySpread);
					float min = float(-currWepData->mySpread);

					float randomSpreadX = CU::Math::RandomRange<float>(min, max) / 100.f;
					float randomSpreadY = CU::Math::RandomRange<float>(min, max) / 100.f;

					CU::Matrix44<float> rotation;
					rotation.myMatrix[8] = randomSpreadX;
					rotation.myMatrix[9] = randomSpreadY;

					CU::Vector4<float> pos = orientation.GetPos();
					orientation = rotation * orientation;
					orientation.SetPos(pos);

					dir = dir * rotation;
				}

				if (myEntity.GetType() == eEntityType::PLAYER)
				{
					CU::Matrix44<float> rotation;
					rotation.myMatrix[8] = aShootNote.myEnititySteering.x;
					rotation.myMatrix[9] = -aShootNote.myEnititySteering.y;

					CU::Vector4<float> pos = orientation.GetPos();
					orientation = rotation * orientation;
					orientation.SetPos(pos);
				}
				else
				{
					CU::Normalize(dir);
					CU::Vector3<float> up(dir);
					up = up * CU::Matrix44<float>::CreateRotateAroundX(globalPi / 2.f);
					up = up * CU::Matrix44<float>::CreateRotateAroundY(globalPi / 2.f);
					CU::Normalize(up);

					CU::Vector3<float> right = CU::Cross<float>(up, dir);
					CU::Normalize(right);
					up = CU::Cross<float>(dir, right);
					CU::Normalize(up);


					orientation.myMatrix[0] = right.x;
					orientation.myMatrix[1] = right.y;
					orientation.myMatrix[2] = right.z;
					orientation.myMatrix[3] = 0;

					orientation.myMatrix[4] = up.x;
					orientation.myMatrix[5] = up.y;
					orientation.myMatrix[6] = up.z;
					orientation.myMatrix[7] = 0;

					orientation.myMatrix[8] = dir.x;
					orientation.myMatrix[9] = dir.y;
					orientation.myMatrix[10] = dir.z;
					orientation.myMatrix[11] = 0;
				}

				if (aShootNote.myIsRocket == false)
				{
					if (currWepData->myBulletType == eBulletType::MACHINGUN_BULLET_LEVEL_2
						|| currWepData->myBulletType == eBulletType::MACHINGUN_BULLET_LEVEL_3)
					{
						if (myShootSwitch == false)
						{
							CU::Vector3<float> pos = orientation.GetPos();
							pos += orientation.GetRight() * 5.f;
							orientation.SetPos(pos);
							PostMaster::GetInstance()->SendMessage(BulletMessage(currWepData->myBulletType
								, orientation, myEntity.GetType(), aShootNote.myEnitityVelocity
								, dir
								, HasPowerUp(ePowerUpType::HOMING) || currWepData->myIsHoming ? myHomingTarget : nullptr));
							myShootSwitch = true;
						}
						else
						{
							CU::Vector3<float> pos = orientation.GetPos();
							pos += orientation.GetRight() * -5.f;
							orientation.SetPos(pos);
							PostMaster::GetInstance()->SendMessage(BulletMessage(currWepData->myBulletType
								, orientation, myEntity.GetType(), aShootNote.myEnitityVelocity
								, dir
								, HasPowerUp(ePowerUpType::HOMING) || currWepData->myIsHoming ? myHomingTarget : nullptr));
							myShootSwitch = false;
						}
					}
					else
					{
						PostMaster::GetInstance()->SendMessage(BulletMessage(currWepData->myBulletType
							, orientation, myEntity.GetType(), aShootNote.myEnitityVelocity
							, dir
							, HasPowerUp(ePowerUpType::HOMING) || currWepData->myIsHoming ? myHomingTarget : nullptr));
					}
				}
				else
				{
					if (currWepData->myBulletType == eBulletType::ROCKET_MISSILE_LEVEL_1
						|| currWepData->myBulletType == eBulletType::ROCKET_MISSILE_LEVEL_3)
					{
						PostMaster::GetInstance()->SendMessage(BulletMessage(currWepData->myBulletType
							, orientation, myEntity.GetType(), aShootNote.myEnitityVelocity
							, dir
							, HasPowerUp(ePowerUpType::HOMING) || currWepData->myIsHoming ? myHomingTarget : nullptr
							, currWepData->myHomingTurnRateModifier));
					}



					if (currWepData->myBulletType == eBulletType::ROCKET_MISSILE_LEVEL_2
						|| currWepData->myBulletType == eBulletType::ROCKET_MISSILE_LEVEL_3)
					{
						CU::Vector3<float> pos = orientation.GetPos();
						pos -= orientation.GetForward() * 40.f;
						pos += orientation.GetRight() * 20.f;
						orientation.SetPos(pos);
						PostMaster::GetInstance()->SendMessage(BulletMessage(currWepData->myBulletType
							, orientation, myEntity.GetType(), aShootNote.myEnitityVelocity
							, dir
							, HasPowerUp(ePowerUpType::HOMING) || currWepData->myIsHoming ? myHomingTarget : nullptr
							, currWepData->myHomingTurnRateModifier));

						pos = orientation.GetPos();
						pos += orientation.GetRight() * -40.f;
						orientation.SetPos(pos);
						PostMaster::GetInstance()->SendMessage(BulletMessage(currWepData->myBulletType
							, orientation, myEntity.GetType(), aShootNote.myEnitityVelocity
							, dir
							, HasPowerUp(ePowerUpType::HOMING) || currWepData->myIsHoming ? myHomingTarget : nullptr
							, currWepData->myHomingTurnRateModifier));
					}
				}

				currWepData->myCurrentTime = 0.f;
			}
		}
	}
}

void ShootingComponent::ReceiveNote(const InputNote& aMessage)
{
	if (myHasWeapon == true)
	{
		SetCurrentWeaponID(aMessage.myKey);

		if (myHasSwitchWeapon == false && aMessage.myKey == 0)
		{
			myEntity.GetComponent<GUIComponent>()->RemoveTutorialMessage("Change weapon on '1' and '2'");
			myHasSwitchWeapon = true;
		}
	}
}

void ShootingComponent::ReceiveNote(const PowerUpNote& aNote)
{
	if (aNote.myType == ePowerUpType::EMP || aNote.myType == ePowerUpType::HOMING
		|| aNote.myType == ePowerUpType::FIRERATEBOOST)
	{
		SetActivatePowerUp(aNote.myType, true);
	}
	else
	{
		return;
	}
	if (aNote.myType == ePowerUpType::EMP
		|| aNote.myType == ePowerUpType::HOMING)
	{
		bool powerUpFound = false;
		for (int i = 0; i < myPowerUps.Size(); ++i)
		{
			if (myPowerUps[i].myPowerUpType == aNote.myType)
			{
				myPowerUps[i].myPowerUpCoolDownReducer = 0.f;
				//myPowerUps[i].myPowerUpDuration += aNote.myDuration;
				AddDuration(aNote.myType, aNote.myDuration);
				myPowerUps[i].myPowerUpType = aNote.myType;
				myPowerUps[i].myPowerUpValue = aNote.myValue;
				powerUpFound = true;
				break;
			}
		}
		if (powerUpFound == false)
		{
			WeaponPowerUp powerUp;
			powerUp.myPowerUpCoolDownReducer = 0.f;
			//powerUp.myPowerUpDuration = aNote.myDuration;
			AddDuration(aNote.myType, aNote.myDuration);
			powerUp.myPowerUpType = aNote.myType;
			powerUp.myPowerUpValue = aNote.myValue;
			myPowerUps.Add(powerUp);
			myEntity.SendNote(GUINote(myWeapons[myCurrentWeaponID].myIsHoming || powerUp.myPowerUpType == ePowerUpType::HOMING, eGUINoteType::HOMING_TARGET));
		}

	}
	else if (aNote.myType == ePowerUpType::FIRERATEBOOST)
	{
		bool powerUpFound = false;
		for (int i = 0; i < myPowerUps.Size(); ++i)
		{
			if (myPowerUps[i].myPowerUpType == aNote.myType)
			{
				myPowerUps[i].myPowerUpCoolDownReducer = aNote.myValue;
				//myPowerUps[i].myPowerUpDuration += aNote.myDuration;
				AddDuration(aNote.myType, aNote.myDuration);
				myPowerUps[i].myPowerUpType = aNote.myType;
				myPowerUps[i].myPowerUpValue = 0.f;
				powerUpFound = true;
				break;
			}
		}
		if (powerUpFound == false)
		{
			WeaponPowerUp powerUp;
			powerUp.myPowerUpCoolDownReducer = aNote.myValue;
			//powerUp.myPowerUpDuration = aNote.myDuration;
			AddDuration(aNote.myType, aNote.myDuration);
			powerUp.myPowerUpType = aNote.myType;
			powerUp.myPowerUpValue = 0.f;
			myPowerUps.Add(powerUp);
		}

	}
}

void ShootingComponent::ReceiveNote(const EMPNote&)
{
	if (HasPowerUp(ePowerUpType::EMP) == true)
	{
		SetActivatePowerUp(ePowerUpType::EMP, false);
		ActivatePowerUp(ePowerUpType::EMP);
	}
}

void ShootingComponent::AddWeapon(const WeaponDataType& aWeapon)
{
	WeaponData newWeapon;

	newWeapon.myHomingTurnRateModifier = aWeapon.myHomingTurnRateModifier;
	newWeapon.myBulletsPerShot = aWeapon.myBulletsPerShot;
	newWeapon.myCoolDownTime = aWeapon.myCoolDownTime;
	newWeapon.myCurrentTime = aWeapon.myCoolDownTime;
	newWeapon.myPosition = aWeapon.myPosition;
	newWeapon.myIsHoming = aWeapon.myIsHoming;
	newWeapon.mySpread = aWeapon.mySpread;
	newWeapon.myType = aWeapon.myType;
	newWeapon.myMultiplier = 1;

	newWeapon.myBulletType = ConvertToBulletEnum(aWeapon.myBulletType);

	if (newWeapon.myBulletType == eBulletType::COUNT)
	{
		std::string errorMessage = "[ShootingComponent] No bullet with name " + aWeapon.myBulletType;
		DL_ASSERT(errorMessage.c_str());
	}

	newWeapon.myID = myWeapons.Size();

	myWeapons.Add(newWeapon);
	myHasWeapon = true;

	if (myWeapons.Size() >= 3)
	{
		myEntity.GetComponent<GUIComponent>()->SetRocketValues(myWeapons[2].myCurrentTime, myWeapons[2].myCoolDownTime);
	}
	myEntity.SendNote(GUINote(myWeapons[myCurrentWeaponID].myIsHoming || HasPowerUp(ePowerUpType::HOMING), eGUINoteType::HOMING_TARGET));
}

void ShootingComponent::UpgradeWeapon(const WeaponDataType& aWeapon, int aWeaponID, bool anIsIngame)
{
	if (aWeaponID >= myWeapons.Size())
	{
		AddWeapon(aWeapon);

		if (anIsIngame == true)
		{
			if (aWeaponID == 0)
			{
				myHasShotMachinegun = false;
				myEntity.GetComponent<GUIComponent>()->ShowTutorialMessage("Use left mouse button to shoot machinegun");
			}
			else if (aWeaponID == 1)
			{
				myHasSwitchWeapon = false;
				myEntity.GetComponent<GUIComponent>()->ShowTutorialMessage("Press '1' to change to machinegun again");
			}
			else if (aWeaponID == 2)
			{
				myHasShotRocket = false;
				myEntity.GetComponent<GUIComponent>()->ShowTutorialMessage("Use right mouse button to shoot a rocket");
			}
		}

		return;
	}
	myWeapons[aWeaponID].myHomingTurnRateModifier = aWeapon.myHomingTurnRateModifier;
	myWeapons[aWeaponID].myBulletsPerShot = aWeapon.myBulletsPerShot;
	myWeapons[aWeaponID].myCoolDownTime = aWeapon.myCoolDownTime;
	myWeapons[aWeaponID].myCurrentTime = aWeapon.myCoolDownTime;
	myWeapons[aWeaponID].myPosition = aWeapon.myPosition;
	myWeapons[aWeaponID].myIsHoming = aWeapon.myIsHoming;
	myWeapons[aWeaponID].mySpread = aWeapon.mySpread;
	myWeapons[aWeaponID].myType = aWeapon.myType;
	myWeapons[aWeaponID].myMultiplier = 1;
	myWeapons[aWeaponID].myBulletType = ConvertToBulletEnum(aWeapon.myBulletType);

	if (myWeapons[aWeaponID].myBulletType == eBulletType::COUNT)
	{
		std::string errorMessage = "[ShootingComponent] No bullet with name " + aWeapon.myBulletType;
		DL_ASSERT(errorMessage.c_str());
	}

	myWeapons[aWeaponID].myID = aWeaponID;
	myEntity.SendNote(GUINote(myWeapons[myCurrentWeaponID].myIsHoming || HasPowerUp(ePowerUpType::HOMING), eGUINoteType::HOMING_TARGET));
}

void ShootingComponent::SetCurrentWeaponID(int anID)
{
	myCurrentWeaponID = anID;

	if (anID >= myWeapons.Size())
	{
		myCurrentWeaponID = myWeapons.Size() - 1;
	}
	myEntity.SendNote(GUINote(myWeapons[myCurrentWeaponID].myIsHoming || HasPowerUp(ePowerUpType::HOMING), eGUINoteType::HOMING_TARGET));
}

bool ShootingComponent::HasPowerUp(ePowerUpType aPowerUp)
{
	for (int i = 0; i < myPowerUps.Size(); ++i)
	{
		if (myPowerUps[i].myPowerUpType == aPowerUp)
		{
			return true;
		}
	}

	return false;
}

void ShootingComponent::SetActivatePowerUp(ePowerUpType aType, bool aValue)
{
	switch (aType)
	{
	case ePowerUpType::FIRERATEBOOST:
		myFireRatePowerUp = aValue;
		break;
	case ePowerUpType::EMP:
		myEMPPowerUp = aValue;
		break;
	case ePowerUpType::HOMING:
		myHomingPowerUp = aValue;
		break;
	default:
		DL_ASSERT("INVALID POWERUP");
		break;
	}
}

void ShootingComponent::ActivatePowerUp(ePowerUpType aPowerUp)
{
	DL_ASSERT_EXP(aPowerUp == ePowerUpType::EMP, "Tried to activate anything other than EMP!!!");
	for (int i = myPowerUps.Size() - 1; i >= 0; --i)
	{
		if (myPowerUps[i].myPowerUpType == aPowerUp)
		{
			//MAKE AWESOME THINGY
			PostMaster::GetInstance()->SendMessage(EMPMessage(myEntity.myOrientation.GetPos(), myEMPTime));
			//SKICKA MEDDELANDE TILL SCNENEN OM ATT DEN FÅR RENDERA EMP
			PostMaster::GetInstance()->SendMessage(PowerUpMessage(aPowerUp, myEntity.myOrientation.GetPos()
				, myPowerUps[i].myPowerUpValue, myEMPPowerUpDuration));
			myEMPPowerUp = false;
			myPowerUps.RemoveCyclicAtIndex(i);
			return;
		}
	}
}

void ShootingComponent::AddDuration(ePowerUpType aPowerUp, float aTime)
{
	switch (aPowerUp)
	{
	case ePowerUpType::FIRERATEBOOST:
		myFireRatePowerUpDuration = aTime;
		break;
	case ePowerUpType::EMP:
		myEMPPowerUpDuration = aTime;
		break;
	case ePowerUpType::HOMING:
		myHomingPowerUpDuration = aTime;
		break;
	default:
		DL_ASSERT("INVALID POWERUP");
		break;
	}
}

void ShootingComponent::Reset()
{
	myPowerUps.RemoveAll();
	myHomingTarget = nullptr;
}
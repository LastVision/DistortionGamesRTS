#include "stdafx.h"

#include "ActorComponentData.h"
#include "AnimationComponentData.h"
#include "BuildingComponentData.h"
#include "CollisionComponentData.h"
#include <CommonHelper.h>
#include "ComponentLoader.h"
#include "ControllerComponentData.h"
#include "EntityEnumConverter.h"
#include "GraphicsComponentData.h"
#include "HealthComponentData.h"
#include "TriggerComponentData.h"

void ComponentLoader::LoadActorComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, ActorComponentData& aOutputData)
{
	aOutputData.myExistsInEntity = true;

	for (tinyxml2::XMLElement* e = aDocument.FindFirstChild(aSourceElement); e != nullptr; e = aDocument.FindNextElement(e))
	{
		std::string elementName = CU::ToLower(e->Name());
		if (elementName == CU::ToLower("Speed"))
		{
			aDocument.ForceReadAttribute(e, "value", aOutputData.myMoveSpeed);
		}
		else
		{
			FailedToReadChildElementMessage(e->Name(), aSourceElement->Name());
		}
	}
}

void ComponentLoader::LoadAnimationComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, AnimationComponentData& aOutputData)
{
	aOutputData.myExistsInEntity = true;

	for (tinyxml2::XMLElement* e = aDocument.FindFirstChild(aSourceElement); e != nullptr; e = aDocument.FindNextElement(e))
	{
		std::string elementName = CU::ToLower(e->Name());
		if (elementName == CU::ToLower("Model"))
		{
			std::string modelPath;
			std::string effectPath;
			aDocument.ForceReadAttribute(e, "modelPath", modelPath);
			aDocument.ForceReadAttribute(e, "shaderPath", effectPath);
			aOutputData.myModelPath = modelPath.c_str();
			aOutputData.myEffectPath = effectPath.c_str();
		}
		else if (elementName == CU::ToLower("Animation"))
		{
			AnimationLoadData newAnimation;
			std::string animPath;
			int animState;

			aDocument.ForceReadAttribute(e, "path", animPath);
			aDocument.ForceReadAttribute(e, "state", animState);
			aDocument.ForceReadAttribute(e, "loop", newAnimation.myLoopFlag);
			aDocument.ForceReadAttribute(e, "resetTime", newAnimation.myResetTimeOnRestart);
			newAnimation.myAnimationPath = animPath.c_str();
			newAnimation.myEntityState = static_cast<eEntityState>(animState);

			aOutputData.myAnimations.Insert(static_cast<int>(newAnimation.myEntityState), newAnimation);
		}
		else
		{
			FailedToReadChildElementMessage(e->Name(), aSourceElement->Name());
		}
	}
}

void ComponentLoader::LoadBuidlingComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, BuildingCompnentData& aOutputData)
{
	aOutputData.myExistsInEntity = true;

	aOutputData.myBuildUnitTypes[0] = eUnitType::NOT_A_UNIT;
	aOutputData.myBuildUnitTypes[1] = eUnitType::NOT_A_UNIT;
	aOutputData.myBuildUnitTypes[2] = eUnitType::NOT_A_UNIT;

	int numberOfUnitType = 0;
	for (tinyxml2::XMLElement* e = aDocument.FindFirstChild(aSourceElement); e != nullptr; e = aDocument.FindNextElement(e))
	{
		std::string elementName = CU::ToLower(e->Name());
		if (elementName == CU::ToLower("Unit"))
		{
			DL_ASSERT_EXP(numberOfUnitType <= 3, "You can't assign more than three unit types in a building.");
			std::string unitType;
			aDocument.ForceReadAttribute(e, "type", unitType);

			eUnitType buildUnitType = EntityEnumConverter::ConvertStringToUnitType(CU::ToLower(unitType));
			aOutputData.myBuildUnitTypes.Insert(numberOfUnitType, buildUnitType);
			numberOfUnitType++;
		}
		else 
		{
			FailedToReadChildElementMessage(e->Name(), aSourceElement->Name());
		}
	}
}

void ComponentLoader::LoadCollisionComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, CollisionComponentData& aOutputData)
{
	aOutputData.myExistsInEntity = true;

	for (tinyxml2::XMLElement* e = aDocument.FindFirstChild(aSourceElement); e != nullptr; e = aDocument.FindNextElement(e))
	{
		std::string elementName = CU::ToLower(e->Name());
		if (elementName == CU::ToLower("Radius"))
		{
			aDocument.ForceReadAttribute(e, "value", aOutputData.myRadius);
		}
		else
		{
			FailedToReadChildElementMessage(e->Name(), aSourceElement->Name());
		}
	}
}

void ComponentLoader::LoadControllerComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, ControllerComponentData& aOutputData)
{
	aOutputData.myExistsInEntity = true;

	for (tinyxml2::XMLElement* e = aDocument.FindFirstChild(aSourceElement); e != nullptr; e = aDocument.FindNextElement(e))
	{
		std::string elementName = CU::ToLower(e->Name());
		if (elementName == CU::ToLower("VisionRange"))
		{
			aDocument.ForceReadAttribute(e, "value", aOutputData.myVisionRange);
		}
		else if (elementName == CU::ToLower("AttackRange"))
		{
			aDocument.ForceReadAttribute(e, "value", aOutputData.myAttackRange);
		}
		else
		{
			FailedToReadChildElementMessage(e->Name(), aSourceElement->Name());
		}
	}
}

void ComponentLoader::LoadGraphicsComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, GraphicsComponentData& aOutputData)
{
	aOutputData.myExistsInEntity = true;

	for (tinyxml2::XMLElement* e = aDocument.FindFirstChild(aSourceElement); e != nullptr; e = aDocument.FindNextElement(e))
	{
		std::string elementName = CU::ToLower(e->Name());
		if (elementName == CU::ToLower("Model"))
		{
			std::string modelPath;
			std::string effectPath;
			aDocument.ForceReadAttribute(e, "modelPath", modelPath);
			aDocument.ForceReadAttribute(e, "shaderPath", effectPath);
			aOutputData.myModelPath = modelPath.c_str();
			aOutputData.myEffectPath = effectPath.c_str();
		}
		else
		{
			FailedToReadChildElementMessage(e->Name(), aSourceElement->Name());
		}
	}
}

void ComponentLoader::LoadHealthComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, HealthComponentData& aOutputData)
{
	aOutputData.myExistsInEntity = true;

	for (tinyxml2::XMLElement* e = aDocument.FindFirstChild(aSourceElement); e != nullptr; e = aDocument.FindNextElement(e))
	{
		std::string elementName = CU::ToLower(e->Name());
		if (elementName == CU::ToLower("Health"))
		{
			aDocument.ForceReadAttribute(e, "value", aOutputData.myHealth);
		}
		else
		{
			FailedToReadChildElementMessage(e->Name(), aSourceElement->Name());
		}
	}
}

void ComponentLoader::LoadTriggerComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, TriggerComponentData& aOutputData)
{
	aOutputData.myExistsInEntity = true;

	for (tinyxml2::XMLElement* e = aDocument.FindFirstChild(aSourceElement); e != nullptr; e = aDocument.FindNextElement(e))
	{
		std::string elementName = CU::ToLower(e->Name());
		if (elementName == CU::ToLower("Radius"))
		{
			aDocument.ForceReadAttribute(e, "value", aOutputData.myRadius);
		}
		else if (elementName == "type")
		{
			std::string typeString;
			aDocument.ForceReadAttribute(e, "value", typeString);
			typeString = CU::ToLower(typeString);
			if (typeString == "resource")
			{
				aOutputData.myType = eTriggerType::RESOURCE;
			}
			else if (typeString == "victory")
			{
				aOutputData.myType = eTriggerType::VICTORY;
			}
			else
			{
				DL_ASSERT("Unknown trigger type.");
			}
		}
		else
		{
			FailedToReadChildElementMessage(e->Name(), aSourceElement->Name());
		}
	}
}

void ComponentLoader::FailedToReadChildElementMessage(const std::string& aElement, const std::string& aParent)
{
	std::string errorMessage = "The element " + aElement + "in " + aParent +
		"is not supported. Please check so you wrote right else tell a programmer.";
	DL_ASSERT(errorMessage.c_str());
}

#include "stdafx.h"

#include "ActorComponentData.h"
#include "AnimationComponentData.h"
#include <CommonHelper.h>
#include "ComponentLoader.h"
#include "ControllerComponentData.h"
#include "CollisionComponentData.h"
#include "GraphicsComponentData.h"
#include <XMLReader.h>

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


void ComponentLoader::FailedToReadChildElementMessage(const std::string& aElement, const std::string& aParent)
{
	std::string errorMessage = "The element " + aElement + "in " + aParent +
		"is not supported. Please check so you wrote right else tell a programmer.";
	DL_ASSERT(errorMessage.c_str());
}

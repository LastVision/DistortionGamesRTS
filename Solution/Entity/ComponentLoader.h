#pragma once
#include "EntityEnum.h"

struct ActorComponentData;
struct AnimationComponentData;
struct BuildingCompnentData;
struct CollisionComponentData;
struct ControllerComponentData;
struct GraphicsComponentData;
struct HealthComponentData;
struct TriggerComponentData;

class XMLReader;

namespace tinyxml2
{
	class XMLElement;
}

class ComponentLoader
{
public:
	void LoadActorComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, ActorComponentData& aOutputData);
	void LoadAnimationComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, AnimationComponentData& aOutputData);
	void LoadBuidlingComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, BuildingCompnentData& aOutputData);
	void LoadCollisionComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, CollisionComponentData& aOutputData);
	void LoadControllerComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, ControllerComponentData& aOutputData);
	void LoadGraphicsComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, GraphicsComponentData& aOutputData);
	void LoadHealthComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, HealthComponentData& aOutputData);
	void LoadTriggerComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, TriggerComponentData& aOutputData);

	static const eEntityType ConvertStringToEntityType(const std::string& entityType);
private:
	void FailedToReadChildElementMessage(const std::string& aElement, const std::string& aParent);
};
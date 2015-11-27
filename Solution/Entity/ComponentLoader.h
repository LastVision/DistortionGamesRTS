#pragma once

struct AnimationComponentData;
struct CollisionComponentData;
struct GraphicsComponentData;
struct MovementComponentData;

class XMLReader;

namespace tinyxml2
{
	class XMLElement;
}

class ComponentLoader
{
public:
	void LoadAnimationComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, AnimationComponentData& aOutputData);
	void LoadGraphicsComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, GraphicsComponentData& aOutputData);
	void LoadMovementComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, MovementComponentData& aOutputData);
	void LoadCollisionComponent(XMLReader& aDocument, tinyxml2::XMLElement* aSourceElement, CollisionComponentData& aOutputData);
private:
	void FailedToReadChildElementMessage(const std::string& aElement, const std::string& aParent);
};
#pragma once

namespace Prism
{
	class Instance;
}

struct SelectionComponentData;

class SelectionComponent : public Component
{
public:
	SelectionComponent(Entity& aEntity, SelectionComponentData& aComponentData);
	~SelectionComponent();

	void Update(float aDeltaTime) override;

	static eComponentType GetTypeStatic();
	eComponentType GetType() override;

	Prism::Instance* GetInstance();

	bool GetIsRemovedFromScene() const;
	void SetIsRemovedFromScene(bool aIsRemoved);

private:
	Prism::Instance* myInstance;
	bool myPreviousHover;
	float myCullingRadius;
	bool myIsRemovedFromScene;
};

inline eComponentType SelectionComponent::GetTypeStatic()
{
	return eComponentType::SELECTION;
}

inline eComponentType SelectionComponent::GetType()
{
	return GetTypeStatic();
}

inline Prism::Instance* SelectionComponent::GetInstance()
{
	return myInstance;
}

inline bool SelectionComponent::GetIsRemovedFromScene() const
{
	return myIsRemovedFromScene;
}

inline void SelectionComponent::SetIsRemovedFromScene(bool aIsRemoved)
{
	myIsRemovedFromScene = aIsRemoved;
}

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

private:
	Prism::Instance* myHover;
};

inline eComponentType SelectionComponent::GetTypeStatic()
{
	return eComponentType::SELECTION;
}

inline eComponentType SelectionComponent::GetType()
{
	return GetTypeStatic();
}

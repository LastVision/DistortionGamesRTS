#pragma once

class Entity;
class PollingStation
{
public:
	static PollingStation* GetInstance();
	static void Destroy();

	void RegisterEntity(Entity* aEntity);

	Entity* FindClosestEntity(const CU::Vector3<float>& aPosition, int aEntityOwner, float aMaxDistance2 = FLT_MAX);
	Entity* FindEntityAtPosition(const CU::Vector3<float>& aPosition, int aEntityOwner);
	void FindAllEntitiesCloseToEntity(const Entity* anEntity, float aRadius, CU::GrowingArray<Entity*>& someEntitiesOut);
	
	const CU::GrowingArray<Entity*>& GetUnits(eOwnerType anOwner) const;
	const CU::GrowingArray<Entity*>& GetResourcePoints() const;
	const CU::GrowingArray<Entity*>& GetVictoryPoints() const;

	const Entity* GetBase(eOwnerType anOwner) const;

	void CleanUp();



private:
	PollingStation();
	~PollingStation();

	void FindClosestEntity(const CU::Vector3<float>& aPosition, const CU::GrowingArray<Entity*>& someEntities
		,float aMaxDistance, float& aBestDistance, Entity** aOutEntity);
	void FindAllEntitiesCloseToEntity(const CU::GrowingArray<Entity*>& someEntitiesIn, const Entity* anEntity, float aRadius
		, CU::GrowingArray<Entity*>& someEntitiesOut);

	CU::GrowingArray<Entity*> myPlayerUnits;
	CU::GrowingArray<Entity*> myAIUnits;
	CU::GrowingArray<Entity*> myNeutralUnits;
	CU::GrowingArray<Entity*> myResourcePoints;
	CU::GrowingArray<Entity*> myVictoryPoints;

	Entity* myPlayerBuilding;
	Entity* myAIBuilding;

	static PollingStation* myInstance;
};


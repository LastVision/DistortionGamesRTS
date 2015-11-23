#pragma once

struct ShootNote
{
	ShootNote(const CU::Vector3<float>& aVelocity, const CU::Vector3<float>& aDirection
		, const CU::Vector2<float>& anEnititySteering = { 0.f, 0.f }, bool anIsRocket = false);

	const CU::Vector3<float> myEnitityVelocity;
	const CU::Vector2<float> myEnititySteering;
	const CU::Vector3<float> myDirection;
	const bool myIsRocket;

private:
	void operator=(ShootNote&) = delete;
};

inline ShootNote::ShootNote(const CU::Vector3<float>& aVelocity, const CU::Vector3<float>& aDirection
	, const CU::Vector2<float>& anEnititySteering, bool anIsRocket)
	: myEnitityVelocity(aVelocity)
	, myEnititySteering(anEnititySteering)
	, myDirection(aDirection)
	, myIsRocket(anIsRocket)
{
}
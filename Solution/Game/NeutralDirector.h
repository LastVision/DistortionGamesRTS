#pragma once
#include "Director.h"
class NeutralDirector : public Director
{
public:
	NeutralDirector(eOwnerType aOwnerType, const Prism::Terrain& aTerrain);
	~NeutralDirector();
};


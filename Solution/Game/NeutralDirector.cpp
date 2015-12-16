#include "stdafx.h"
#include "NeutralDirector.h"


NeutralDirector::NeutralDirector(eOwnerType aOwnerType, const Prism::Terrain& aTerrain)
	: Director(aOwnerType, aTerrain)
{
}


NeutralDirector::~NeutralDirector()
{
}

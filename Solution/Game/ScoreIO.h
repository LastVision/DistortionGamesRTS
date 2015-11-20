#pragma once
#include "LevelScore.h"

class ScoreIO
{
public:
	static SaveScore Load(int aLevel);
	static void Save(const SaveScore& aScore, int aLevel);
};


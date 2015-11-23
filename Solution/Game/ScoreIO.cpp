#include "stdafx.h"

#include <CommonHelper.h>
#include <fstream>
#include "ScoreIO.h"


SaveScore ScoreIO::Load(int aLevel)
{
	SaveScore score;
	
	std::string path(CU::GetMyDocumentFolderPath());
	path += "Raven\\gfx.dist";
	path += std::to_string(aLevel);
	std::ifstream stream(path.c_str(), std::ios::binary);
	if (stream.fail() == true)
	{
		//std::string error = "Could not open save file";
		//DL_ASSERT(error.c_str());
		
		return score;
	}
	char my_4_ByteBuffer[4];
	stream.read(my_4_ByteBuffer, 4);
	score.myStars = *(reinterpret_cast<int*>(my_4_ByteBuffer));
	stream.read(my_4_ByteBuffer, 4);
	score.myCompletedOptional = *(reinterpret_cast<int*>(my_4_ByteBuffer));
	stream.read(my_4_ByteBuffer, 4);
	score.myTotalOptional = *(reinterpret_cast<int*>(my_4_ByteBuffer));
	stream.read(my_4_ByteBuffer, 4);
	score.myDifficulty = *(reinterpret_cast<int*>(my_4_ByteBuffer));

	return score;
}

void ScoreIO::Save(const SaveScore& aScore, int aLevel)
{
	SaveScore scoreBefore = Load(aLevel);
	
	if (scoreBefore.myDifficulty < aScore.myDifficulty && aScore.myStars > 0)
	{
		std::string path(CU::GetMyDocumentFolderPath());
		path += "Raven\\gfx.dist";
		path += std::to_string(aLevel);
		std::ofstream file(path.c_str(), std::ofstream::binary);

		file.write((char*)&aScore.myStars, sizeof(int));
		file.write((char*)&aScore.myCompletedOptional, sizeof(int));
		file.write((char*)&aScore.myTotalOptional, sizeof(int));
		file.write((char*)&aScore.myDifficulty, sizeof(int));
		file.close();
	}
	else if (scoreBefore.myDifficulty == aScore.myDifficulty)
	{
		if (scoreBefore.myStars < aScore.myStars)
		{
			std::string path(CU::GetMyDocumentFolderPath());
			path += "Raven\\gfx.dist";
			path += std::to_string(aLevel);
			std::ofstream file(path.c_str(), std::ofstream::binary);

			file.write((char*)&aScore.myStars, sizeof(int));
			file.write((char*)&aScore.myCompletedOptional, sizeof(int));
			file.write((char*)&aScore.myTotalOptional, sizeof(int));
			file.write((char*)&aScore.myDifficulty, sizeof(int));
			file.close();
		}
	}
	else
	{
		//Do nothing
	}
}
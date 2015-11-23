#pragma once
#include "Enums.h"

namespace CU
{
	class InputWrapper;
}

namespace Prism
{
	class DirectionalLight;
	class PointLight;
	class SpotLight;
	class Sprite;
};

class Level;
class Entity;

struct Difficult
{
	eDifficult myType;
	float myMultiplier;
	float myHealthMultiplier;
};

class LevelFactory
{
public:
	LevelFactory(const std::string& aLevelListPath, CU::InputWrapper* anInputWrapper);
	~LevelFactory();

	Level* LoadLevel(const int& anID, const int &aDifficultID);
	Level* LoadCurrentLevel();
	Level* LoadNextLevel();
	
	bool IsLastLevel() const;
	void LoadLevelListFromXML(const std::string& aXMLPath); 
	void ReadXML(const std::string& aFilePath);
	void ReadLevelSettings();
	void LoadPlayer();

	const volatile bool& IsLevelLoading() const;
	const int& GetLevelID() const;

	bool IsClean();

	void Cleanup();

private:
	void FindTextures(const char *aDirName);
	void LoadTexture(const std::string& aPath);
	void LoadLights(XMLReader& aReader, tinyxml2::XMLElement* aLevelElement);
	void LoadDirectionalLights(XMLReader& aReader, tinyxml2::XMLElement* aLevelElement);
	void LoadProps(XMLReader& aReader, tinyxml2::XMLElement* aLevelElement);
	void LoadDefendables(XMLReader& aReader, tinyxml2::XMLElement* aLevelElement, float aDifficultScale);
	void LoadStructures(XMLReader& aReader, tinyxml2::XMLElement* aLevelElement);
	void LoadTriggers(XMLReader& aReader, tinyxml2::XMLElement* aLevelElement);
	void LoadPowerups(XMLReader& aReader, tinyxml2::XMLElement* aLevelElement);
	void LoadDifficults();
	void FillDataPropOrDefendable(XMLReader& aReader, tinyxml2::XMLElement* aLevelElement, Entity* aEntityToCreate);

	void AddToScene();

	void SetSkySphere(const std::string& aModelFilePath, const std::string& aEffectFileName);

	CU::InputWrapper* myInputWrapper;
	Level* myCurrentLevel;
	Level* myOldLevel;
	CU::Vector4f myAmbientHue;
	CU::GrowingArray<Prism::DirectionalLight*> myDirectionalLights;
	CU::GrowingArray<Prism::PointLight*> myPointLights;
	CU::GrowingArray<Prism::SpotLight*> mySpotLights;

	CU::StaticArray<Difficult, static_cast<int>(eDifficult::_COUNT)> myDifficults;

	std::unordered_map<int, std::string> myLevelPaths;

	int myCurrentID;
	int myCurrentDifficultyID;

	volatile bool myIsLoading;

	std::thread* myLoadLevelThread;
};

inline const volatile bool& LevelFactory::IsLevelLoading() const
{
	return myIsLoading;
}

inline const int& LevelFactory::GetLevelID() const
{
	return myCurrentID;
}

inline bool LevelFactory::IsClean()
{
	return myLoadLevelThread == nullptr;
}
#pragma once
#include <Vector.h>
#include <GrowingArray.h>

class Level;
class XMLReader;

namespace Prism
{
	class DirectionalLight;
	class PointLight;
	class SpotLight;
	class Camera;
}

namespace tinyxml2
{
	class XMLElement;
}

class LevelFactory
{
public:
	LevelFactory(const std::string& aLevelListPath, Prism::Camera& aCamera);
	~LevelFactory();

	Level* LoadLevel(const int& aID);
	Level* LoadCurrentLevel();
	Level* LoadNextLevel();

	const volatile bool& IsLevelLoading() const;
	const int& GetLevelID() const;

	bool IsLastLevel() const;

	bool IsClean();
	void Cleanup();
private:
	void ReadLevelList(const std::string& aLevelListPath);
	void ReadLevel(const std::string& aLevelPath);
	void ReadLevelSetting(const std::string& aLevelPath);

	void FindTextures(const char* aDirName);
	void LoadTexture(const std::string& aPath);
	void LoadLights(XMLReader& aReader, tinyxml2::XMLElement* aLevelElement);
	void LoadDirectionalLights(XMLReader& aReader, tinyxml2::XMLElement* aLevelElement);
	void LoadProps(XMLReader& aReader, tinyxml2::XMLElement* aLevelElement);
	void LoadTerrain(XMLReader& aReader, tinyxml2::XMLElement* aLevelElement);

	Level* myCurrentLevel;
	Level* myOldLevel;

	Prism::Camera& myCamera;

	CU::Vector4f myAmbientHue;
	CU::GrowingArray<Prism::DirectionalLight*> myDirectionalLights;
	CU::GrowingArray<Prism::PointLight*> myPointLights;
	CU::GrowingArray<Prism::SpotLight*> mySpotLights;

	std::unordered_map<int, std::string> myLevelPaths;

	int myCurrentID;

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
#include "stdafx.h"

#include "AIDirector.h"
#include <CommonHelper.h>
#include <DirectionalLight.h>
#include "dirent.h"
#include <Effect.h>
#include <EffectContainer.h>
#include <EngineEnums.h>
#include <EntityFactory.h>
#include "Level.h"
#include "LevelFactory.h"
#include <MathHelper.h>
#include <ModelLoader.h>
#include <NavMesh.h>
#include "PlayerDirector.h"
#include <PointLight.h>
#include <SpotLight.h>
#include <Terrain.h>
#include <TextureContainer.h>
#include <Scene.h>
#include <XMLReader.h>

LevelFactory::LevelFactory(const std::string& aLevelListPath, Prism::Camera& aCamera, GUI::Cursor* aCursor)
	: myCurrentLevel(nullptr)
	, myCurrentID(0)
	, myOldLevel(nullptr)
	, myIsLoading(false)
	, myLoadLevelThread(nullptr)
	, myDirectionalLights(4)
	, mySpotLights(4)
	, myPointLights(4)
	, myLevelPaths(2)
	, myCamera(aCamera)
	, myCursor(aCursor)
{
	ReadLevelList(aLevelListPath);
}

LevelFactory::~LevelFactory()
{
	SAFE_DELETE(myLoadLevelThread);
	myDirectionalLights.DeleteAll();
}

Level* LevelFactory::LoadLevel(const int& aID)
{
	if (myLevelPaths.find(aID) == myLevelPaths.end())
	{
		DL_ASSERT("[LevelFactory] Non-existing ID in LoadLevel! ID must correspond with levelList.xml");
	}
	myCurrentID = aID;

	return LoadCurrentLevel();
}

Level* LevelFactory::LoadCurrentLevel()
{
	myIsLoading = true;
	delete myCurrentLevel;
	myCurrentLevel = nullptr;

	LoadTerrain(myLevelPaths[myCurrentID]);
	myCurrentLevel = new Level(myCamera, myTerrain, myCursor);

	SAFE_DELETE(myLoadLevelThread);
	//myLoadLevelThread = new std::thread(&LevelFactory::ReadLevel, this, myLevelPaths[myCurrentID]);
	ReadLevel(myLevelPaths[myCurrentID]);

	return myCurrentLevel;
}

Level* LevelFactory::LoadNextLevel()
{
	if (IsLastLevel() == true)
	{
		return myCurrentLevel;
	}

	return LoadLevel(myCurrentID + 1);
}

bool LevelFactory::IsLastLevel() const
{
	if (myLevelPaths.find(myCurrentID + 1) == myLevelPaths.end())
	{
		return true;
	}
	return false;
}

void LevelFactory::Cleanup()
{
	myLoadLevelThread->join();
	SAFE_DELETE(myLoadLevelThread);
}

void LevelFactory::ReadLevelList(const std::string& aLevelListPath)
{
	myLevelPaths.clear();
	XMLReader reader;
	reader.OpenDocument(aLevelListPath);
	std::string levelPath = "";
	int ID = 0;
	int lastID = ID - 1;

	tinyxml2::XMLElement* rootElement = reader.ForceFindFirstChild("root");
	for (tinyxml2::XMLElement* levelElement = reader.FindFirstChild(rootElement); levelElement != nullptr; levelElement = reader.FindNextElement(levelElement))
	{
		lastID = ID;

		reader.ForceReadAttribute(levelElement, "ID", ID);
		reader.ForceReadAttribute(levelElement, "path", levelPath);
		myLevelPaths[ID] = levelPath;

		if (ID - 1 != lastID)
		{
			DL_ASSERT("[LevelFactory] Wrong ID-number in levelList.xml! The numbers should be counting up, in order.");
		}
		if (myCurrentID >= 10)
		{
			DL_ASSERT("[LevelFactory] Can't handle level ID with two digits.");
		}
	}
	reader.CloseDocument();
}

void LevelFactory::ReadLevel(const std::string& aLevelPath)
{
	Prism::ModelLoader* modelLoader = Prism::ModelLoader::GetInstance();
	Prism::EffectContainer* effectContainer = Prism::EffectContainer::GetInstance();
	modelLoader->Pause();
	Prism::Engine::GetInstance()->myIsLoading = true;

	//FindTextures("Data/Resource/Texture/Particle/");

	myDirectionalLights.DeleteAll();
	myPointLights.DeleteAll();
	mySpotLights.DeleteAll();

	ReadLevelSetting(aLevelPath);

	Sleep(10);
	XMLReader reader;
	reader.OpenDocument(aLevelPath);
	tinyxml2::XMLElement* levelElement = reader.ForceFindFirstChild("root");
	levelElement = reader.ForceFindFirstChild(levelElement, "scene");

	std::string cubeMap;
	reader.ReadAttribute(reader.ForceFindFirstChild(levelElement, "cubemap"), "source", cubeMap);
	effectContainer->SetCubeMap(cubeMap);

	LoadLights(reader, levelElement);
	LoadBases(reader, levelElement);
	LoadProps(reader, levelElement);
	reader.CloseDocument();

	modelLoader->UnPause();
	modelLoader->WaitUntilFinished();

	effectContainer->GetEffect("Data/Resource/Shader/S_effect_pbl.fx")->SetAmbientHue(myAmbientHue);

	myTerrain->CreateNavMesh();
	//for (int i = 0; i < myCurrentLevel->myEntities.Size(); ++i)
	//{
	//	myTerrain->GetNavMesh()->Cut(myCurrentLevel->myEntities[i]->GetCutMesh());
	//}
	myTerrain->CreatePathFinder();

	Prism::Engine::GetInstance()->myIsLoading = false;

	myIsLoading = false;
}

void LevelFactory::ReadLevelSetting(const std::string& aLevelPath)
{
	XMLReader reader;
	int dotPos = aLevelPath.find_first_of(".");
	std::string settingPath = aLevelPath.substr(0, dotPos) + "_settings.xml";
	reader.OpenDocument(settingPath);
	tinyxml2::XMLElement* rootElement = reader.ForceFindFirstChild("root");
	tinyxml2::XMLElement* ambientElement = reader.ForceFindFirstChild(rootElement, "Ambient");
	myAmbientHue = { 0.f, 0.f, 0.f, 0.f };
	reader.ForceReadAttribute(ambientElement, "r", myAmbientHue.x);
	reader.ForceReadAttribute(ambientElement, "g", myAmbientHue.y);
	reader.ForceReadAttribute(ambientElement, "b", myAmbientHue.z);

	myAmbientHue.x = myAmbientHue.x / 255.f;
	myAmbientHue.y = myAmbientHue.y / 255.f;
	myAmbientHue.z = myAmbientHue.z / 255.f;

	reader.CloseDocument();
}

void LevelFactory::FindTextures(const char* aDirName)
{
	DIR *dir;
	char buffer[PATH_MAX + 2];
	char *p = buffer;
	const char *src;
	char *end = &buffer[PATH_MAX];
	int ok;

	/* Copy directory name to buffer */
	src = aDirName;
	while (p < end  &&  *src != '\0') {
		*p++ = *src++;
	}
	*p = '\0';

	/* Open directory stream */
	dir = opendir(aDirName);
	if (dir != NULL) {
		struct dirent *ent;

		/* Print all files and directories within the directory */
		while ((ent = readdir(dir)) != NULL) {
			char *q = p;
			char c;

			/* Get final character of directory name */
			if (buffer < q) {
				c = q[-1];
			}
			else {
				c = ':';
			}

			/* Append directory separator if not already there */
			if (c != ':'  &&  c != '/'  &&  c != '\\') {
				*q++ = '/';
			}

			/* Append file name */
			src = ent->d_name;
			while (q < end  &&  *src != '\0') {
				*q++ = *src++;
			}
			*q = '\0';

			/* Decide what to do with the directory entry */
			switch (ent->d_type) {
			case DT_LNK:
			case DT_REG:
				/* Output file name with directory */
				//printf("%s\n", buffer);
				LoadTexture(buffer);
				break;

			case DT_DIR:
				/* Scan sub-directory recursively */
				if (strcmp(ent->d_name, ".") != 0
					&& strcmp(ent->d_name, "..") != 0) {
					FindTextures(buffer);
				}
				break;

			default:
				/* Ignore device entries */
				/*NOP*/;
			}

		}

		closedir(dir);
		ok = 1;

	}
	else {
		DL_ASSERT(CU::Concatenate("Cannot open directory %s", aDirName));
	}
}

void LevelFactory::LoadTexture(const std::string& aPath)
{
	if (aPath.compare(aPath.size() - 4, 4, ".dds") == 0)
	{
		Prism::TextureContainer::GetInstance()->GetTexture(aPath);
	}
}

void LevelFactory::LoadLights(XMLReader& aReader, tinyxml2::XMLElement* aLevelElement)
{
	LoadDirectionalLights(aReader, aLevelElement);
}

void LevelFactory::LoadDirectionalLights(XMLReader& aReader, tinyxml2::XMLElement* aLevelElement)
{
	for (tinyxml2::XMLElement* entityElement = aReader.FindFirstChild(aLevelElement, "directionallight"); entityElement != nullptr;
		entityElement = aReader.FindNextElement(entityElement, "directionallight"))
	{
		tinyxml2::XMLElement* directionalElement = aReader.ForceFindFirstChild(entityElement, "rotation");

		Prism::DirectionalLight* newDirLight = new Prism::DirectionalLight();

		CU::Vector3<float> lightDirection;
		aReader.ForceReadAttribute(directionalElement, "X", lightDirection.x);
		aReader.ForceReadAttribute(directionalElement, "Y", lightDirection.y);
		aReader.ForceReadAttribute(directionalElement, "Z", lightDirection.z);

		CU::Matrix44<float> orientation;

		CU::GetOrientation(orientation, lightDirection);
		CU::Vector3<float> direction(0.f, 0.f, 1.f);

		direction = direction * orientation;

		newDirLight->SetDir(direction);

		directionalElement = aReader.ForceFindFirstChild(entityElement, "color");

		CU::Vector4<float> lightColor;
		aReader.ForceReadAttribute(directionalElement, "R", lightColor.x);
		aReader.ForceReadAttribute(directionalElement, "G", lightColor.y);
		aReader.ForceReadAttribute(directionalElement, "B", lightColor.z);
		aReader.ForceReadAttribute(directionalElement, "A", lightColor.w);
		newDirLight->SetColor(lightColor);

		myDirectionalLights.Add(newDirLight);
	}

	for (int i = 0; i < myDirectionalLights.Size(); ++i)
	{
		myCurrentLevel->myScene->AddLight(myDirectionalLights[i]);
	}
}

void LevelFactory::LoadProps(XMLReader& aReader, tinyxml2::XMLElement* aLevelElement)
{
	for (tinyxml2::XMLElement* entityElement = aReader.FindFirstChild(aLevelElement, "prop"); entityElement != nullptr;
		entityElement = aReader.FindNextElement(entityElement, "prop"))
	{
		std::string propType;
		aReader.ForceReadAttribute(entityElement, "propType", propType);
		propType = CU::ToLower(propType);
		tinyxml2::XMLElement* propElement = aReader.ForceFindFirstChild(entityElement, "position");
		CU::Vector3<float> propPosition;
		aReader.ForceReadAttribute(propElement, "X", propPosition.x);
		aReader.ForceReadAttribute(propElement, "Y", propPosition.y);
		aReader.ForceReadAttribute(propElement, "Z", propPosition.z);

		propElement = aReader.ForceFindFirstChild(entityElement, "rotation");
		CU::Vector3<float> propRotation;
		aReader.ForceReadAttribute(propElement, "X", propRotation.x);
		aReader.ForceReadAttribute(propElement, "Y", propRotation.y);
		aReader.ForceReadAttribute(propElement, "Z", propRotation.z);

		propElement = aReader.ForceFindFirstChild(entityElement, "scale");
		CU::Vector3<float> propScale;
		aReader.ForceReadAttribute(propElement, "X", propScale.x);
		aReader.ForceReadAttribute(propElement, "Y", propScale.y);
		aReader.ForceReadAttribute(propElement, "Z", propScale.z);

		propRotation.x = CU::Math::DegreeToRad(propRotation.x);
		propRotation.y = CU::Math::DegreeToRad(propRotation.y);
		propRotation.z = CU::Math::DegreeToRad(propRotation.z);

		myCurrentLevel->myEntities.Add(EntityFactory::CreateEntity(eOwnerType::NEUTRAL, EntityFactory::ConvertStringToEntityType(propType),
			Prism::eOctreeType::STATIC, *myCurrentLevel->myScene, propPosition, *myCurrentLevel->myTerrain, propRotation, propScale));
		myCurrentLevel->myEntities.GetLast()->AddToScene();
		myCurrentLevel->myEntities.GetLast()->Reset();
	}
}

void LevelFactory::LoadBases(XMLReader& aReader, tinyxml2::XMLElement* aLevelElement)
{
	int enemyBase = 0;
	int playerBase = 0;
	for (tinyxml2::XMLElement* baseElement = aReader.FindFirstChild(aLevelElement); baseElement != nullptr;
		baseElement = aReader.FindNextElement(baseElement))
	{
		std::string elementName = CU::ToLower(baseElement->Name());
		if (elementName == "enemybase" || elementName == "playerbase")
		{
			std::string baseType;
			aReader.ForceReadAttribute(baseElement, "baseType", baseType);
			baseType = CU::ToLower(baseType);

			tinyxml2::XMLElement* propElement = aReader.ForceFindFirstChild(baseElement, "position");
			CU::Vector3<float> propPosition;
			aReader.ForceReadAttribute(propElement, "X", propPosition.x);
			aReader.ForceReadAttribute(propElement, "Y", propPosition.y);
			aReader.ForceReadAttribute(propElement, "Z", propPosition.z);

			propElement = aReader.ForceFindFirstChild(baseElement, "rotation");
			CU::Vector3<float> propRotation;
			aReader.ForceReadAttribute(propElement, "X", propRotation.x);
			aReader.ForceReadAttribute(propElement, "Y", propRotation.y);
			aReader.ForceReadAttribute(propElement, "Z", propRotation.z);

			propElement = aReader.ForceFindFirstChild(baseElement, "scale");
			CU::Vector3<float> propScale;
			aReader.ForceReadAttribute(propElement, "X", propScale.x);
			aReader.ForceReadAttribute(propElement, "Y", propScale.y);
			aReader.ForceReadAttribute(propElement, "Z", propScale.z);

			propRotation.x = CU::Math::DegreeToRad(propRotation.x);
			propRotation.y = CU::Math::DegreeToRad(propRotation.y);
			propRotation.z = CU::Math::DegreeToRad(propRotation.z);

			if (elementName == "enemybase")
			{
				myCurrentLevel->myAI->myBuilding = EntityFactory::CreateEntity(eOwnerType::ENEMY, 
					EntityFactory::ConvertStringToEntityType(baseType), Prism::eOctreeType::STATIC, 
					*myCurrentLevel->myScene, propPosition, *myCurrentLevel->myTerrain, propRotation, propScale);
				myCurrentLevel->myAI->myBuilding->AddToScene();
				myCurrentLevel->myAI->myBuilding->Reset();
				enemyBase++;
			}
			else if (elementName == "playerbase")
			{
				myCurrentLevel->myPlayer->myBuilding = EntityFactory::CreateEntity(eOwnerType::PLAYER,
					EntityFactory::ConvertStringToEntityType(baseType), Prism::eOctreeType::STATIC,
					*myCurrentLevel->myScene, propPosition, *myCurrentLevel->myTerrain, propRotation, propScale);
				myCurrentLevel->myPlayer->myBuilding->AddToScene();
				myCurrentLevel->myPlayer->myBuilding->Reset();
				playerBase++;
			}
		}
	}
	DL_ASSERT_EXP(enemyBase <= 1, "Enemy can't have more than one base.");
	DL_ASSERT_EXP(enemyBase >= 1, "Enemy can't have less than one base.");
	DL_ASSERT_EXP(playerBase <= 1, "Player can't have more than one base.");
	DL_ASSERT_EXP(playerBase >= 1, "Player can't have less than one base.");

}

void LevelFactory::LoadTerrain(const std::string& aLevelPath)
{
	XMLReader reader;
	reader.OpenDocument(aLevelPath);
	tinyxml2::XMLElement* levelElement = reader.ForceFindFirstChild("root");
	levelElement = reader.ForceFindFirstChild(levelElement, "scene");
	std::string heightMap;
	std::string texturePath;

	tinyxml2::XMLElement* terrainElement = reader.FindFirstChild(levelElement, "terrain");
	reader.ForceReadAttribute(terrainElement, "heightmap", heightMap);
	reader.ForceReadAttribute(terrainElement, "texture", texturePath);

	myTerrain = new Prism::Terrain(heightMap, texturePath, { 256.f, 256.f }, 25.5f, CU::Matrix44<float>());
	reader.CloseDocument();
}


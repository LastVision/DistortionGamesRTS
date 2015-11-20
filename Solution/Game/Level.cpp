#include "stdafx.h"

#include "AIComponent.h"
#include <AudioInterface.h>
#include "BulletManager.h"
#include <Camera.h>
#include "CollisionComponent.h"
#include "CollisionManager.h"
#include "Constants.h"
#include "ConversationManager.h"
#include "DefendMessage.h"
#include <EngineEnums.h>
#include <Effect.h>
#include "EffectContainer.h"
#include "EMPMessage.h"
#include <Engine.h>
#include <EngineEnums.h>
#include "Entity.h"
#include "EntityFactory.h"
#include "FadeMessage.h"
#include "ParticleEmitterComponent.h"
#include "EmitterManager.h"
#include "EventManager.h"
#include <FileWatcher.h>
#include "GameStateMessage.h"
#include "GraphicsComponent.h"
#include "GUIComponent.h"
#include "GUINote.h"
#include "HealthComponent.h"
#include "Instance.h"
#include "InputComponent.h"
#include <InputWrapper.h>
#include "Level.h"
#include "LevelScoreMessage.h"
#include "MissionManager.h"
#include "ModelLoader.h"
#include "ModelProxy.h"
#include "PhysicsComponent.h"
#include "PostMaster.h"
#include "PowerUpComponent.h"
#include "PowerUpMessage.h"
#include "PropComponent.h"
#include <Renderer.h>
#include <Scene.h>
#include "ShieldComponent.h"
#include "ShootingComponent.h"
#include "SpawnEnemyMessage.h"
#include "SpawnPowerUpMessage.h"
#include "StreakEmitterComponent.h"
#include <Texture.h>
#include "WeaponFactory.h"
#include <XMLReader.h>

Level::Level(CU::InputWrapper* aInputWrapper, int aLevelID, int aDifficultyID)
	: myEntities(4096)
	, myComplete(false)
	, myUsePostProcessing(true)
	, mySkySphere(nullptr)
	, myEntityFactory(nullptr)
	, myWeaponFactory(nullptr)
	, myBulletManager(nullptr)
	, myCollisionManager(nullptr)
	, myMissionManager(nullptr)
	, myEventManager(nullptr)
	, myConversationManager(nullptr)
	, myEntityToDefend(nullptr)
	, myEmitterManager(nullptr)
	, myEMPScale(0.f)
	, myEMPTimer(0.f)
	, myEMPActivated(false)
	, myIsSkipable(false)
{
	myInputWrapper = aInputWrapper;
	myLevelScore.myLevel = aLevelID;
	myLevelScore.myDifficulty = aDifficultyID;

	PostMaster::GetInstance()->Subscribe(eMessageType::SPAWN_ENEMY, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::POWER_UP, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::DEFEND, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::SPAWN_POWERUP, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::EMP, this);
	PostMaster::GetInstance()->Subscribe(eMessageType::LEVEL_SCORE, this);
	Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_BackgroundMusic", 0);
	Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_BattleMusic", 0);
	Prism::Audio::AudioInterface::GetInstance()->PostEvent("Pause_BattleMusicNoFade", 0);

	myRenderer = new Prism::Renderer();
	myPBL = Prism::Engine::GetInstance()->GetEffectContainer()->GetEffect("Data/Resource/Shader/S_effect_pbl.fx");

	myPBL->SetEMPScale(myEMPScale);
}

Level::~Level()
{
	Prism::Engine::GetInstance()->GetModelLoader()->ClearLoadJobs();
	while (Prism::Engine::GetInstance()->GetModelLoader()->IsLoading() == true)
	{
	}
	Prism::Audio::AudioInterface::GetInstance()->PostEvent("Stop_BackgroundMusic", 0);
	Prism::Audio::AudioInterface::GetInstance()->PostEvent("Stop_BattleMusic", 0);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::SPAWN_ENEMY, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::POWER_UP, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::DEFEND, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::SPAWN_POWERUP, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::EMP, this);
	PostMaster::GetInstance()->UnSubscribe(eMessageType::LEVEL_SCORE, this);
	delete myCamera;
	delete myGlassCockpit;
	myGlassCockpit = nullptr;

	for (int i = 0; i < myEntities.Size(); i++)
	{
		//if (myEntities[i] != myPlayer)
		//{
			delete myEntities[i];
			myEntities[i] = nullptr;
		//}
	}
	delete mySkySphere;
	delete myEntityFactory;
	delete myWeaponFactory;
	delete myBulletManager;
	delete myCollisionManager;
	delete myMissionManager;
	delete myEventManager;
	delete myConversationManager;
	delete myScene;
	mySkySphere = nullptr;
	myScene = nullptr;

	delete myRenderer;
	myRenderer = nullptr;
	
	delete myEmitterManager;
	myEmitterManager = nullptr;

	Prism::Engine::GetInstance()->GetFileWatcher()->Clear();
}

bool Level::LogicUpdate(float aDeltaTime)
{
	myCollisionManager->CleanUp();

	if (myPlayer->GetAlive() == false || myEntityToDefend != nullptr && myEntityToDefend->GetAlive() == false)
	{
		return true;
	}

	for (int i = myEntities.Size() - 1; i >= 0; --i)
	{
		if (myEntities[i]->GetAlive() == false)
		{
			if (myEntities[i]->GetType() == eEntityType::ENEMY)
			{
				++myLevelScore.myKilledEnemies;
			}
			myEntities.DeleteCyclicAtIndex(i);
			continue;
		}

		myEntities[i]->Update(aDeltaTime);

		if (myEntities[i]->GetType() == eEntityType::ENEMY)
		{
			myPlayer->SendNote<GUINote>(GUINote(myEntities[i], eGUINoteType::ENEMY));
		}
	}

#ifndef RELEASE_BUILD
	if (myInputWrapper->KeyIsPressed(DIK_SPACE) == true)
	{
		myEMPTimer = 10.f;
		myEMPScale = 0.f;
		myEMPActivated = true;
		myEMPPosition = myPlayer->myOrientation.GetPos();
		myEMPHexagon->myOrientation.SetPos(myEMPPosition);
		myEMPHexagon->GetComponent<GraphicsComponent>()->SetScale({ myEMPScale, myEMPScale, myEMPScale });
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_EMPShot", 0);
	}
#endif

	if (myEMPActivated == true)
	{
		myEMPTimer -= aDeltaTime;
		myEMPScale += 1000 * aDeltaTime;
		myPBL->SetEMPScale(myEMPScale * 4);
		myPBL->SetEMPPosition(myEMPPosition);

		myEMPHexagon->myOrientation.SetPos(myEMPPosition);
		myEMPHexagon->GetComponent<GraphicsComponent>()->SetScale({ myEMPScale * 15.f, myEMPScale* 15.f, myEMPScale* 15.f });

		myEMPHexagon2->myOrientation.SetPos(myEMPPosition);
		myEMPHexagon2->GetComponent<GraphicsComponent>()->SetScale({ myEMPScale * 14.f, myEMPScale* 14.f, myEMPScale* 14.f });

		myEMPHexagon->myOrientation = CU::Matrix44<float>::CreateRotateAroundY(aDeltaTime * 1.5f) * myEMPHexagon->myOrientation;
		myEMPHexagon->myOrientation = CU::Matrix44<float>::CreateRotateAroundX(aDeltaTime * 1.5f) * myEMPHexagon->myOrientation;
		myEMPHexagon2->myOrientation = CU::Matrix44<float>::CreateRotateAroundY(-aDeltaTime * 1.5f) * myEMPHexagon2->myOrientation;
		myEMPHexagon2->myOrientation = CU::Matrix44<float>::CreateRotateAroundX(-aDeltaTime * 1.5f) * myEMPHexagon2->myOrientation;

		if (myEMPTimer <= 0.f)
		{
			myEMPScale = 0.f;
			myPBL->SetEMPScale(myEMPScale);
			myEMPActivated = false;
		}
	}

	if (myIsSkipable == true && myInputWrapper->KeyIsPressed(DIK_RETURN) == true)
	{
		CompleteLevel();
	}

	myEmitterManager->UpdateEmitters(aDeltaTime,myWorldMatrix);

	myPlayer->GetComponent<InputComponent>()->SetSkyPosition();
	myPlayer->GetComponent<GUIComponent>()->SetCockpitOrientation();

#ifndef RELEASE_BUILD
	UpdateDebug();
#endif
	myCollisionManager->Update();
	myBulletManager->Update(aDeltaTime);

	bool forceNextMission = false;
#ifndef RELEASE_BUILD
	if (myInputWrapper->KeyDown(DIK_F4) == true)
	{
		forceNextMission = true;
	}
#endif
	myMissionManager->Update(aDeltaTime, forceNextMission);
	
	myEventManager->Update();
	myCamera->Update(aDeltaTime);
	return myComplete;
}

void Level::Render(bool aIsActiveState)
{
	if (myUsePostProcessing == true)
	{
		myRenderer->BeginScene();

		Prism::Engine::GetInstance()->DisableZBuffer();
		mySkySphere->Render(*myCamera);
		Prism::Engine::GetInstance()->EnableZBuffer();

		myRenderer->EndScene(Prism::ePostProcessing::NONE);

		myRenderer->BeginScene();
		myScene->Render(myBulletManager->GetInstances());


		myRenderer->EndScene(Prism::ePostProcessing::BLOOM);
		myRenderer->FinalRender();

		myPlayer->GetComponent<GUIComponent>()->RenderShield(Prism::Engine::GetInstance()->GetWindowSize(), myInputWrapper->GetMousePosition(), aIsActiveState);

		myScene->RenderCockpit();

		if (myEMPActivated == true)
		{
			Prism::Engine::GetInstance()->DisableCulling();
			myEMPHexagon->GetComponent<GraphicsComponent>()->GetInstance()->Render(*myCamera);
			myEMPHexagon2->GetComponent<GraphicsComponent>()->GetInstance()->Render(*myCamera);
			Prism::Engine::GetInstance()->EnableCulling();
		}

		myEmitterManager->RenderEmitters(myCamera);
		myGlassCockpit->Render(*myCamera);
	}
	else
	{
		Prism::Engine::GetInstance()->DisableZBuffer();
		mySkySphere->Render(*myCamera);
		Prism::Engine::GetInstance()->EnableZBuffer();

		myScene->Render(myBulletManager->GetInstances());

		myEmitterManager->RenderEmitters(myCamera);
	}
	
	for (int i = 0; i < myEntities.Size(); ++i)
	{
		StreakEmitterComponent* emitter = myEntities[i]->GetComponent<StreakEmitterComponent>();
		if (emitter != nullptr)
		{
			emitter->Render();
		}
	}

	myBulletManager->RenderStreaks();

#ifdef _DEBUG
	//Prism::Engine::GetInstance()->PrintText(myCollisionManager->GetDebugText(), { 800.f, -76.f }, Prism::eTextType::DEBUG_TEXT);
#endif

	if (myIsSkipable == true && aIsActiveState == true)
	{
		Prism::Engine::GetInstance()->PrintText("Press [Enter] to skip tutorial."
			, { (Prism::Engine::GetInstance()->GetWindowSize().x * 0.5f) - 210.f, -(Prism::Engine::GetInstance()->GetWindowSize().y * 0.5f) - 300.f }
			, Prism::eTextType::RELEASE_TEXT);
	}
	
	myPlayer->GetComponent<GUIComponent>()->Render(Prism::Engine::GetInstance()->GetWindowSize(), myInputWrapper->GetMousePosition(), aIsActiveState);

#ifndef RELEASE_BUILD
	Prism::Engine::GetInstance()->PrintText(static_cast<float>(myPlayer->myOrientation.GetPos().x), CU::Vector2<float>(0, 0), Prism::eTextType::DEBUG_TEXT);
	Prism::Engine::GetInstance()->PrintText(static_cast<float>(myPlayer->myOrientation.GetPos().y), CU::Vector2<float>(0, -30), Prism::eTextType::DEBUG_TEXT);
	Prism::Engine::GetInstance()->PrintText(static_cast<float>(myPlayer->myOrientation.GetPos().z), CU::Vector2<float>(0, -60), Prism::eTextType::DEBUG_TEXT);

	Prism::Engine::GetInstance()->PrintText(std::to_string(myPlayer->GetComponent<HealthComponent>()->GetHealth()), { 0, -100.f }, Prism::eTextType::DEBUG_TEXT);
	Prism::Engine::GetInstance()->PrintText(std::to_string(myPlayer->GetComponent<ShieldComponent>()->GetCurrentShieldStrength()), { 0, -120.f }, Prism::eTextType::DEBUG_TEXT);

	Prism::Engine::GetInstance()->PrintText(myPlayer->GetComponent<PhysicsComponent>()->GetVelocity().x, { 0, -140.f }, Prism::eTextType::DEBUG_TEXT);
	Prism::Engine::GetInstance()->PrintText(myPlayer->GetComponent<PhysicsComponent>()->GetVelocity().y, { 0, -160.f }, Prism::eTextType::DEBUG_TEXT);
	Prism::Engine::GetInstance()->PrintText(myPlayer->GetComponent<PhysicsComponent>()->GetVelocity().z, { 0, -180.f }, Prism::eTextType::DEBUG_TEXT);
#endif
}

void Level::OnResize(int aWidth, int aHeight)
{
	if (myCamera != nullptr)
	{
		myCamera->OnResize(aWidth, aHeight);
	}
	myRenderer->OnResize(float(aWidth), float(aHeight));
}

Entity* Level::AddTrigger(XMLReader& aReader, tinyxml2::XMLElement* aElement)
{
	Entity* newEntity = new Entity(eEntityType::TRIGGER, *myScene, Prism::eOctreeType::NOT_IN_OCTREE);
	float entityRadius;
	aReader.ForceReadAttribute(aElement, "radius", entityRadius);
	myEntityFactory->CopyEntity(newEntity, "trigger");

	newEntity->GetComponent<CollisionComponent>()->SetCollisionRadius(entityRadius);

	tinyxml2::XMLElement* triggerElement = aReader.ForceFindFirstChild(aElement, "position");
	CU::Vector3<float> triggerPosition;
	aReader.ForceReadAttribute(triggerElement, "X", triggerPosition.x);
	aReader.ForceReadAttribute(triggerElement, "Y", triggerPosition.y);
	aReader.ForceReadAttribute(triggerElement, "Z", triggerPosition.z);
	newEntity->myOriginalOrientation.SetPos(triggerPosition*10.f);
	newEntity->myOrientation = newEntity->myOriginalOrientation;

	myEntities.Add(newEntity);
	myCollisionManager->Add(myEntities.GetLast()->GetComponent<CollisionComponent>(), eEntityType::TRIGGER);

	return myEntities.GetLast();
}

Entity* Level::GetEntityWithName(const std::string& aName)
{
	for (int i = 0; i < myEntities.Size(); ++i)
	{
		if (CU::ToLower(myEntities[i]->GetName()) == CU::ToLower(aName))
		{
			return myEntities[i];
		}
	}
	return nullptr;
}

int Level::GetEnemiesAlive() const
{
	return myCollisionManager->GetEnemiesAlive();
}

Entity* Level::GetPlayer()
{
	return myPlayer;
}

void Level::ReceiveMessage(const SpawnEnemyMessage& aMessage)
{
	eEntityType entityType = eEntityType::ENEMY;
	if (aMessage.myType == "ally")
	{
		entityType = eEntityType::ALLY;
		myPlayer->GetComponent<GUIComponent>()->ShowTimedMessage("Reinforcement has arrived!", 3.f);
	}

	Entity* newEntity = new Entity(entityType, *myScene, Prism::eOctreeType::DYNAMIC);
	
	myEntityFactory->CopyEntity(newEntity, aMessage.myType);
	if (aMessage.myPowerUpName != "")
	{
		newEntity->SetPowerUpName(aMessage.myPowerUpName);
	}

	newEntity->myOrientation.SetPos(aMessage.myPosition);
	
	newEntity->myOrientation = CU::GetOrientation(newEntity->myOrientation, aMessage.myRotation);

	myCollisionManager->Add(newEntity->GetComponent<CollisionComponent>(), entityType);

	if (entityType == eEntityType::ALLY)
	{
		newEntity->GetComponent<AIComponent>()->SetAllyTargets(myCollisionManager->GetEnemies());
	}
	else
	{
		if (myEntityToDefend != nullptr)
		{
			newEntity->GetComponent<AIComponent>()->SetEntityToFollow(myEntityToDefend, myPlayer);
		}
		else
		{
			newEntity->GetComponent<AIComponent>()->SetEntityToFollow(myPlayer, myPlayer);
		}
		++myLevelScore.myTotalEnemies;
	}

	myEntities.Add(newEntity);
	myScene->AddInstance(newEntity->GetComponent<GraphicsComponent>()->GetInstance());
}

void Level::ReceiveMessage(const PowerUpMessage& aMessage)
{
	if (aMessage.GetPowerupType() == ePowerUpType::WEAPON_UPGRADE)
	{
		bool changeWeapon = true;

		if (aMessage.GetUpgradeID() >= 2 && myPlayer->GetComponent<ShootingComponent>()->GetWeaponSize() == 1) // handles a rare exception
		{
			myPlayer->GetComponent<ShootingComponent>()->UpgradeWeapon(myWeaponFactory->GetWeapon("W_gun_shotgun_level_1"), 1, true);
			changeWeapon = false;
		}

		myPlayer->GetComponent<ShootingComponent>()->UpgradeWeapon(myWeaponFactory->GetWeapon(aMessage.GetUprgade()), aMessage.GetUpgradeID(), true);

		if (aMessage.GetUpgradeID() < 2 && changeWeapon == true)
		{
			myPlayer->GetComponent<ShootingComponent>()->SetCurrentWeaponID(aMessage.GetUpgradeID()); // Autochanges weapon on upgrade
		}
	}
}

void Level::ReceiveMessage(const DefendMessage& aMessage)
{
	if (aMessage.myType == DefendMessage::eType::ENTITY)
	{
		myEntityToDefend = aMessage.myEntity;
	}
	else if (aMessage.myType == DefendMessage::eType::COMPLETE)
	{
		myEntityToDefend = nullptr;
	}
}

void Level::ReceiveMessage(const SpawnPowerUpMessage& aMessage)
{
	Entity* newEntity = new Entity(eEntityType::POWERUP, *myScene, Prism::eOctreeType::STATIC);

	newEntity->myOriginalOrientation.SetPos(aMessage.myOrientation.GetPos());
	newEntity->myOrientation = aMessage.myOrientation;

	myEntityFactory->CopyEntity(newEntity, aMessage.myPowerUpName);
	newEntity->GetComponent<PowerUpComponent>()->SetPlayer(myPlayer);
	myCollisionManager->Add(newEntity->GetComponent<CollisionComponent>(), eEntityType::POWERUP);
	myEntities.Add(newEntity);
	myScene->AddInstance(newEntity->GetComponent<GraphicsComponent>()->GetInstance());

	if (newEntity->GetComponent<ParticleEmitterComponent>() != nullptr)
	{
		myEmitterManager->AddEmitter(newEntity->GetComponent<ParticleEmitterComponent>());
	}
	Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_SpawnPowerUp", 0); 
}

void Level::ReceiveMessage(const EMPMessage& aMessage)
{
	myEMPTimer = aMessage.myEMPTime;
	myEMPScale = 0.f;
	myEMPActivated = true;
	myEMPPosition = aMessage.myPosition;

	myEMPHexagon->myOrientation.SetPos(myEMPPosition);
	myEMPHexagon->GetComponent<GraphicsComponent>()->SetScale({ myEMPScale, myEMPScale, myEMPScale });
	myEMPHexagon2->myOrientation.SetPos(myEMPPosition);
	myEMPHexagon2->GetComponent<GraphicsComponent>()->SetScale({ myEMPScale, myEMPScale, myEMPScale });
	Prism::Audio::AudioInterface::GetInstance()->PostEvent("Play_EMPShot", 0);

	PostMaster::GetInstance()->SendMessage(FadeMessage(0.25f));
}

void Level::ReceiveMessage(const LevelScoreMessage& aMessage)
{
	if (aMessage.myScoreMessageType == eLevelScoreMessageType::PLAYER_SHOT)
	{
		++myLevelScore.myTotalShotsFired;
	}
	else if (aMessage.myScoreMessageType == eLevelScoreMessageType::PLAYER_HIT_ENEMY)
	{
		++myLevelScore.myShotsHit;
	}
	else if (aMessage.myScoreMessageType == eLevelScoreMessageType::OPTIONAL_MISSION_ADDED)
	{
		++myLevelScore.myTotalOptional;
	}
	else if (aMessage.myScoreMessageType == eLevelScoreMessageType::OPTIONAL_MISSION_COMPLETED)
	{
		++myLevelScore.myCompletedOptional;
	}
}

void Level::CompleteLevel()
{
	PostMaster::GetInstance()->SendMessage(GameStateMessage(eGameState::COMPLETE_LEVEL));
}

void Level::UpdateDebug()
{
	if (myInputWrapper->KeyDown(DIK_N) == true)
	{
		myPlayer->GetComponent<HealthComponent>()->RemoveHealth(10);
	}
	if (myInputWrapper->KeyDown(DIK_M) == true)
	{
		myPlayer->GetComponent<HealthComponent>()->RemoveHealth(myPlayer->GetComponent<HealthComponent>()->GetHealth());
	}
	if (myInputWrapper->KeyDown(DIK_V) == true)
	{
		myPlayer->GetComponent<HealthComponent>()->SetInvulnerability();
	}
	if (myInputWrapper->KeyDown(DIK_B) == true)
	{
		CompleteLevel();
	}
	if (myInputWrapper->KeyDown(DIK_P))
	{
		Prism::Engine::GetInstance()->ToggleWireframe();
	}
	if (myInputWrapper->KeyDown(DIK_7))
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("Mute", 0);
	}
	if (myInputWrapper->KeyDown(DIK_8))
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("UnMute", 0);
	}
	if (myInputWrapper->KeyDown(DIK_9))
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("LowerVolume", 0);
	}
	if (myInputWrapper->KeyDown(DIK_0))
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("IncreaseVolume", 0);
	}

	if (myInputWrapper->KeyDown(DIK_T))
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("MuteMusic", 0);
	}
	if (myInputWrapper->KeyDown(DIK_Y))
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("UnMuteMusic", 0);
	}
	if (myInputWrapper->KeyDown(DIK_U))
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("LowerMusic", 0);
	}
	if (myInputWrapper->KeyDown(DIK_I))
	{
		Prism::Audio::AudioInterface::GetInstance()->PostEvent("IncreaseMusic", 0);
	}
	if (myInputWrapper->KeyDown(DIK_F7))
	{
		myUsePostProcessing = !myUsePostProcessing;
	}
	if (myInputWrapper->KeyDown(DIK_F6))
	{
		Prism::Engine::GetInstance()->TogglePBLPixelShader();
	}
}
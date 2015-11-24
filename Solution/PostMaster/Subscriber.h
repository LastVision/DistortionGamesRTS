#pragma once

class Message;
class BulletCollisionToGUIMessage;
class BulletMessage;
class ConversationMessage;
class DefendMessage;
class FadeMessage;
class GameStateMessage;
class SpawnEnemyMessage;
class EnqueueEventMessage;
class EventQueueEmptyMessage;
class EnemyKilledMessage;
class PowerUpMessage;
class ResizeMessage;
class SpawnPowerUpMessage;
class KillStructureMessage;
class DestroyEmitterMessage;
class EMPMessage;
class SpawnExplosionMessage;
class LevelScoreMessage;

class Subscriber
{
public:
	Subscriber();
	~Subscriber();

	virtual void ReceiveMessage(const Message& aMessage);
	virtual void ReceiveMessage(const ResizeMessage& aMessage);
};


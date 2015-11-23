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

	virtual void ReceiveMessage(const BulletMessage& aMessage);
	virtual void ReceiveMessage(const ConversationMessage& aMessage);
	virtual void ReceiveMessage(const EnemyKilledMessage& aMessage);
	virtual void ReceiveMessage(const Message& aMessage);
	virtual void ReceiveMessage(const FadeMessage& aMessage);
	virtual void ReceiveMessage(const GameStateMessage& aMessage);
	virtual void ReceiveMessage(const PowerUpMessage& aMessage);
	virtual void ReceiveMessage(const SpawnEnemyMessage& aMessage);
	virtual void ReceiveMessage(const EnqueueEventMessage& aMessage);
	virtual void ReceiveMessage(const EventQueueEmptyMessage& aMessage);
	virtual void ReceiveMessage(const DefendMessage& aMessage);
	virtual void ReceiveMessage(const ResizeMessage& aMessage);
	virtual void ReceiveMessage(const BulletCollisionToGUIMessage& aMessage);
	virtual void ReceiveMessage(const SpawnPowerUpMessage& aMessage);
	virtual void ReceiveMessage(const KillStructureMessage& aMessage);
	virtual void ReceiveMessage(const DestroyEmitterMessage& aMessage);
	virtual void ReceiveMessage(const EMPMessage& aMessage);
	virtual void ReceiveMessage(const SpawnExplosionMessage& aMessage);
	virtual void ReceiveMessage(const LevelScoreMessage& aMessage);
};


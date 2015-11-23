#include "stdafx.h"
#include "Subscriber.h"
#include "Message.h"

Subscriber::Subscriber()
{
}


Subscriber::~Subscriber()
{
}

void Subscriber::ReceiveMessage(const Message&)
{
}

void Subscriber::ReceiveMessage(const BulletMessage&)
{
}

void Subscriber::ReceiveMessage(const ConversationMessage&)
{
}

void Subscriber::ReceiveMessage(const FadeMessage&)
{
}

void Subscriber::ReceiveMessage(const GameStateMessage&)
{
}

void Subscriber::ReceiveMessage(const SpawnEnemyMessage&)
{
}

void Subscriber::ReceiveMessage(const EnqueueEventMessage&)
{
}

void Subscriber::ReceiveMessage(const EventQueueEmptyMessage&)
{
}

void Subscriber::ReceiveMessage(const EnemyKilledMessage&)
{
}

void Subscriber::ReceiveMessage(const PowerUpMessage&)
{
}

void Subscriber::ReceiveMessage(const DefendMessage&)
{
}

void Subscriber::ReceiveMessage(const ResizeMessage&)
{
}

void Subscriber::ReceiveMessage(const BulletCollisionToGUIMessage&)
{
}

void Subscriber::ReceiveMessage(const SpawnPowerUpMessage&)
{
}

void Subscriber::ReceiveMessage(const KillStructureMessage&)
{
}

void Subscriber::ReceiveMessage(const DestroyEmitterMessage&)
{
}

void Subscriber::ReceiveMessage(const EMPMessage&)
{
}

void Subscriber::ReceiveMessage(const SpawnExplosionMessage&)
{
}

void Subscriber::ReceiveMessage(const LevelScoreMessage&)
{
}
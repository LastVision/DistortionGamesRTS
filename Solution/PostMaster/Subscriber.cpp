#include "stdafx.h"
#include "Subscriber.h"
#include "Message.h"

Subscriber::Subscriber()
{
}


Subscriber::~Subscriber()
{
}

void Subscriber::ReceiveMessage(const FadeMessage&)
{
}

void Subscriber::ReceiveMessage(const EmitterMessage&)
{
}

void Subscriber::ReceiveMessage(const GameStateMessage&)
{
}

void Subscriber::ReceiveMessage(const LUACinematicMessage&)
{
}

void Subscriber::ReceiveMessage(const LUAToggleRenderLinesMessage&)
{
}

void Subscriber::ReceiveMessage(const LUAMoveCameraMessage&)
{
}

void Subscriber::ReceiveMessage(const LUARunScriptMessage&)
{
}

void Subscriber::ReceiveMessage(const Message&)
{
}

void Subscriber::ReceiveMessage(const MinimapMoveMessage&)
{
}

void Subscriber::ReceiveMessage(const MoveCameraMessage&)
{
}

void Subscriber::ReceiveMessage(const OnClickMessage&)
{
}

void Subscriber::ReceiveMessage(const ResizeMessage&)
{
}

void Subscriber::ReceiveMessage(const ResourceMessage&)
{
}

void Subscriber::ReceiveMessage(const SpawnUnitMessage&)
{
}

void Subscriber::ReceiveMessage(const TextMessage&)
{
}

void Subscriber::ReceiveMessage(const TimeMultiplierMessage&)
{
}

void Subscriber::ReceiveMessage(const ToggleBuildTimeMessage&)
{
}

void Subscriber::ReceiveMessage(const ToggleGUIMessage&)
{
}

void Subscriber::ReceiveMessage(const TriggerMessage&)
{
}

void Subscriber::ReceiveMessage(const VictoryMessage&)
{
}

void Subscriber::ReceiveMessage(const UpgradeUnitMessage&)
{
}
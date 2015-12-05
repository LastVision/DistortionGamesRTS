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

void Subscriber::ReceiveMessage(const ResizeMessage&)
{
}

void Subscriber::ReceiveMessage(const GameStateMessage&)
{
}

void Subscriber::ReceiveMessage(const OnClickMessage&)
{
}

void Subscriber::ReceiveMessage(const MoveCameraMessage&)
{
}

void Subscriber::ReceiveMessage(const LUAMoveCameraMessage&)
{
}

void Subscriber::ReceiveMessage(const SpawnUnitMessage&)
{
}

void Subscriber::ReceiveMessage(const LUACinematicMessage&)
{
}

void Subscriber::ReceiveMessage(const FadeMessage&)
{
}

void Subscriber::ReceiveMessage(const ToggleGUIMessage&)
{
}


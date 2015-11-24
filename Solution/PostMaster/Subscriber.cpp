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

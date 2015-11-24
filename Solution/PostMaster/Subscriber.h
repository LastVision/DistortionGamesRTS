#pragma once

class Message;
class GameStateMessage;
class ResizeMessage;

class Subscriber
{
public:
	Subscriber();
	~Subscriber();

	virtual void ReceiveMessage(const Message& aMessage);
	virtual void ReceiveMessage(const ResizeMessage& aMessage);
	virtual void ReceiveMessage(const GameStateMessage& aMessage);
};


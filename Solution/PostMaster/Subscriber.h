#pragma once

struct Message;
struct GameStateMessage;
struct ResizeMessage;
struct OnClickMessage;

class Subscriber
{
public:
	Subscriber();
	~Subscriber();

	virtual void ReceiveMessage(const Message& aMessage);
	virtual void ReceiveMessage(const ResizeMessage& aMessage);
	virtual void ReceiveMessage(const GameStateMessage& aMessage);
	virtual void ReceiveMessage(const OnClickMessage& aMessage);
};


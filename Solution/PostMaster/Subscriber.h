#pragma once

struct Message;
struct GameStateMessage;
struct ResizeMessage;
struct OnClickMessage;
struct MoveCameraMessage;

class Subscriber
{
public:
	Subscriber();
	~Subscriber();

	virtual void ReceiveMessage(const Message& aMessage);
	virtual void ReceiveMessage(const ResizeMessage& aMessage);
	virtual void ReceiveMessage(const GameStateMessage& aMessage);
	virtual void ReceiveMessage(const OnClickMessage& aMessage);
	virtual void ReceiveMessage(const MoveCameraMessage& aMessage);
};


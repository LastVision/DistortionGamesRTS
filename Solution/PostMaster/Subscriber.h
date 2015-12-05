#pragma once

struct Message;
struct GameStateMessage;
struct ResizeMessage;
struct OnClickMessage;
struct MoveCameraMessage;
struct LUAMoveCameraMessage;
struct SpawnUnitMessage;
struct LUACinematicMessage;
struct FadeMessage;
struct ToggleGUIMessage;

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
	virtual void ReceiveMessage(const LUAMoveCameraMessage& aMessage);
	virtual void ReceiveMessage(const SpawnUnitMessage& aMessage);
	virtual void ReceiveMessage(const LUACinematicMessage& aMessage);
	virtual void ReceiveMessage(const FadeMessage& aMessage);
	virtual void ReceiveMessage(const ToggleGUIMessage& aMessage);
};


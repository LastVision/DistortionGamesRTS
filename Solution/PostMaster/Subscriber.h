#pragma once

struct AITimeMultiplierMessage;
struct FadeMessage;
struct GameStateMessage;
struct LUACinematicMessage;
struct LUAToggleRenderLinesMessage;
struct LUAMoveCameraMessage;
struct LUARunScriptMessage;
struct Message;
struct MoveCameraMessage;
struct OnClickMessage;
struct ResizeMessage;
struct ResourceMessage;
struct SpawnUnitMessage;
struct ToggleGUIMessage;
struct TriggerMessage;

class Subscriber
{
public:
	Subscriber();
	~Subscriber();

	virtual void ReceiveMessage(const AITimeMultiplierMessage& aMessage);
	virtual void ReceiveMessage(const FadeMessage& aMessage);
	virtual void ReceiveMessage(const GameStateMessage& aMessage);
	virtual void ReceiveMessage(const LUACinematicMessage& aMessage);
	virtual void ReceiveMessage(const LUAToggleRenderLinesMessage& aMessage);
	virtual void ReceiveMessage(const LUAMoveCameraMessage& aMessage);
	virtual void ReceiveMessage(const LUARunScriptMessage& aMessage);
	virtual void ReceiveMessage(const Message& aMessage);
	virtual void ReceiveMessage(const MoveCameraMessage& aMessage);
	virtual void ReceiveMessage(const OnClickMessage& aMessage);
	virtual void ReceiveMessage(const ResizeMessage& aMessage);
	virtual void ReceiveMessage(const ResourceMessage& aMessage);
	virtual void ReceiveMessage(const SpawnUnitMessage& aMessage);
	virtual void ReceiveMessage(const ToggleGUIMessage& aMessage);
	virtual void ReceiveMessage(const TriggerMessage& aMessage);
};


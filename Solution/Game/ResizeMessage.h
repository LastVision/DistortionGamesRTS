#pragma once

#include <Engine.h>
#include "Message.h"
#include <Vector.h>

class ResizeMessage : public Message
{
public:

	ResizeMessage(int aWidth, int aHeight);

	const CU::Vector2<int>& GetResolution() const;

private:
	
	CU::Vector2<int> myResolution;

};

inline ResizeMessage::ResizeMessage(int aWidth, int aHeight)
	: Message(eMessageType::RESIZE)
{
	myResolution = { aWidth, aHeight };
}

inline const CU::Vector2<int>& ResizeMessage::GetResolution() const
{
	return myResolution;
}

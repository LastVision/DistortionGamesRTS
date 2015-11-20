#include "stdafx.h"
#include "FadeMessage.h"


FadeMessage::FadeMessage(float aDuration)
	: Message(eMessageType::FADE)
	, myDuration(aDuration)
{
}
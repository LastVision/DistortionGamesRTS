#pragma once
#include "Tracker.h"

#define new (Prism::Tracker::GetInstance()->Allocate(__LINE__, __FILE__, __FUNCTION__), false) ? 0 : new
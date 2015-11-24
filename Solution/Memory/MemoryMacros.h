#pragma once
#include "MemoryTracker.h"

#define new (Prism::MemoryTracker::GetInstance()->Allocate(__LINE__, __FILE__, __FUNCTION__), false) ? 0 : new
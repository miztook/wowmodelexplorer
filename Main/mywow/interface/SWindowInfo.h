#pragma once

#include "base.h"

struct SWindowInfo
{
	SWindowInfo() : hwnd(NULL_PTR), width(0), height(0), scale(1.0f) {}

	window_type hwnd;
	u32 width;
	u32 height;
	f32 scale;
};

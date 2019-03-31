#pragma once

#include "base.h"

struct SWindowInfo
{
	SWindowInfo() : hwnd(nullptr), width(0), height(0), scale(1.0f) {}

	window_type hwnd;
	uint32_t width;
	uint32_t height;
	float scale;
};

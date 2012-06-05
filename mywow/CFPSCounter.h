#pragma once

#include "base.h"

class CFPSCounter
{
public:
	CFPSCounter();

	//
	f32 getFPS() const { return FPS; }
	void registerFrame(u32 now);

private:
	f32 FPS;
	u32 StartTime;

	u32 FrameCounted;
};
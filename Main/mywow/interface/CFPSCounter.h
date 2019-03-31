#pragma once

#include "core.h"

class CFPSCounter
{
public:
	CFPSCounter()
	{
		FPS = 60;
		LastTime = 0;
		FrameCounted = 0;
	}

	//
	float getFPS() const { return FPS; }
	void registerFrame(uint32_t now);
	

private:
	float FPS;
	uint32_t LastTime;
	uint32_t FrameCounted;
};

inline void CFPSCounter::registerFrame(uint32_t now)
{
	++FrameCounted;

	const uint32_t milliseconds = now - LastTime;

	if ( milliseconds >= 1500 )
	{
		const float invMilli = reciprocal_((float)milliseconds);
		FPS = (1000*FrameCounted) * invMilli ;
		FrameCounted = 0;
		LastTime = now;
	}
}

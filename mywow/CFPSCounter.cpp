#include "stdafx.h"
#include "CFPSCounter.h"
#include "core.h"

CFPSCounter::CFPSCounter()
{
	FPS = 60;
	StartTime = 0;
	FrameCounted = 0;
}

void CFPSCounter::registerFrame(u32 now)
{
	++FrameCounted;

	const u32 milliseconds = now - StartTime;

	if ( milliseconds >= 1500 )
	{
		const f32 invMilli = reciprocal_((f32)milliseconds);
		FPS = (1000*FrameCounted) * invMilli ;
		FrameCounted = 0;
		StartTime = now;
	}
}
#pragma once

#include "base.h"

class CTimer
{
private:
	DISALLOW_COPY_AND_ASSIGN(CTimer);

public:
	CTimer();

public:
	void beginPerf(bool enable);

	void endPerf(bool enble, u32& time);

	u32 getMillisecond() const;

	u32 getMicrosecond() const;
	
	void calculateTime();

	u32 getTimeSinceStart() const { return timeSinceStart; }

	u32 getTimeSinceLastFrame() const { return timeSinceLastFrame; }

	bool checkFrameLimit(s32 limit);

private:
	u64	PerfFreq;
	u64	PerfStart;
	u64	PerfEnd;

	u32		timelastframe;
	u32		timeSinceStart, timeSinceLastFrame;

	u32		FrameInterval;
};
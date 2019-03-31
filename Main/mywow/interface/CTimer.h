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

	void endPerf(bool enble, uint32_t& time);

	uint32_t getMillisecond() const;

	uint32_t getMicrosecond() const;
	
	void calculateTime();

	uint32_t getTimeSinceStart() const { return timeSinceStart; }

	uint32_t getTimeSinceLastFrame() const { return timeSinceLastFrame; }

	bool checkFrameLimit(int32_t limit);

private:
	uint64_t	PerfFreq;
	uint64_t	PerfStart;
	uint64_t	PerfEnd;

	uint32_t		timelastframe;
	uint32_t		timeSinceStart, timeSinceLastFrame;

	uint32_t		FrameInterval;
};
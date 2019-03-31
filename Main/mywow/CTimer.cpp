#include "stdafx.h"
#include "CTimer.h"

CTimer::CTimer()
	: timelastframe(0), timeSinceStart(0), timeSinceLastFrame(0)
	, FrameInterval(0)
{
#ifdef MW_PLATFORM_WINDOWS
	LARGE_INTEGER li;
	::QueryPerformanceFrequency(&li);
	PerfFreq = li.QuadPart;
#endif
	PerfStart = PerfEnd = 0;
}

void CTimer::beginPerf( bool enable )
{
	if (!enable)
		return;

#ifdef MW_PLATFORM_WINDOWS
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	PerfStart = li.QuadPart;
#else
	timeval time;
	gettimeofday(&time, nullptr);
	PerfStart = time.tv_sec * 1000000 + time.tv_usec;
#endif
}

void CTimer::endPerf( bool enble, uint32_t& time )
{
	if (!enble)
		return;

#ifdef MW_PLATFORM_WINDOWS
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	PerfEnd = li.QuadPart;
	time = (uint32_t)((PerfEnd - PerfStart) * 1000000 / PerfFreq);
#else
	timeval tv;
	gettimeofday(&tv, nullptr);
	PerfEnd = tv.tv_sec * 1000000 + tv.tv_usec;
	time = (uint32_t)(PerfEnd - PerfStart);
#endif
}

uint32_t CTimer::getMillisecond() const
{
#ifdef MW_PLATFORM_WINDOWS
	LARGE_INTEGER time;
	QueryPerformanceCounter(&time);
	return (uint32_t)(time.QuadPart * 1000 / PerfFreq);
#else
	timeval time;
	gettimeofday(&time, nullptr);
	return (uint32_t)(time.tv_sec * 1000 + time.tv_usec / 1000);
#endif
}

uint32_t CTimer::getMicrosecond() const
{
#ifdef MW_PLATFORM_WINDOWS
	LARGE_INTEGER time;
	QueryPerformanceCounter(&time);
	return (uint32_t)(time.QuadPart * 1000000 / PerfFreq);
#else
	timeval time;
	gettimeofday(&time, nullptr);
	return (uint32_t)(time.tv_sec * 1000000 + time.tv_usec);
#endif
}

void CTimer::calculateTime()
{
	//time
	uint32_t tick = getMillisecond();

	static bool bFirst = true;	
	if ( bFirst)
	{
		timelastframe = tick;
		timeSinceStart = timeSinceLastFrame = 0;
		bFirst = false;
		return;
	}

	timeSinceLastFrame = tick - timelastframe;
	if (timeSinceLastFrame > 1000)
		timeSinceLastFrame = 1000;

	timeSinceStart += timeSinceLastFrame;
	timelastframe = tick;
}

bool CTimer::checkFrameLimit(int32_t limit)
{
	if (limit <= -1)
		return true;

	FrameInterval += timeSinceLastFrame;

	if (FrameInterval < 1000/(uint32_t)limit)		//use foreground limit
	{
		return false;
	}
	else
	{
		timeSinceLastFrame = FrameInterval;
		FrameInterval = 0;

		return true;
	}
}


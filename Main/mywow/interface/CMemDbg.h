#pragma once


#ifdef MW_PLATFORM_WINDOWS
#include "base.h"
#include <crtdbg.h>

class CMemDbg
{
private:
	DISALLOW_COPY_AND_ASSIGN(CMemDbg);

public:
	CMemDbg() {}

	void beginCheckPoint();

	bool endCheckPoint();

	void outputDifference(const c8* funcname);

	void setAllocHook(bool enable, u32 nMaxAlloc = 1000);

	void outputMaxMemoryUsed();

private:
	_CrtMemState		OldState;
	_CrtMemState		NewState;
	_CrtMemState		DiffState;
};

#endif
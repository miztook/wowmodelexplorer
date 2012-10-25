#pragma once

#include <windows.h>

class CLock
{
public:
	CLock(CRITICAL_SECTION* cs) : CS(cs)
	{
		::EnterCriticalSection(CS);
	}

	~CLock()
	{
		::LeaveCriticalSection(CS);
	}
private:
	CRITICAL_SECTION* CS;
};
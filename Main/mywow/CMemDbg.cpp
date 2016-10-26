#include "stdafx.h"
#include "CMemDbg.h"

#ifdef MW_PLATFORM_WINDOWS

#include "CSysUtility.h"

u32 g_maxAlloc = 1000;

int __cdecl MyAllocHook(
	int      nAllocType,
	void   * pvData,
	size_t   nSize,
	int      nBlockUse,
	long     lRequest,
	const unsigned char * szFileName,
	int      nLine
	)
{
	const char *operation[] = { "", "allocating", "re-allocating", "freeing" };
	const char *blockType[] = { "Free", "Normal", "CRT", "Ignore", "Client" };

	if ( nBlockUse == _CRT_BLOCK )   // Ignore internal C runtime library allocations
		return( TRUE );

	ASSERT( ( nAllocType > 0 ) && ( nAllocType < 4 ) );
	ASSERT( ( nBlockUse >= 0 ) && ( nBlockUse < 5 ) );

	if (nSize > g_maxAlloc && nSize <= 4 && (nAllocType == 1 || nAllocType == 2))
	{
		CSysUtility::outputDebug("Memory operation in %s, line %d: %s a %d-byte '%s' block (#%ld)\n",
			szFileName, nLine, operation[nAllocType], nSize, 
			blockType[nBlockUse], lRequest );
	}

	return( TRUE );         // Allow the memory operation to proceed
}

void CMemDbg::beginCheckPoint()
{
	_CrtMemCheckpoint(&OldState);
}

bool CMemDbg::endCheckPoint()
{
	_CrtMemCheckpoint(&NewState);

	s32 diff = _CrtMemDifference(&DiffState, &OldState, &NewState);
	return diff == 0;
}

void CMemDbg::outputDifference( const c8* funcname )
{
	CSysUtility::outputDebug("%s memory used: %0.2f M\n", funcname,
		DiffState.lSizes[_NORMAL_BLOCK] / 1048576.f);
}

void CMemDbg::setAllocHook( bool enable, u32 nMaxAlloc /*= 1000*/ )
{
	if (enable)
	{
		_CrtSetAllocHook(MyAllocHook);
		g_maxAlloc = nMaxAlloc;
	}
	else
	{
		_CrtSetAllocHook(NULL_PTR);
	}
}

void CMemDbg::outputMaxMemoryUsed()
{
	_CrtMemCheckpoint(&OldState);

	CSysUtility::outputDebug("maximum memory used: %0.2f M\n", OldState.lHighWaterCount / 1048576.f);
}

#endif



#include "stdafx.h"
#include "CMiniDump.h"

#ifdef MW_PLATFORM_WINDOWS

#include <DbgHelp.h>

typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
	CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
	CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
	CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam
	);

void CMiniDump::begin()
{
	::SetUnhandledExceptionFilter( TopLevelFilter );
}

void CMiniDump::end()
{
	::SetUnhandledExceptionFilter( nullptr );
}

LONG WINAPI CMiniDump::TopLevelFilter( struct _EXCEPTION_POINTERS *pExceptionInfo )
{
	LONG retval = EXCEPTION_CONTINUE_SEARCH;

	// firstly see if dbghelp.dll is around and has the function we need
	// look next to the EXE first, as the one in System32 might be old 
	// (e.g. Windows 2000)
	HMODULE hDll = nullptr;
	char szDbgHelpPath[_MAX_PATH];

	GetModuleFileName( nullptr, szDbgHelpPath, _MAX_PATH );
	
	char* pSlash = strrchr( szDbgHelpPath, '\\' );
	if (pSlash)
	{
		strcpy_s( pSlash+1, _MAX_PATH - (pSlash - szDbgHelpPath), "DBGHELP.DLL" );
		hDll = ::LoadLibrary( szDbgHelpPath );
	}
	
	if (hDll==nullptr)
	{
		// load any version we can
		hDll = ::LoadLibrary( "DBGHELP.DLL" );
	}

	if (!hDll)
		return retval;

	MINIDUMPWRITEDUMP pDump = (MINIDUMPWRITEDUMP)::GetProcAddress( hDll, "MiniDumpWriteDump" );
	if (!pDump)
		return retval;

	char szDumpPath[_MAX_PATH];

	GetModuleFileName( nullptr, szDumpPath, _MAX_PATH );

	// work out a good place for the dump file
	pSlash = strrchr( szDumpPath, '.' );
	if (pSlash)
	{
		char timebuf[64];
		SYSTEMTIME time;
		GetLocalTime(&time);
		sprintf_s(timebuf, 64, "-%dm_%dd_%dh_%dm.dmp", time.wMonth, time.wDay, time.wHour, time.wMinute);

		strcpy_s( pSlash, _MAX_PATH - (pSlash - szDumpPath), timebuf );
	}
	else
		return retval;

	// ask the user if they want to save a dump file

	// create the file
	HANDLE hFile = ::CreateFile( szDumpPath, GENERIC_WRITE, FILE_SHARE_WRITE, nullptr, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, nullptr );

	if (hFile!=INVALID_HANDLE_VALUE)
	{
		_MINIDUMP_EXCEPTION_INFORMATION ExInfo;

		ExInfo.ThreadId = ::GetCurrentThreadId();
		ExInfo.ExceptionPointers = pExceptionInfo;
		ExInfo.ClientPointers = FALSE;

		// write the dump
		BOOL bOK = pDump( GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, nullptr, nullptr );
		if (bOK)
		{
			retval = EXCEPTION_EXECUTE_HANDLER;
		}
		::CloseHandle(hFile);
	}

	return retval;
}

#endif



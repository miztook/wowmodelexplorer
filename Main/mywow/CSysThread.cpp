#include "stdafx.h"
#include "CSysThread.h"
#include "base.h"

#ifdef MW_PLATFORM_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h>
#else
#include <pthread.h>
#endif

enum MW_THREAD_SINGAL
{
	MW_THREAD_SIGNAL_NORMAL		= 0, //正常运行
	MW_THREAD_SIGNAL_STOP		= 1, //停止
	MW_THREAD_SIGNAL_HUNG		= 2, //挂起
};


#ifdef MW_PLATFORM_WINDOWS
DWORD WINAPI PlatformThreadFunction(LPVOID p)
{
	if (NULL_PTR == p)
		return 0;

	thread_type* pInfo = static_cast<thread_type*>(p);
	pInfo->function(pInfo->param);

	return 0;
}
#else
void* PlatformThreadFunction(void* p)
{
	if (NULL_PTR == p)
		return 0;

	thread_type* pinfo = static_cast<thread_type*>(p);
	while (pinfo->signal == MW_THREAD_SIGNAL_HUNG)
		usleep(1000);

	//开始执行
	pinfo->function(pinfo->param);

	return NULL_PTR;
}
#endif

void INIT_THREAD( thread_type* thread, THREAD_FUNC func, void* param, bool suspend )
{
	thread->function		= func;
	thread->param		= param;
	thread->signal		= MW_THREAD_SIGNAL_NORMAL;

#ifdef MW_PLATFORM_WINDOWS
	DWORD threadId;
	thread->handle = static_cast<void*>(
		::CreateThread(NULL_PTR, 0, PlatformThreadFunction, thread, suspend ? CREATE_SUSPENDED : 0, &threadId));
#else
	if (suspend)
		thread->signal = MW_THREAD_SIGNAL_HUNG;

	int r = pthread_create(
			(pthread_t*)&(thread->handle),
			NULL_PTR,
			PlatformThreadFunction,
			thread);
	if (r != 0)
	{
		assert(false);
	}
#endif
}

void DESTROY_THREAD( thread_type* thread )
{
#ifdef MW_PLATFORM_WINDOWS
	::CloseHandle(thread->handle);
#else
	thread->handle = NULL_PTR;
#endif
}

void RESUME_THREAD( thread_type* thread )
{
#ifdef MW_PLATFORM_WINDOWS
	::ResumeThread(thread->handle);
#else
	thread->signal = MW_THREAD_SIGNAL_NORMAL;
#endif
}

bool WAIT_THREAD( thread_type* thread)
{
#ifdef MW_PLATFORM_WINDOWS
	return WAIT_OBJECT_0 == ::WaitForSingleObject((HANDLE)thread->handle, INFINITE);
#else
    return pthread_join((pthread_t)(thread->handle), NULL_PTR) == 0;
#endif
}

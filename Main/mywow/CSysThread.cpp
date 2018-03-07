#include "stdafx.h"
#include "CSysThread.h"
#include <cassert>

#include "compileconfig.h"

#ifdef MW_PLATFORM_WINDOWS

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <process.h>

#else
#include <pthread.h>
#endif

enum MW_THREAD_SINGAL
{
	MW_THREAD_SIGNAL_NORMAL = 0, //正常运行
	MW_THREAD_SIGNAL_STOP = 1, //停止
	MW_THREAD_SIGNAL_HUNG = 2, //挂起
};

// #ifdef A_PLATFORM_WIN_DESKTOP
// DWORD WINAPI PlatformThreadFunction(LPVOID p)
// {
// 	if (nullptr == p)
// 		return 0;
//
// 	thread_type* pInfo = static_cast<thread_type*>(p);
// 	pInfo->function(pInfo->param);
// 	pInfo->running = false;
//
// 	return 0;
// }
// #else
// void* PlatformThreadFunction(void* p)
// {
// 	if (nullptr == p)
// 		return 0;
//
// 	thread_type* pInfo = static_cast<thread_type*>(p);
// 	while (pInfo->signal == MW_THREAD_SIGNAL_HUNG)
// 		usleep(1000);
//
// 	//开始执行
// 	pInfo->function(pInfo->param);
// 	pInfo->running = false;
//
// 	return nullptr;
// }
// #endif

void INIT_THREAD(thread_type* thread, const THREAD_FUNC& func, void* param, bool suspend)
{
	thread->function = func;
	thread->param = param;
	thread->signal = MW_THREAD_SIGNAL_NORMAL;
	thread->running = !suspend;

#ifdef MW_PLATFORM_WINDOWS
	unsigned int threadId;
	thread->handle = reinterpret_cast<void*>(
		_beginthreadex(nullptr, 0,
		[](void* p)->unsigned int {
		if (nullptr == p)
			return 0;

		thread_type* pInfo = static_cast<thread_type*>(p);
		pInfo->function(pInfo->param);
		pInfo->running = false;

		return 0;
	},
		thread,
		suspend ? CREATE_SUSPENDED : 0,
		&threadId));
#else
	if (suspend)
		thread->signal = MW_THREAD_SIGNAL_HUNG;

	int r = pthread_create(
		(pthread_t*)&(thread->handle),
		nullptr,
		[](void* p)->void* {
		if (nullptr == p)
			return 0;

		thread_type* pInfo = static_cast<thread_type*>(p);
		while (pInfo->signal == MW_THREAD_SIGNAL_HUNG)
			usleep(1000);

		//开始执行
		pInfo->function(pInfo->param);
		pInfo->running = false;

		return nullptr;
	},
		thread);
	if (r != 0)
	{
		assert(false);
	}
#endif
}

void DESTROY_THREAD(thread_type* thread)
{
#ifdef MW_PLATFORM_WINDOWS
	::CloseHandle(thread->handle);
	thread->handle = nullptr;
#else
	thread->handle = nullptr;
#endif
}

void RESUME_THREAD(thread_type* thread)
{
	if (thread->running)
		return;

#ifdef MW_PLATFORM_WINDOWS
	::ResumeThread(thread->handle);
#else
	thread->signal = MW_THREAD_SIGNAL_NORMAL;
#endif

	thread->running = true;
}

bool WAIT_THREAD(thread_type* thread)
{
	if (!thread->running)
		return true;

#ifdef MW_PLATFORM_WINDOWS
	return WAIT_OBJECT_0 == ::WaitForSingleObject((HANDLE)thread->handle, INFINITE);
#else
	return pthread_join((pthread_t)(thread->handle), nullptr) == 0;
#endif
}

bool IS_THREAD_RUNNING(thread_type* thread)
{
	return thread->running;
}

void THIS_THREAD_SLEEP(unsigned int millisecond)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(millisecond));
}

#ifdef MW_PLATFORM_WINDOWS

void INIT_TASK(task_type* task, const THREAD_FUNC& func, void* param)
{
	task->m_future = std::async(std::launch::async, func, param);
	task->valid = true;
}

void DESTROY_TASK(task_type* task)
{
	task->valid = false;
}

bool WAIT_TASK(task_type* task)
{
	if (!task->valid)
		return false;

	task->m_future.wait();
	return true;
}

bool IS_TASK_RUNNING(task_type* task)
{
	if (!task->valid)
		return false;

	auto status = task->m_future.wait_for(std::chrono::milliseconds(1));
	return status == std::future_status::timeout;
}

#endif
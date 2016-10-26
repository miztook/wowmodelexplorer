#pragma once


#ifdef MW_PLATFORM_WINDOWS

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1	// Exclude rarely-used stuff from Windows headers
#endif
#include <windows.h>

typedef CRITICAL_SECTION lock_type;
struct event_type
{
	HANDLE handle;
};

#else
#include <unistd.h>
#include <pthread.h>
typedef pthread_mutex_t lock_type;
struct event_type
{
	pthread_mutex_t		mutex;
	pthread_cond_t		cond;
	bool trigger;
};
#endif

//lock
inline void BEGIN_LOCK(lock_type* cs)
{
#ifdef MW_PLATFORM_WINDOWS
	::EnterCriticalSection(cs);
#else
	::pthread_mutex_lock(cs);
#endif
}

inline void END_LOCK(lock_type* cs)
{
#ifdef MW_PLATFORM_WINDOWS
	::LeaveCriticalSection(cs);
#else
	::pthread_mutex_unlock(cs);
#endif
}

inline void INIT_LOCK(lock_type* cs)
{
#ifdef MW_PLATFORM_WINDOWS
	::InitializeCriticalSection(cs);
#else
	::pthread_mutex_init(cs, NULL_PTR);
#endif
}

inline void DESTROY_LOCK(lock_type* cs)
{
#ifdef MW_PLATFORM_WINDOWS
	::DeleteCriticalSection(cs);
#else
	::pthread_mutex_destroy(cs);
#endif
}

//event
inline void INIT_EVENT(event_type* eve, const char* name)
{
#ifdef MW_PLATFORM_WINDOWS
	eve->handle = CreateEventA(NULL, FALSE, FALSE, name);
#else
	::pthread_mutex_init(&eve->mutex, NULL);
	::pthread_cond_init(&eve->cond, NULL);
	eve->trigger = false;
#endif
}

inline void DESTROY_EVENT(event_type* eve)
{
#ifdef MW_PLATFORM_WINDOWS
	::CloseHandle(eve->handle);
#else
	::pthread_mutex_destroy(&eve->mutex);
	::pthread_cond_destroy(&eve->cond);
	eve->trigger = false;
#endif
}

inline bool WAIT_EVENT(event_type* eve, int millisecond = -1)
{
#ifdef MW_PLATFORM_WINDOWS
	return WAIT_OBJECT_0 == ::WaitForSingleObject(eve->handle, millisecond >= 0 ? (DWORD)millisecond : INFINITE);
#else
	if (0 != pthread_mutex_lock(&eve->mutex))
		return false;

	int ret = 0;
	if (!eve->trigger)
	{
		if (millisecond >= 0)
		{
			struct timespec wait_time;
			wait_time.tv_sec = millisecond / 1000;
			wait_time.tv_nsec = (millisecond % 1000) * 1000000;
			ret = pthread_cond_timedwait(&eve->cond, &eve->mutex, &wait_time);
		}
		else
		{
			ret = pthread_cond_wait(&eve->cond, &eve->mutex);
		}
	}	
	eve->trigger = false;
	pthread_mutex_unlock(&eve->mutex);
	return ret == 0;
#endif
}

inline void SET_EVENT(event_type* eve)
{
#ifdef MW_PLATFORM_WINDOWS
	::SetEvent(eve->handle);
#else
	if (0 != pthread_mutex_lock(&eve->mutex))
		return;
	::pthread_cond_signal(&eve->cond);
	eve->trigger = true;
	pthread_mutex_unlock(&eve->mutex);	
#endif
}

inline void SLEEP(int millisecond)
{
#ifdef MW_PLATFORM_WINDOWS
	::Sleep((DWORD)millisecond);
#else
	::usleep(millisecond * 1000);
#endif
}




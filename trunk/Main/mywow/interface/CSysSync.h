#pragma once

#include "compileconfig.h"
#include <atomic>

template<class T>
using atomic_type = std::atomic<T>;

#include <mutex>
#include <condition_variable>

using lock_type = std::mutex;
typedef struct
{
	std::mutex		mutex;
	std::condition_variable		cond;
	bool trigger;
} _event_type;

using event_type = _event_type;

//lock
inline void BEGIN_LOCK(lock_type* cs)
{
	cs->lock();
}

inline void END_LOCK(lock_type* cs)
{
	cs->unlock();
}

inline void INIT_LOCK(lock_type* cs)
{
}

inline void DESTROY_LOCK(lock_type* cs)
{
}

//event
inline void INIT_EVENT(event_type* eve)
{
	std::unique_lock<std::mutex> locker(eve->mutex);
	eve->trigger = false;
}

inline void DESTROY_EVENT(event_type* eve)
{
	std::unique_lock<std::mutex> locker(eve->mutex);
	eve->trigger = false;
}

inline void WAIT_EVENT(event_type* eve)
{
	std::unique_lock<std::mutex> locker(eve->mutex);
	eve->cond.wait(locker, [eve]{ return eve->trigger; });
	eve->trigger = false;
}

inline void SET_EVENT(event_type* eve)
{
	std::unique_lock<std::mutex> locker(eve->mutex);
	eve->trigger = true;
	eve->cond.notify_one();
}

using CLock = std::lock_guard < std::mutex >;


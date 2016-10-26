#pragma once

#include <cassert>

typedef int (*THREAD_FUNC)(void* param);

struct thread_type
{
	void*			handle;
	THREAD_FUNC		function;
	void*			param;
	int				signal;
};

typedef int (*THREAD_FUNC)(void* param);

void INIT_THREAD(thread_type* thread, THREAD_FUNC func, void* param, bool suspend);

void DESTROY_THREAD(thread_type* thread);

void RESUME_THREAD(thread_type* thread);

bool WAIT_THREAD(thread_type* thread);




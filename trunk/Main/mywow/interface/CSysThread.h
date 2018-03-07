#pragma once

#include "compileconfig.h"
#include <chrono>
#include <thread>
#include <functional>
#include <future>

// struct task : asio::coroutine
// {
// 	int num = 10;
// 	char operator()(char c)
// 	{
// 		reenter(this)
// 		{
// 			do
// 			{
// 				--num;
// 				yield return c;
// 			} while (num > 0);
// 		}
// 		return '\0';
// 	}
// };
//
// task t;
// while (!t.is_complete())
// {
// 	char c = t('m');
// 	std::cout << c << std::endl;
// }

using THREAD_FUNC = std::function<int(void*)>;

struct thread_type
{
	void*			handle;
	THREAD_FUNC		function;
	void*			param;
	int				signal;
	bool	running;
};

void INIT_THREAD(thread_type* thread, const THREAD_FUNC& func, void* param, bool suspend = false);

void DESTROY_THREAD(thread_type* thread);

void RESUME_THREAD(thread_type* thread);

bool WAIT_THREAD(thread_type* thread);

bool IS_THREAD_RUNNING(thread_type* thread);

void THIS_THREAD_SLEEP(unsigned int millisecond);

#ifdef MW_PLATFORM_WINDOWS

struct task_type
{
	std::future<int> m_future;
	bool valid;
};

void INIT_TASK(task_type* task, const THREAD_FUNC& func, void* param);

void DESTROY_TASK(task_type* task);

bool WAIT_TASK(task_type* task);

bool IS_TASK_RUNNING(task_type* task);

#endif

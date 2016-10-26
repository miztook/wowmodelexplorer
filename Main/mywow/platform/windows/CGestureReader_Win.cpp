#include "stdafx.h"
#include "CGestureReader.h"

#ifdef MW_PLATFORM_WINDOWS

#include "mywow.h"

#include "CGestureRecognizer_Joystick.h"
#include "CGestureRecognizer_Tap1.h"
#include "CGestureRecognizer_Tap2.h"
#include "CGestureRecognizer_Pinch.h"
#include "CGestureRecognizer_Pan1.h"
#include "CGestureRecognizer_Pan2.h"
#include "CGestureRecognizer_Longpress.h"
#include "CGestureRecognizer_Swipe.h"
#include "CGestureRecognizer_Circle.h"

static const f32 g_doubleTapMaxOffset = 3.0f;

CGestureReader::CGestureReader()
{
	memset(GestureRecognizers, 0, sizeof(GestureRecognizers));

	GestureRecognizers[GestureType_Joystick] = new CGestureRecognizer_Joystick(this);
	GestureRecognizers[GestureType_Tap_1] = new CGestureRecognizer_Tap1(this);
	GestureRecognizers[GestureType_Tap_2] = new CGestureRecognizer_Tap2(this);
	GestureRecognizers[GestureType_Pinch] = new CGestureRecognizer_Pinch(this);
	GestureRecognizers[GestureType_Pan_1] = new CGestureRecognizer_Pan1(this);
	GestureRecognizers[GestureType_Pan_2] = new CGestureRecognizer_Pan2(this);
	GestureRecognizers[GestureType_LongPress] = new CGestureRecognizer_Longpress(this);
	GestureRecognizers[GestureType_Swipe] = new CGestureRecognizer_Swipe(this);
	GestureRecognizers[GestureType_Circle] = new CGestureRecognizer_Circle(this);

	GesturePool.allocateAll(30);
	InitializeListHead(&GestureInfoList);

	INIT_LOCK(&cs);

	memset(FingerId, 0, sizeof(FingerId));
	memset(CacheButtonDown, 0, sizeof(CacheButtonDown));

	HWND hwnd = g_Engine->getWindowInfo().hwnd;
	::SetTimer(hwnd, (UINT_PTR)hwnd, 20, NULL_PTR);
}

CGestureReader::~CGestureReader()
{
	HWND hwnd = g_Engine->getWindowInfo().hwnd;
	::KillTimer(hwnd, (UINT_PTR)hwnd);

	DESTROY_LOCK(&cs);

	for (u32 i=0; i<GestureType_Num; ++i)
	{
		delete GestureRecognizers[i];
	}
}

void CGestureReader::filterWindowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	u32 now = g_Engine->getTimer()->getMillisecond();

	switch(message)
	{
	case WM_TIMER:
		{
			tick();
		}
		break;
	case WM_LBUTTONDOWN:
		{
			f32 time = now * 0.001f;

			SGesTouchInfo touch;
			touch.fingerID = FingerId[WIN_TOUCH_LBUTTON];
			touch.tapCount = 1;

			s32 x = (int)(short)LOWORD(lParam);
			s32 y = (int)(short)HIWORD(lParam);

			touch.phase = TouchPhase_Begin;
			touch.posX = (f32)x;
			touch.posY = (f32)y;

			touch.deltaTime = 0.0f;
			touch.deltaX = 0.0f; //touch.posX - CacheButtonDown[WIN_TOUCH_LBUTTON].posX;
			touch.deltaY = 0.0f; //touch.posY - CacheButtonDown[WIN_TOUCH_LBUTTON].posY;

			if (touch.deltaTime <= GetDoubleClickTime() * 0.001f &&
				fabs(touch.deltaX) < g_doubleTapMaxOffset &&
				fabs(touch.deltaY) < g_doubleTapMaxOffset)
			{
				touch.tapCount = 2;
			}

			STouchCache& cache = CacheButtonDown[WIN_TOUCH_LBUTTON];
			//save cache
			cache.time = time;
			cache.posX = touch.posX;
			cache.posY = touch.posY;

			readTouchInfo(&touch, 1);
			::SetCapture(hWnd);
		}
		break;
	case WM_LBUTTONUP:
		{			
			f32 time = now * 0.001f;

			SGesTouchInfo touch;
			touch.fingerID = FingerId[WIN_TOUCH_LBUTTON];
			touch.tapCount = 1;

			s32 x = (int)(short)LOWORD(lParam);
			s32 y = (int)(short)HIWORD(lParam);

			touch.phase = TouchPhase_End;
			touch.posX = (f32)x;
			touch.posY = (f32)y;

			STouchCache& cache = CacheButtonDown[WIN_TOUCH_LBUTTON];

			touch.deltaTime = time - cache.time;
			if (touch.deltaTime < 0.0f)
				touch.deltaTime += FLT_MAX;
			touch.deltaX = touch.posX - cache.posX;
			touch.deltaY = touch.posY - cache.posY;

			if (touch.deltaTime <= GetDoubleClickTime() * 0.001f &&
				fabs(touch.deltaX) < g_doubleTapMaxOffset &&
				fabs(touch.deltaY) < g_doubleTapMaxOffset)
			{
				touch.tapCount = 2;
			}

			//save cache
// 			cache.time = time;
// 			cache.posX = touch.posX;
// 			cache.posY = touch.posY;

			readTouchInfo(&touch, 1);
			::ReleaseCapture();
		}
		break;
	case WM_RBUTTONDOWN:
		{			
			f32 time = now * 0.001f;

			SGesTouchInfo touch;
			touch.fingerID = FingerId[WIN_TOUCH_RBUTTON];
			touch.tapCount = 1;

			s32 x = (int)(short)LOWORD(lParam);
			s32 y = (int)(short)HIWORD(lParam);

			touch.phase = TouchPhase_Begin;
			touch.posX = (f32)x;
			touch.posY = (f32)y;

			touch.deltaTime = 0.0f;
			touch.deltaX = 0.0f; //touch.posX - CacheButtonDown[WIN_TOUCH_RBUTTON].posX;
			touch.deltaY = 0.0f; //touch.posY - CacheButtonDown[WIN_TOUCH_RBUTTON].posY;

			if (touch.deltaTime <= GetDoubleClickTime() * 0.001f &&
				fabs(touch.deltaX) < g_doubleTapMaxOffset &&
				fabs(touch.deltaY) < g_doubleTapMaxOffset)
			{
				touch.tapCount = 2;
			}

			STouchCache& cache = CacheButtonDown[WIN_TOUCH_RBUTTON];
			//save cache
			cache.time = time;
			cache.posX = touch.posX;
			cache.posY = touch.posY;

			readTouchInfo(&touch, 1);
			::SetCapture(hWnd);
		}
		break;
	case WM_RBUTTONUP:
		{		
			f32 time = now * 0.001f;

			SGesTouchInfo touch;
			touch.fingerID = FingerId[WIN_TOUCH_RBUTTON];
			touch.tapCount = 1;

			s32 x = (int)(short)LOWORD(lParam);
			s32 y = (int)(short)HIWORD(lParam);

			touch.phase = TouchPhase_End;
			touch.posX = (f32)x;
			touch.posY = (f32)y;

			STouchCache& cache = CacheButtonDown[WIN_TOUCH_RBUTTON];

			touch.deltaTime = time - cache.time;
			if (touch.deltaTime < 0.0f)
				touch.deltaTime += FLT_MAX;
			touch.deltaX = touch.posX - cache.posX;
			touch.deltaY = touch.posY - cache.posY;

			if (touch.deltaTime <= GetDoubleClickTime() * 0.001f &&
				fabs(touch.deltaX) < g_doubleTapMaxOffset &&
				fabs(touch.deltaY) < g_doubleTapMaxOffset)
			{
				touch.tapCount = 2;
			}

			//save cache
// 			cache.time = time;
// 			cache.posX = touch.posX;
// 			cache.posY = touch.posY;

			readTouchInfo(&touch, 1);
			::ReleaseCapture();
		}
		break;
	case WM_MOUSEMOVE:
		{
			SGesTouchInfo moveTouch[2];

			u32 index = WIN_TOUCH_NONE;

			if (wParam & MK_LBUTTON)
			{		
				f32 time = now * 0.001f;

				SGesTouchInfo& left = moveTouch[index];
				left.fingerID = FingerId[WIN_TOUCH_LBUTTON];
				left.tapCount = 1;

				s32 x = (int)(short)LOWORD(lParam);
				s32 y = (int)(short)HIWORD(lParam);

				left.phase = TouchPhase_Move;
				left.posX = (f32)x;
				left.posY = (f32)y;

				STouchCache& cache = CacheButtonDown[WIN_TOUCH_LBUTTON];

				left.deltaTime = time - cache.time;
				if (left.deltaTime < 0.0f)
					left.deltaTime += FLT_MAX;
				left.deltaX = left.posX - cache.posX;
				left.deltaY = left.posY - cache.posY;

				//save cache
				cache.time = time;
				cache.posX = left.posX;
				cache.posY = left.posY;

				++index;
			}

			if (wParam & MK_RBUTTON)
			{
				f32 time = now * 0.001f;

				SGesTouchInfo& right = moveTouch[index];
				right.fingerID = FingerId[WIN_TOUCH_RBUTTON];
				right.tapCount = 1;

				s32 x = (int)(short)LOWORD(lParam);
				s32 y = (int)(short)HIWORD(lParam);

				right.phase = TouchPhase_Move;
				right.posX = (f32)x;
				right.posY = (f32)y;

				STouchCache& cache = CacheButtonDown[WIN_TOUCH_RBUTTON];

				right.deltaTime = time - cache.time;
				if (right.deltaTime < 0.0f)
					right.deltaTime += FLT_MAX;
				right.deltaX = right.posX - cache.posX;
				right.deltaY = right.posY - cache.posY;

				//save cache
				cache.time = time;
				cache.posX = right.posX;
				cache.posY = right.posY;

				++index;
			}

			if (index > WIN_TOUCH_NONE)
				readTouchInfo(moveTouch, index);
		}
		break;
	case WM_MOUSEWHEEL:
		{
			s32 zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
			f32 score = zDelta / 120.0f;
			score = 1.0f + 0.1f * score;
			if (score < 0.1f)
				score = 0.1f;

			SGestureInfo gesInfo;
			InitializeListHead(&gesInfo.Link);
			gesInfo.type = GestureType_Pinch;
			gesInfo.state = GestureState_Begin;
			gesInfo.param.pinch_scale = score;
			gesInfo.param.pinch_velocity = score - 1.0f;
			addGestureToQueue(gesInfo);

			gesInfo.state = GestureState_Change;
			addGestureToQueue(gesInfo);

			gesInfo.state = GestureState_End;
			addGestureToQueue(gesInfo);
		}
		break;
	default:
		break;
	}
}

#endif


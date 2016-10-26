#include "stdafx.h"
#include "CGestureReader.h"

#ifdef MW_PLATFORM_IOS

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

#include "CGestureReader.h"

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <UIKit/UIGestureRecognizerSubclass.h>

#define MAX_TOUCH_POINT		32

//ios code

@interface CUIRecognizerHookIOS : UIGestureRecognizer
{
    CGestureReader*     m_GestureReader;
    CGestureReader::T_TouchCacheMap		m_TouchCacheMap;
	NSTimer*		m_idTimer;
}
- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event;
- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event;
- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event;
- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event;
- (BOOL)canPreventGestureRecognizer:(UIGestureRecognizer *)preventedGestureRecognizer;
- (BOOL)canBePreventedByGestureRecognizer:(UIGestureRecognizer *)preventingGestureRecognizer;
- (void)handleGesture;
- (void)timerFired:(NSTimer*) timer;
- (id) init;
- (void)setGestureReader:(CGestureReader*) reader;

@end

@implementation CUIRecognizerHookIOS

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	u32 now = g_Engine->getTimer()->getMillisecond();
	f32 time = now * 0.001f;

    SGesTouchInfo touchArray[MAX_TOUCH_POINT];
	u32 size = min_((u32)touches.count, (u32)MAX_TOUCH_POINT);
	NSArray* allTouches = touches.allObjects;

	CGFloat scale = g_Engine->getWindowInfo().scale;

	for (u32 i=0; i<size; ++i)
	{
		SGesTouchInfo& touch = touchArray[i];

		UITouch* pRawTouch = allTouches[i];
		CGPoint ptTouch = [pRawTouch locationInView:self.view];
		//CGPoint ptPrev = [pRawTouch previousLocationInView:self.view];

		touch.fingerID = (ptr_t)( void*)pRawTouch;
		touch.tapCount = (s32)pRawTouch.tapCount;

		touch.phase = TouchPhase_Begin;
		touch.posX = ptTouch.x * scale;
		touch.posY = ptTouch.y * scale;

		touch.deltaTime =0.0f;
		touch.deltaX = 0.0f; //touch.posX - ptPrev.x;
		touch.deltaY = 0.0f; //touch.posY - ptPrev.y;

		CGestureReader::STouchCache& cache = m_TouchCacheMap[touch.fingerID];
		//save cache
		cache.time = time;
		cache.posX = touch.posX;
		cache.posY = touch.posY;
	}

	m_GestureReader->readTouchInfo(touchArray, size);
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    u32 now = g_Engine->getTimer()->getMillisecond();
	f32 time = now * 0.001f;

    SGesTouchInfo touchArray[MAX_TOUCH_POINT];
	u32 size = min_((u32)touches.count, (u32)MAX_TOUCH_POINT);
	NSArray* allTouches = touches.allObjects;

	CGFloat scale = g_Engine->getWindowInfo().scale;

	for (u32 i=0; i<size; ++i)
	{
		SGesTouchInfo& touch = touchArray[i];

		UITouch* pRawTouch = allTouches[i];
		CGPoint ptTouch = [pRawTouch locationInView:self.view];

		touch.fingerID = (ptr_t)( void*)pRawTouch;
		touch.tapCount = (s32)pRawTouch.tapCount;

		touch.phase = TouchPhase_Move;
		touch.posX = ptTouch.x * scale;
		touch.posY = ptTouch.y * scale;
		
		CGestureReader::STouchCache& cache = m_TouchCacheMap[touch.fingerID];

		touch.deltaTime = time - cache.time;
		if (touch.deltaTime < 0.0f)
			touch.deltaTime += FLT_MAX;
		touch.deltaX = touch.posX - cache.posX;
		touch.deltaY = touch.posY - cache.posY;

		//save cache
		cache.time = time;
		cache.posX = touch.posX;
		cache.posY = touch.posY;
	}

	m_GestureReader->readTouchInfo(touchArray, size);
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
	u32 now = g_Engine->getTimer()->getMillisecond();
	f32 time = now * 0.001f;

    SGesTouchInfo touchArray[MAX_TOUCH_POINT];
	u32 size = min_((u32)touches.count, (u32)MAX_TOUCH_POINT);
	NSArray* allTouches = touches.allObjects;

	CGFloat scale = g_Engine->getWindowInfo().scale;

	for (u32 i=0; i<size; ++i)
	{
		SGesTouchInfo& touch = touchArray[i];

		UITouch* pRawTouch = allTouches[i];
		CGPoint ptTouch = [pRawTouch locationInView:self.view];

		touch.fingerID = (ptr_t)( void*)pRawTouch;
		touch.tapCount = (s32)pRawTouch.tapCount;

		touch.phase = TouchPhase_End;
		touch.posX = ptTouch.x * scale;
		touch.posY = ptTouch.y * scale;
		
		CGestureReader::T_TouchCacheMap::iterator itr = m_TouchCacheMap.find(touch.fingerID);
		if(itr != m_TouchCacheMap.end())
		{
			touch.deltaTime = time - itr->second.time;
			if (touch.deltaTime < 0.0f)
				touch.deltaTime += FLT_MAX;
			touch.deltaX = touch.posX - itr->second.posX;
			touch.deltaY = touch.posY - itr->second.posY;

			//erase cache
			m_TouchCacheMap.erase(itr);
		}
		else
		{
			touch.deltaTime = 0.0f;
			touch.deltaX = 0.0f;
			touch.deltaY = 0.0f;
		}
	}

	m_GestureReader->readTouchInfo(touchArray, size);
}

- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    u32 now = g_Engine->getTimer()->getMillisecond();
	f32 time = now * 0.001f;

    SGesTouchInfo touchArray[MAX_TOUCH_POINT];
	u32 size = min_((u32)touches.count, (u32)MAX_TOUCH_POINT);
	NSArray* allTouches = touches.allObjects;

	CGFloat scale = g_Engine->getWindowInfo().scale;

	for (u32 i=0; i<size; ++i)
	{
		SGesTouchInfo& touch = touchArray[i];

		UITouch* pRawTouch = allTouches[i];
		CGPoint ptTouch = [pRawTouch locationInView:self.view];

		touch.fingerID = (ptr_t)( void*)pRawTouch;
		touch.tapCount = (s32)pRawTouch.tapCount;

		touch.phase = TouchPhase_Cancel;
		touch.posX = ptTouch.x * scale;
		touch.posY = ptTouch.y * scale;
		
		CGestureReader::T_TouchCacheMap::iterator itr = m_TouchCacheMap.find(touch.fingerID);
		if(itr != m_TouchCacheMap.end())
		{
			touch.deltaTime = time - itr->second.time;
			if (touch.deltaTime < 0.0f)
				touch.deltaTime += FLT_MAX;
			touch.deltaX = touch.posX - itr->second.posX;
			touch.deltaY = touch.posY - itr->second.posY;

			//erase cache
			m_TouchCacheMap.erase(itr);
		}
		else
		{
			touch.deltaTime = 0.0f;
			touch.deltaX = 0.0f;
			touch.deltaY = 0.0f;
		}
	}

	m_GestureReader->readTouchInfo(touchArray, size);
}

- (BOOL)canPreventGestureRecognizer:(UIGestureRecognizer *)preventedGestureRecognizer
{
    return NO;
}

- (BOOL)canBePreventedByGestureRecognizer:(UIGestureRecognizer *)preventingGestureRecognizer
{
    return NO;
}

- (void)handleGesture
{
    
}

- (void)timerFired:(NSTimer*) timer
{
	g_Engine->getGestureReader()->tick();
}

- (id)init
{
	id rv = [super initWithTarget:self action:@selector(handleGesture)];
	self.cancelsTouchesInView = NO;
	self.delaysTouchesBegan = NO;
	self.delaysTouchesEnded = NO;
	return rv;
}

- (void)setGestureReader:(CGestureReader *)reader
{
    m_GestureReader = reader;
    
    m_idTimer = [NSTimer timerWithTimeInterval:0.02 target:self selector:@selector(timerFired:) userInfo:nil repeats:YES];
	[[NSRunLoop currentRunLoop] addTimer:m_idTimer forMode:NSDefaultRunLoopMode];
}

@end

//
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

	//
	UIApplication* app = [UIApplication sharedApplication];
	UIWindow* win = app.windows[0];
    CUIRecognizerHookIOS* recognizer = [[CUIRecognizerHookIOS alloc] init];
    [recognizer setGestureReader:this];
	[win.rootViewController.view addGestureRecognizer:recognizer];
}

CGestureReader::~CGestureReader()
{
	DESTROY_LOCK(&cs);

	for (u32 i=0; i<GestureType_Num; ++i)
	{
		delete GestureRecognizers[i];
	}
}

#endif
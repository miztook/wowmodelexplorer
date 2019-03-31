#pragma once

#include "linklist.h"

//input

enum E_TOUCH_PHASE
{
	TouchPhase_Begin = 0,
	TouchPhase_Move,
	TouchPhase_Stationary,
	TouchPhase_End,
	TouchPhase_Cancel,
};

struct SGesTouchInfo
{
	SGesTouchInfo() : skip(false) {}
	
	uintptr_t		fingerID;
	float		posX;
	float		posY;
	float		deltaX;
	float		deltaY;
	float		deltaTime;	
	int32_t		tapCount;
	E_TOUCH_PHASE		phase;				//E_TOUCH_PHASE
	bool		skip;
};

//output
enum E_GESTURE_TYPE
{
	GestureType_None = 0,

	GestureType_Joystick,			//虚拟摇杆
	GestureType_Tap_1,			//单指轻拍
	GestureType_Tap_2,			//双指轻拍
	GestureType_Pinch,			//双指捏合
	GestureType_Pan_1,			//单指拖动
	GestureType_Pan_2,			//双指拖动
	GestureType_LongPress,		//单指长按
	GestureType_Swipe,			//快速滑动
	GestureType_Circle,			//单指画圈

	GestureType_Num,
};

inline const char* getGestureTypeName(E_GESTURE_TYPE type)
{
	switch (type)
	{
	case GestureType_Joystick:
		return "GestureType_Joystick";
	case GestureType_Tap_1:
		return "GestureType_Tap1";
	case GestureType_Tap_2:
		return "GestureType_Tap2";
	case GestureType_Pinch:
		return "GestureType_Pinch";
	case GestureType_Pan_1:
		return "GestureType_Pan1";
	case GestureType_Pan_2:
		return "GestureType_Pan2";
	case GestureType_LongPress:
		return "GestureType_Longpress";
	case GestureType_Swipe:
		return "GestureType_Swipe";
	case GestureType_Circle:
		return "GestureType_Circle";
	default:
		ASSERT(false);
		return "GestureType_None";
	}
}

enum E_GESTURE_STATE
{
	GestureState_None = 0,
	
	GestureState_Begin,
	GestureState_Change,
	GestureState_End,

	GestureState_Num,
};

inline const char* getGestureStateName(E_GESTURE_STATE state)
{
	switch(state)
	{
	case GestureState_Begin:
		return "GestureState_Begin";
	case GestureState_Change:
		return "GestureState_Change";
	case GestureState_End:
		return "GestureState_End";
	default:
		ASSERT(false);
		return "GestureState_None";
	}
}

enum E_SWIPE_DIR
{
	SwipeDir_None = 0,
	SwipeDir_Left,
	SwipeDir_Right,
	SwipeDir_Up,
	SwipeDir_Down,

};

enum E_CICLE_DIR
{
	CircleDir_None = 0,
	CircleDir_Clockwise,
	CircleDir_CounterClockwise,
};

struct SGestureInfo
{
	union Params
	{
		struct
		{
			float joystick_xPos;
			float joystick_yPos;
			float joystick_xDelta;
			float joystick_yDelta;
		};

		struct
		{
			float tap1_xPos;
			float tap1_yPos;
			int32_t tap1_tapCount;
		};

		struct
		{
			float tap2_xPos;
			float tap2_yPos;
		};

		struct
		{
			float pinch_scale;
			float pinch_velocity;
		};

		struct
		{
			float pan1_xPos;
			float pan1_yPos;
			float pan1_xVelocity;
			float pan1_yVelocity;
		};

		struct
		{
			float pan2_xPos;
			float pan2_yPos;
			float pan2_xVelocity;
			float pan2_yVelocity;
		};

		struct
		{
			float longpress_xPos;
			float longpress_yPos;
		};

		struct
		{
			float swipe_xPos;
			float swipe_yPos;
			E_SWIPE_DIR swipe_direction;
		};

		struct
		{
			E_CICLE_DIR circle_direction;
		};
	};

	LENTRY		Link;
	E_GESTURE_TYPE		type;
	E_GESTURE_STATE	state;

	Params param; 
};


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
	
	ptr_t		fingerID;
	f32		posX;
	f32		posY;
	f32		deltaX;
	f32		deltaY;
	f32		deltaTime;	
	s32		tapCount;
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

inline const c8* getGestureTypeName(E_GESTURE_TYPE type)
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

inline const c8* getGestureStateName(E_GESTURE_STATE state)
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
			f32 joystick_xPos;
			f32 joystick_yPos;
			f32 joystick_xDelta;
			f32 joystick_yDelta;
		};

		struct
		{
			f32 tap1_xPos;
			f32 tap1_yPos;
			s32 tap1_tapCount;
		};

		struct
		{
			f32 tap2_xPos;
			f32 tap2_yPos;
		};

		struct
		{
			f32 pinch_scale;
			f32 pinch_velocity;
		};

		struct
		{
			f32 pan1_xPos;
			f32 pan1_yPos;
			f32 pan1_xVelocity;
			f32 pan1_yVelocity;
		};

		struct
		{
			f32 pan2_xPos;
			f32 pan2_yPos;
			f32 pan2_xVelocity;
			f32 pan2_yVelocity;
		};

		struct
		{
			f32 longpress_xPos;
			f32 longpress_yPos;
		};

		struct
		{
			f32 swipe_xPos;
			f32 swipe_yPos;
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


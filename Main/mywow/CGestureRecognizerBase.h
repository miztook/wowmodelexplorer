#pragma once

#include "IGestureRecognizer.h"
#include "core.h"

class CGestureReader;

class CGestureRecognizerBase : public IGestureRecognizer
{
public:
	explicit CGestureRecognizerBase(CGestureReader* gestureReader);
	virtual ~CGestureRecognizerBase();

public:
	virtual void setHelperFunction_Joystick(fn_isValidJoystickPosition callback) {}

	virtual void tick();

	virtual void reset();		//clear all touch info

	virtual void onEnterState(E_RECOGNIZER_STATE state);

	E_RECOGNIZER_STATE getState() const { return State; }

protected:
	bool changeState(E_RECOGNIZER_STATE state);

	s32 findTouchInfo(ptr_t fingerId, const std::vector<SGesTouchInfo>& touchInfos);

protected:
	std::vector<SGesTouchInfo>		TouchInfoArray;
	u32 TimeStamp;

	CGestureReader*			GestureReader;
	E_GESTURE_TYPE			Type;
	E_RECOGNIZER_STATE		State;

//utility
protected:		
	static f32 getTouchDistanceSQ(const SGesTouchInfo& touch1, const SGesTouchInfo& touch2);
	static f32 getTouchDistance(const SGesTouchInfo& touch1, const SGesTouchInfo& touch2);
	static f32 getTouchDot(const SGesTouchInfo& touch1, const SGesTouchInfo& touch2);

	static f32 getfPinchVelocityLowpass() { return 0.8f; }
	static f32 getfPanVelocityLowpass() { return 0.8f; }
	static f32 getfLongPressTrigInterval() { return 1.5f; }
	static f32 getfRecognizerFailInterval() { return 0.8f; }
	static f32 getfDoubleTouchDiffTime() { return 0.1f; }
	static f32 getfSwipeMaxStayTime() { return 0.4f; }
	static f32 getfPinchDoubleTouchDiffTime() { return 0.5f; }

	static f32 getThreshold_MoveDist();
	static f32 getThreshold_PanMoveDist();
	static f32 getThreshold_Pan2MoveDist();
	static f32 getThreshold_PinchDist();
	static f32 getThreshold_SwipeDist();
	static f32 getThreshold_SwipeSpeedDist();
	static f32 getThreshold_PanMaxSpeedDist();
	static f32 getThreshold_CalcAngleDist();
	static f32 getThreshold_Pan2TouchSepDist();
	static f32 getThreshold_Tap2TouchSepDist();
	static f32 getThreshold_Pan2MoveDir();
	static f32 getThreshold_SwipeDirValid();

};

inline f32 CGestureRecognizerBase::getTouchDistanceSQ(const SGesTouchInfo& touch1, const SGesTouchInfo& touch2)
{
	return vector2df(touch1.posX, touch1.posY).getDistanceFromSQ(vector2df(touch2.posX, touch2.posY));
}

inline f32 CGestureRecognizerBase::getTouchDistance(const SGesTouchInfo& touch1, const SGesTouchInfo& touch2)
{
	return vector2df(touch1.posX, touch1.posY).getDistanceFrom(vector2df(touch2.posX, touch2.posY));
}

inline f32 CGestureRecognizerBase::getTouchDot(const SGesTouchInfo& touch1, const SGesTouchInfo& touch2)
{
	return  vector2df(touch1.deltaX, touch1.deltaY).dotProduct(vector2df(touch2.deltaX, touch2.deltaY));
}
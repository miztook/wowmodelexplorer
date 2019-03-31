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

	int32_t findTouchInfo(uintptr_t fingerId, const std::vector<SGesTouchInfo>& touchInfos);

protected:
	std::vector<SGesTouchInfo>		TouchInfoArray;
	uint32_t TimeStamp;

	CGestureReader*			GestureReader;
	E_GESTURE_TYPE			Type;
	E_RECOGNIZER_STATE		State;

//utility
protected:		
	static float getTouchDistanceSQ(const SGesTouchInfo& touch1, const SGesTouchInfo& touch2);
	static float getTouchDistance(const SGesTouchInfo& touch1, const SGesTouchInfo& touch2);
	static float getTouchDot(const SGesTouchInfo& touch1, const SGesTouchInfo& touch2);

	static float getfPinchVelocityLowpass() { return 0.8f; }
	static float getfPanVelocityLowpass() { return 0.8f; }
	static float getfLongPressTrigInterval() { return 1.5f; }
	static float getfRecognizerFailInterval() { return 0.8f; }
	static float getfDoubleTouchDiffTime() { return 0.1f; }
	static float getfSwipeMaxStayTime() { return 0.4f; }
	static float getfPinchDoubleTouchDiffTime() { return 0.5f; }

	static float getThreshold_MoveDist();
	static float getThreshold_PanMoveDist();
	static float getThreshold_Pan2MoveDist();
	static float getThreshold_PinchDist();
	static float getThreshold_SwipeDist();
	static float getThreshold_SwipeSpeedDist();
	static float getThreshold_PanMaxSpeedDist();
	static float getThreshold_CalcAngleDist();
	static float getThreshold_Pan2TouchSepDist();
	static float getThreshold_Tap2TouchSepDist();
	static float getThreshold_Pan2MoveDir();
	static float getThreshold_SwipeDirValid();

};

inline float CGestureRecognizerBase::getTouchDistanceSQ(const SGesTouchInfo& touch1, const SGesTouchInfo& touch2)
{
	return vector2df(touch1.posX, touch1.posY).getDistanceFromSQ(vector2df(touch2.posX, touch2.posY));
}

inline float CGestureRecognizerBase::getTouchDistance(const SGesTouchInfo& touch1, const SGesTouchInfo& touch2)
{
	return vector2df(touch1.posX, touch1.posY).getDistanceFrom(vector2df(touch2.posX, touch2.posY));
}

inline float CGestureRecognizerBase::getTouchDot(const SGesTouchInfo& touch1, const SGesTouchInfo& touch2)
{
	return  vector2df(touch1.deltaX, touch1.deltaY).dotProduct(vector2df(touch2.deltaX, touch2.deltaY));
}
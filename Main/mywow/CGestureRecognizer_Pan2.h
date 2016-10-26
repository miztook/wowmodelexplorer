#pragma once

#include "CGestureRecognizerBase.h"

class CGestureReader;

class CGestureRecognizer_Pan2 : public CGestureRecognizerBase
{
public:
	explicit CGestureRecognizer_Pan2(CGestureReader* gestureReader);

public:
	virtual void tick();

	virtual void reset();

	virtual void onTouchBegin(const std::vector<SGesTouchInfo>& touchInfos);
	virtual void onTouchMove(const std::vector<SGesTouchInfo>& touchInfos);
	virtual void onTouchEnd(const std::vector<SGesTouchInfo>& touchInfos);
	virtual void onTouchCancel(const std::vector<SGesTouchInfo>& touchInfos);

	virtual void onEnterState(E_RECOGNIZER_STATE state);

private:
	bool checkPan2MoveDelta(f32 deltaX1, f32 deltaY1, f32 deltaX2, f32 deltaY2);
	bool checkPan2TouchDistance(const SGesTouchInfo& touch1, const SGesTouchInfo& touch2);
	void outputPan2Gesture(E_GESTURE_STATE state);

private:
	f32 BeginPosX;
	f32 BeginPosY;
	f32 LastPosX;
	f32 LastPosY;
	f32 LastVelocityX;
	f32 LastVelocityY;
};
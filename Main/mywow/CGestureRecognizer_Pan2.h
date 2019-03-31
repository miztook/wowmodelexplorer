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
	bool checkPan2MoveDelta(float deltaX1, float deltaY1, float deltaX2, float deltaY2);
	bool checkPan2TouchDistance(const SGesTouchInfo& touch1, const SGesTouchInfo& touch2);
	void outputPan2Gesture(E_GESTURE_STATE state);

private:
	float BeginPosX;
	float BeginPosY;
	float LastPosX;
	float LastPosY;
	float LastVelocityX;
	float LastVelocityY;
};
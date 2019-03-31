#pragma once

#include "CGestureRecognizerBase.h"

class CGestureReader;

class CGestureRecognizer_Pan1 : public CGestureRecognizerBase
{
public:
	explicit CGestureRecognizer_Pan1(CGestureReader* gestureReader);

public:
	virtual void tick();

	virtual void reset();

	virtual void onTouchBegin(const std::vector<SGesTouchInfo>& touchInfos);
	virtual void onTouchMove(const std::vector<SGesTouchInfo>& touchInfos);
	virtual void onTouchEnd(const std::vector<SGesTouchInfo>& touchInfos);
	virtual void onTouchCancel(const std::vector<SGesTouchInfo>& touchInfos);

	virtual void onEnterState(E_RECOGNIZER_STATE state);

private:
	bool checkTouchMove(const SGesTouchInfo& touchInfo);
	void outputPanGesture(E_GESTURE_STATE state);

private:
	float		BeginPosX;
	float		BeginPosY;
	float		LastVelocityX;
	float		LastVelocityY;
	bool		Moving;
};
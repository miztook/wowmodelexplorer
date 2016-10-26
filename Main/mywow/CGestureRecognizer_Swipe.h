#pragma once

#include "CGestureRecognizerBase.h"

class CGestureReader;

class CGestureRecognizer_Swipe : public CGestureRecognizerBase
{
public:
	explicit CGestureRecognizer_Swipe(CGestureReader* gestureReader);

public:
	virtual void tick();

	virtual void reset();

	virtual void onTouchBegin(const std::vector<SGesTouchInfo>& touchInfos);
	virtual void onTouchMove(const std::vector<SGesTouchInfo>& touchInfos);
	virtual void onTouchEnd(const std::vector<SGesTouchInfo>& touchInfos);
	virtual void onTouchCancel(const std::vector<SGesTouchInfo>& touchInfos);

	virtual void onEnterState(E_RECOGNIZER_STATE state);

private:
	void outputSwipeGesture(E_GESTURE_STATE state);
	E_SWIPE_DIR calcSwipeDirection();

private:
	f32 ChangePosX;
	f32 ChangePosY;
};
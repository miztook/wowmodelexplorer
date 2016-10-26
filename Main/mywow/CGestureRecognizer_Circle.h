#pragma once

#include "CGestureRecognizerBase.h"

class CGestureReader;

class CGestureRecognizer_Circle : public CGestureRecognizerBase
{
public:
	explicit CGestureRecognizer_Circle(CGestureReader* gestureReader);

public:
	virtual void tick();

	virtual void reset();

	virtual void onTouchBegin(const std::vector<SGesTouchInfo>& touchInfos);
	virtual void onTouchMove(const std::vector<SGesTouchInfo>& touchInfos);
	virtual void onTouchEnd(const std::vector<SGesTouchInfo>& touchInfos);
	virtual void onTouchCancel(const std::vector<SGesTouchInfo>& touchInfos);

	virtual void onEnterState(E_RECOGNIZER_STATE state);

private:
	E_CICLE_DIR calcNewTouchDir(const SGesTouchInfo& touchInfo);
	bool checkNewTouchSpeed(const SGesTouchInfo& touchInfo);
	bool checkGestureComplete();
	void outputCircleGesture(E_GESTURE_STATE state);

private:
	E_CICLE_DIR		Direction;
};
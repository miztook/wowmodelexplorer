#pragma once

#include "CGestureRecognizerBase.h"

class CGestureReader;

class CGestureRecognizer_Tap2 : public CGestureRecognizerBase
{
public:
	explicit CGestureRecognizer_Tap2(CGestureReader* gestureReader);

public:
	virtual void tick();

	virtual void onTouchBegin(const std::vector<SGesTouchInfo>& touchInfos);
	virtual void onTouchMove(const std::vector<SGesTouchInfo>& touchInfos);
	virtual void onTouchEnd(const std::vector<SGesTouchInfo>& touchInfos);
	virtual void onTouchCancel(const std::vector<SGesTouchInfo>& touchInfos);

	virtual void onEnterState(E_RECOGNIZER_STATE state);

	bool checkForbidGestures();

	bool checkTap2TouchDistance(const SGesTouchInfo& touch1, const SGesTouchInfo& touch2);

private:
	void outputTap2Gesture(E_GESTURE_STATE state);
};
#pragma once

#include "CGestureRecognizerBase.h"

class CGestureReader;

//recognizer
class CGestureRecognizer_Tap1 : public CGestureRecognizerBase
{
public:
	explicit CGestureRecognizer_Tap1(CGestureReader* gestureReader);

public:
	virtual void tick();

	virtual void onTouchBegin(const std::vector<SGesTouchInfo>& touchInfos);
	virtual void onTouchMove(const std::vector<SGesTouchInfo>& touchInfos);
	virtual void onTouchEnd(const std::vector<SGesTouchInfo>& touchInfos);
	virtual void onTouchCancel(const std::vector<SGesTouchInfo>& touchInfos);

	virtual void onEnterState(E_RECOGNIZER_STATE state);

	bool checkForbidGestures();

private:
	void outputTapGesture(E_GESTURE_STATE state);
};
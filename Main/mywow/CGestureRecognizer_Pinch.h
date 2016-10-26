#pragma once

#include "CGestureRecognizerBase.h"

class CGestureReader;

class CGestureRecognizer_Pinch : public CGestureRecognizerBase
{
public:
	explicit CGestureRecognizer_Pinch(CGestureReader* gestureReader);

public:
	virtual void tick();

	virtual void reset();

	virtual void onTouchBegin(const std::vector<SGesTouchInfo>& touchInfos);
	virtual void onTouchMove(const std::vector<SGesTouchInfo>& touchInfos);
	virtual void onTouchEnd(const std::vector<SGesTouchInfo>& touchInfos);
	virtual void onTouchCancel(const std::vector<SGesTouchInfo>& touchInfos);

	virtual void onEnterState(E_RECOGNIZER_STATE state);

private:
	bool checkPinchMoveDir();
	void outputPinchGesture(E_GESTURE_STATE state);

private:
	f32 OriginalDistance;		//distance between two fingers
	f32 LastScale;
	f32 LastVelocity;
};
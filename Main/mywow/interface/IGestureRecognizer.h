#pragma once

#include "base.h"
#include "GestureDef.h"
#include <vector>

typedef bool (*fn_isValidJoystickPosition)(s32 x, s32 y);

class IGestureRecognizer
{
public:
	virtual ~IGestureRecognizer() {}

public:
	enum E_RECOGNIZER_STATE
	{
		RecognizerState_None = 0,
		RecognizerState_Begin,
		RecognizerState_Changed,
		RecognizerState_End,
		RecognizerState_Cancel,
		RecognizerState_Fail,
	};

public:
	virtual void setHelperFunction_Joystick(fn_isValidJoystickPosition callback) = 0;

	virtual void tick() = 0;

	virtual void reset() = 0;		//clear all touch info

	virtual void onTouchBegin(const std::vector<SGesTouchInfo>& touchInfos) = 0;
	virtual void onTouchMove(const std::vector<SGesTouchInfo>& touchInfos) = 0;
	virtual void onTouchEnd(const std::vector<SGesTouchInfo>& touchInfos) = 0;
	virtual void onTouchCancel(const std::vector<SGesTouchInfo>& touchInfos) = 0;

	virtual void onEnterState(E_RECOGNIZER_STATE state) = 0;
};
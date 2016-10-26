#pragma once

#include "base.h"

class IInputListener
{
public:
	virtual ~IInputListener() {}

public:
	virtual void onMouseMessage(window_type hwnd, E_INPUT_MESSAGE message, int x, int y) = 0;
	virtual void onMouseWheel(window_type hwnd, int zDelta) = 0;
	virtual void onKeyMessage(window_type hwnd, E_INPUT_MESSAGE message, int key) = 0;
};
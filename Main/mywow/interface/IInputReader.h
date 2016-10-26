#pragma once

#include "base.h"

class IInputListener;

//输入有两种方式, 一种是鼠标键盘类的即时输入, 需要用listener来处理, 需要迅速响应而且必须在同一个线程
//另一种是缓存类的输入, 如触摸, 需要建立一个队列，在每一帧取队列来处理，可以不在同一个线程
class IInputReader
{
public:
	virtual ~IInputReader() {}

public:
	virtual bool acquire(E_INPUT_DEVICE device) = 0;
	virtual bool unacquire(E_INPUT_DEVICE device) = 0;
	virtual bool capture(E_INPUT_DEVICE device) = 0;
	virtual bool isKeyPressed(u8 keycode) = 0;
	virtual bool isMousePressed(E_MOUSE_BUTTON button) = 0;

	virtual void addListener(IInputListener* listener) = 0;
	virtual void removeListener(IInputListener* listener) = 0;
};
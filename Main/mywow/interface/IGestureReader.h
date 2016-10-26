#pragma once

#include "base.h"
#include "GestureDef.h"

//捕获gesture消息, 添加到队列
//需要维护多个recognizer, 负责管理gesture的状态
//接受gesture输入 -> recognizer处理 -> 入队列

class IGestureReader
{
public:
	virtual ~IGestureReader() {}

public:
	virtual void tick() = 0;

	virtual void readTouchInfo(const SGesTouchInfo* arrTouches, u32 count) = 0;

	virtual bool addGestureToQueue(const SGestureInfo& gesInfo) = 0;

	virtual bool removeGestureFromQueue(SGestureInfo& gesInfo) = 0;

	virtual bool isQueueEmpty() const = 0;
};
#include "stdafx.h"
#include "CGestureRecognizer_Longpress.h"
#include "mywow.h"
#include "CGestureReader.h"

//
//单指长按
//
CGestureRecognizer_Longpress::CGestureRecognizer_Longpress( CGestureReader* gestureReader )
	: CGestureRecognizerBase(gestureReader)
{
	Type = GestureType_LongPress;
}

void CGestureRecognizer_Longpress::tick()
{
	CGestureRecognizerBase::tick();

	if (State == RecognizerState_None && !TouchInfoArray.empty())
	{
		u32 now = g_Engine->getTimer()->getMillisecond();
		if (now - TimeStamp > (u32)(getfLongPressTrigInterval() * 1000))
			changeState(RecognizerState_Begin);
	}
}

void CGestureRecognizer_Longpress::onTouchBegin( const std::vector<SGesTouchInfo>& touchInfos )
{
	if (State != RecognizerState_None)
		return;

	if (!TouchInfoArray.empty())
	{
		// touch cached but long press have not begin
		changeState(RecognizerState_Fail);
	}
	else if (touchInfos.size() == 1)
	{
		TouchInfoArray.push_back(touchInfos.front());
		TimeStamp = g_Engine->getTimer()->getMillisecond();
	}
}

void CGestureRecognizer_Longpress::onTouchMove( const std::vector<SGesTouchInfo>& touchInfos )
{
	if (State == RecognizerState_None)
	{
		if (!TouchInfoArray.empty())
		{
			s32 index = findTouchInfo(TouchInfoArray.front().fingerID, touchInfos);
			if (index != -1)
			{
				f32 distance = getTouchDistance(TouchInfoArray.front(), touchInfos[index]);
				if (distance > getThreshold_MoveDist())
					changeState(RecognizerState_Fail);
			}
			else
			{
				changeState(RecognizerState_Fail);
			}
		}
		return;
	}

	if (TouchInfoArray.empty() ||
		(State != RecognizerState_Begin && State != RecognizerState_Changed))
		return;

	SGesTouchInfo& head = TouchInfoArray.front();
	s32 index = findTouchInfo(head.fingerID, touchInfos);
	if (index != -1)
	{
		head = touchInfos[index];
		outputLongpressGesture(GestureState_Change);
		changeState(RecognizerState_Changed);
	}
}

void CGestureRecognizer_Longpress::onTouchEnd( const std::vector<SGesTouchInfo>& touchInfos )
{
	if (State == RecognizerState_None)
	{
		if (!TouchInfoArray.empty())
			changeState(RecognizerState_Fail);
		return;
	}

	if (TouchInfoArray.empty() ||
		(State != RecognizerState_Begin && State != RecognizerState_Changed))
		return;

	SGesTouchInfo& head = TouchInfoArray.front();
	s32 index = findTouchInfo(head.fingerID, touchInfos);
	if (index != -1)
	{
		head = touchInfos[index];
		changeState(RecognizerState_End);
	}
}

void CGestureRecognizer_Longpress::onTouchCancel( const std::vector<SGesTouchInfo>& touchInfos )
{
	if (State == RecognizerState_None)
	{
		if (!TouchInfoArray.empty())
			changeState(RecognizerState_Fail);
		return;
	}

	if (TouchInfoArray.empty() ||
		(State != RecognizerState_Begin && State != RecognizerState_Changed))
		return;

	SGesTouchInfo& head = TouchInfoArray.front();
	s32 index = findTouchInfo(head.fingerID, touchInfos);
	if (index != -1)
	{
		head = touchInfos[index];
		changeState(RecognizerState_Cancel);
	}
}

void CGestureRecognizer_Longpress::onEnterState( E_RECOGNIZER_STATE state )
{
	if (state == RecognizerState_Begin)
	{
		outputLongpressGesture(GestureState_Begin);
		outputLongpressGesture(GestureState_Change);
	}
	else if (state == RecognizerState_End)
	{
		outputLongpressGesture(GestureState_End);
	}
	else if (state == RecognizerState_Cancel)
	{
		outputLongpressGesture(GestureState_End);
	}

	CGestureRecognizerBase::onEnterState(state);
}

void CGestureRecognizer_Longpress::outputLongpressGesture( E_GESTURE_STATE state )
{
	if (TouchInfoArray.empty())
	{
		ASSERT(false);
		return;
	}

	SGestureInfo gesInfo;
	InitializeListHead(&gesInfo.Link);

	gesInfo.type = Type;
	gesInfo.state = state;
	gesInfo.param.longpress_xPos = TouchInfoArray.front().posX;
	gesInfo.param.longpress_yPos = TouchInfoArray.front().posY;

	GestureReader->addGestureToQueue(gesInfo);
}
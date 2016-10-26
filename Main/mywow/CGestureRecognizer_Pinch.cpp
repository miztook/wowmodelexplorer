#include "stdafx.h"
#include "CGestureRecognizer_Pinch.h"
#include "mywow.h"
#include "CGestureReader.h"

//
//Ë«Ö¸ÄóºÏ
//
CGestureRecognizer_Pinch::CGestureRecognizer_Pinch( CGestureReader* gestureReader )
	: CGestureRecognizerBase(gestureReader)
{
	Type = GestureType_Pinch;

	OriginalDistance = 0.0f;
	LastScale = 0.0f;
	LastVelocity = 0.0f;
}

void CGestureRecognizer_Pinch::tick()
{
	CGestureRecognizerBase::tick();

	if (State == RecognizerState_None &&
		TouchInfoArray.size() == 1)
	{
		u32 now = g_Engine->getTimer()->getMillisecond();
		if (now - TimeStamp > (u32)(getfPinchDoubleTouchDiffTime() * 1000))
		{	
			changeState(RecognizerState_Fail);
		}
	}
}

void CGestureRecognizer_Pinch::reset()
{
	CGestureRecognizerBase::reset();
	OriginalDistance = 0.0f;
	LastScale = 0.0f;
	LastVelocity = 0.0f;
}

void CGestureRecognizer_Pinch::onTouchBegin( const std::vector<SGesTouchInfo>& touchInfos )
{
	if (State != RecognizerState_None || touchInfos.empty())
		return;

	u32 numCache = (u32)TouchInfoArray.size();
	u32 numBegin = (u32)touchInfos.size();
	if (numBegin + numCache > 2)
	{
		changeState(RecognizerState_Fail);
	}
	else
	{
		if (numCache == 1)			//1+1
		{
			TouchInfoArray.push_back(touchInfos.front());
			TimeStamp = g_Engine->getTimer()->getMillisecond();
			changeState(RecognizerState_Begin);
		}
		else
		{
			if (numBegin == 2)				//2
			{
				TouchInfoArray.push_back(touchInfos.front());
				TouchInfoArray.push_back(touchInfos.back());
				TimeStamp = g_Engine->getTimer()->getMillisecond();
				changeState(RecognizerState_Begin);
			}
			else				//1
			{
				TouchInfoArray.push_back(touchInfos.front());
				TimeStamp = g_Engine->getTimer()->getMillisecond();
			}
		}
	}
}

void CGestureRecognizer_Pinch::onTouchMove( const std::vector<SGesTouchInfo>& touchInfos )
{
	if (TouchInfoArray.size() != 2 ||
		(State != RecognizerState_Begin && State != RecognizerState_Changed))
		return;

	bool changeHead = false;
	bool changeTail = false;
	SGesTouchInfo& head = TouchInfoArray.front();
	SGesTouchInfo& tail = TouchInfoArray.back();
	s32 index0 = findTouchInfo(head.fingerID, touchInfos);
	s32 index1 = findTouchInfo(tail.fingerID, touchInfos);

	if (index0 != -1 &&
		getTouchDistance(head, touchInfos[index0]) > getThreshold_PinchDist())
	{
		head = touchInfos[index0];
		changeHead = true;
	}

	if (index1 != -1 &&
		getTouchDistance(tail, touchInfos[index1]) > getThreshold_PinchDist())
	{
		tail = touchInfos[index1];
		changeTail = true;
	}

	if (changeHead || changeTail)
	{
		if (checkPinchMoveDir())
		{
			if (State == RecognizerState_Begin)
			{
				outputPinchGesture(GestureState_Begin);
			}

			changeState(RecognizerState_Changed);
			outputPinchGesture(GestureState_Change);
		}
		else
		{
			changeState(RecognizerState_Begin);
		}
	}
}

void CGestureRecognizer_Pinch::onTouchEnd( const std::vector<SGesTouchInfo>& touchInfos )
{
	u32 size = (u32)TouchInfoArray.size();
	if (size == 1)
	{
		SGesTouchInfo& head = TouchInfoArray.front();
		s32 index = findTouchInfo(head.fingerID, touchInfos);
		if (index != -1)
		{
			head = touchInfos[index];
			changeState(RecognizerState_Fail);
		}
	}
	else if(size == 2)
	{
		SGesTouchInfo& head = TouchInfoArray.front();
		SGesTouchInfo& tail = TouchInfoArray.back();
		s32 index0 = findTouchInfo(head.fingerID, touchInfos);
		s32 index1 = findTouchInfo(tail.fingerID, touchInfos);
		if (index0 != -1 || index1 != -1)
		{
			if (index0 != -1)
				head = touchInfos[index0];

			if (index1 != -1)
				tail = touchInfos[index1];

			if (State == RecognizerState_Changed)
				outputPinchGesture(GestureState_End);

			changeState(RecognizerState_End);
		}
	}
}

void CGestureRecognizer_Pinch::onTouchCancel( const std::vector<SGesTouchInfo>& touchInfos )
{
	u32 size = (u32)TouchInfoArray.size();
	if (size == 1)
	{
		SGesTouchInfo& head = TouchInfoArray.front();
		s32 index = findTouchInfo(head.fingerID, touchInfos);
		if (index != -1)
		{
			head = touchInfos[index];
			changeState(RecognizerState_Fail);
		}
	}
	else if(size == 2)
	{
		SGesTouchInfo& head = TouchInfoArray.front();
		SGesTouchInfo& tail = TouchInfoArray.back();
		s32 index0 = findTouchInfo(head.fingerID, touchInfos);
		s32 index1 = findTouchInfo(tail.fingerID, touchInfos);
		if (index0 != -1 || index1 != -1)
		{
			if (index0 != -1)
				head = touchInfos[index0];

			if (index1)
				tail = touchInfos[index1];

			if (State == RecognizerState_Changed)
				outputPinchGesture(GestureState_End);

			changeState(RecognizerState_Cancel);
		}
	}
}

void CGestureRecognizer_Pinch::onEnterState( E_RECOGNIZER_STATE state )
{
	if (state == RecognizerState_Begin)
	{
		if (TouchInfoArray.size() != 2)
			return;

		const SGesTouchInfo& head = TouchInfoArray.front();
		const SGesTouchInfo& tail = TouchInfoArray.back();
		OriginalDistance = getTouchDistance(head, tail);
		LastScale = 1.0f;
	}

	CGestureRecognizerBase::onEnterState(state);
}

bool CGestureRecognizer_Pinch::checkPinchMoveDir()
{
	if (TouchInfoArray.size() != 2)
		return false;

	f32 dot = getTouchDot(TouchInfoArray.front(), TouchInfoArray.back());

	return dot < 0.0f; // 0.707f;		//cos(45)
}

void CGestureRecognizer_Pinch::outputPinchGesture( E_GESTURE_STATE state )
{
	if (TouchInfoArray.size() != 2)
		return;

	SGestureInfo gesInfo;
	InitializeListHead(&gesInfo.Link);

	gesInfo.type = Type;
	gesInfo.state = state;

	u32 now = g_Engine->getTimer()->getMillisecond();

	if (state == GestureState_Begin)
	{
		gesInfo.param.pinch_scale = 1.0f;
		gesInfo.param.pinch_velocity = 0.0f;
	}
	else
	{
		f32 distance = getTouchDistance(TouchInfoArray.front(), TouchInfoArray.back());
		u32 delta = now - TimeStamp;
		f32 curScale = OriginalDistance > 0.0f ? distance / OriginalDistance : 1.0f;
		f32 curVelocity = delta ? (curScale - LastScale) / (delta * 0.001f) : LastVelocity;

		gesInfo.param.pinch_scale = curScale;
		gesInfo.param.pinch_velocity = getfPinchVelocityLowpass() * curVelocity + (1.0f - getfPinchVelocityLowpass()) * LastVelocity;
	}

	TimeStamp = now;
	LastScale = gesInfo.param.pinch_scale;
	LastVelocity = gesInfo.param.pinch_velocity;

	GestureReader->addGestureToQueue(gesInfo);
}

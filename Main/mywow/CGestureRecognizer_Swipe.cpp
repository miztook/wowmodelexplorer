#include "stdafx.h"
#include "CGestureRecognizer_Swipe.h"
#include "mywow.h"
#include "CGestureReader.h"

//
//¿ìËÙ»¬¶¯
//
CGestureRecognizer_Swipe::CGestureRecognizer_Swipe( CGestureReader* gestureReader )
	: CGestureRecognizerBase(gestureReader)
{
	Type = GestureType_Swipe;

	ChangePosX = 0.0f;
	ChangePosY = 0.0f;
}

void CGestureRecognizer_Swipe::tick()
{
	CGestureRecognizerBase::tick();

	if (State == RecognizerState_Begin)
	{
		u32 now = g_Engine->getTimer()->getMillisecond();
		if (now - TimeStamp > (u32)(getfSwipeMaxStayTime() * 1000))
			changeState(RecognizerState_Fail);
	}
}

void CGestureRecognizer_Swipe::reset()
{
	CGestureRecognizerBase::reset();

	ChangePosX = 0.0f;
	ChangePosY = 0.0f;
}

void CGestureRecognizer_Swipe::onTouchBegin( const std::vector<SGesTouchInfo>& touchInfos )
{
	if (State == RecognizerState_Begin)
	{
		if (!touchInfos.empty())
		{
			//other finger begin, fail
			changeState(RecognizerState_Fail);
		}
	}
	else if (State == RecognizerState_None)
	{
		if (touchInfos.size() == 1)
		{
			TouchInfoArray.push_back(touchInfos.front());
			TimeStamp = g_Engine->getTimer()->getMillisecond();
			changeState(RecognizerState_Begin);
		}
	}
}

void CGestureRecognizer_Swipe::onTouchMove( const std::vector<SGesTouchInfo>& touchInfos )
{
	if (TouchInfoArray.empty() ||
		(State != RecognizerState_Begin && State != RecognizerState_Changed))
		return;

	SGesTouchInfo& head = TouchInfoArray.front();

	int index = findTouchInfo(head.fingerID, touchInfos);
	if (index != -1)
	{
		const SGesTouchInfo& touch = touchInfos[index];

		if(touchInfos[index].deltaTime < 0.001f ||
			getTouchDistance(head, touch) < getThreshold_SwipeDist())
			return;			//ignore
		
		head = touch;
		f32 speedX = touch.deltaTime > 0.0f ? touch.deltaX / touch.deltaTime : 0.0f;
		f32 speedY = touch.deltaTime > 0.0f ? touch.deltaY / touch.deltaTime : 0.0f;
		if (vector2df(speedX, speedY).getLength() > getThreshold_SwipeSpeedDist())
		{
			ChangePosX = touch.posX;
			ChangePosY = touch.posY;
		}
		else
		{
			changeState(RecognizerState_Fail);
		}
	}
	else
	{
		changeState(RecognizerState_Fail);
	}

}

void CGestureRecognizer_Swipe::onTouchEnd( const std::vector<SGesTouchInfo>& touchInfos )
{
	if (TouchInfoArray.empty())
		return;

	SGesTouchInfo& head = TouchInfoArray.front();

	s32 index = findTouchInfo(head.fingerID, touchInfos);
	if (index != -1)
	{
		const SGesTouchInfo& touch = touchInfos[index];

		if(touchInfos[index].deltaTime < 0.001f ||
			getTouchDistance(head, touch) < getThreshold_SwipeDist())
			return;			//ignore

		head = touch;
		f32 speedX = touch.deltaTime > 0.0f ? touch.deltaX / touch.deltaTime : 0.0f;
		f32 speedY = touch.deltaTime > 0.0f ? touch.deltaY / touch.deltaTime : 0.0f;
		if (vector2df(speedX, speedY).getLength() > getThreshold_SwipeSpeedDist())
		{
			changeState(RecognizerState_End);
		}
		else
		{
			changeState(RecognizerState_Fail);
		}
	}
	else
	{
		changeState(RecognizerState_Fail);
	}
}

void CGestureRecognizer_Swipe::onTouchCancel( const std::vector<SGesTouchInfo>& touchInfos )
{
	if (TouchInfoArray.empty())
		return;

	SGesTouchInfo& head = TouchInfoArray.front();
	s32 index = findTouchInfo(head.fingerID, touchInfos);
	if (index != -1)
	{
		head = touchInfos[index];
		changeState(RecognizerState_Cancel);
	}
}

void CGestureRecognizer_Swipe::onEnterState( E_RECOGNIZER_STATE state )
{
	if (state == RecognizerState_End)
		outputSwipeGesture(GestureState_Change);

	CGestureRecognizerBase::onEnterState(state);
}

void CGestureRecognizer_Swipe::outputSwipeGesture( E_GESTURE_STATE state )
{
	if (TouchInfoArray.empty())
		return;

	E_SWIPE_DIR dir = calcSwipeDirection();
	if (dir == SwipeDir_None)
		return;

	SGestureInfo gesInfo;
	InitializeListHead(&gesInfo.Link);

	gesInfo.type = Type;
	gesInfo.state = state;
	gesInfo.param.swipe_xPos = ChangePosX;
	gesInfo.param.swipe_yPos = ChangePosY;
	gesInfo.param.swipe_direction = dir;

	GestureReader->addGestureToQueue(gesInfo);
}

E_SWIPE_DIR CGestureRecognizer_Swipe::calcSwipeDirection()
{
	if (TouchInfoArray.empty())
		return SwipeDir_None;

	f32 deltaX = TouchInfoArray.front().deltaX;
	f32 deltaY = TouchInfoArray.front().deltaY;
	
	vector2df v(deltaX, deltaY);
	if (equals_(v.getLength(), 0.0f))
		return SwipeDir_None;

	v.normalize();

	f32 thresholdDir = getThreshold_SwipeDirValid();
	if (v.X > thresholdDir)
		return SwipeDir_Right;
	else if (v.X < -thresholdDir)
		return SwipeDir_Left;
	else if (v.Y > thresholdDir)
		return SwipeDir_Down;
	else if (v.Y < -thresholdDir)
		return SwipeDir_Up;
	else 
		return SwipeDir_None;
}
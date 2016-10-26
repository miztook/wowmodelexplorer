#include "stdafx.h"
#include "CGestureRecognizer_Pan2.h"
#include "mywow.h"
#include "CGestureReader.h"

//
//双指拖动
//
CGestureRecognizer_Pan2::CGestureRecognizer_Pan2( CGestureReader* gestureReader )
	: CGestureRecognizerBase(gestureReader)
{
	Type = GestureType_Pan_2;
	LastPosX = 0.0f;
	LastPosY = 0.0f;
	BeginPosX = 0.0f;
	BeginPosY = 0.0f;
	LastVelocityX = 0.0f;
	LastVelocityY = 0.0f;
}

void CGestureRecognizer_Pan2::tick()
{
	CGestureRecognizerBase::tick();

	if (State == RecognizerState_None && !TouchInfoArray.empty())
	{
		u32 now = g_Engine->getTimer()->getMillisecond();
		if (now - TimeStamp > (u32)(getfDoubleTouchDiffTime() * 1000))
			changeState(RecognizerState_Fail);
	}
	else if (State == RecognizerState_Begin)
	{
		u32 now = g_Engine->getTimer()->getMillisecond();
		if (now - TimeStamp > (u32)(getfRecognizerFailInterval() * 1000))
			changeState(RecognizerState_Fail);
	}
}

void CGestureRecognizer_Pan2::reset()
{
	CGestureRecognizerBase::reset();

	LastPosX = 0.0f;
	LastPosY = 0.0f;
	BeginPosX = 0.0f;
	BeginPosY = 0.0f;
	LastVelocityX = 0.0f;
	LastVelocityY = 0.0f;
}

void CGestureRecognizer_Pan2::onTouchBegin( const std::vector<SGesTouchInfo>& touchInfos )
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
			const SGesTouchInfo& head = TouchInfoArray.front();
			const SGesTouchInfo& tail = TouchInfoArray.back();

			TimeStamp = g_Engine->getTimer()->getMillisecond();
// 			if (!checkPan2TouchDistance(head, tail))
// 				changeState(RecognizerState_Fail);
// 			else
				changeState(RecognizerState_Begin);
		}
		else
		{
			if (numBegin == 2)				//2
			{
				TouchInfoArray.push_back(touchInfos.front());
				TouchInfoArray.push_back(touchInfos.back());
				
				const SGesTouchInfo& head = TouchInfoArray.front();
				const SGesTouchInfo& tail = TouchInfoArray.back();

				TimeStamp = g_Engine->getTimer()->getMillisecond();
// 				if (!checkPan2TouchDistance(head, tail))
// 					changeState(RecognizerState_Fail);
// 				else
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

void CGestureRecognizer_Pan2::onTouchMove( const std::vector<SGesTouchInfo>& touchInfos )
{
	if (TouchInfoArray.size() != 2 ||
		(State != RecognizerState_Begin && State != RecognizerState_Changed))
	return;

	f32 deltaX0 = 0.0f;
	f32 deltaY0 = 0.0f;
	f32 deltaX1 = 0.0f;
	f32 deltaY1 = 0.0f;

	SGesTouchInfo& head = TouchInfoArray.front();
	SGesTouchInfo& tail = TouchInfoArray.back();

	s32 index0 = findTouchInfo(head.fingerID, touchInfos);
	if (index0 != -1 &&
		getTouchDistance(head, touchInfos[index0]) > getThreshold_Pan2MoveDist())
	{
		deltaX0 = touchInfos[index0].posX - head.posX;
		deltaY0 = touchInfos[index0].posY - head.posY;
		head = touchInfos[index0];
	}
	else
	{
		return;
	}

	s32 index1 = findTouchInfo(tail.fingerID, touchInfos);
	if (index1 != -1 &&
		getTouchDistance(tail, touchInfos[index1]) > getThreshold_Pan2MoveDist())
	{
		deltaX1 = touchInfos[index1].posX - tail.posX;
		deltaY1 = touchInfos[index1].posY - tail.posY;
		tail = touchInfos[index1];
	}
	else
	{
		return;
	}

	//head, tail moved
	E_RECOGNIZER_STATE state = GestureReader->getGestureState(GestureType_Pinch);
	bool bPinch = ( state == IGestureRecognizer::RecognizerState_Begin || state == IGestureRecognizer::RecognizerState_Changed );
	if (bPinch || checkPan2MoveDelta(deltaX0, deltaY0, deltaX1, deltaY1))		//如果当前在pinch，不进行Pan2方向检查
// 		&&
// 		checkPan2TouchDistance(head, tail))
	{
		if (State == RecognizerState_Begin)
			outputPan2Gesture(GestureState_Begin);

		changeState(RecognizerState_Changed);
		outputPan2Gesture(GestureState_Change);

		TimeStamp = g_Engine->getTimer()->getMillisecond();
	}
	else
	{
		outputPan2Gesture(GestureState_End);
		changeState(RecognizerState_Fail);
	}
}

void CGestureRecognizer_Pan2::onTouchEnd( const std::vector<SGesTouchInfo>& touchInfos )
{
	if (TouchInfoArray.size() == 1)
	{
		SGesTouchInfo& head = TouchInfoArray.front();
		s32 index = findTouchInfo(head.fingerID, touchInfos);
		if (index != -1)
		{
			head = touchInfos[index];
			changeState(RecognizerState_Fail);
		}
	}
	else if (TouchInfoArray.size() == 2)
	{
		SGesTouchInfo& head = TouchInfoArray.front();
		SGesTouchInfo& tail = TouchInfoArray.back();
		s32 index0 = findTouchInfo(head.fingerID, touchInfos);
		s32 index1 = findTouchInfo(tail.fingerID, touchInfos);

		if (index0 != -1 || index1 != -1)
		{
			//一个touch结束
			if (index0 != -1)
				head = touchInfos[index0];

			if (index1 != -1)
				tail = touchInfos[index1];

			if (State == RecognizerState_Changed)
				outputPan2Gesture(GestureState_End);

			changeState(RecognizerState_End);
		}
	}
}

void CGestureRecognizer_Pan2::onTouchCancel( const std::vector<SGesTouchInfo>& touchInfos )
{
	if (TouchInfoArray.size() == 1)
	{
		SGesTouchInfo& head = TouchInfoArray.front();
		s32 index = findTouchInfo(head.fingerID, touchInfos);
		if (index != -1)
		{
			head = touchInfos[index];
			changeState(RecognizerState_Fail);
		}
	}
	else if (TouchInfoArray.size() == 2)
	{
		SGesTouchInfo& head = TouchInfoArray.front();
		SGesTouchInfo& tail = TouchInfoArray.back();
		s32 index0 = findTouchInfo(head.fingerID, touchInfos);
		s32 index1 = findTouchInfo(tail.fingerID, touchInfos);

		if (index0 != -1 || index1 != -1)
		{
			//一个touch结束
			if (index0 != -1)
				head = touchInfos[index0];

			if (index1 != -1)
				tail = touchInfos[index1];

			if (State == RecognizerState_Changed)
				outputPan2Gesture(GestureState_End);

			changeState(RecognizerState_Cancel);
		}
	}
}

void CGestureRecognizer_Pan2::onEnterState( E_RECOGNIZER_STATE state )
{
	if (state == RecognizerState_Begin)
	{
		if (TouchInfoArray.empty())
			return;

		const SGesTouchInfo& head = TouchInfoArray.front();
		const SGesTouchInfo& tail = TouchInfoArray.back();
		BeginPosX = (head.posX + tail.posX) * 0.5f;
		BeginPosY = (head.posY + tail.posY) * 0.5f;
	}
	CGestureRecognizerBase::onEnterState(state);
}

bool CGestureRecognizer_Pan2::checkPan2MoveDelta( f32 deltaX1, f32 deltaY1, f32 deltaX2, f32 deltaY2 )
{
	vector2df v1(deltaX1, deltaY1);
	vector2df v2(deltaX2, deltaY2);

	if (v1.getLength() > getThreshold_CalcAngleDist() &&
		v2.getLength() > getThreshold_CalcAngleDist())
	{
		v1.normalize();
		v2.normalize();
		if (v1.dotProduct(v2) < getThreshold_Pan2MoveDir())
			return false;
	}

	return true;
}

bool CGestureRecognizer_Pan2::checkPan2TouchDistance(const SGesTouchInfo& touch1, const SGesTouchInfo& touch2)
{
	vector2df v1(touch1.posX, touch1.posY);
	vector2df v2(touch2.posX, touch2.posY);
	if (v1.getDistanceFrom(v2) > getThreshold_Pan2TouchSepDist())
		return false;

	return true;
}

void CGestureRecognizer_Pan2::outputPan2Gesture( E_GESTURE_STATE state )
{
	if (TouchInfoArray.size() != 2)
		return;

	SGestureInfo gesInfo;
	InitializeListHead(&gesInfo.Link);

	gesInfo.type = Type;
	gesInfo.state = state;
	if (state == GestureState_Begin)
	{
		gesInfo.param.pan2_xPos = BeginPosX;
		gesInfo.param.pan2_yPos = BeginPosY;
		gesInfo.param.pan2_xVelocity = 0.0f;
		gesInfo.param.pan2_yVelocity = 0.0f;

		LastPosX = BeginPosX;
		LastPosY = BeginPosY;
		LastVelocityX = 0.0f;
		LastVelocityY = 0.0f;
	}
	else
	{
		const SGesTouchInfo& head = TouchInfoArray.front();
		const SGesTouchInfo& tail = TouchInfoArray.back();
		f32 curPosX = (head.posX + tail.posX) * 0.5f;
		f32 curPoxY = (head.posY + tail.posY) * 0.5f;

		f32 deltaTime = (g_Engine->getTimer()->getMillisecond() - TimeStamp) * 0.001f;
		f32 curVelocityX = deltaTime > 0.0f ? (curPosX - LastPosX) / deltaTime : 0.0f;
		f32 curVelocityY = deltaTime > 0.0f ? (curPoxY - LastPosY) / deltaTime : 0.0f;

		gesInfo.param.pan2_xPos = curPosX;
		gesInfo.param.pan2_yPos = curPoxY;
		gesInfo.param.pan2_xVelocity = curVelocityX * getfPanVelocityLowpass() * LastVelocityX * (1.0f - getfPanVelocityLowpass());
		gesInfo.param.pan2_yVelocity = curVelocityY * getfPanVelocityLowpass() * LastVelocityY * (1.0f - getfPanVelocityLowpass());
	
		LastPosX = gesInfo.param.pan2_xPos;
		LastPosY = gesInfo.param.pan2_yPos;
		LastVelocityX = gesInfo.param.pan2_xVelocity;
		LastVelocityY = gesInfo.param.pan2_yVelocity;
	}

	GestureReader->addGestureToQueue(gesInfo);
}
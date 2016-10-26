#include "stdafx.h"
#include "CGestureRecognizer_Pan1.h"
#include "mywow.h"
#include "CGestureReader.h"

//
//单指拖动
//
CGestureRecognizer_Pan1::CGestureRecognizer_Pan1( CGestureReader* gestureReader )
	: CGestureRecognizerBase(gestureReader)
{
	Type = GestureType_Pan_1;

	BeginPosX = 0.0f;
	BeginPosY = 0.0f;
	LastVelocityX = 0.0f;
	LastVelocityY = 0.0f;
	Moving = false;
}

void CGestureRecognizer_Pan1::tick()
{
	CGestureRecognizerBase::tick();

	if (State == RecognizerState_Begin)
	{
		u32 now = g_Engine->getTimer()->getMillisecond();
		if (now - TimeStamp > (u32)(getfRecognizerFailInterval() * 1000))
			changeState(RecognizerState_Fail);
	}
}

void CGestureRecognizer_Pan1::reset()
{
	CGestureRecognizerBase::reset();

	BeginPosX = 0.0f;
	BeginPosY = 0.0f;
	LastVelocityX = 0.0f;
	LastVelocityY = 0.0f;
	Moving = false;
}

void CGestureRecognizer_Pan1::onTouchBegin( const std::vector<SGesTouchInfo>& touchInfos )
{
	if (State != RecognizerState_None || !TouchInfoArray.empty())
		return;

	if (touchInfos.size() == 1)
	{
		TouchInfoArray.push_back(touchInfos.front());
		TimeStamp = g_Engine->getTimer()->getMillisecond();
		changeState(RecognizerState_Begin);
	}
}

void CGestureRecognizer_Pan1::onTouchMove( const std::vector<SGesTouchInfo>& touchInfos )
{
	if (TouchInfoArray.empty() ||
		(State != RecognizerState_Begin && State != RecognizerState_Changed))
		return;

	SGesTouchInfo& head = TouchInfoArray.front();

	s32 index = findTouchInfo(head.fingerID, touchInfos);
	if (index == -1)
		return;

	if (Moving)			//已经开始移动
	{
		head = touchInfos[index];

		if (!checkTouchMove(touchInfos[index]))			//移动结束
		{
			if (State == RecognizerState_Changed)
				outputPanGesture(GestureState_End);

			changeState(RecognizerState_Fail);
			return;
		}

		if (State == RecognizerState_Begin)
			outputPanGesture(GestureState_Begin);

		changeState(RecognizerState_Changed);
		outputPanGesture(GestureState_Change);
	}
	else
	{
		if (getTouchDistance(head, touchInfos[index]) > getThreshold_Pan2MoveDist())
		{
			Moving = true;
			head = touchInfos[index];

			if (!checkTouchMove(touchInfos[index]))			//移动结束
			{
				if (State == RecognizerState_Changed)
					outputPanGesture(GestureState_End);

				changeState(RecognizerState_Fail);
				return;
			}

			if (State == RecognizerState_Begin)
				outputPanGesture(GestureState_Begin);

			changeState(RecognizerState_Changed);
			outputPanGesture(GestureState_Change);
		}
	}
}

void CGestureRecognizer_Pan1::onTouchEnd( const std::vector<SGesTouchInfo>& touchInfos )
{
	if (TouchInfoArray.empty())
		return;

	SGesTouchInfo& head = TouchInfoArray.front();

	s32 index = findTouchInfo(head.fingerID, touchInfos);
	if (index == -1)
		return;

	head = touchInfos[index];
	if (State == RecognizerState_Changed)
		outputPanGesture(GestureState_End);

	changeState(RecognizerState_End);
	Moving = false;
}

void CGestureRecognizer_Pan1::onTouchCancel( const std::vector<SGesTouchInfo>& touchInfos )
{
	if (TouchInfoArray.empty())
		return;

	SGesTouchInfo& head = TouchInfoArray.front();

	s32 index = findTouchInfo(head.fingerID, touchInfos);
	if (index == -1)
		return;

	head = touchInfos[index];
	if (State == RecognizerState_Changed)
		outputPanGesture(GestureState_End);

	changeState(RecognizerState_Cancel);
	Moving = false;
}

void CGestureRecognizer_Pan1::onEnterState( E_RECOGNIZER_STATE state )
{
	if (state == RecognizerState_Begin)
	{
		if (TouchInfoArray.empty())
			return;

		const SGesTouchInfo& head = TouchInfoArray.front();
		BeginPosX = head.posX;
		BeginPosY = head.posY;
	}

	CGestureRecognizerBase::onEnterState(state);
}

bool CGestureRecognizer_Pan1::checkTouchMove( const SGesTouchInfo& touchInfo )
{
	return true;
}

void CGestureRecognizer_Pan1::outputPanGesture( E_GESTURE_STATE state )
{
	if (TouchInfoArray.empty())
		return;

	SGestureInfo gesInfo;
	gesInfo.type = Type;
	gesInfo.state = state;

	if (state == GestureState_Begin)
	{
		gesInfo.param.pan1_xPos = BeginPosX;
		gesInfo.param.pan1_yPos = BeginPosY;
		gesInfo.param.pan1_xVelocity = 0.0f;
		gesInfo.param.pan1_yVelocity = 0.0f;
	}
	else
	{
		const SGesTouchInfo& head = TouchInfoArray.front();
		gesInfo.param.pan1_xPos = head.posX;
		gesInfo.param.pan1_yPos = head.posY;
		f32 curVelocityX = head.deltaTime > 0.0f ? head.deltaX / head.deltaTime : 0.0f;
		f32 curVelocityY = head.deltaTime > 0.0f ? head.deltaY / head.deltaTime : 0.0f;
		gesInfo.param.pan1_xVelocity = curVelocityX * getfPanVelocityLowpass() + LastVelocityX * (1.0f - getfPanVelocityLowpass());
		gesInfo.param.pan1_yVelocity = curVelocityY * getfPanVelocityLowpass() + LastVelocityY * (1.0f - getfPanVelocityLowpass());
	}

	LastVelocityX = gesInfo.param.pan1_xVelocity;
	LastVelocityY = gesInfo.param.pan1_yVelocity;

	GestureReader->addGestureToQueue(gesInfo);
}
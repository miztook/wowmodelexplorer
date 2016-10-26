#include "stdafx.h"
#include "CGestureRecognizer_Joystick.h"
#include "mywow.h"
#include "CGestureReader.h"

//
//虚拟摇杆
//
CGestureRecognizer_Joystick::CGestureRecognizer_Joystick( CGestureReader* gestureReader )
	: CGestureRecognizerBase(gestureReader)
{
	Type = GestureType_Joystick;
	FnIsValidPosition = NULL_PTR;
}

void CGestureRecognizer_Joystick::onTouchBegin( const std::vector<SGesTouchInfo>& touchInfos )
{
	if (!TouchInfoArray.empty() || State != RecognizerState_None)			//
		return;

	if (!FnIsValidPosition)
	{
//		ASSERT(false);
		return;
	}

	s32 index = -1;
	for (s32 i=0; i<(s32)touchInfos.size(); ++i)
	{
		const SGesTouchInfo& info = touchInfos[i];
		if (!info.skip && FnIsValidPosition((s32)info.posX, (s32)info.posY))
		{
			index = i;
			break;
		}
	}

	if (index == -1)			//not valid area
		return;

	TouchInfoArray.push_back(touchInfos[index]);

	changeState(RecognizerState_Begin);
}

void CGestureRecognizer_Joystick::onTouchMove( const std::vector<SGesTouchInfo>& touchInfos )
{
	if (TouchInfoArray.empty() || (State != RecognizerState_Begin && State != RecognizerState_Changed))			//
		return;

	const SGesTouchInfo& head = TouchInfoArray.front();

	s32 index = findTouchInfo(head.fingerID, touchInfos);
	if (index == -1)			//not valid touch finger id
		return;

	const SGesTouchInfo& touch = touchInfos[index];

	if (State == RecognizerState_Changed)			//如果已经是move, 则继续发送move
	{
		//put
		SGestureInfo gesInfo;
		InitializeListHead(&gesInfo.Link);

		gesInfo.type = Type;
		gesInfo.state = GestureState_Change;
		gesInfo.param.joystick_xPos = touch.posX;
		gesInfo.param.joystick_yPos = touch.posY;
		gesInfo.param.joystick_xDelta = touch.posX - head.posX;
		gesInfo.param.joystick_yDelta = touch.posY - head.posY;

		GestureReader->addGestureToQueue(gesInfo);
	}
	else
	{
		f32 distance = getTouchDistanceSQ(head, touch);			//有距离偏移，由begin变move
		if (!equals_(distance, 0.0f))
		{
			ASSERT(State == RecognizerState_Begin);
			changeState(RecognizerState_Changed);

			//put
			SGestureInfo gesInfo;
			InitializeListHead(&gesInfo.Link);

			gesInfo.type = Type;
			gesInfo.state = GestureState_Change;
			gesInfo.param.joystick_xPos = touch.posX;
			gesInfo.param.joystick_yPos = touch.posY;
			gesInfo.param.joystick_xDelta = touch.posX - head.posX;
			gesInfo.param.joystick_yDelta = touch.posY - head.posY;

			GestureReader->addGestureToQueue(gesInfo);
		}
	}
}

void CGestureRecognizer_Joystick::onTouchEnd( const std::vector<SGesTouchInfo>& touchInfos )
{
	if (TouchInfoArray.empty() || (State != RecognizerState_Begin && State != RecognizerState_Changed))			//
		return;

	const SGesTouchInfo& head = TouchInfoArray.front();

	s32 index = findTouchInfo(head.fingerID, touchInfos);
	if (index == -1)			//not valid touch finger id
		return;

	SGesTouchInfo* touch = const_cast<SGesTouchInfo*>(&touchInfos[index]);

	TouchInfoArray.push_back(*touch);

	if (State == RecognizerState_Changed)
	{
		changeState(RecognizerState_End);
		touch->skip = true;					//后面的recognizer不再处理end包

		SGesTouchInfo infoFake = *touch;
		infoFake.skip = false;
		infoFake.phase = TouchPhase_Cancel;
		GestureReader->readTouchInfo(&infoFake, 1);		//发送cancel, joystick完成后取消其他手势
	}
	else
	{
		changeState(RecognizerState_Fail);
	}
}

void CGestureRecognizer_Joystick::onTouchCancel( const std::vector<SGesTouchInfo>& touchInfos )
{
	if (TouchInfoArray.empty())
		return;

	const SGesTouchInfo& head = TouchInfoArray[0];

	s32 index = findTouchInfo(head.fingerID, touchInfos);
	if (index == -1)			//not valid touch finger id
		return;

	TouchInfoArray.push_back(touchInfos[index]);
	changeState(RecognizerState_Cancel);
}

void CGestureRecognizer_Joystick::onEnterState( E_RECOGNIZER_STATE state )
{
	if (state == RecognizerState_Changed)
	{
		if (!TouchInfoArray.empty())
		{
			//put
			SGestureInfo gesInfo;
			InitializeListHead(&gesInfo.Link);

			gesInfo.type = Type;
			gesInfo.state = GestureState_Begin;
			gesInfo.param.joystick_xPos = TouchInfoArray.front().posX;
			gesInfo.param.joystick_yPos = TouchInfoArray.front().posY;
			gesInfo.param.joystick_xDelta = 0.0f;
			gesInfo.param.joystick_yDelta = 0.0f;

			GestureReader->addGestureToQueue(gesInfo);
		}
	}
	else if (state == RecognizerState_End ||
		state == RecognizerState_Cancel ||
		state == RecognizerState_Fail)
	{
		if (!TouchInfoArray.empty())
		{
			//put
			SGestureInfo gesInfo;
			InitializeListHead(&gesInfo.Link);

			gesInfo.type = Type;
			gesInfo.state = GestureState_End;
			gesInfo.param.joystick_xPos = TouchInfoArray.back().posX;
			gesInfo.param.joystick_yPos = TouchInfoArray.back().posY;
			gesInfo.param.joystick_xDelta = 0.0f;
			gesInfo.param.joystick_yDelta = 0.0f;

			GestureReader->addGestureToQueue(gesInfo);
		}
	}

	CGestureRecognizerBase::onEnterState(state);
}
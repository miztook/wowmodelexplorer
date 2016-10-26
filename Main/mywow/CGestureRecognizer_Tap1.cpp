#include "stdafx.h"
#include "CGestureRecognizer_Tap1.h"
#include "mywow.h"
#include "CGestureReader.h"

//
//µ¥Ö¸ÇáÅÄ
//
CGestureRecognizer_Tap1::CGestureRecognizer_Tap1( CGestureReader* gestureReader )
	: CGestureRecognizerBase(gestureReader)
{
	Type = GestureType_Tap_1;
}

void CGestureRecognizer_Tap1::tick()
{
	CGestureRecognizerBase::tick();

	if (State == RecognizerState_Begin)
	{
		u32 now = g_Engine->getTimer()->getMillisecond();
		if (now  - TimeStamp > (u32)(getfRecognizerFailInterval()* 1000))
			changeState(RecognizerState_Fail);
	}
}

void CGestureRecognizer_Tap1::onTouchBegin( const std::vector<SGesTouchInfo>& touchInfos )
{
	if (State != RecognizerState_None || touchInfos.empty())
		return;

	if (TouchInfoArray.empty() && touchInfos.size() == 1)
	{
		TouchInfoArray.push_back(touchInfos.front());
		TimeStamp = g_Engine->getTimer()->getMillisecond();
		changeState(RecognizerState_Begin);
	}
}

void CGestureRecognizer_Tap1::onTouchMove( const std::vector<SGesTouchInfo>& touchInfos )
{
	if (TouchInfoArray.empty() || State != RecognizerState_Begin)
		return;

	const SGesTouchInfo& head = TouchInfoArray[0];

	s32 index = findTouchInfo(head.fingerID, touchInfos);
	if (index == -1)			//not valid touch finger id
		return;

	f32 distance = getTouchDistance(head, touchInfos[index]);
	if (distance > getThreshold_MoveDist())
		changeState(RecognizerState_Fail);
}

void CGestureRecognizer_Tap1::onTouchEnd( const std::vector<SGesTouchInfo>& touchInfos )
{
	if (TouchInfoArray.empty() || State != RecognizerState_Begin)
		return;

	const SGesTouchInfo& head = TouchInfoArray[0];

	s32 index = findTouchInfo(head.fingerID, touchInfos);
	if (index == -1)			//not valid touch finger id
		return;

	f32 distance = getTouchDistanceSQ(head, touchInfos[index]);
	if (distance > getThreshold_MoveDist() ||
        !checkForbidGestures())
		changeState(RecognizerState_Fail);
	else
		changeState(RecognizerState_End);
}

void CGestureRecognizer_Tap1::onTouchCancel( const std::vector<SGesTouchInfo>& touchInfos )
{
	if (TouchInfoArray.empty() || State != RecognizerState_Begin)
		return;

	const SGesTouchInfo& head = TouchInfoArray[0];

	s32 index = findTouchInfo(head.fingerID, touchInfos);
	if (index == -1)			//not valid touch finger id
		return;

	changeState(RecognizerState_Cancel);
}

void CGestureRecognizer_Tap1::onEnterState( E_RECOGNIZER_STATE state )
{
	if (state == RecognizerState_Begin)
	{
		outputTapGesture(GestureState_Begin);
	}
	else if (state == RecognizerState_End)
	{
		outputTapGesture(GestureState_Change);
		outputTapGesture(GestureState_End);
	}
// 	else if (state == RecognizerState_Cancel ||
// 		state == RecognizerState_Fail)
// 	{
// 		outputTapGesture(GestureState_End);
// 	}

	CGestureRecognizerBase::onEnterState(state);
}

void CGestureRecognizer_Tap1::outputTapGesture( E_GESTURE_STATE state )
{
	if (TouchInfoArray.empty())
	{
		ASSERT(false);
		return;
	}

	//put 
	SGestureInfo gesInfo;
	InitializeListHead(&gesInfo.Link);

	gesInfo.type = Type;
	gesInfo.state = state;
	gesInfo.param.tap1_xPos = TouchInfoArray.front().posX;
	gesInfo.param.tap1_yPos = TouchInfoArray.front().posY;
	gesInfo.param.tap1_tapCount = TouchInfoArray.front().tapCount;

	GestureReader->addGestureToQueue(gesInfo);
}

bool CGestureRecognizer_Tap1::checkForbidGestures()
{
	E_RECOGNIZER_STATE stateTap2 = GestureReader->getGestureState(GestureType_Tap_2);
	bool bTap2 = ( stateTap2 == IGestureRecognizer::RecognizerState_End);

	return !bTap2;
}

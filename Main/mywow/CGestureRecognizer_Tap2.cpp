#include "stdafx.h"
#include "CGestureRecognizer_Tap2.h"
#include "mywow.h"
#include "CGestureReader.h"

//
//双指轻拍
//
CGestureRecognizer_Tap2::CGestureRecognizer_Tap2( CGestureReader* gestureReader )
	: CGestureRecognizerBase(gestureReader)
{
	Type = GestureType_Tap_2;
}

void CGestureRecognizer_Tap2::tick()
{
	CGestureRecognizerBase::tick();

	u32 now = g_Engine->getTimer()->getMillisecond();
	if (State == RecognizerState_None && !TouchInfoArray.empty())
	{
		if (now - TimeStamp > (u32)(getfDoubleTouchDiffTime() * 1000))
			changeState(RecognizerState_Fail);
	} 
	else if (State == RecognizerState_Begin)
	{
		if (now - TimeStamp > (u32)(getfRecognizerFailInterval() * 1000))
			changeState(RecognizerState_Fail);
	}
	else if (State == RecognizerState_Changed)
	{
		if (now - TimeStamp > (u32)(getfDoubleTouchDiffTime() * 1000))
			changeState(RecognizerState_Fail);
	}
}

void CGestureRecognizer_Tap2::onTouchBegin( const std::vector<SGesTouchInfo>& touchInfos )
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
			if (!checkTap2TouchDistance(TouchInfoArray.front(), TouchInfoArray.back()))
			{
				changeState(RecognizerState_Fail);
			}
			else
			{
				TimeStamp = g_Engine->getTimer()->getMillisecond();
				changeState(RecognizerState_Begin);
			}
		}
		else
		{
			if (numBegin == 2)				//2
			{
				TouchInfoArray.push_back(touchInfos.front());
				TouchInfoArray.push_back(touchInfos.back());
				if (!checkTap2TouchDistance(TouchInfoArray.front(), TouchInfoArray.back()))
				{
					changeState(RecognizerState_Fail);
				}
				else
				{
					TimeStamp = g_Engine->getTimer()->getMillisecond();
					changeState(RecognizerState_Begin);
				}
			}
			else				//1
			{
				TouchInfoArray.push_back(touchInfos.front());
				TimeStamp = g_Engine->getTimer()->getMillisecond();
			}
		}
	}
}

void CGestureRecognizer_Tap2::onTouchMove( const std::vector<SGesTouchInfo>& touchInfos )
{
	if (TouchInfoArray.empty() ||
		(State != RecognizerState_Begin && State != RecognizerState_Changed))
		return;

	if (TouchInfoArray.size() == 1)
	{
		SGesTouchInfo& head = TouchInfoArray.front();

		s32 index = findTouchInfo(head.fingerID, touchInfos);
		//超过一定距离，替换第一个touch，重新cache
		if (index != -1 &&
			getTouchDistance(head, touchInfos[index]) > getThreshold_MoveDist())
		{
			head = touchInfos[index];		
			changeState(RecognizerState_Fail);
		}
	}
	else
	{
		SGesTouchInfo& head = TouchInfoArray.front();
		SGesTouchInfo& tail = TouchInfoArray.back();

		s32 index0 = findTouchInfo(head.fingerID, touchInfos);
		s32 index1 = findTouchInfo(tail.fingerID, touchInfos);
		//超过一定距离，替换touch，重新cache
		if (index0 != -1 &&
			getTouchDistance(head, touchInfos[index0]) > getThreshold_MoveDist())
		{
			head = touchInfos[index0];		
			changeState(RecognizerState_Fail);
		}
		else if (index1 != -1 &&
			getTouchDistance(tail, touchInfos[index1]) > getThreshold_MoveDist())
		{
			tail = touchInfos[index1];
			changeState(RecognizerState_Fail);
		}
	}
}

void CGestureRecognizer_Tap2::onTouchEnd( const std::vector<SGesTouchInfo>& touchInfos )
{
	if (TouchInfoArray.empty())
		return;

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
	else
	{
		SGesTouchInfo& head = TouchInfoArray.front();
		SGesTouchInfo& tail = TouchInfoArray.back();
		s32 index0 = findTouchInfo(head.fingerID, touchInfos);
		s32 index1 = findTouchInfo(tail.fingerID, touchInfos);
		if (index0 != -1 && index1 != -1)
		{
			//两个touch同时结束
			head = touchInfos[index0];
			tail = touchInfos[index1];
			changeState(RecognizerState_End);
		}
		else if (index0 != -1 || index1 != -1)
		{
			//一个touch结束
			if (index0 != -1)
				head = touchInfos[index0];

			if (index1 != -1)
				tail = touchInfos[index1];

			if (State == RecognizerState_Changed)
			{
				changeState(RecognizerState_End);
			}
			else if (State == RecognizerState_Begin)
			{
				TimeStamp = g_Engine->getTimer()->getMillisecond();
				changeState(RecognizerState_Changed);
			}
			else 
			{
				changeState(RecognizerState_Fail);
			}
		}
	}
}

void CGestureRecognizer_Tap2::onTouchCancel( const std::vector<SGesTouchInfo>& touchInfos )
{
	if (TouchInfoArray.empty())
		return;

	if (TouchInfoArray.size() == 1)
	{
		SGesTouchInfo& head = TouchInfoArray.front();
		s32 index = findTouchInfo(head.fingerID, touchInfos);
		if (index != -1)
		{
			head = touchInfos[index];
			changeState(RecognizerState_Cancel);
		}
	}
	else
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

			changeState(RecognizerState_Cancel);
		}
	}
}

void CGestureRecognizer_Tap2::onEnterState( E_RECOGNIZER_STATE state )
{
	if (state == RecognizerState_Begin)
	{
		outputTap2Gesture(GestureState_Begin);
	}
	else if (state == RecognizerState_End)
	{
		outputTap2Gesture(GestureState_Change);
		outputTap2Gesture(GestureState_End);
	}
// 	else if (state == RecognizerState_Cancel ||
// 		state == RecognizerState_Fail)
// 	{
// 		outputTap2Gesture(GestureState_End);
// 	}

	CGestureRecognizerBase::onEnterState(state);
}

bool CGestureRecognizer_Tap2::checkTap2TouchDistance(const SGesTouchInfo& touch1, const SGesTouchInfo& touch2)
{
	vector2df v1(touch1.posX, touch1.posY);
	vector2df v2(touch2.posX, touch2.posY);
	if ( (v1 - v2).getLength() > getThreshold_Tap2TouchSepDist())
		return false;

	return true;
}

void CGestureRecognizer_Tap2::outputTap2Gesture( E_GESTURE_STATE state )
{
	if (TouchInfoArray.size() < 2)
		return;

	SGestureInfo gestureInfo;
	InitializeListHead(&gestureInfo.Link);

	const SGesTouchInfo& head = TouchInfoArray.front();
	const SGesTouchInfo& tail = TouchInfoArray.back();

	gestureInfo.type = Type;
	gestureInfo.state = state;
	gestureInfo.param.tap2_xPos = (head.posX + tail.posX) * 0.5f;
	gestureInfo.param.tap2_yPos = (head.posY + tail.posY) * 0.5f;

	GestureReader->addGestureToQueue(gestureInfo);
}

bool CGestureRecognizer_Tap2::checkForbidGestures()
{
	E_RECOGNIZER_STATE statePinch = GestureReader->getGestureState(GestureType_Pinch);
	E_RECOGNIZER_STATE statePan2 = GestureReader->getGestureState(GestureType_Pan_2);
	bool bPinch = ( statePinch == IGestureRecognizer::RecognizerState_Begin || statePinch == IGestureRecognizer::RecognizerState_Changed );
	bool bPan2 = ( statePan2 == IGestureRecognizer::RecognizerState_Begin || statePan2 == IGestureRecognizer::RecognizerState_Changed );
	
	return !bPinch && !bPan2;
}

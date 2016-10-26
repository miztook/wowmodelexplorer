#include "stdafx.h"
#include "CGestureRecognizer_Circle.h"
#include "mywow.h"
#include "CGestureReader.h"

//
//单指画圈
//
CGestureRecognizer_Circle::CGestureRecognizer_Circle( CGestureReader* gestureReader )
	: CGestureRecognizerBase(gestureReader)
{
	Type = GestureType_Circle;
	Direction = CircleDir_None;
}

void CGestureRecognizer_Circle::tick()
{
	CGestureRecognizerBase::tick();

	if ((State == RecognizerState_None && !TouchInfoArray.empty()) ||
		State == RecognizerState_Begin)
	{
		u32 now = g_Engine->getTimer()->getMillisecond();
		if (now - TimeStamp > (u32)(getfRecognizerFailInterval() * 1000))
		{
			if (State == RecognizerState_Begin)
				outputCircleGesture(GestureState_End);

			changeState(RecognizerState_Fail);
		}
	}
}

void CGestureRecognizer_Circle::reset()
{
	CGestureRecognizerBase::reset();
	Direction = CircleDir_None;
}

void CGestureRecognizer_Circle::onTouchBegin( const std::vector<SGesTouchInfo>& touchInfos )
{
	if (State != RecognizerState_None || !TouchInfoArray.empty())
		return;

	if (touchInfos.size() == 1)
	{
		TouchInfoArray.push_back(touchInfos.front());
		TimeStamp = g_Engine->getTimer()->getMillisecond();
	}
}

void CGestureRecognizer_Circle::onTouchMove( const std::vector<SGesTouchInfo>& touchInfos )
{
	if (TouchInfoArray.empty() ||
		(State != RecognizerState_Begin && State != RecognizerState_Changed))
		return;

	const SGesTouchInfo& head = TouchInfoArray.front();
	s32 index = findTouchInfo(head.fingerID, touchInfos);
	if (index != -1 &&
		getTouchDistance(head, touchInfos[index]) > getThreshold_MoveDist())
	{
		const SGesTouchInfo& touch = touchInfos[index];

		//先检查touch的速度
		if (!checkNewTouchSpeed(touch))
		{
			if (State == RecognizerState_Begin)
				outputCircleGesture(GestureState_End);

			changeState(RecognizerState_Fail);
			return;
		}

		u32 curSize = (u32)TouchInfoArray.size();
		if (curSize == 1)
		{
			TouchInfoArray.push_back(touch);
			TimeStamp = g_Engine->getTimer()->getMillisecond();
		}
		else if (curSize == 2)
		{
			E_CICLE_DIR dir = calcNewTouchDir(touch);
			if (dir == CircleDir_None)
			{
				// three sample in the same line, ignore this sample
				TimeStamp = g_Engine->getTimer()->getMillisecond();
				return;
			}

			Direction = dir;		// three sample to decide the direction
			TouchInfoArray.push_back(touch);
			TimeStamp = g_Engine->getTimer()->getMillisecond();
			changeState(RecognizerState_Begin);
		}
		else
		{
			// more than 3 samples cached
			E_CICLE_DIR dir = calcNewTouchDir(touch);
			if (dir != Direction)
			{
				outputCircleGesture(GestureState_End);
				changeState(RecognizerState_Fail);
				return;
			}

			if (TouchInfoArray.size() < 6)			//max 6
				TouchInfoArray.push_back(touch);
			TimeStamp = g_Engine->getTimer()->getMillisecond();

			if (checkGestureComplete())
				changeState(RecognizerState_End);
		}
	}
}

void CGestureRecognizer_Circle::onTouchEnd( const std::vector<SGesTouchInfo>& touchInfos )
{
	if (TouchInfoArray.empty())
		return;

	const SGesTouchInfo& head = TouchInfoArray.front();

	s32 index = findTouchInfo(head.fingerID, touchInfos);
	if (index == -1)
		return;

	if (TouchInfoArray.size() >= 3)			//check complete
	{
		const SGesTouchInfo& touch = touchInfos[index];
		E_CICLE_DIR dir = calcNewTouchDir(touch);
		if (dir == Direction)
		{
			TouchInfoArray.push_back(touch);
			TimeStamp = g_Engine->getTimer()->getMillisecond();

			if (checkGestureComplete())
			{
				changeState(RecognizerState_End);
				return;
			}
		}

		if (State == RecognizerState_Begin)
			outputCircleGesture(GestureState_End);
	}

	changeState(RecognizerState_Fail);
}

void CGestureRecognizer_Circle::onTouchCancel( const std::vector<SGesTouchInfo>& touchInfos )
{
	if (TouchInfoArray.empty())
		return;

	const SGesTouchInfo& head = TouchInfoArray.front();

	s32 index = findTouchInfo(head.fingerID, touchInfos);
	if (index == -1)
		return;

	if (TouchInfoArray.size() >= 3)			//check complete
	{
		const SGesTouchInfo& touch = touchInfos[index];
		E_CICLE_DIR dir = calcNewTouchDir(touch);
		if (dir == Direction)
		{
			TouchInfoArray.push_back(touch);
			TimeStamp = g_Engine->getTimer()->getMillisecond();

			if (checkGestureComplete())
			{
				changeState(RecognizerState_End);
				return;
			}
		}

		if (State == RecognizerState_Begin)
			outputCircleGesture(GestureState_End);
	}

	changeState(RecognizerState_Cancel);
}

void CGestureRecognizer_Circle::onEnterState( E_RECOGNIZER_STATE state )
{
	if (State == RecognizerState_Begin)
	{
		outputCircleGesture(GestureState_Begin);
	}
	else if (State == RecognizerState_End)
	{
		outputCircleGesture(GestureState_Change);
		outputCircleGesture(GestureState_End);
	}

	CGestureRecognizerBase::onEnterState(state);
}

E_CICLE_DIR CGestureRecognizer_Circle::calcNewTouchDir( const SGesTouchInfo& touchInfo )
{
	u32 curSize = (u32)TouchInfoArray.size();
	if (curSize < 2)
		return CircleDir_None;

	vector3df v0(TouchInfoArray[curSize-2].posX, TouchInfoArray[curSize-2].posY, 0);
	vector3df v1(TouchInfoArray[curSize-1].posX, TouchInfoArray[curSize-1].posY, 0);
	vector3df v2(touchInfo.posX, touchInfo.posY, 0);

	vector3df edge1(v1 - v0);
	edge1.normalize();
	vector3df edge2(v2 - v0);
	edge2.normalize();

	vector3df cross = edge1.crossProduct(edge2);
	if (cross.Z > ROUNDING_ERROR_f32)
		return CircleDir_Clockwise;
	else if (cross.Z < -ROUNDING_ERROR_f32)
		return CircleDir_CounterClockwise;
	else
		return CircleDir_None;
}

bool CGestureRecognizer_Circle::checkNewTouchSpeed( const SGesTouchInfo& touchInfo )
{
	f32 speedX = touchInfo.deltaTime > 0.0f ? touchInfo.deltaX / touchInfo.deltaTime : 0.0f;
	f32 speedY = touchInfo.deltaTime > 0.0f ? touchInfo.deltaY / touchInfo.deltaTime : 0.0f;
	vector2df v(speedX, speedY);
	if (v.getLength() < getThreshold_PanMaxSpeedDist())
		return false;
	
	return true;
}

bool CGestureRecognizer_Circle::checkGestureComplete()
{
	u32 curSize = (u32)TouchInfoArray.size();
	// at least 4 samples required to complete
	if (curSize < 4 || Direction == CircleDir_None)
		return false;

	vector3df v0(TouchInfoArray[0].posX, TouchInfoArray[0].posY, 0);
	vector3df v1(TouchInfoArray[1].posX, TouchInfoArray[1].posY, 0);
	vector3df v2(TouchInfoArray[curSize-2].posX, TouchInfoArray[curSize-2].posY, 0);
	vector3df v3(TouchInfoArray[curSize-1].posX, TouchInfoArray[curSize-1].posY, 0);

	vector3df edge1(v1 - v0);
	edge1.normalize();
	vector3df edge2(v3 - v2);
	edge2.normalize();

	vector3df cross = edge1.crossProduct(edge2);
	if ((Direction == CircleDir_Clockwise && cross.Z < -ROUNDING_ERROR_f32) ||
		(Direction == CircleDir_CounterClockwise && cross.Z > ROUNDING_ERROR_f32))
	{
		// more than 180 degree
		return true;
	}

	return false;
}

void CGestureRecognizer_Circle::outputCircleGesture( E_GESTURE_STATE state )
{
	if (TouchInfoArray.empty())
		return;

	SGestureInfo gesInfo;
	InitializeListHead(&gesInfo.Link);

	gesInfo.type = Type;
	gesInfo.state = state;
	gesInfo.param.circle_direction = Direction;

	GestureReader->addGestureToQueue(gesInfo);
}

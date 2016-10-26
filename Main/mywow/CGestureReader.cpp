#include "stdafx.h"
#include "CGestureReader.h"
#include "CGestureRecognizerBase.h"
#include "CSysUtility.h"

void CGestureReader::tick()
{
	BEGIN_LOCK(&cs);

	for (u32 i=0; i<GestureType_Num; ++i)
	{
		CGestureRecognizerBase* recognizer = GestureRecognizers[i];
		if(recognizer)
			recognizer->tick();
	}

	END_LOCK(&cs);
}

void CGestureReader::readTouchInfo( const SGesTouchInfo* arrTouches, u32 count )
{
	if (!arrTouches || !count)
		return;

	BeginTouchInfos.clear();
	MoveTouchInfos.clear();
	EndTouchInfos.clear();
	CancelTouchInfos.clear();

	for (u32 i=0; i<count; ++i)
	{
		const SGesTouchInfo& touch = arrTouches[i];

		switch(touch.phase)
		{
		case TouchPhase_Begin:
			BeginTouchInfos.push_back(touch);
			break;
		case TouchPhase_Move:
			MoveTouchInfos.push_back(touch);
			break;
		case TouchPhase_End:
			EndTouchInfos.push_back(touch);
			break;
		case TouchPhase_Cancel:
			CancelTouchInfos.push_back(touch);
			break;
		default:
			ASSERT(false);
			break;
		}
	}

	BEGIN_LOCK(&cs);

	for (u32 i=0; i<GestureType_Num; ++i)
	{
		CGestureRecognizerBase* recognizer = GestureRecognizers[i];
		if(!recognizer)
			continue;

		if (!BeginTouchInfos.empty())
			recognizer->onTouchBegin(BeginTouchInfos);

		if (!MoveTouchInfos.empty())
			recognizer->onTouchMove(MoveTouchInfos);

		if (!EndTouchInfos.empty())
			recognizer->onTouchEnd(EndTouchInfos);

		if (!CancelTouchInfos.empty())
			recognizer->onTouchCancel(CancelTouchInfos);
	}

	END_LOCK(&cs);
}

bool CGestureReader::addGestureToQueue( const SGestureInfo& gesInfo )
{
	//enqueue
	SGestureInfo* info = GesturePool.get();
	if (!info)
	{
		CSysUtility::outputDebug("gesture info exceed!");
		//队列超出限额
		return false;
	}

	*info = gesInfo;
	InitializeListHead(&info->Link);

	InsertTailList(&GestureInfoList, &info->Link);

	//CSysUtility::outputDebug("add gesture info %s, %s", getGestureTypeName(info->type), getGestureStateName(info->state));

	return true;
}

bool CGestureReader::removeGestureFromQueue( SGestureInfo& gesInfo )
{
	if(isQueueEmpty())
		return false;

	PLENTRY entry = RemoveHeadList(&GestureInfoList);
	SGestureInfo* info = reinterpret_cast<SGestureInfo*>CONTAINING_RECORD(entry, SGestureInfo, Link);
	gesInfo = *info;

	GesturePool.put(info);

	return true;
}

bool CGestureReader::isQueueEmpty() const
{
	return IsListEmpty(&GestureInfoList);
}

IGestureRecognizer::E_RECOGNIZER_STATE CGestureReader::getGestureState( E_GESTURE_TYPE type )
{
	return GestureRecognizers[type]->getState();
}


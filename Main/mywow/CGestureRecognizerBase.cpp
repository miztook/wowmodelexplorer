#include "stdafx.h"
#include "CGestureRecognizerBase.h"
#include "mywow.h"
#include "CGestureReader.h"

CGestureRecognizerBase::CGestureRecognizerBase( CGestureReader* gestureReader )
	: GestureReader(gestureReader), Type(GestureType_None), State(RecognizerState_None)
{
	TimeStamp = 0;
}

CGestureRecognizerBase::~CGestureRecognizerBase()
{

}

void CGestureRecognizerBase::tick()
{
	if (State == RecognizerState_Fail ||
		State == RecognizerState_End ||
		State == RecognizerState_Cancel)
	{
		changeState(RecognizerState_None);
	}
}

void CGestureRecognizerBase::reset()
{
	TouchInfoArray.clear();
	TimeStamp = 0;
}

void CGestureRecognizerBase::onEnterState( E_RECOGNIZER_STATE state )
{
	switch(state)
	{
	case RecognizerState_End:
	case RecognizerState_Cancel:
	case RecognizerState_None:
		reset();
		break;
	case RecognizerState_Fail:
		reset();
		break;
	default:
		break;
	}
}

bool CGestureRecognizerBase::changeState( E_RECOGNIZER_STATE state )
{
	State = state;
	onEnterState(State);
	return true;
}

s32 CGestureRecognizerBase::findTouchInfo( ptr_t fingerId, const std::vector<SGesTouchInfo>& touchInfos )
{
	s32 index = -1;
	for (s32 i=0; i<(s32)touchInfos.size(); ++i)
	{
		const SGesTouchInfo& info = touchInfos[i];
		if (!info.skip && info.fingerID == fingerId)
		{
			index = i;
			break;
		}
	}

	return index;
}

f32 CGestureRecognizerBase::getThreshold_Pan2TouchSepDist()
{
	//return 500.0f * g_pAWindow->m_fScale;
	return g_Engine->getWindowInfo().width * 0.8f;
}

f32 CGestureRecognizerBase::getThreshold_Tap2TouchSepDist()
{
//	return 200.0f * g_pAWindow->m_fScale;
	return g_Engine->getWindowInfo().width * 0.3f;
}

f32 CGestureRecognizerBase::getThreshold_MoveDist()
{
	return 15.0f * g_Engine->getWindowInfo().scale;
}

f32 CGestureRecognizerBase::getThreshold_PanMoveDist()
{
	return 2.0f * g_Engine->getWindowInfo().scale;
}

f32 CGestureRecognizerBase::getThreshold_Pan2MoveDist()
{
	return 0.0f * g_Engine->getWindowInfo().scale;
}

f32 CGestureRecognizerBase::getThreshold_PinchDist()
{
	return 3.0f * g_Engine->getWindowInfo().scale;
}

f32 CGestureRecognizerBase::getThreshold_SwipeDist()
{
	return 10.0f * g_Engine->getWindowInfo().scale;
}

f32 CGestureRecognizerBase::getThreshold_SwipeSpeedDist()
{
	return 100.0f * g_Engine->getWindowInfo().scale;
}

f32 CGestureRecognizerBase::getThreshold_PanMaxSpeedDist()
{
	return 2500.0f * g_Engine->getWindowInfo().scale;
}

f32 CGestureRecognizerBase::getThreshold_CalcAngleDist()
{
	return 4.0f * g_Engine->getWindowInfo().scale;
}

f32 CGestureRecognizerBase::getThreshold_Pan2MoveDir()
{
	return cosf(DEGTORAD * 45.0f);
}

f32 CGestureRecognizerBase::getThreshold_SwipeDirValid()
{
	return cosf(DEGTORAD * 22.5f);
}


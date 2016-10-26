#include "game.h"

#ifdef MW_USE_TOUCH_INPUT

void processGesture(const SGestureInfo& gesInfo);
void processPan1(const SGestureInfo& gesInfo);
void processPan2(const SGestureInfo& gesInfo);
void processPinch(const SGestureInfo& gesInfo);

void createInput()
{
}

void destroyInput()
{

}

void processInput()
{
	IGestureReader* gestureReader = g_Engine->getGestureReader();
	SGestureInfo gesInfo;
	while (gestureReader->removeGestureFromQueue(gesInfo))
	{
		processGesture(gesInfo);
		//CSysUtility::outputDebug("remove gesture info %s, %s", getGestureTypeName(gesInfo.type), getGestureStateName(gesInfo.state));
	}
}

void processGesture( const SGestureInfo& gesInfo )
{
	switch(gesInfo.type)
	{
	case GestureType_Pan_1:
		processPan1(gesInfo);
		break;
	case GestureType_Pan_2:
		processPan2(gesInfo);
		break;
	case GestureType_Pinch:
		processPinch(gesInfo);
		break;
	default:
		break;
	}
}

void processPan1( const SGestureInfo& gesInfo )
{
#ifdef MW_PLATFORM_WINDOWS

	static f32 lastX = 0.0f;
	static f32 lastY = 0.0f;
	static bool capture = false;

	ISceneManager* smgr = g_Engine->getSceneManager();
	if (!smgr)	return;

	ICamera* cam = smgr->getActiveCamera();
	if(!cam)	return;

	if (gesInfo.state == GestureState_Begin)
	{
		lastX = gesInfo.param.pan1_xPos;
		lastY = gesInfo.param.pan1_yPos;
		capture = true;
	}
	else if (gesInfo.state == GestureState_Change)
	{
		f32 x = gesInfo.param.pan1_xPos;
		f32 y = gesInfo.param.pan1_yPos;

		const float blend = 0.8f;				//旋转柔和

		if (capture)
		{
			float deltaX = x - lastX;
			float deltaY = y - lastY;

			bool xChange = fabs(deltaX) > 0.5f;
			bool yChange = fabs(deltaY) > 0.5f;

			if (  xChange || yChange  )
			{
				float blendDeltaX = xChange ? deltaX * blend : deltaX;
				float blendDeltaY = yChange ? deltaY * blend : deltaY;
		
				g_Client->onMouseMove( blendDeltaY * 0.3f, blendDeltaX * 0.3f );

				lastX += blendDeltaX;
				lastY += blendDeltaY;
			}
		}
	}
	else
	{
		lastX = lastY = 0.0f;
		capture = false;
	}
	
#endif
}

void processPan2( const SGestureInfo& gesInfo )
{
#ifndef MW_PLATFORM_WINDOWS

	static f32 lastX = 0.0f;
	static f32 lastY = 0.0f;
	static bool capture = false;

	ISceneManager* smgr = g_Engine->getSceneManager();
	if (!smgr)	return;

	ICamera* cam = smgr->getActiveCamera();
	if(!cam)	return;

	if (gesInfo.state == GestureState_Begin)
	{
		lastX = gesInfo.param.pan1_xPos;
		lastY = gesInfo.param.pan1_yPos;
		capture = true;
	}
	else if (gesInfo.state == GestureState_Change)
	{
		f32 x = gesInfo.param.pan1_xPos;
		f32 y = gesInfo.param.pan1_yPos;

		const float blend = 0.8f;				//旋转柔和

		if (capture)
		{
			float deltaX = x - lastX;
			float deltaY = y - lastY;

			bool xChange = fabs(deltaX) > 0.5f;
			bool yChange = fabs(deltaY) > 0.5f;

			if (  xChange || yChange  )
			{
				float blendDeltaX = xChange ? deltaX * blend : deltaX;
				float blendDeltaY = yChange ? deltaY * blend : deltaY;

				g_Client->onMouseMove( blendDeltaY * 0.3f, blendDeltaX * 0.3f );

				lastX += blendDeltaX;
				lastY += blendDeltaY;
			}
		}
	}
	else
	{
		lastX = lastY = 0.0f;
		capture = false;
	}

#endif
}

void processPinch( const SGestureInfo& gesInfo )
{		
	ISceneManager* smgr = g_Engine->getSceneManager();
	if (!smgr)	return;

	ICamera* cam = smgr->getActiveCamera();
	if(!cam)	return;

	if (gesInfo.state == GestureState_Change)
	{
		g_Client->onMouseWheel((gesInfo.param.pinch_scale - 1.0f) * 10);
	}
}



#endif
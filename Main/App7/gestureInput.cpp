#include "game.h"

#ifdef MW_USE_TOUCH_INPUT

void processGesture(const SGestureInfo& gesInfo);
void processPan1(const SGestureInfo& gesInfo);
void processPan2(const SGestureInfo& gesInfo);
void processPinch(const SGestureInfo& gesInfo);
void processTap1(const SGestureInfo& gesInfo);
void processTap2(const SGestureInfo& gesInfo);

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
	case GestureType_Tap_1:
		processTap1(gesInfo);
		break;
	case GestureType_Tap_2:
		processTap2(gesInfo);
		break;
	default:
		break;
	}
}

void processTap1(const SGestureInfo& gesInfo)
{
}

void processTap2(const SGestureInfo& gesInfo)
{
}

void processPan1( const SGestureInfo& gesInfo )
{
//#ifdef MW_PLATFORM_WINDOWS

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
		
				// 					if (LCapture)			
				// 						cam->pitch_yaw_Maya( blendDeltaY * 0.3f, blendDeltaX * 0.3f );			
				// 					else
				// 						cam->move_offset_Maya( blendDeltaX * 0.02f, blendDeltaY * 0.02f);

                if(abs_(blendDeltaX) > abs_(blendDeltaY))
                    cam->pitch_yaw_FPS(0, blendDeltaX * 0.1f);
                else
                    cam->pitch_yaw_FPS(blendDeltaY * 0.1f, 0);

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
	
//#endif
}

void processPan2( const SGestureInfo& gesInfo )
{
    /*
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
		lastX = gesInfo.param.pan2_xPos;
		lastY = gesInfo.param.pan2_yPos;
		capture = true;
	}
	else if (gesInfo.state == GestureState_Change)
	{
		f32 x = gesInfo.param.pan2_xPos;
		f32 y = gesInfo.param.pan2_yPos;

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

				cam->pitch_yaw_Maya( blendDeltaY * 0.3f, blendDeltaX * 0.3f );			
				//else
				//	cam->move_offset_Maya( blendDeltaX * 0.02f, blendDeltaY * 0.02f);

				//					cam->pitch_yaw_FPS(blendDeltaY * 0.2f, blendDeltaX * 0.2f);

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
     */
}

void processPinch( const SGestureInfo& gesInfo )
{	
	static f32 lastScale = 1.0f;
	
	ISceneManager* smgr = g_Engine->getSceneManager();
	if (!smgr)	return;

	ICamera* cam = smgr->getActiveCamera();
	if(!cam)	return;

	if (gesInfo.state == GestureState_Begin)
	{
		lastScale = 1.0f;
	}
	else if (gesInfo.state == GestureState_Change)
	{
		ICamera::SKeyControl keyControl = {0};
		keyControl.front = gesInfo.param.pinch_scale > lastScale;
		keyControl.back = gesInfo.param.pinch_scale < lastScale;

#ifdef MW_PLATFORM_IOS
		cam->onKeyMove(max_(1.0f, abs_(gesInfo.param.pinch_scale - lastScale)) * 5.0f, keyControl);
#else
		cam->onKeyMove(max_(1.0f, abs_(gesInfo.param.pinch_scale - lastScale)) * 50.0f, keyControl);
#endif
	}
	else
	{
		lastScale = 1.0f;
	}
}



#endif
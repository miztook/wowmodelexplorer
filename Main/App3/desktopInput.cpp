#include "game.h"

#ifndef MW_USE_TOUCH_INPUT

class CInputListener : public IInputListener
{
public:
	CInputListener()
	{
		LastX = LastY = 0;
		LCapture = RCapture = false;
	}
public:
	virtual void onMouseMessage(window_type hwnd, E_INPUT_MESSAGE message, int _x, int _y);
	virtual void onMouseWheel(window_type hwnd, int zDelta);
	virtual void onKeyMessage(window_type hwnd, E_INPUT_MESSAGE message, int key);

private:
	f32		LastX;
	f32		LastY;
	bool		LCapture;
	bool		RCapture;
};

void CInputListener::onKeyMessage( window_type hwnd, E_INPUT_MESSAGE message, int key )
{
	if (message == Key_Up && key == VK_ESCAPE)
	{
		g_bExit = true;
	}
}

void CInputListener::onMouseMessage(window_type hwnd, E_INPUT_MESSAGE message, int _x, int _y)
{
	ISceneManager* smgr = g_Engine->getSceneManager();
	if (!smgr)	return;

	ICamera* cam = smgr->getActiveCamera();
	if(!cam)	return;

	switch (message)
	{
	case Mouse_LeftButtonDown:
		{
			if (!RCapture)
			{
				LastX = (float)_x;
				LastY = (float)_y;
				SetCapture( hwnd );
				LCapture = true;
			}
		}
		break;
	case Mouse_LeftButtonUp:
		{
			if (LCapture)
			{
				LastX = LastY = 0;
				ReleaseCapture();
				LCapture = false;
			}	
		}
		break;
	case Mouse_RightButtonDown:
		{
			if (!LCapture)
			{
				LastX = (float)_x;
				LastY = (float)_y;
				SetCapture( hwnd );
				RCapture = true;
			}
		}
		break;
	case Mouse_RightButtonUp:
		{
			if (RCapture)
			{
				LastX = LastY = 0;
				ReleaseCapture();
				RCapture = false;
			}	
		}
		break;
	case Mouse_Move:
		{
			float x = (float)_x;
			float y = (float)_y;

			const float blend = 0.8f;				//Ðý×ªÈáºÍ

			if (LCapture || RCapture)
			{
				float deltaX = x - LastX;
				float deltaY = y - LastY;

				bool xChange = fabs(deltaX) > 0.5f;
				bool yChange = fabs(deltaY) > 0.5f;

				if (  xChange || yChange  )
				{
					float blendDeltaX = xChange ? deltaX * blend : deltaX;
					float blendDeltaY = yChange ? deltaY * blend : deltaY;

					if (LCapture)			
						cam->pitch_yaw_Maya( blendDeltaY * 0.3f, blendDeltaX * 0.3f );			
					else
						cam->move_offset_Maya( blendDeltaX * 0.02f, blendDeltaY * 0.02f);

					//					cam->pitch_yaw_FPS(blendDeltaY * 0.2f, blendDeltaX * 0.2f);

					LastX += blendDeltaX;
					LastY += blendDeltaY;
				}
			}
		}
		break;
	default:
		break;
	}
}

void CInputListener::onMouseWheel(window_type hwnd, int zDelta)
{
	if(::GetActiveWindow() != hwnd) return;

	ISceneManager* smgr = g_Engine->getSceneManager();
	if (!smgr)	return;

	ICamera* cam = smgr->getActiveCamera();
	if(!cam)	return;

	ICamera::SKeyControl keyControl = {0};
	keyControl.front = zDelta > 0;
	keyControl.back = zDelta < 0;

	cam->onKeyMove(0.01f * abs_(zDelta), keyControl);
}

CInputListener g_listener;


void createInput()
{
	IInputReader* inputReader = g_Engine->getInputReader();
	inputReader->addListener(&g_listener);

	inputReader->acquire(EID_KEYBOARD);
	inputReader->acquire(EID_MOUSE);
}

void destroyInput()
{
	IInputReader* inputReader = g_Engine->getInputReader();
	inputReader->removeListener(&g_listener);
}

void processInput()
{
	static u32 startTime = g_Engine->getTimer()->getTimeSinceStart();

	u32 now = g_Engine->getTimer()->getTimeSinceStart();
	u32 time =  min_(now - startTime, 500u);
	startTime = now;

	IInputReader* inputReader = g_Engine->getInputReader();
	//instant message
	if (inputReader->capture(EID_KEYBOARD))
	{
		ICamera* cam = g_Engine->getSceneManager()->getActiveCamera();
		ICamera::SKeyControl keyControl;
		keyControl.front = inputReader->isKeyPressed('W');	
		keyControl.back =  inputReader->isKeyPressed('S');
		keyControl.left = inputReader->isKeyPressed('A');	
		keyControl.right =  inputReader->isKeyPressed('D');	
		keyControl.up =  inputReader->isKeyPressed(VK_SPACE);	
		keyControl.down = false;

		if (cam)
		{
			cam->onKeyMove(0.02f * time, keyControl);
		}
	}
}

#endif
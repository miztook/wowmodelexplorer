#include "stdafx.h"
#include "CInputReader.h"

#ifdef MW_PLATFORM_WINDOWS

#include "CSysUtility.h"
#include "IInputListener.h"

CInputReader::CInputReader()
{
	ProcessKeyboard = ProcessMouse = true;
}

CInputReader::~CInputReader()
{
	unacquire(EID_KEYBOARD);
	unacquire(EID_MOUSE);
}

bool CInputReader::acquire( E_INPUT_DEVICE device )
{
	if (device == EID_KEYBOARD)
		ProcessKeyboard = true;
	else if (device == EID_MOUSE)
		ProcessMouse = true;

	return true;
}

bool CInputReader::unacquire( E_INPUT_DEVICE device )
{
	if (device == EID_KEYBOARD)
		ProcessKeyboard = false;
	else if (device == EID_MOUSE)
		ProcessMouse = false;

	return false;
}

bool CInputReader::capture( E_INPUT_DEVICE device )
{
	return true;
}

bool CInputReader::isKeyPressed( u8 keycode )
{
	return ProcessKeyboard && CSysUtility::isKeyPressed(keycode);
}

bool CInputReader::isMousePressed( E_MOUSE_BUTTON button )
{
	if (!ProcessMouse)
		return false;

	switch(button)
	{
	case EMB_LEFT:
		return CSysUtility::isKeyPressed(VK_LBUTTON);
	case EMB_RIGHT:
		return CSysUtility::isKeyPressed(VK_RBUTTON);
	case EMB_MIDDLE:
		return CSysUtility::isKeyPressed(VK_MBUTTON);
		break;
	default:
		return false;
	}
}

void CInputReader::addListener( IInputListener* listener )
{
	ListenerList.push_back(listener);
}

void CInputReader::removeListener( IInputListener* listener )
{
	for (T_ListenerList::const_iterator itr = ListenerList.begin(); itr != ListenerList.end(); ++itr)
	{
		if ((*itr) == listener)
		{
			ListenerList.erase(itr);
			break;
		}
	}
}

void CInputReader::filterWindowProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	if (ProcessMouse)
	{
		switch(message)
		{
		case WM_LBUTTONDOWN:
			{
				for (T_ListenerList::const_iterator itr = ListenerList.begin(); itr != ListenerList.end(); ++itr)
				{
					(*itr)->onMouseMessage(hWnd, 
						Mouse_LeftButtonDown,
						(int)(short)LOWORD(lParam), 
						(int)(short)HIWORD(lParam));
				}
			}
			break;
		case WM_LBUTTONUP:
			{
				for (T_ListenerList::const_iterator itr = ListenerList.begin(); itr != ListenerList.end(); ++itr)
				{
					(*itr)->onMouseMessage(hWnd,
						Mouse_LeftButtonUp,
						(int)(short)LOWORD(lParam), 
						(int)(short)HIWORD(lParam));
				}
			}

			break;
		case WM_RBUTTONDOWN:
			{
				for (T_ListenerList::const_iterator itr = ListenerList.begin(); itr != ListenerList.end(); ++itr)
				{
					(*itr)->onMouseMessage(hWnd,
						Mouse_RightButtonDown,
						(int)(short)LOWORD(lParam), 
						(int)(short)HIWORD(lParam));
				}
			}
			break;
		case WM_RBUTTONUP:
			{
				for (T_ListenerList::const_iterator itr = ListenerList.begin(); itr != ListenerList.end(); ++itr)
				{
					(*itr)->onMouseMessage(hWnd,
						Mouse_RightButtonUp,
						(int)(short)LOWORD(lParam), 
						(int)(short)HIWORD(lParam));
				}
			}
			break;
		case WM_MOUSEMOVE:
			{
				for (T_ListenerList::const_iterator itr = ListenerList.begin(); itr != ListenerList.end(); ++itr)
				{
					(*itr)->onMouseMessage(hWnd,
						Mouse_Move,
						(int)(short)LOWORD(lParam), 
						(int)(short)HIWORD(lParam));
				}
			}
			break;
		case WM_MOUSEWHEEL:
			{
				for (T_ListenerList::const_iterator itr = ListenerList.begin(); itr != ListenerList.end(); ++itr)
				{
					(*itr)->onMouseWheel(hWnd, (int)(short)GET_WHEEL_DELTA_WPARAM(wParam));
				}
			}
			break;
		default:
			break;
		}
	}
	
	if (ProcessKeyboard)
	{
		switch(message)
		{
			case WM_KEYDOWN:
				{
					for (T_ListenerList::const_iterator itr = ListenerList.begin(); itr != ListenerList.end(); ++itr)
					{
						(*itr)->onKeyMessage(hWnd,
							Key_Down,
							(int)(short)wParam);
					}
				}
				break;
			case WM_KEYUP:
				{
					for (T_ListenerList::const_iterator itr = ListenerList.begin(); itr != ListenerList.end(); ++itr)
					{
						(*itr)->onKeyMessage(hWnd,
							Key_Up,
							(int)(short)wParam);
					}
				}
				break;
			default:
				break;
		}
	}
}

#endif
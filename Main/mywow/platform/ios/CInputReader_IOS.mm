#include "stdafx.h"
#include "CInputReader.h"

#ifdef MW_PLATFORM_IOS

#include "CSysUtility.h"

CInputReader::CInputReader()
{
	ProcessKeyboard = ProcessMouse = false;
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

	return false;
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

#endif
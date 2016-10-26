#include "stdafx.h"
#include "CDxInputReader.h"
#include "CSysUtility.h"
#include "IInputListener.h"

#ifdef MW_PLATFORM_WINDOWS

#ifdef MW_USE_DIRECTINPUT

#define  KEYBOARD_BUFFERSIZE		8
#define  MOUSE_BUFFERSIZE	16

CDxInputReader::SDIKChar g_DikChars[] =
{
	{DIK_ESCAPE, VK_ESCAPE},
	{DIK_RETURN, VK_RETURN},
	{DIK_SPACE, VK_SPACE},

	{DIK_1, '1'},
	{DIK_2, '2'},
	{DIK_3, '3'},
	{DIK_4, '4'},
	{DIK_5, '5'},
	{DIK_6, '6'},
	{DIK_7, '7'},
	{DIK_8, '8'},
	{DIK_9, '9'},
	{DIK_0, '0'},
	
	{DIK_UP, VK_UP},
	{DIK_DOWN, VK_DOWN},
	{DIK_LEFT, VK_LEFT},
	{DIK_RIGHT, VK_RIGHT},

	{DIK_A, 'A'},
	{DIK_B, 'B'},
	{DIK_C, 'C'},
	{DIK_D, 'D'},
	{DIK_E, 'E'},
	{DIK_F, 'F'},
	{DIK_G, 'G'},
	{DIK_H, 'H'},
	{DIK_I, 'I'},
	{DIK_J, 'J'},
	{DIK_K, 'K'},
	{DIK_L, 'L'},
	{DIK_M, 'M'},
	{DIK_N, 'N'},
	{DIK_O, 'O'},
	{DIK_P, 'P'},
	{DIK_Q, 'Q'},
	{DIK_R, 'R'},
	{DIK_S, 'S'},
	{DIK_T, 'T'},
	{DIK_U, 'U'},
	{DIK_V, 'V'},
	{DIK_W, 'W'},
	{DIK_Y, 'Y'},
	{DIK_X, 'X'},
	{DIK_Z, 'Z'},
};

typedef HRESULT (WINAPI *DXINPUT8CREATE)(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID *ppvOut, LPUNKNOWN punkOuter);

CDxInputReader::CDxInputReader()
{
	CHECK_SIZEOF4BYTES(*this);

	HWnd = NULL_PTR;
	HLib = NULL_PTR;

	pIDInput = NULL_PTR;
	pIDeviceKeyboard = NULL_PTR;
	pIDeviceMouse = NULL_PTR;

	memset( Diks, 0, sizeof(u8) * 256 );
	memset( &Dims2, 0, sizeof(DIMOUSESTATE2) );
	MouseButtons = 0;

	ModifierKey = EMK_NONE;

	u32 num = sizeof(g_DikChars) / sizeof(SDIKChar);
	for (u32 i=0; i<num; ++i)
	{
		DIK2CharMap[g_DikChars[i].dik] = g_DikChars[i].key;
		Char2DIKMap[g_DikChars[i].key] = g_DikChars[i].dik;
	}
}

CDxInputReader::~CDxInputReader()
{
	releaseMouse();
	releaseKeyboard();

	if (pIDInput)
		pIDInput->Release();

	if(HLib)
		FreeLibrary(HLib);
}

bool CDxInputReader::acquire(E_INPUT_DEVICE device)
{
	HRESULT hr = S_FALSE;
	switch(device)
	{
	case EID_KEYBOARD:
			hr = pIDeviceKeyboard->Acquire();
		break;
	case EID_MOUSE:
			hr = pIDeviceMouse->Acquire();
		break;
	default:
		break;
	}
	return hr == DI_OK;
}

bool CDxInputReader::unacquire(E_INPUT_DEVICE device)
{
	HRESULT hr = S_FALSE;
	switch(device)
	{
	case EID_KEYBOARD:
		hr = pIDeviceKeyboard->Unacquire();
		break;
	case EID_MOUSE:
		hr = pIDeviceMouse->Unacquire();
		break;
	default:
		break;
	}
	return hr == DI_OK;
}

bool CDxInputReader::capture(E_INPUT_DEVICE device)
{
	if (device == EID_KEYBOARD)
		return captureKeyboard();
	else if (device == EID_MOUSE)
		return captureMouse();
	else
		return false;
}

bool CDxInputReader::captureKeyboard()
{
	HRESULT hr;

	//捕捉当前状态
	memset( Diks, 0, sizeof(u8) * 256 );
	hr = pIDeviceKeyboard->GetDeviceState( sizeof(Diks), Diks );
	if (hr != DI_OK)
	{
		//DI_BUFFEROVERFLOW 或 DI_OHTERAPPHASPRIO 不处理
		if ( hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
		{
			pIDeviceKeyboard->Acquire();
			return false;
		}
	}

	//捕捉按键消息
	DIDEVICEOBJECTDATA didods[KEYBOARD_BUFFERSIZE];
	DWORD dwElements = KEYBOARD_BUFFERSIZE;
	hr = pIDeviceKeyboard->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), didods, &dwElements, 0);
	if (hr != DI_OK)
	{
		//DI_BUFFEROVERFLOW 或 DI_OHTERAPPHASPRIO 不处理
		if ( hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
		{
			pIDeviceKeyboard->Acquire();
		}
		return false;
	}

	if (dwElements == 0)
		return true;

// 	u32 modifier = EMK_NONE;
// 	if ((Diks[DIK_LCONTROL] & 0x80) || (Diks[DIK_RCONTROL] & 0x80))
// 		modifier |= EMK_CTRL;
// 	if ((Diks[DIK_LSHIFT] & 0x80) || (Diks[DIK_RSHIFT] & 0x80))
// 		modifier |= EMK_SHIFT;
// 	if ((Diks[DIK_LALT] & 0x80) || (Diks[DIK_RALT] & 0x80))
// 		modifier |= EMK_ALT;

	for (u32 i=0; i<dwElements; ++i)
	{
		bool down = (didods[i].dwData & 0x80) != 0;
		T_DIK2CharMap::const_iterator keyItr = DIK2CharMap.find((u8)didods[i].dwOfs);
		if (keyItr == DIK2CharMap.end())
			continue;
		u8 key = keyItr->second;
		E_INPUT_MESSAGE message = down ? Key_Down : Key_Up;
		for (T_ListenerList::const_iterator itr = ListenerList.begin(); itr != ListenerList.end(); ++itr)
		{
			(*itr)->onKeyMessage(HWnd, message, key);
		}
	}

	return true;
}

bool CDxInputReader::captureMouse()
{
	HRESULT hr;

	//捕捉当前状态
	memset( &Dims2, 0, sizeof(DIMOUSESTATE2) );
	MouseButtons = 0;

	hr = pIDeviceMouse->GetDeviceState( sizeof(DIMOUSESTATE2), &Dims2 );
	if (hr != DI_OK)
	{
		//DI_BUFFEROVERFLOW 或 DI_OHTERAPPHASPRIO 不处理
		if ( hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
		{
			pIDeviceMouse->Acquire();
		}
		return false;
	}

	vector3di coord(Dims2.lX, Dims2.lY, Dims2.lZ);
	for (u32 i=0; i<3; ++i)
	{
		if (Dims2.rgbButtons[i] & 0x80)
			MouseButtons |= (1<<i);
	}

	//捕捉按键消息
	DIDEVICEOBJECTDATA didods[MOUSE_BUFFERSIZE];
	DWORD dwElements = MOUSE_BUFFERSIZE;
	hr = pIDeviceMouse->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), didods, &dwElements, 0);
	if (hr != DI_OK)
	{
		//DI_BUFFEROVERFLOW 或 DI_OHTERAPPHASPRIO 不处理
		if ( hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
		{
			pIDeviceMouse->Acquire();
		}
		return false;
	}

	for (u32 i=0; i<dwElements; ++i)
	{
		bool down = (didods[i].dwData & 0x80) != 0;
		E_MOUSE_BUTTON button = EMB_NONE;
		s32 deltaX = 0;
		s32 deltaY = 0;
		s32 deltaZ = 0;
		switch(didods[i].dwOfs)
		{
		case DIMOFS_BUTTON0:
			button = EMB_LEFT;
			break;
		case DIMOFS_BUTTON1:
			button = EMB_RIGHT;
			break;
		case DIMOFS_BUTTON2:			//middle
			button = EMB_MIDDLE;
			break;
		case DIMOFS_X:
			deltaX = (s32)didods[i].dwData;
			break;
		case DIMOFS_Y:
			deltaY = (s32)didods[i].dwData;
			break;
		case DIMOFS_Z:
			deltaZ = (s32)didods[i].dwData;
			break;
		default:
			break;
		}

		for (T_ListenerList::const_iterator itr = ListenerList.begin(); itr != ListenerList.end(); ++itr)
		{
			vector2di pos;
			if(!CSysUtility::getCursorPosition(pos))
				return false;

			if (button == EMB_LEFT)
			{
				if (down)
					(*itr)->onMouseMessage(HWnd, Mouse_LeftButtonDown, pos.X, pos.Y);
				else
					(*itr)->onMouseMessage(HWnd, Mouse_LeftButtonUp, pos.X, pos.Y);
			}
			else if (button == EMB_RIGHT)
			{
				if (down)
					(*itr)->onMouseMessage(HWnd,Mouse_RightButtonDown, pos.X, pos.Y);
				else
					(*itr)->onMouseMessage(HWnd, Mouse_RightButtonUp, pos.X, pos.Y);
			}
			else if (button == EMB_MIDDLE && deltaZ)
			{
				(*itr)->onMouseWheel(HWnd, deltaZ);
			}

			if (deltaX || deltaY)
				(*itr)->onMouseMessage(HWnd, Mouse_Move, deltaX, deltaY);
		}
	}

	return true;
}

bool CDxInputReader::isKeyPressed( u8 keycode )
{
	T_Char2DIKMap::const_iterator itr = Char2DIKMap.find(keycode);
	if (itr != Char2DIKMap.end())
	{
		u8 dik = itr->second;
		return (Diks[dik] & 0x80) != 0;
	}	
	return false;
}

bool CDxInputReader::isMousePressed( E_MOUSE_BUTTON button )
{
	 return (MouseButtons & (int)button) != 0;
}

bool CDxInputReader::initDevice(HWND hwnd)
{
	ASSERT(::IsWindow(hwnd));

	HLib = ::LoadLibraryA("dinput8.dll");
	if (!HLib)
	{
		ASSERT(false);
		return false;
	}

	HWnd = hwnd;

	DXINPUT8CREATE dx8create = (DXINPUT8CREATE)::GetProcAddress(HLib, "DirectInput8Create");
	if(!dx8create) { ASSERT(false); return false; }

	if (FAILED(dx8create( GetModuleHandle(NULL_PTR), DIRECTINPUT_VERSION, IID_IDirectInput8,
		(LPVOID*)&pIDInput, NULL_PTR)))
	{
		ASSERT(false);
		return false;
	}

	if (!initializeKeyboard() || !initializeMouse())
		return false;

	return true;
}

bool CDxInputReader::initializeKeyboard()
{
	if (FAILED(pIDInput->CreateDevice( GUID_SysKeyboard, &pIDeviceKeyboard, NULL_PTR ) ))
	{
		ASSERT(false);
		return false;
	}

	if (FAILED(pIDeviceKeyboard->SetDataFormat(&c_dfDIKeyboard)))
	{
		ASSERT(false);
		return false;
	}

	DWORD dwCoopFlags = DISCL_EXCLUSIVE | DISCL_FOREGROUND;
	if (FAILED(pIDeviceKeyboard->SetCooperativeLevel(HWnd, dwCoopFlags)))
	{
		ASSERT(false);
		return false;
	}

	//buffer size
	DIPROPDWORD dipdw;
	dipdw.diph.dwSize = sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwObj = 0;
	dipdw.diph.dwHow = DIPH_DEVICE;
	dipdw.dwData = KEYBOARD_BUFFERSIZE;

	if (FAILED(pIDeviceKeyboard->SetProperty( DIPROP_BUFFERSIZE, &dipdw.diph )))
	{
		ASSERT(false);
		return false;
	}

	return true;
}

bool CDxInputReader::initializeMouse()
{
	if (FAILED(pIDInput->CreateDevice( GUID_SysMouse, &pIDeviceMouse, NULL_PTR ) ))
	{
		ASSERT(false);
		return false;
	}

	if (FAILED(pIDeviceMouse->SetDataFormat(&c_dfDIMouse2)))
	{
		ASSERT(false);
		return false;
	}

	DWORD dwCoopFlags = DISCL_EXCLUSIVE | DISCL_FOREGROUND;
	if (FAILED(pIDeviceMouse->SetCooperativeLevel(HWnd, dwCoopFlags)))
	{
		ASSERT(false);
		return false;
	}

	//buffer size
	DIPROPDWORD dipdw;
	dipdw.diph.dwSize = sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwObj = 0;
	dipdw.diph.dwHow = DIPH_DEVICE;
	dipdw.dwData = MOUSE_BUFFERSIZE;

	if (FAILED(pIDeviceMouse->SetProperty( DIPROP_BUFFERSIZE, &dipdw.diph )))
	{
		ASSERT(false);
		return false;
	}

	return true;
}

void CDxInputReader::releaseKeyboard()
{
	if (pIDeviceKeyboard)
	{
		pIDeviceKeyboard->Unacquire();
		pIDeviceKeyboard->Release();
	}
}

void CDxInputReader::releaseMouse()
{
	if (pIDeviceMouse)
	{
		pIDeviceMouse->Unacquire();
		pIDeviceMouse->Release();
	}
}

void CDxInputReader::addListener( IInputListener* listener )
{
	ListenerList.push_back(listener);
}

void CDxInputReader::removeListener( IInputListener* listener )
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

#endif

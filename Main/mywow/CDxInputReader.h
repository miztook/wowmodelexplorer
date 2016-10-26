#pragma once

#include "core.h"
#include "IInputReader.h"
#include <unordered_map>
#include <list>

#ifdef MW_PLATFORM_WINDOWS

#ifdef MW_USE_DIRECTINPUT

#include <dinput.h>

class CDxInputReader : public IInputReader
{
private:
	DISALLOW_COPY_AND_ASSIGN(CDxInputReader);

public: 
	CDxInputReader();
	~CDxInputReader();

public:
	virtual bool acquire(E_INPUT_DEVICE device);
	virtual bool unacquire(E_INPUT_DEVICE device);
	virtual bool capture(E_INPUT_DEVICE device);		//capture当前设备状态
	virtual bool isKeyPressed(u8 keycode);
	virtual bool isMousePressed(E_MOUSE_BUTTON button);

	virtual void addListener(IInputListener* listener);
	virtual void removeListener(IInputListener* listener);

	bool initDevice(HWND hwnd);

public:
	struct SDIKChar
	{
		u8	dik;
		u8	key;
	};

private:
	bool initializeKeyboard();
	bool initializeMouse();
	void releaseKeyboard();
	void releaseMouse();
	bool captureKeyboard();
	bool captureMouse();

private:
	HWND	HWnd;
	HMODULE		HLib;

	LPDIRECTINPUT8		pIDInput;
	LPDIRECTINPUTDEVICE8		pIDeviceKeyboard;
	LPDIRECTINPUTDEVICE8		pIDeviceMouse;

	u8	Diks[256];
	DIMOUSESTATE2		Dims2;
	u32		MouseButtons;

	E_MODIFIER_KEY		ModifierKey;

	typedef std::unordered_map<u8, u8> T_DIK2CharMap;
	T_DIK2CharMap	DIK2CharMap;

	typedef std::unordered_map<u8, u8> T_Char2DIKMap;
	T_Char2DIKMap	Char2DIKMap;

	typedef std::list<IInputListener*, qzone_allocator<IInputListener*> >		T_ListenerList;
	T_ListenerList	ListenerList;
};

#endif

#endif
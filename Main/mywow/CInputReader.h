#pragma once

#include "IInputReader.h"
#include <list>

class CInputReader : public IInputReader
{
private:
	DISALLOW_COPY_AND_ASSIGN(CInputReader);

public: 
	CInputReader();
	~CInputReader();

public:
	virtual bool acquire(E_INPUT_DEVICE device);
	virtual bool unacquire(E_INPUT_DEVICE device);
	virtual bool capture(E_INPUT_DEVICE device);
	virtual bool isKeyPressed(u8 keycode);
	virtual bool isMousePressed(E_MOUSE_BUTTON button);

	virtual void addListener(IInputListener* listener);
	virtual void removeListener(IInputListener* listener);

#ifdef MW_PLATFORM_WINDOWS
public:
	void filterWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
#endif

private:
	bool ProcessMouse;
	bool ProcessKeyboard;

	typedef std::list<IInputListener*, qzone_allocator<IInputListener*> >		T_ListenerList;
	T_ListenerList	ListenerList;
};
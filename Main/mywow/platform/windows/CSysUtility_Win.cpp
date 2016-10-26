#include "stdafx.h"
#include "CSysUtility.h"
#include "mywow.h"
#include "CInputReader.h"
#include "CGestureReader.h"

#include "compileconfig.h"

#ifdef MW_PLATFORM_WINDOWS

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h>
#include <WinInet.h>

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (g_Engine)
	{
		IMessageHandler* handler = g_Engine->getMessageHandler();
		if (handler)
		{
			switch(message)
			{
			case WM_QUIT:
			case WM_CLOSE:
				handler->onExit(hWnd);
				break;
			case WM_SIZE:
				handler->onSize(hWnd, (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam));
				break;
			default:
				break;
			}
		}

#ifdef MW_USE_TOUCH_INPUT
		IGestureReader* gestureReader = g_Engine->getGestureReader();
		if (gestureReader)
			static_cast<CGestureReader*>(gestureReader)->filterWindowProc(hWnd, message, wParam, lParam);
#else
		IInputReader* inputReader = g_Engine->getInputReader();
		if (inputReader)
			static_cast<CInputReader*>(inputReader)->filterWindowProc(hWnd, message, wParam, lParam);
#endif
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

bool CSysUtility::isKeyPressed( u8 vk )
{
	return ( GetAsyncKeyState(vk) & 0x8000 ) != 0;
}

bool CSysUtility::getCursorPosition( vector2di& pos )
{
	POINT pt;
	if( ::GetCursorPos(&pt) )
	{
		pos.X = (s32)pt.x;
		pos.Y = (s32)pt.y;
		return true;
	}
	return false;
}

bool CSysUtility::openURLtoJsonFile( const c8* url, const c8* filename )
{
	string_path path = g_Engine->getFileSystem()->getBaseDirectory();
	path.append(filename);

	//process url
	string1024 strFinal;
	{
		string1024 strUrl = url;

		u32 startPos = 0;
		s32 find = -1;
		if (!strUrl.beginWith("http://"))
		{
			strFinal = "http://";
		}
		else
		{
			startPos = (u32)strlen("http://");
		}

		find = strUrl.findNext('/', startPos);
		if (find == -1)
			return false;

		{
			string1024 tmp;
			strUrl.subString(0, find + 1, tmp);
			strFinal.append(tmp);
		}

		startPos = find + 1;

		strFinal.append("api/wow/character/");

		for (u32 i=0; i<3; ++i)
		{			
			find = strUrl.findNext('/', startPos);
			if (find == -1)
				return false;

			startPos = find + 1;
		}
		
		u32 pos = startPos;

		//realm
		find = strUrl.findNext('/', startPos);
		if (find == -1)
			return false;

		startPos = find + 1;
	
		//name
		find = strUrl.findNext('/', startPos);
		if (find == -1)
		{
			find = strUrl.findNext('?', startPos);
		}
		if (find == -1)
			return false;

		{
			string1024 tmp;
			strUrl.subString(pos, find - pos, tmp);
			strFinal.append(tmp);
		}

		strFinal.append("?fields=appearance,items");
	}

	bool ret = false;

	//request
	c8 buffer[1024];

	HINTERNET hSession = InternetOpen("mywowUrl", INTERNET_OPEN_TYPE_PRECONFIG, NULL_PTR, NULL_PTR, 0);
	if(hSession != NULL_PTR)
	{
		HINTERNET hHttp = InternetOpenUrl(hSession, strFinal.c_str(), NULL_PTR, 0, INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE, 0);

		if (hHttp != NULL_PTR)
		{
			IWriteFile* file = g_Engine->getFileSystem()->createAndWriteFile(path.c_str(), false);
			if (file)
			{
				ULONG Number = 1;
				while (Number > 0)
				{
					memset(buffer, 0, 1024);

					InternetReadFile(hHttp, buffer, 1023, &Number);

					file->writeText(buffer, Number);
				}

				delete file;

				ret = true;
			}

			InternetCloseHandle(hHttp);
			hHttp = NULL_PTR;
		}

		InternetCloseHandle(hSession);
		hSession = NULL_PTR;
	} 

	return ret;
}

void CSysUtility::messageBoxWarning( const c8* msg )
{
	MessageBoxA(NULL_PTR, msg, "warning", MB_ICONEXCLAMATION);
}

void CSysUtility::outputDebug( const c8* format, ... )
{
	c8 str[1024];

	va_list va;
	va_start( va, format );
	Q_vsprintf( str, 1024, format, va );
	va_end( va );

	Q_strcat(str, 1024, "\n");
	OutputDebugStringA(str);
}

void CSysUtility::exitProcess( int ret )
{
	::ExitProcess(ret);
}

SWindowInfo CSysUtility::createWindow( const char* caption, const dimension2du& windowSize, f32 scale, bool fullscreen, bool hide )
{
	if (fullscreen)
		hide = false;

	HINSTANCE hInstance = ::GetModuleHandle(NULL_PTR);
	HWND hwnd;

	const char* className = "mywowWindow";

	WNDCLASSEX wcex;
	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL_PTR;
	wcex.hCursor		= ::LoadCursor(NULL_PTR, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)COLOR_BACKGROUND;
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= className;
	wcex.hIconSm		= 0;
	wcex.hIcon			= ::LoadIcon(NULL_PTR, IDI_APPLICATION);

	::RegisterClassEx( &wcex );

	RECT clientSize;
	clientSize.top = 0;
	clientSize.left = 0;
	clientSize.right = (LONG)(windowSize.Width);
	clientSize.bottom = (LONG)(windowSize.Height);

	DWORD style = WS_POPUP;

	if (!fullscreen && !hide)
		style = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

	if (hide)
	{
		style &= (~WS_VISIBLE);
	}

	AdjustWindowRect(&clientSize, style, FALSE);

	s32 width = clientSize.right - clientSize.left;
	s32 height = clientSize.bottom - clientSize.top;

	s32 windowLeft = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
	s32 windowTop = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

	if ( windowLeft < 0 )
		windowLeft = 0;
	if ( windowTop < 0 )
		windowTop = 0;	// make sure window menus are in screen on creation

	hwnd = ::CreateWindow( className, caption, style, windowLeft, windowTop,
		width, height, NULL_PTR, NULL_PTR, hInstance, NULL_PTR);

	::ShowWindow(hwnd, hide? SW_HIDE : SW_SHOW);
	::UpdateWindow(hwnd);

	::MoveWindow(hwnd, windowLeft, windowTop, width, height, TRUE);

	if (!hide)
	{
		::SetActiveWindow(hwnd);
		::SetForegroundWindow(hwnd);
	}

	SWindowInfo windowInfo;
	windowInfo.width = (u32)width;
	windowInfo.height = (u32)height;
	windowInfo.scale = scale;
	windowInfo.hwnd = hwnd;

	return windowInfo;
}



#endif
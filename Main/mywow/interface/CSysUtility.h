#pragma once

#include "core.h"
#include "SWindowInfo.h"

class CSysUtility
{
public:
	static bool isKeyPressed(u8 vk);

	static bool getCursorPosition(vector2di& pos);

	static bool openURLtoJsonFile(const c8* url, const c8* filename);

	static SWindowInfo createWindow( const char* caption, const dimension2du& windowSize, f32 scale, bool fullscreen, bool hide );

	static void messageBoxWarning(const c8* msg);

	static void outputDebug(const c8* format, ...);

	static void exitProcess(int ret);

};
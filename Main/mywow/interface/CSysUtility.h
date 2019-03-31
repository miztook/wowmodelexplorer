#pragma once

#include "core.h"
#include "SWindowInfo.h"

class CSysUtility
{
public:
	static bool isKeyPressed(uint8_t vk);

	static bool getCursorPosition(vector2di& pos);

	static bool openURLtoJsonFile(const char* url, const char* filename);

	static SWindowInfo createWindow( const char* caption, const dimension2du& windowSize, float scale, bool fullscreen, bool hide );

	static void messageBoxWarning(const char* msg);

	static void outputDebug(const char* format, ...);

	static void exitProcess(int ret);

};
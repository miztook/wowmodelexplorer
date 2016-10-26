#include "stdafx.h"
#include "CSysUtility.h"
#include "mywow.h"

#include "compileconfig.h"

#ifdef MW_PLATFORM_IOS

#import <Foundation/NSObjCRuntime.h>

bool CSysUtility::isKeyPressed( u8 vk )
{
	return false;
}

bool CSysUtility::getCursorPosition( vector2di& pos )
{
	return false;
}

bool CSysUtility::openURLtoJsonFile( const c8* url, const c8* filename )
{
	return false;
}

void CSysUtility::messageBoxWarning( const c8* msg )
{
	
}

void CSysUtility::outputDebug( const c8* format, ... )
{
	c8 str[1024];

	va_list va;
	va_start( va, format );
	Q_vsprintf( str, 1024, format, va );
	va_end( va );

    NSLog(@"%s\n", str);

}

void CSysUtility::exitProcess( int ret )
{
	exit(ret);
}

SWindowInfo CSysUtility::createWindow( const char* caption, const dimension2du& windowSize, f32 scale, bool fullscreen, bool hide )
{
	SWindowInfo windowInfo;
	windowInfo.width = windowSize.Width;
	windowInfo.height = windowSize.Height;
	windowInfo.scale = scale;
	windowInfo.hwnd = NULL;

	return windowInfo;
}

#endif
#include "stdafx.h"
#include "editor_Driver.h"

void  Driver_setDisplayMode( uint32_t width, uint32_t height )
{
	g_Engine->getDriver()->setDisplayMode(dimension2du(width, height));
}

void Driver_getDisplayMode( uint32_t* width, uint32_t* height )
{
	dimension2du size = g_Engine->getDriver()->getDisplayMode();
	*width = size.Width;
	*height = size.Height;
}


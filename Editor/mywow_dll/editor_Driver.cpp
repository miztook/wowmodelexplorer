#include "stdafx.h"
#include "editor_Driver.h"

void  Driver_setDisplayMode( u32 width, u32 height )
{
	g_Engine->getDriver()->setDisplayMode(dimension2du(width, height));
}

void Driver_getDisplayMode( u32* width, u32* height )
{
	dimension2du size = g_Engine->getDriver()->getDisplayMode();
	*width = size.Width;
	*height = size.Height;
}


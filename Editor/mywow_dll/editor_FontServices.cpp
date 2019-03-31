#include "stdafx.h"
#include "editor_FontServices.h"

void FontServices_onWindowSizeChanged( uint32_t width, uint32_t height )
{
	g_Engine->getFontServices()->onWindowSizeChanged(dimension2du(width, height));
}

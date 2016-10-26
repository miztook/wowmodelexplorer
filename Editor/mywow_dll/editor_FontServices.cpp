#include "stdafx.h"
#include "editor_FontServices.h"

void FontServices_onWindowSizeChanged( u32 width, u32 height )
{
	g_Engine->getFontServices()->onWindowSizeChanged(dimension2du(width, height));
}

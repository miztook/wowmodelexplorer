#include "stdafx.h"
#include "editor_DrawServices.h"

void DrawServices_setLineZTestEnable( bool enable )
{
	SMaterial& mat = g_Engine->getDrawServices()->getLineMaterial();
	mat.ZBuffer = enable ? ECFN_LESSEQUAL : ECFN_ALWAYS;
}

bool DrawServices_getLineZTestEnable()
{
	SMaterial& mat = g_Engine->getDrawServices()->getLineMaterial();
	return mat.ZBuffer == ECFN_LESSEQUAL;
}

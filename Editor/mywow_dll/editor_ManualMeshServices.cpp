#include "stdafx.h"
#include "editor_ManualMeshServices.h"

bool  ManualMeshServices_addGridMesh( const char* name, uint32_t xzCount, float gridSize, SColor color )
{
	return g_Engine->getManualMeshServices()->addGridLineMesh(name, xzCount, gridSize, color);
}

void  ManualMeshServices_removeMesh( const char* name )
{
	g_Engine->getManualMeshServices()->removeMesh(name);
}
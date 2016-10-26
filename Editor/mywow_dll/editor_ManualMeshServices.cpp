#include "stdafx.h"
#include "editor_ManualMeshServices.h"

bool  ManualMeshServices_addGridMesh( const c8* name, u32 xzCount, f32 gridSize, SColor color )
{
	return g_Engine->getManualMeshServices()->addGridLineMesh(name, xzCount, gridSize, color);
}

void  ManualMeshServices_removeMesh( const c8* name )
{
	g_Engine->getManualMeshServices()->removeMesh(name);
}
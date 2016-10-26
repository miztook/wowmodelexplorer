#pragma once

#include "editor_base.h"
#include "SColor.h"

MW_API bool  ManualMeshServices_addGridMesh(const c8* name, u32 xzCount, f32 gridSize, SColor color);
MW_API void  ManualMeshServices_removeMesh(const c8* name);
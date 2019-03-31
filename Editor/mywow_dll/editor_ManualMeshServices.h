#pragma once

#include "editor_base.h"
#include "SColor.h"

MW_API bool  ManualMeshServices_addGridMesh(const char* name, uint32_t xzCount, float gridSize, SColor color);
MW_API void  ManualMeshServices_removeMesh(const char* name);

#pragma once

#include "editor_base.h"
#include "editor_structs.h"

using namespace editor;

MW_API void WDTSceneNode_setCurrentTile(IWDTSceneNode* node, int32_t row, int32_t col);
MW_API void WDTSceneNode_getCenter(IWDTSceneNode* node, vector3df* pos);
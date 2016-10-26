#pragma once

#include "editor_base.h"
#include "editor_structs.h"

using namespace editor;

MW_API void WDTSceneNode_setCurrentTile(IWDTSceneNode* node, s32 row, s32 col);
MW_API void WDTSceneNode_getCenter(IWDTSceneNode* node, vector3df* pos);
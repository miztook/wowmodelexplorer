#pragma once

#include "editor_base.h"

MW_API f32 World_getHeightNormal(IWorld* world, f32 x, f32 z, vector3df* normal);

MW_API void World_setMapTileSceneNode(IWorld* world, IMapTileSceneNode* node);

MW_API void World_setWdtSceneNode(IWorld* world, IWDTSceneNode* node);
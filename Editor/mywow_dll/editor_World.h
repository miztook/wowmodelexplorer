#pragma once

#include "editor_base.h"

MW_API float World_getHeightNormal(IWorld* world, float x, float z, vector3df* normal);

MW_API void World_setMapTileSceneNode(IWorld* world, IMapTileSceneNode* node);

MW_API void World_setWdtSceneNode(IWorld* world, IWDTSceneNode* node);
#pragma once

#include "editor_base.h"
#include "editor_structs.h"

using namespace editor;

MW_API void CoordSceneNode_setPosition2D(ICoordSceneNode* node, ICoordSceneNode::E_POSITION_2D position, f32 distance);
MW_API void CoordSceneNode_setAxisColor(ICoordSceneNode* node, SColor colorX, SColor colorY, SColor colorZ);
MW_API void CoordSceneNode_setAxisText(ICoordSceneNode* node, const c8* textX, const c8* textY, const c8* textZ);
MW_API void CoordSceneNode_setAxisVisible(ICoordSceneNode* node, bool xVisible, bool yVisible, bool zVisible);

MW_API void CoordSceneNode_setDir(ICoordSceneNode* node, ICoordSceneNode::E_AXIS axis, vector3df dir);
MW_API void CoordSceneNode_getDir(ICoordSceneNode* node, ICoordSceneNode::E_AXIS axis, vector3df* dir);

MW_API void CoordSceneNode_pitch_yaw_FPS(ICoordSceneNode* node, float pitchDegree, float yawDegree);
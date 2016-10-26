#pragma once

#include "editor_base.h"
#include "editor_structs.h"

using namespace editor;

MW_API void  M2SceneNode_getFileM2(IM2SceneNode* node, SFileM2* filem2);
MW_API M2Type  M2SceneNode_getType(IM2SceneNode* node);
MW_API bool  M2SceneNode_isNpc(IM2SceneNode* node);
MW_API void  M2SceneNode_buildVisibleGeosets(IM2SceneNode* node);
MW_API void  M2SceneNode_updateCharacter(IM2SceneNode* node);
MW_API bool  M2SceneNode_updateNpc(IM2SceneNode* node, s32 npcid);

//camera
MW_API bool  M2SceneNode_setModelCamera(IM2SceneNode* node, s32 cameraIndex);
MW_API s32  M2SceneNode_getModelCamera(IM2SceneNode* node);

//animations
MW_API bool  M2SceneNode_playAnimationByName(IM2SceneNode* node, const c8* name, u32 subIdx, bool loop, s32 timeblend);
MW_API bool  M2SceneNode_playAnimationByIndex(IM2SceneNode* node, u32 anim, bool loop, s32 timeblend);
MW_API u32  M2SceneNode_getAnimationCount(IM2SceneNode* node);
MW_API bool  M2SceneNode_getAnimation(IM2SceneNode* node, u32 index, SAnimation* anim);


//item
MW_API void  M2SceneNode_resetSlot(IM2SceneNode* node, s32 slot);

//animation player
MW_API void  M2SceneNode_pause(IM2SceneNode* node);
MW_API void  M2SceneNode_resume(IM2SceneNode* node);
MW_API void  M2SceneNode_stop(IM2SceneNode* node);
MW_API void  M2SceneNode_step(IM2SceneNode* node, f32 frame);
MW_API bool  M2SceneNode_isPlaying(IM2SceneNode* node);
MW_API s32  M2SceneNode_getCurrentAnimationIndex(IM2SceneNode* node);
MW_API f32  M2SceneNode_getCurrentFrame(IM2SceneNode* node);
MW_API void  M2SceneNode_setCurrentFrame(IM2SceneNode* node, f32 frame);
MW_API void  M2SceneNode_setLoop(IM2SceneNode* node, bool loop);
MW_API bool  M2SceneNode_isLoop(IM2SceneNode* node);

//show/hide
MW_API void  M2SceneNode_showModelPart(IM2SceneNode* node, E_MODEL_PART modelpart, bool show);
MW_API void  M2SceneNode_showEditPart(IM2SceneNode* node, E_MODEL_EDIT_PART editpart, bool show);
MW_API void  M2SceneNode_setWireframe(IM2SceneNode* node, editor::E_OVERRIDE_WIREFRAME wireframe);

//child scene nodes
MW_API bool M2SceneNode_getChildSceneNodes(IM2SceneNode* node, editor::SM2ChildSceneNodes* childSceneNodes);

//file property
MW_API const c8* M2SceneNode_getReplaceTextureFileName(IM2SceneNode* node, editor::E_MODEL_REPLACE_TEXTURE texture);
MW_API const c8* M2SceneNode_getTextureFileName(IM2SceneNode* node, u32 index);
MW_API const c8* M2SceneNode_getRegionTextureFileName(IM2SceneNode* node, ECharRegions region);
MW_API bool  M2SceneNode_getRenderFlag(IM2SceneNode* node, SRenderFlag* renderflag, u32 index);
MW_API bool  M2SceneNode_getGeoset(IM2SceneNode* node, u32 index, editor::SGeoset* geoset);
MW_API void  M2SceneNode_showGeoset(IM2SceneNode* node, u32 index, bool show);
MW_API bool M2SceneNode_isGeosetShow(IM2SceneNode* node, u32 index);
MW_API void M2SceneNode_showAttachment(IM2SceneNode* node, u32 index, bool show);
MW_API bool M2SceneNode_isAttachmentShow(IM2SceneNode* node, u32 index);

//
MW_API void M2SceneNode_setModelAlpha(IM2SceneNode* node, f32 val);
MW_API f32 M2SceneNode_getModelAlpha(IM2SceneNode* node);
MW_API void M2SceneNode_setModelColor(IM2SceneNode* node, SColor color);
MW_API void M2SceneNode_getModelColor(IM2SceneNode* node, SColor* color);

//m2move
MW_API wow_m2Move* M2SceneNode_getM2Move(IM2SceneNode* node);
MW_API wow_m2instance* M2SceneNode_getM2Instance(IM2SceneNode* node);
MW_API wow_m2FSM* M2SceneNode_getM2FSM(IM2SceneNode* node);
MW_API wow_m2appearance* M2SceneNode_getM2Appearance(IM2SceneNode* node);
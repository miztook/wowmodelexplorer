#pragma once

#include "editor_base.h"
#include "editor_structs.h"

using namespace editor;

MW_API void  WMOSceneNode_showEditPart(IWMOSceneNode* node, editor::E_WMO_EDIT_PART editpart, bool show);
MW_API void  WMOSceneNode_setWireframe(IWMOSceneNode* node, editor::E_OVERRIDE_WIREFRAME wireframe);

MW_API void  WMOSceneNode_getFileWMO(IWMOSceneNode* node, SFileWMO* filewmo);

MW_API bool  WMOSceneNode_getGroup(IWMOSceneNode* node, u32 index, editor::SWMOGroup* group);
MW_API void  WMOSceneNode_showGroup(IWMOSceneNode* node, u32 index, bool show);
MW_API bool WMOSceneNode_isGroupShow(IWMOSceneNode* node, u32 index);

MW_API void WMOSceneNode_showGroupBox(IWMOSceneNode* node, u32 index, bool show, SColor color);
MW_API bool WMOSceneNode_isGroupBoxShow(IWMOSceneNode* node, u32 index);

MW_API bool WMOSceneNode_getPortal(IWMOSceneNode* node, u32 index, editor::SWMOPortal* portal);
MW_API void WMOSceneNode_showPortal(IWMOSceneNode* node, u32 index, bool show, SColor color);
MW_API bool WMOSceneNode_isPortalShow(IWMOSceneNode* node, u32 index);

MW_API void WMOSceneNode_showPortalGroups(IWMOSceneNode* node, u32 index, bool show);
MW_API bool WMOSceneNode_isPortalGroupsShow(IWMOSceneNode* node, u32 index);

MW_API bool WMOSceneNode_getDoodadName(IWMOSceneNode* node, u32 index, c8* filename, u32 size);
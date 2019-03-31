#pragma once

#include "editor_base.h"
#include "editor_structs.h"

using namespace editor;

MW_API void  WMOSceneNode_showEditPart(IWMOSceneNode* node, editor::E_WMO_EDIT_PART editpart, bool show);
MW_API void  WMOSceneNode_setWireframe(IWMOSceneNode* node, editor::E_OVERRIDE_WIREFRAME wireframe);

MW_API void  WMOSceneNode_getFileWMO(IWMOSceneNode* node, SFileWMO* filewmo);

MW_API bool  WMOSceneNode_getGroup(IWMOSceneNode* node, uint32_t index, editor::SWMOGroup* group);
MW_API void  WMOSceneNode_showGroup(IWMOSceneNode* node, uint32_t index, bool show);
MW_API bool WMOSceneNode_isGroupShow(IWMOSceneNode* node, uint32_t index);

MW_API void WMOSceneNode_showGroupBox(IWMOSceneNode* node, uint32_t index, bool show, SColor color);
MW_API bool WMOSceneNode_isGroupBoxShow(IWMOSceneNode* node, uint32_t index);

MW_API bool WMOSceneNode_getPortal(IWMOSceneNode* node, uint32_t index, editor::SWMOPortal* portal);
MW_API void WMOSceneNode_showPortal(IWMOSceneNode* node, uint32_t index, bool show, SColor color);
MW_API bool WMOSceneNode_isPortalShow(IWMOSceneNode* node, uint32_t index);

MW_API void WMOSceneNode_showPortalGroups(IWMOSceneNode* node, uint32_t index, bool show);
MW_API bool WMOSceneNode_isPortalGroupsShow(IWMOSceneNode* node, uint32_t index);

MW_API bool WMOSceneNode_getDoodadName(IWMOSceneNode* node, uint32_t index, char* filename, uint32_t size);

#pragma once

#include "editor_base.h"
#include "editor_structs.h"

MW_API void  mwTool_create();
MW_API void  mwTool_destroy();

MW_API bool mwTool_openUrlToJsonFile(const c8* url, const c8* filename);
MW_API bool mwTool_parseCharacterArmoryInfo(const c8* filename, editor::SCharacterArmoryInfo* charInfo);
MW_API bool mwTool_exportBlpAsTga(const c8* blpfilename, const c8* tgafilename, bool bAlpha);
MW_API bool mwTool_exportBlpAsTgaDir(const char* blpfilename, const char* dirname, bool bAlpha);

MW_API bool mwTool_exportM2SceneNodeToOBJ(IM2SceneNode* node, const c8* dirname);
MW_API bool mwTool_exportWMOSceneNodeToOBJ(IWMOSceneNode* node, const c8* dirname);
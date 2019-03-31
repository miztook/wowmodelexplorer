#pragma once

#include "editor_base.h"
#include "editor_structs.h"

MW_API void  mwTool_create();
MW_API void  mwTool_destroy();

MW_API bool mwTool_openUrlToJsonFile(const char* url, const char* filename);
MW_API bool mwTool_parseCharacterArmoryInfo(const char* filename, editor::SCharacterArmoryInfo* charInfo);
MW_API bool mwTool_exportBlpAsTga(const char* blpfilename, const char* tgafilename, bool bAlpha);
MW_API bool mwTool_exportBlpAsTgaDir(const char* blpfilename, const char* dirname, bool bAlpha);

MW_API bool mwTool_exportM2SceneNodeToOBJ(IM2SceneNode* node, const char* dirname);
MW_API bool mwTool_exportWMOSceneNodeToOBJ(IWMOSceneNode* node, const char* dirname);

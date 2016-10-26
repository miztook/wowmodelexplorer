#pragma once

#include "editor_base.h"
#include "editor_structs.h"

using namespace editor;

MW_API u32  WowDatabase_getItemCount();
MW_API bool  WowDatabase_getItem(u32 i, editor::SItem* item);
MW_API bool WowDatabase_getItemById(s32 id, editor::SItem* item);
MW_API u32  WowDatabase_getNpcCount();
MW_API bool  WowDatabase_getNpc(u32 i, editor::SNpc* npc);
MW_API bool  WowDatabase_getNpcById(s32 id, editor::SNpc* npc);
MW_API bool  WowDatabase_getRaceId(const c8* racename, u32* id);
MW_API const c8* WowDatabase_getRaceName(u32 id);
MW_API u32  WowDatabase_getStartOutfitCount(u32 race, bool female);
MW_API bool  WowDatabase_getStartOutfit(u32 race, bool female, u32 i, editor::SStartOutfit* entry);
MW_API u32  WowDatabase_getSetCount();
MW_API bool  WowDatabase_getSet(u32 i, editor::SEntry* entry);
MW_API u32  WowDatabase_getMapCount();
MW_API bool  WowDatabase_getMap(u32 i, editor::SMap* map);
MW_API u32  WowDatabase_getSpellVisualEffectCount();
MW_API bool  WowDatabase_getSpellVisualEffectId(u32 i, int* id);
MW_API u32 WowDatabase_getWmoCount();
MW_API const c8* WowDatabase_getWMOFileName(u32 index, bool shortname);
MW_API u32 WowDatabase_getWorldModelCount();
MW_API const c8* WowDatabase_getWorldModelFileName(u32 index, bool shortname);
MW_API u32 WowDatabase_getTextureCount();
MW_API const c8* WowDatabase_getTextureFileName(u32 index);
MW_API const c8* WowDatabase_getClassShortName(u32 id);
MW_API bool WowDatabase_getClassInfo(const c8* shortname, editor::SEntry* entry);
MW_API u32 WowDatabase_getRidableCount();
MW_API bool WowDatabase_getRidable(u32 i, editor::SRidable* ridable);

MW_API void  WowDatabase_buildItems();
MW_API bool  WowDatabase_buildNpcs(const c8* filename);
MW_API void  WowDatabase_buildStartOutfitClass();
MW_API void  WowDatabase_buildMaps();
MW_API void  WowDatabase_buildWmos();
MW_API void  WowDatabase_buildWorldModels();
MW_API void  WowDatabase_buildTextures();
MW_API void WowDatabase_buildRidables(const c8* filename);

MW_API bool  WowDatabase_getCharacterPath(const c8* raceName, bool female, bool isHD, c8* path, u32 size);
MW_API bool  WowDatabase_itemIsCorrectType(s32 type, s32 slot);
MW_API void  WowDatabase_getMaxCharFeature(u32 race, bool female, bool isHD, SCharFeature* feature);
MW_API bool  WowDatabase_getNpcPath(s32 npcid, bool isHD, c8* path, u32 size);
MW_API bool  WowDatabase_getItemVisualPath(s32 visualId, c8* path, u32 size);
MW_API const c8*  WowDatabase_getSpellVisualEffectName(s32 visual);
MW_API bool  WowDatabase_getSpellVisualEffectPath(s32 visualId, c8* path, u32 size);
MW_API bool WowDatabase_getItemPath(s32 itemid, c8* modelpath, u32 modelSize, c8* texturepath, u32 texSize);
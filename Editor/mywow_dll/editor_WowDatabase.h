#pragma once

#include "editor_base.h"
#include "editor_structs.h"

using namespace editor;

MW_API uint32_t  WowDatabase_getItemCount();
MW_API bool  WowDatabase_getItem(uint32_t i, editor::SItem* item);
MW_API bool WowDatabase_getItemById(int32_t id, editor::SItem* item);
MW_API uint32_t  WowDatabase_getNpcCount();
MW_API bool  WowDatabase_getNpc(uint32_t i, editor::SNpc* npc);
MW_API bool  WowDatabase_getNpcById(int32_t id, editor::SNpc* npc);
MW_API bool  WowDatabase_getRaceId(const char* racename, uint32_t* id);
MW_API const char* WowDatabase_getRaceName(uint32_t id);
MW_API uint32_t  WowDatabase_getStartOutfitCount(uint32_t race, bool female);
MW_API bool  WowDatabase_getStartOutfit(uint32_t race, bool female, uint32_t i, editor::SStartOutfit* entry);
MW_API uint32_t  WowDatabase_getSetCount();
MW_API bool  WowDatabase_getSet(uint32_t i, editor::SEntry* entry);
MW_API uint32_t  WowDatabase_getMapCount();
MW_API bool  WowDatabase_getMap(uint32_t i, editor::SMap* map);
MW_API uint32_t  WowDatabase_getSpellVisualEffectCount();
MW_API bool  WowDatabase_getSpellVisualEffectId(uint32_t i, int* id);
MW_API uint32_t WowDatabase_getWmoCount();
MW_API const char* WowDatabase_getWMOFileName(uint32_t index, bool shortname);
MW_API uint32_t WowDatabase_getWorldModelCount();
MW_API const char* WowDatabase_getWorldModelFileName(uint32_t index, bool shortname);
MW_API uint32_t WowDatabase_getTextureCount();
MW_API const char* WowDatabase_getTextureFileName(uint32_t index);
MW_API const char* WowDatabase_getClassShortName(uint32_t id);
MW_API bool WowDatabase_getClassInfo(const char* shortname, editor::SEntry* entry);
MW_API uint32_t WowDatabase_getRidableCount();
MW_API bool WowDatabase_getRidable(uint32_t i, editor::SRidable* ridable);

MW_API void  WowDatabase_buildItems();
MW_API bool  WowDatabase_buildNpcs(const char* filename);
MW_API void  WowDatabase_buildStartOutfitClass();
MW_API void  WowDatabase_buildMaps();
MW_API void  WowDatabase_buildWmos();
MW_API void  WowDatabase_buildWorldModels();
MW_API void  WowDatabase_buildTextures();
MW_API void WowDatabase_buildRidables(const char* filename);

MW_API bool  WowDatabase_getCharacterPath(const char* raceName, bool female, bool isHD, char* path, uint32_t size);
MW_API bool  WowDatabase_itemIsCorrectType(int32_t type, int32_t slot);
MW_API void  WowDatabase_getMaxCharFeature(uint32_t race, bool female, bool isHD, SCharFeature* feature);
MW_API bool  WowDatabase_getNpcPath(int32_t npcid, bool isHD, char* path, uint32_t size);
MW_API bool WowDatabase_getItemPath(int32_t itemid, char* modelpath, uint32_t modelSize, char* texturepath, uint32_t texSize);

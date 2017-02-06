#include "stdafx.h"
#include "editor_WowDatabase.h"

c8 g_wmodbname[DEFAULT_SIZE];
c8 g_svdbname[DEFAULT_SIZE];
c8 g_worldmodelname[DEFAULT_SIZE];

u32  WowDatabase_getItemCount()
{
	return g_Engine->getWowDatabase()->getItemCount();
}

bool  WowDatabase_getItem( u32 i, editor::SItem* item )
{
	const SItemRecord* r = g_Engine->getWowDatabase()->getItem(i);
	if (!r || strlen(r->name) > 127)
		return false;

	utf8to16(r->name, item->name, DEFAULT_SIZE);
	item->id = r->id;
	item->type = r->type;
	g_Engine->getWowDatabase()->getSubClassName(r->itemclass, r->subclass, item->subclassname, DEFAULT_SIZE);

	return true;
}

bool WowDatabase_getItemById( s32 id, editor::SItem* item )
{
	const SItemRecord* r = g_Engine->getWowDatabase()->getItemById(id);
	if (!r || strlen(r->name) > 127)
		return false;

	utf8to16(r->name, item->name, DEFAULT_SIZE);
	item->id = r->id;
	item->type = r->type;
	g_Engine->getWowDatabase()->getSubClassName(r->itemclass, r->subclass, item->subclassname, DEFAULT_SIZE);

	return true;
}

u32  WowDatabase_getNpcCount()
{
	return g_Engine->getWowDatabase()->getNpcCount();
}

bool  WowDatabase_getNpc( u32 i, editor::SNpc* npc )
{
	const SNPCRecord* r = g_Engine->getWowDatabase()->getNPC(i);
	if (!r || strlen(r->name) > 127)
	{
		return false;
	}

	utf8to16(r->name, npc->name, DEFAULT_SIZE);
	npc->modelDisplayId = r->model;
	npc->modelId = g_Engine->getWowDatabase()->getNpcModelId(r->model);
	g_Engine->getWowDatabase()->getNpcTypeName(r->type, npc->type, DEFAULT_SIZE);

	return true;
}

bool WowDatabase_getNpcById( s32 id, editor::SNpc* npc )
{
	const SNPCRecord* r = g_Engine->getWowDatabase()->getNPCById(id);
	if (!r || strlen(r->name) > 127)
	{
		return false;
	}

	utf8to16(r->name, npc->name, DEFAULT_SIZE);
	npc->modelDisplayId = r->model;
	npc->modelId = g_Engine->getWowDatabase()->getNpcModelId(r->model);
	g_Engine->getWowDatabase()->getNpcTypeName(r->type, npc->type, DEFAULT_SIZE);

	return true;
}

bool  WowDatabase_getRaceId( const c8* racename, u32* id )
{
	return g_Engine->getWowDatabase()->getRaceId(racename, *id);
}

const c8* WowDatabase_getRaceName( u32 id )
{
	return g_Engine->getWowDatabase()->getRaceName(id);
}

const c8* WowDatabase_getClassShortName( u32 id )
{
	return g_Engine->getWowDatabase()->getClassShortName(id);
}

bool WowDatabase_getClassInfo( const c8* shortname, editor::SEntry* entry )
{
	u32 id;
	bool ret = g_Engine->getWowDatabase()->getClassInfo(shortname, entry->name, DEFAULT_SIZE, &id);

	entry->id = (s32)id;
	return ret;
}

void  WowDatabase_buildItems()
{
	g_Engine->getWowDatabase()->buildItems();
}

bool  WowDatabase_buildNpcs( const c8* filename )
{
	return g_Engine->getWowDatabase()->buildNpcs(filename);
}

void  WowDatabase_buildStartOutfitClass()
{
	g_Engine->getWowDatabase()->buildStartOutfitClass();
}

void WowDatabase_buildMaps()
{
	g_Engine->getWowDatabase()->buildMaps();
}

void WowDatabase_buildWmos()
{
	g_Engine->getWowDatabase()->buildWmos();
}

void WowDatabase_buildWorldModels()
{
	g_Engine->getWowDatabase()->buildWorldModels();
}

void WowDatabase_buildTextures()
{
	g_Engine->getWowDatabase()->buildTextures();
}

void WowDatabase_buildRidables(const c8* filename)
{
	g_Engine->getWowDatabase()->buildRidables(filename);
}

bool  WowDatabase_getCharacterPath( const c8* raceName, bool female, bool isHD, c8* path, u32 size )
{
	return g_Engine->getWowDatabase()->getCharacterPath(raceName, female, isHD, path, size);
}

bool  WowDatabase_itemIsCorrectType( s32 type, s32 slot )
{
	return isCorrectType(type, slot);
}

u32  WowDatabase_getStartOutfitCount( u32 race, bool female )
{
	return g_Engine->getWowDatabase()->getNumStartOutfits(race, female);
}

bool  WowDatabase_getStartOutfit( u32 race, bool female, u32 i, editor::SStartOutfit* entry )
{
	const SStartOutfitEntry* ent = g_Engine->getWowDatabase()->getStartOutfit(race, female, i);
	if (!ent)
		return false;
	
	entry->id = ent->id;
	wcscpy_s((wchar_t*)entry->name, DEFAULT_SIZE, (wchar_t*)ent->name); 
	wcscpy_s((wchar_t*)entry->shortname, DEFAULT_SIZE, (wchar_t*)ent->shortname);
	return true;
}

u32  WowDatabase_getSetCount()
{
	return g_Engine->getWowDatabase()->getItemSetDB()->getNumRecords();
}

bool  WowDatabase_getSet(u32 i, editor::SEntry* entry)
{
	return g_Engine->getWowDatabase()->getSet(i, entry->id, entry->name, DEFAULT_SIZE);
}


u32  WowDatabase_getMapCount()
{
	return g_Engine->getWowDatabase()->getMapDB()->getNumRecords();
}

bool  WowDatabase_getMap( u32 i, editor::SMap* map )
{
	dbc::record r = g_Engine->getWowDatabase()->getMapDB()->getRecord(i);
	if (!r.isValid())
		return false;

	map->id = r.getID();
	map->type = r.getInt(mapDB::AreaType);
	utf8to16(r.getString(mapDB::Name), map->name, DEFAULT_SIZE);
	return true;
}

u32 WowDatabase_getWmoCount()
{
	return g_Engine->getWowDatabase()->getNumWmos();
}

const c8* WowDatabase_getWMOFileName( u32 index, bool shortname )
{
	const c8* filename = g_Engine->getWowDatabase()->getWmoFileName(index);
	if (!filename)
		return "";

	if (shortname)
	{
		getFileNameNoExtensionA(filename, g_wmodbname, DEFAULT_SIZE);
		return g_wmodbname;
	}
	else
	{
		return filename;
	}
}

u32 WowDatabase_getWorldModelCount()
{
	return g_Engine->getWowDatabase()->getNumWorldModels();
}

const c8* WowDatabase_getWorldModelFileName(u32 index, bool shortname)
{
	const c8* filename = g_Engine->getWowDatabase()->getWorldModelFileName(index);
	if (!filename)
		return "";

	if (shortname)
	{
		getFileNameNoExtensionA(filename, g_worldmodelname, DEFAULT_SIZE);
		return g_worldmodelname;
	}
	else
	{
		return filename;
	}
}

u32 WowDatabase_getTextureCount()
{
	return g_Engine->getWowDatabase()->getNumTextures();
}

const c8* WowDatabase_getTextureFileName( u32 index )
{
	const c8* filename = g_Engine->getWowDatabase()->getTextureFileName(index);
	if (!filename)
		return "";

	return filename;
}

u32  WowDatabase_getSpellVisualEffectCount()
{
	return g_Engine->getWowDatabase()->getSpellVisualEffectNameDB()->getNumRecords();
}

bool  WowDatabase_getSpellVisualEffectId( u32 i, int* id )
{
	dbc::record r = g_Engine->getWowDatabase()->getSpellVisualEffectNameDB()->getRecord(i);
	if (!r.isValid())
		return false;

	*id = r.getID();
	return true;
}

u32 WowDatabase_getRidableCount()
{
	return g_Engine->getWowDatabase()->getNumRidables();
}

bool WowDatabase_getRidable( u32 i, editor::SRidable* ridable )
{
	const ::SRidable* r = g_Engine->getWowDatabase()->getRidable(i);
	if(!r)
		return false;

	ridable->npcid = r->npcid;
	ridable->mountflag = r->mountflag;

	return true;
}

void  WowDatabase_getMaxCharFeature( u32 race, bool female, bool isHD, SCharFeature* feature )
{
	wowDatabase* wdb = g_Engine->getWowDatabase();
	feature->skinColor = wdb->getMaxSkinColor(race, female, isHD);
	feature->faceType = wdb->getMaxFaceType(race, female, isHD);
	feature->hairColor = wdb->getMaxHairColor(race, female, isHD);
	feature->hairStyle = wdb->getMaxHairStyle(race, female, isHD);
	feature->facialHair = wdb->getMaxFacialHairStyle(race, female);
}

bool  WowDatabase_getNpcPath( s32 npcid, bool isHD, c8* path, u32 size )
{
	return g_Engine->getWowDatabase()->getNpcPath(npcid, isHD, path, size);
}

bool  WowDatabase_getItemVisualPath( s32 visualId, c8* path, u32 size )
{
	if (visualId == 0)
		return false;

	return g_Engine->getWowDatabase()->getItemVisualPath(visualId, path, size);
}

const c8* WowDatabase_getSpellVisualEffectName( s32 visualId )
{
	dbc::record r = g_Engine->getWowDatabase()->getSpellVisualEffectNameDB()->getByID(visualId);
	if (!r.isValid())
		return "";

	const c8* name = r.getString(spellVisualEffectNameDB::Model);
	getFileNameNoExtensionA(name, g_svdbname, DEFAULT_SIZE);
	return g_svdbname;
}

bool  WowDatabase_getSpellVisualEffectPath( s32 visualId, c8* path, u32 size )
{
	if (visualId == 0)
		return false;

	return g_Engine->getWowDatabase()->getEffectVisualPath(visualId, path, size);
}

bool WowDatabase_getItemPath(s32 itemid, c8* modelpath, u32 modelSize, c8* texturepath, u32 texSize)
{
	if(itemid == 0)
		return false;

	return g_Engine->getWowDatabase()->getItemPath(itemid, modelpath, modelSize, texturepath, texSize);
}









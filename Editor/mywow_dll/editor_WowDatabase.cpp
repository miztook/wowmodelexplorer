#include "stdafx.h"
#include "editor_WowDatabase.h"
using namespace WowClassic;

char g_wmodbname[DEFAULT_SIZE];
char g_svdbname[DEFAULT_SIZE];
char g_worldmodelname[DEFAULT_SIZE];

uint32_t  WowDatabase_getItemCount()
{
	return g_Engine->getWowDatabase()->getItemCount();
}

bool  WowDatabase_getItem( uint32_t i, editor::SItem* item )
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

bool WowDatabase_getItemById( int32_t id, editor::SItem* item )
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

uint32_t  WowDatabase_getNpcCount()
{
	return g_Engine->getWowDatabase()->getNpcCount();
}

bool  WowDatabase_getNpc( uint32_t i, editor::SNpc* npc )
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

bool WowDatabase_getNpcById( int32_t id, editor::SNpc* npc )
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

bool  WowDatabase_getRaceId( const char* racename, uint32_t* id )
{
	return g_Engine->getWowDatabase()->getRaceId(racename, *id);
}

const char* WowDatabase_getRaceName( uint32_t id )
{
	return g_Engine->getWowDatabase()->getRaceName(id);
}

const char* WowDatabase_getClassShortName( uint32_t id )
{
	return g_Engine->getWowDatabase()->getClassShortName(id);
}

bool WowDatabase_getClassInfo( const char* shortname, editor::SEntry* entry )
{
	uint32_t id;
	bool ret = g_Engine->getWowDatabase()->getClassInfo(shortname, entry->name, DEFAULT_SIZE, &id);

	entry->id = (int32_t)id;
	return ret;
}

void  WowDatabase_buildItems()
{
	g_Engine->getWowDatabase()->buildItems();
}

bool  WowDatabase_buildNpcs( const char* filename )
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

void WowDatabase_buildRidables(const char* filename)
{
	g_Engine->getWowDatabase()->buildRidables(filename);
}

bool  WowDatabase_getCharacterPath( const char* raceName, bool female, bool isHD, char* path, uint32_t size )
{
	return g_Engine->getWowDatabase()->getCharacterPath(raceName, female, isHD, path, size);
}

bool  WowDatabase_itemIsCorrectType( int32_t type, int32_t slot )
{
	return isCorrectType(type, slot);
}

uint32_t  WowDatabase_getStartOutfitCount( uint32_t race, bool female )
{
	return g_Engine->getWowDatabase()->getNumStartOutfits(race, female);
}

bool  WowDatabase_getStartOutfit( uint32_t race, bool female, uint32_t i, editor::SStartOutfit* entry )
{
	const SStartOutfitEntry* ent = g_Engine->getWowDatabase()->getStartOutfit(race, female, i);
	if (!ent)
		return false;
	
	entry->id = ent->id;
	wcscpy_s((wchar_t*)entry->name, DEFAULT_SIZE, (wchar_t*)ent->name); 
	wcscpy_s((wchar_t*)entry->shortname, DEFAULT_SIZE, (wchar_t*)ent->shortname);
	return true;
}

uint32_t  WowDatabase_getSetCount()
{
	return g_Engine->getWowDatabase()->getItemSetDB()->getNumRecords();
}

bool  WowDatabase_getSet(uint32_t i, editor::SEntry* entry)
{
	return g_Engine->getWowDatabase()->getSet(i, entry->id, entry->name, DEFAULT_SIZE);
}


uint32_t  WowDatabase_getMapCount()
{
	return g_Engine->getWowDatabase()->getMapDB()->getNumRecords();
}

bool  WowDatabase_getMap( uint32_t i, editor::SMap* map )
{
	auto r = g_Engine->getWowDatabase()->getMapDB()->getRecord(i);
	if (!r.isValid())
		return false;

	map->id = r.getID();
	map->type = r.getInt(mapDB::AreaType);
	utf8to16(r.getString(mapDB::Name), map->name, DEFAULT_SIZE);
	return true;
}

uint32_t WowDatabase_getWmoCount()
{
	return g_Engine->getWowDatabase()->getNumWmos();
}

const char* WowDatabase_getWMOFileName( uint32_t index, bool shortname )
{
	const char* filename = g_Engine->getWowDatabase()->getWmoFileName(index);
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

uint32_t WowDatabase_getWorldModelCount()
{
	return g_Engine->getWowDatabase()->getNumWorldModels();
}

const char* WowDatabase_getWorldModelFileName(uint32_t index, bool shortname)
{
	const char* filename = g_Engine->getWowDatabase()->getWorldModelFileName(index);
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

uint32_t WowDatabase_getTextureCount()
{
	return g_Engine->getWowDatabase()->getNumTextures();
}

const char* WowDatabase_getTextureFileName( uint32_t index )
{
	const char* filename = g_Engine->getWowDatabase()->getTextureFileName(index);
	if (!filename)
		return "";

	return filename;
}

uint32_t WowDatabase_getRidableCount()
{
	return g_Engine->getWowDatabase()->getNumRidables();
}

bool WowDatabase_getRidable( uint32_t i, editor::SRidable* ridable )
{
	const ::SRidable* r = g_Engine->getWowDatabase()->getRidable(i);
	if(!r)
		return false;

	ridable->npcid = r->npcid;
	ridable->mountflag = r->mountflag;

	return true;
}

void  WowDatabase_getMaxCharFeature( uint32_t race, bool female, bool isHD, SCharFeature* feature )
{
	wowDatabase* wdb = g_Engine->getWowDatabase();
	feature->skinColor = wdb->getMaxSkinColor(race, female, isHD);
	feature->faceType = wdb->getMaxFaceType(race, female, isHD);
	feature->hairColor = wdb->getMaxHairColor(race, female, isHD);
	feature->hairStyle = wdb->getMaxHairStyle(race, female, isHD);
	feature->facialHair = wdb->getMaxFacialHairStyle(race, female);
}

bool  WowDatabase_getNpcPath( int32_t npcid, bool isHD, char* path, uint32_t size )
{
	bool ret = g_Engine->getWowDatabase()->getNpcPath(npcid, isHD, path, size);

#if WOW_VER >= 70
	if (getM2Type(path) == MT_CHARACTER)
		return false;
#endif

	return ret;
}

bool WowDatabase_getItemPath(int32_t itemid, char* modelpath, uint32_t modelSize, char* texturepath, uint32_t texSize)
{
	if(itemid == 0)
		return false;

	return g_Engine->getWowDatabase()->getItemPath(itemid, modelpath, modelSize, texturepath, texSize);
}









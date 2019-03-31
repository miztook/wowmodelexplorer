//
//  wow_base.h
//  mywow
//
//  Created by miao yu on 6/9/14.
//  Copyright (c) 2014 MW. All rights reserved.
//

#pragma once

#include "base.h"
#include "wow_enums.h"
#include "function.h"
#include "fixstring.h"
#include <vector>

struct SItemInfo
{
	int32_t id;
	int32_t itemType;
	int32_t sheathtype;
};

struct SAttachmentInfo
{
	SAttachmentInfo()
	{
		::memset(modelpath, 0, QMAX_PATH);
		::memset(texpath, 0, QMAX_PATH);
	}
	char		modelpath[QMAX_PATH];
	char		texpath[QMAX_PATH];
};

struct SAttachmentEntry
{
	SAttachmentEntry() :  scale(1.0f), node(nullptr), id(-1), slot(-1), attachIndex(-1) { }
    
	float		scale;
	void*		node;
    
	int16_t		id;
	int16_t		slot;
	int16_t		attachIndex;
};

//物品和npc需要建两个单独的内存数据
struct SItemRecord
{
	char name[DEFAULT_SIZE * 2];
	int32_t id, itemclass, subclass, type;

	const bool operator<(const SItemRecord& r) const
	{
		if (type == r.type)
			return Q_stricmp(name, r.name) < 0;
		else
			return type < r.type;
	}
};

class ItemCollections
{
public:
#ifdef USE_QALLOCATOR
	typedef std::map<int32_t, int32_t, std::less<int32_t>, qzone_allocator<std::pair<int32_t, int32_t>>>		T_itemLookup;
#else
	ItemCollections() : itemLookup(1024) {}
	typedef std::unordered_map<int32_t, int32_t>		T_itemLookup;
#endif

public:
	std::vector<SItemRecord>	items;
	T_itemLookup		itemLookup;				//item id 到 index

	const SItemRecord* getById(int32_t id) const;
};


struct SNPCRecord
{
	char name[DEFAULT_SIZE * 2];
	int32_t model;
	int32_t type;

	SNPCRecord() : model(0), type(0)
	{
		memset(name, 0, sizeof(name));
	}

	explicit SNPCRecord(const char* line);

	bool operator<(const SNPCRecord& r) const
	{
		if (type == r.type)
			return Q_stricmp(name, r.name) < 0;
		else
			return type < r.type;
	}
};

class NPCCollections
{
public:
#ifdef USE_QALLOCATOR
	typedef std::map<int32_t, int32_t, std::less<int32_t>, qzone_allocator<std::pair<int32_t, int32_t>>>		T_npcLookup;
#else
	NPCCollections() : npcLookup(1024) { }
	typedef std::unordered_map<int32_t, int32_t>		T_npcLookup;
#endif

public:
	std::vector<SNPCRecord>	npcs;
	T_npcLookup		npcLookup;				//npc model id 到 index

	bool open(const char* filename);

	const SNPCRecord* getById(int32_t id) const;
};

struct SStartOutfitEntry
{
	uint32_t id;
	char16_t name[NAME_SIZE];
	char16_t shortname[NAME_SIZE];
	uint8_t race;
	bool female;
};

class StartOutfitClassCollections
{
public:
	uint32_t getNumStartOutfits(uint32_t race, bool female);
	const SStartOutfitEntry* get(uint32_t race, bool female, uint32_t idx);

	std::vector<SStartOutfitEntry> startOutfits;
};

struct SArea
{
	int32_t id;
	int32_t mapId;
	int32_t parentId;
	char16_t name[DEFAULT_SIZE];
};

struct SWowMapArea
{
	int32_t		id;
	int32_t		areaId;
	float		locLeft;
	float		locRight;
	float		locTop;
	float		locBottom;
	char		name[DEFAULT_SIZE];
};

struct SMapRecord
{
	int32_t id;
	int32_t type;
	char name[DEFAULT_SIZE];
};

class MapCollections
{
public:
#ifdef USE_QALLOCATOR
	typedef std::map<int32_t, int32_t, std::less<int32_t>, qzone_allocator<std::pair<int32_t, int32_t>>>		T_mapLookup;
	typedef std::map<int32_t, int32_t, std::less<int32_t>, qzone_allocator<std::pair<int32_t, int32_t>>>		T_areaLookup;
#else
	typedef std::unordered_map<int32_t, int32_t>		T_mapLookup;
	typedef std::unordered_map<int32_t, int32_t>		T_areaLookup;
#endif

public:
	std::vector<SMapRecord>	maps;

	T_mapLookup		mapLookup;

	const SMapRecord* getMapById(int32_t id) const;
	SMapRecord* getMapById(int32_t id);
};

class WMOCollections
{
public:
	WMOCollections();

	std::vector<string256>		wmos;
};

class WorldModelCollections
{
public:
	WorldModelCollections();

	std::vector<string256>		models;
};

class TextureCollections
{
public:
	TextureCollections();

	std::vector<string256>		textures;
};

struct SRidable
{
	int32_t		npcid;
	int32_t		mountflag;

	explicit SRidable(const char* line);
};

class RidableCollections
{
public:
	RidableCollections();

	std::vector<SRidable>	ridables;

	bool open(const char* filename, const NPCCollections& npcs);
};

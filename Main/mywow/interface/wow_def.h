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
	s32 id;
	s32 itemType;
	s32 sheathtype;
};

struct SAttachmentInfo
{
	SAttachmentInfo()
	{
		::memset(modelpath, 0, QMAX_PATH);
		::memset(texpath, 0, QMAX_PATH);
	}
	c8		modelpath[QMAX_PATH];
	c8		texpath[QMAX_PATH];
};

struct SAttachmentEntry
{
	SAttachmentEntry() :  scale(1.0f), node(nullptr), id(-1), slot(-1), attachIndex(-1) { }
    
	f32		scale;
	void*		node;
    
	s16		id;
	s16		slot;
	s16		attachIndex;
};

struct SSpellVKInfo
{
	SAttachmentEntry	attachmentEntries[SVK_COUNT];
	SAttachmentInfo		attachmentInfos[SVK_COUNT];
};

//物品和npc需要建两个单独的内存数据
struct SItemRecord
{
	c8 name[DEFAULT_SIZE * 2];
	s32 id, itemclass, subclass, type;

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
	typedef std::map<s32, s32, std::less<s32>, qzone_allocator<std::pair<s32, s32>>>		T_itemLookup;
#else
	ItemCollections() : itemLookup(1024) {}
	typedef std::unordered_map<s32, s32>		T_itemLookup;
#endif

public:
	std::vector<SItemRecord>	items;
	T_itemLookup		itemLookup;				//item id 到 index

	const SItemRecord* getById(s32 id) const;
};


struct SNPCRecord
{
	c8 name[DEFAULT_SIZE * 2];
	s32 model;
	s32 type;

	SNPCRecord() : model(0), type(0)
	{
		memset(name, 0, sizeof(name));
	}

	explicit SNPCRecord(const c8* line);

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
	typedef std::map<s32, s32, std::less<s32>, qzone_allocator<std::pair<s32, s32>>>		T_npcLookup;
#else
	NPCCollections() : npcLookup(1024) { }
	typedef std::unordered_map<s32, s32>		T_npcLookup;
#endif

public:
	std::vector<SNPCRecord>	npcs;
	T_npcLookup		npcLookup;				//npc model id 到 index

	bool open(const c8* filename);

	const SNPCRecord* getById(s32 id) const;
};

struct SStartOutfitEntry
{
	u32 id;
	c16 name[NAME_SIZE];
	c16 shortname[NAME_SIZE];
	u8 race;
	bool female;
};

class StartOutfitClassCollections
{
public:
	u32 getNumStartOutfits(u32 race, bool female);
	const SStartOutfitEntry* get(u32 race, bool female, u32 idx);

	std::vector<SStartOutfitEntry> startOutfits;
};

struct SArea
{
	s32 id;
	s32 mapId;
	s32 parentId;
	c16 name[DEFAULT_SIZE];
};

struct SWowMapArea
{
	s32		id;
	s32		areaId;
	f32		locLeft;
	f32		locRight;
	f32		locTop;
	f32		locBottom;
	c8		name[DEFAULT_SIZE];
};

struct SMapRecord
{
	s32 id;
	s32 type;
	c8 name[DEFAULT_SIZE];
};

class MapCollections
{
public:
#ifdef USE_QALLOCATOR
	typedef std::map<s32, s32, std::less<s32>, qzone_allocator<std::pair<s32, s32>>>		T_mapLookup;
	typedef std::map<s32, s32, std::less<s32>, qzone_allocator<std::pair<s32, s32>>>		T_areaLookup;
#else
	typedef std::unordered_map<s32, s32>		T_mapLookup;
	typedef std::unordered_map<s32, s32>		T_areaLookup;
#endif

public:
	std::vector<SMapRecord>	maps;
	std::vector<SArea>		areas;

	T_mapLookup		mapLookup;
	T_areaLookup		areaLookup;

	const SMapRecord* getMapById(s32 id) const;
	SMapRecord* getMapById(s32 id);
	const SArea* getAreaById(s32 id) const;
	SArea* getAreaById(s32 id);
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
	s32		npcid;
	s32		mountflag;

	explicit SRidable(const c8* line);
};

class RidableCollections
{
public:
	RidableCollections();

	std::vector<SRidable>	ridables;

	bool open(const c8* filename, const NPCCollections& npcs);
};

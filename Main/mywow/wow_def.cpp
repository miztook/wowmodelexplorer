#include "stdafx.h"
#include "wow_def.h"
#include "mywow.h"

const SItemRecord* ItemCollections::getById(s32 id) const
{
	T_itemLookup::const_iterator itr = itemLookup.find(id);
	if (itr != itemLookup.end())
	{
		s32 i = itr->second;
		return &items[i];
	}
	return nullptr;
}

SNPCRecord::SNPCRecord(const c8* line)
{
	s32 id;

	Q_sscanf(line, "%d,%d,%d", &id, &model, &type);

	size_t len = strlen(line);
	for (size_t i = len - 2; i>1; i--) {
		if (line[i] == ',')
		{
			Q_strcpy(name, DEFAULT_SIZE * 2, line + i + 1);
			break;
		}
	}

	u32 l = (u32)Q_strlen(name);
	if (l > 0)
		name[l - 1] = '\0';
}

bool NPCCollections::open(const c8* filename)
{
	IFileSystem* fs = g_Engine->getFileSystem();
	string_path path;
	path = fs->getDataDirectory();
	path.append(g_Engine->getWowEnvironment()->getLocale());
	path.append("\\");
	path.append(filename);

	IReadFile* file = fs->createAndOpenFile(path.c_str(), false);
	ASSERT(file);
	if (!file)
		return false;

	npcs.reserve(1024);

	c8 line[512];
	while (file->readText(line, 512))
	{
		SNPCRecord rec(line);

		if (rec.type > 0 && rec.model > 0)
		{
			npcs.emplace_back(rec);
			npcLookup[rec.model] = (s32)(npcs.size() - 1);

			//id_set.insert(rec.id);
		}
	}

	delete file;

	//build other npcs (unknown name)
	const creatureDisplayInfoDB* db = g_Engine->getWowDatabase()->getCreatureDisplayInfoDB();
	for (u32 i = 0; i<db->getNumActualRecords(); ++i)
	{
		dbc::record r = db->getRecord(i);
		s32 id = r.getInt(0);
		if (npcLookup.find(id) != npcLookup.end())		//skip npcs already built
			continue;

		SNPCRecord rec;
		Q_sprintf(rec.name, 32, "Unknown%d", id);
		rec.model = id;
		rec.type = 0;		//unknown

		npcs.emplace_back(rec);
		npcLookup[rec.model] = (s32)(npcs.size() - 1);
	}

	//npcs.shrink_to_fit();

	return true;
}

const SNPCRecord* NPCCollections::getById(s32 id) const
{
	T_npcLookup::const_iterator itr = npcLookup.find(id);
	if (itr != npcLookup.end())
	{
		s32 i = itr->second;
		return &npcs[i];
	}
	return nullptr;
}

const SStartOutfitEntry* StartOutfitClassCollections::get(u32 race, bool female, u32 idx)
{
	u32 count = 0;
	for (u32 i = 0; i<startOutfits.size(); ++i)
	{
		if (startOutfits[i].race == race &&
			startOutfits[i].female == female)
		{
			if (count == idx)
				return &startOutfits[i];
			++count;
		}
	}
	return nullptr;
}

u32 StartOutfitClassCollections::getNumStartOutfits(u32 race, bool female)
{
	u32 count = 0;
	for (u32 i = 0; i<startOutfits.size(); ++i)
	{
		if (startOutfits[i].race == race && startOutfits[i].female == female)
			++count;
	}
	return count;
}


const SMapRecord* MapCollections::getMapById(s32 id) const
{
	T_mapLookup::const_iterator itr = mapLookup.find(id);
	if (itr == mapLookup.end())
		return nullptr;

	s32 index = itr->second;
	return &maps[index];
}

SMapRecord* MapCollections::getMapById(s32 id)
{
	T_mapLookup::iterator itr = mapLookup.find(id);
	if (itr == mapLookup.end())
		return nullptr;

	s32 index = itr->second;
	return &maps[index];
}

const SArea* MapCollections::getAreaById(s32 id) const
{
	T_areaLookup::const_iterator itr = areaLookup.find(id);
	if (itr == areaLookup.end())
		return nullptr;

	s32 index = itr->second;
	return &areas[index];
}

SArea* MapCollections::getAreaById(s32 id)
{
	T_areaLookup::iterator itr = areaLookup.find(id);
	if (itr == areaLookup.end())
		return nullptr;

	s32 index = itr->second;
	return &areas[index];
}

WMOCollections::WMOCollections()
{
	wmos.reserve(1024);
}

WorldModelCollections::WorldModelCollections()
{
	models.reserve(1024);
}

TextureCollections::TextureCollections()
{
	textures.reserve(1024);
}

RidableCollections::RidableCollections()
{
	ridables.reserve(64);
}


SRidable::SRidable(const c8* line)
{
	Q_sscanf(line, "%d,%d", &npcid, &mountflag);
}

bool RidableCollections::open(const c8* filename, const NPCCollections& npcs)
{
	IFileSystem* fs = g_Engine->getFileSystem();
	string_path path;
	path = fs->getDataDirectory();
	path.append(g_Engine->getWowEnvironment()->getLocale());
	path.append("\\");
	path.append(filename);

	IReadFile* file = fs->createAndOpenFile(path.c_str(), false);
	ASSERT(file);
	if (!file)
		return false;

	c8 line[512];
	while (file->readText(line, 512))
	{
		SRidable rec(line);
		if (rec.npcid != 0 || npcs.getById(rec.npcid) != nullptr)
		{
			ridables.emplace_back(rec);
		}
	}

	delete file;

	//ridables.shrink_to_fit();

	return true;
}


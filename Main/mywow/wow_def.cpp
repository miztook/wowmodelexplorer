#include "stdafx.h"
#include "wow_def.h"
#include "mywow.h"

#if WOW_VER >= 70
using namespace WowLegion;
#else
using namespace WowClassic;
#endif

const SItemRecord* ItemCollections::getById(int32_t id) const
{
	T_itemLookup::const_iterator itr = itemLookup.find(id);
	if (itr != itemLookup.end())
	{
		int32_t i = itr->second;
		return &items[i];
	}
	return nullptr;
}

SNPCRecord::SNPCRecord(const char* line)
{
	int32_t id;

	Q_sscanf(line, "%d,%d,%d", &id, &model, &type);

	size_t len = strlen(line);
	for (size_t i = len - 2; i>1; i--) {
		if (line[i] == ',')
		{
			Q_strcpy(name, DEFAULT_SIZE * 2, line + i + 1);
			break;
		}
	}

	uint32_t l = (uint32_t)Q_strlen(name);
	if (l > 0)
		name[l - 1] = '\0';
}

bool NPCCollections::open(const char* filename)
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

	char line[512];
	while (file->readText(line, 512))
	{
		SNPCRecord rec(line);

		if (rec.type > 0 && rec.model > 0)
		{
			npcs.emplace_back(rec);
			npcLookup[rec.model] = (int32_t)(npcs.size() - 1);

			//id_set.insert(rec.id);
		}
	}

	delete file;

	//build other npcs (unknown name)
	const creatureDisplayInfoDB* db = g_Engine->getWowDatabase()->getCreatureDisplayInfoDB();
	for (uint32_t i = 0; i<db->getNumActualRecords(); ++i)
	{
		dbc::record r = db->getRecord(i);
		int32_t id = r.getInt(0);
		if (npcLookup.find(id) != npcLookup.end())		//skip npcs already built
			continue;

		SNPCRecord rec;
		Q_sprintf(rec.name, 32, "Unknown%d", id);
		rec.model = id;
		rec.type = 0;		//unknown

		npcs.emplace_back(rec);
		npcLookup[rec.model] = (int32_t)(npcs.size() - 1);
	}

	//npcs.shrink_to_fit();

	return true;
}

const SNPCRecord* NPCCollections::getById(int32_t id) const
{
	T_npcLookup::const_iterator itr = npcLookup.find(id);
	if (itr != npcLookup.end())
	{
		int32_t i = itr->second;
		return &npcs[i];
	}
	return nullptr;
}

const SStartOutfitEntry* StartOutfitClassCollections::get(uint32_t race, bool female, uint32_t idx)
{
	uint32_t count = 0;
	for (uint32_t i = 0; i<startOutfits.size(); ++i)
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

uint32_t StartOutfitClassCollections::getNumStartOutfits(uint32_t race, bool female)
{
	uint32_t count = 0;
	for (uint32_t i = 0; i<startOutfits.size(); ++i)
	{
		if (startOutfits[i].race == race && startOutfits[i].female == female)
			++count;
	}
	return count;
}


const SMapRecord* MapCollections::getMapById(int32_t id) const
{
	T_mapLookup::const_iterator itr = mapLookup.find(id);
	if (itr == mapLookup.end())
		return nullptr;

	int32_t index = itr->second;
	return &maps[index];
}

SMapRecord* MapCollections::getMapById(int32_t id)
{
	T_mapLookup::iterator itr = mapLookup.find(id);
	if (itr == mapLookup.end())
		return nullptr;

	int32_t index = itr->second;
	return &maps[index];
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


SRidable::SRidable(const char* line)
{
	Q_sscanf(line, "%d,%d", &npcid, &mountflag);
}

bool RidableCollections::open(const char* filename, const NPCCollections& npcs)
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

	char line[512];
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


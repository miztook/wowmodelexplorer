#include "stdafx.h"
#include "wow_dbc.h"
#include "mpq_libmpq.h"
#include "mywow.h"

dbc::dbc( wowEnvironment* env, const c8* filename )
{
	c8 fname[DEFAULT_SIZE];
	getFileNameA(filename, fname, DEFAULT_SIZE);

	MPQFile* file;

	if (_stricmp(fname, "item.dbc") == 0)
	{
		getFileNameNoExtensionA(filename, fname, DEFAULT_SIZE);
		strcat_s(fname, DEFAULT_SIZE, ".db2");
		file = env->openFile(fname);
	}
	else
	{
		file = env->openFile(filename);
	}
	
	_ASSERT(file);

	c8 header[5];
	file->read(header, 4);

	int db_type = 0;

	if (strncmp(header, "WDBC", 4) == 0)
		db_type = 1;
	else if (strncmp(header, "WDB2", 4) == 0)
		db_type = 2;
	else
		_ASSERT(false);

	file->read(&nRecords, 4);
	file->read(&nFields, 4);
	file->read(&RecordSize, 4);
	file->read(&StringSize, 4);


	//check
	if (db_type == 2) {
		file->seek(28, true);
		// just some buggy check
		unsigned int check;
		file->read(&check, 4);
		if (check == 6) // wrong place
			file->seek(-20, true);
		else // check == 17, right place
			file->seek(-4, true);
	}

	_data = new u8[RecordSize * nRecords + StringSize];
	file->read(_data, RecordSize * nRecords + StringSize);
	_recordStart = _data;
	_stringStart = _data + RecordSize * nRecords;

	delete file;

	//build map
	for (u32 i=0; i<nRecords; ++i)
	{
		u32 id =  *reinterpret_cast<u32*>(_recordStart + i * RecordSize);
		RecordLookup[id] = i;
	}
}

dbc::~dbc()
{
	delete[] _data;
}

dbc::record charFacialHairDB::getByParams( unsigned int race, unsigned int gender, unsigned int style )
{
	for (u32 i=0; i<nRecords; ++i)
	{
		dbc::record r = getRecord(i);
		if (r.getUInt(RaceV400) == race &&
			r.getUInt(GenderV400) == gender &&
			r.getUInt(StyleV400) == style)					//id是record的第一个字段
			return r;
	}
	return dbc::record(NULL, NULL);
}

size_t charFacialHairDB::getStylesFor( unsigned int race, unsigned int gender)
{
	size_t n = 0;

	for (u32 i=0; i<nRecords; ++i)
	{
		dbc::record r = getRecord(i);
		if (r.getUInt(RaceV400) == race &&
			r.getUInt(GenderV400) == gender)
			++n;
	}
	return n;
}

dbc::record charHairGeosetsDB::getByParams( unsigned int race, unsigned int gender, unsigned int section )
{
	for (u32 i=0; i<nRecords; ++i)
	{
		dbc::record r = getRecord(i);
		if (r.getUInt(Race) == race &&
			r.getUInt(Gender) == gender &&
			r.getUInt(Section) == section)					//id是record的第一个字段
			return r;
	}
	return dbc::record(NULL, NULL);
}

size_t charHairGeosetsDB::getGeosetsFor( unsigned int race, unsigned int gender )
{
	size_t n = 0;

	for (u32 i=0; i<nRecords; ++i)
	{
		dbc::record r = getRecord(i);
		if (r.getUInt(Race) == race &&
			r.getUInt(Gender) == gender)
			++n;
	}
	return n;
}

dbc::record charRacesDB::getByName( const c8* name )
{
	for (u32 i=0; i<nRecords; ++i)
	{
		dbc::record r = getRecord(i);
		if(_stricmp(r.getString(Name),name) == 0)
			return r;
	}
	return dbc::record(NULL, NULL);
}

dbc::record charSectionsDB::getByParams( size_t race, size_t gender, size_t type, size_t section, size_t color )
{
	for (u32 i=0; i<nRecords; ++i)
	{
		dbc::record r = getRecord(i);
		if (r.getUInt(Race) == race &&
			r.getUInt(Gender) == gender &&
			r.getUInt(Type) == type &&
			r.getUInt(Section) == section &&
			r.getUInt(Color) == color)					//id是record的第一个字段
			return r;
	}
	return dbc::record(NULL, NULL);
}

size_t charSectionsDB::getColorsFor( size_t race, size_t gender, size_t type, size_t section )
{
	size_t n = 0;

	for (u32 i=0; i<nRecords; ++i)
	{
		dbc::record r = getRecord(i);
		if (r.getUInt(Race) == race &&
			r.getUInt(Gender) == gender &&
			r.getUInt(Type) == type &&
			r.getUInt(Section) == section)
			++n;
	}
	return n;
}

size_t charSectionsDB::getSectionsFor( size_t race, size_t gender, size_t type, size_t color )
{
	size_t n = 0;

	for (u32 i=0; i<nRecords; ++i)
	{
		dbc::record r = getRecord(i);
		if (r.getUInt(Race) == race &&
			r.getUInt(Gender) == gender &&
			r.getUInt(Type) == type &&
			r.getUInt(Color) == color)
				++n;
	}
	return n;
}

dbc::record itemSubClassDB::getById( int id, int subid )
{
	for (u32 i=0; i<nRecords; ++i)
	{
		dbc::record r = getRecord(i);
		if (r.getInt(ClassIDV400) == id && r.getInt(SubClassIDV400) == subid)	
			return r;
	}
	return dbc::record(NULL, NULL);
}

bool SItemRecord::init( itemDB* itemDb, const c8* line )
{
	sscanf_s(line, "%u,%u", &id, &quality);

	dbc::record r = itemDb->getByID(id);
	if (!r.isValid())
		return false;

	model = r.getInt(itemDB::ItemDisplayInfo);
	itemclass = r.getInt(itemDB::Itemclass);
	subclass = r.getInt(itemDB::Subclass);
	type = r.getInt(itemDB::InventorySlot);
	switch (r.getInt(itemDB::Sheath))
	{
	case SHEATHETYPE_MAINHAND: sheath = ATT_LEFT_BACK_SHEATH; break;
	case SHEATHETYPE_LARGEWEAPON: sheath = ATT_RIGHT_BACK_SHEATH; break;
	case SHEATHETYPE_HIPWEAPON: sheath = ATT_LEFT_HIP_SHEATH; break;
	case SHEATHETYPE_SHIELD: sheath = ATT_MIDDLE_BACK_SHEATH; break;
	default: sheath = SHEATHETYPE_NONE;
	}

	size_t len = strlen(line);
	for (size_t i=len-2; i>1; i--) {
		if (line[i]==',') {
			utf8to16(line+i+1, name, DEFAULT_SIZE);
			break;
		}
	}

	u32 l = wcslen(name);
	if ( l > 0 )
		name[l-1] = L'\0';

	return true;
}

SItemRecord* ItemCollections::getById( s32 id )
{
	T_itemLookup::iterator itr = itemLookup.find(id);
	if (itr != itemLookup.end()) 
	{
		s32 i = itr->second;
		return &items[i];
	}
	return NULL;
}

bool ItemCollections::open( itemDB* itemDb, itemDisplayDB* itemDisplayDb, const c8* filename )
{
	IFileSystem* fs = g_Engine->getFileSystem();
	fs->changeWorkingDirectoryTo(g_Engine->getWowEnvironment()->getLocalePath());

	IReadFile* file = fs->createAndOpenFile(filename, true);
	_ASSERT(file);

	if (!file)
		return false;

	c8 line[512];
	while (file->readLine(line, 512))
	{
		SItemRecord rec;
		if (rec.init(itemDb, line))
		{
			items.push_back(rec);
			itemLookup[rec.id] = (s32)(items.size() -1);
		}
	}

	/*
	//未翻译的
	for (u32 i=0; i<itemDb->getNumRecords(); ++i)
	{
		dbc::record r = itemDb->getRecord(i);
		s32 id = r.getInt(itemDB::ID);
		if (itemLookup.find(id)!=itemLookup.end())
			continue;

		SItemRecord rec;
		rec.id = id;
		rec.quality = 0;
		rec.model = r.getInt(itemDB::ItemDisplayInfo);
		rec.itemclass = r.getInt(itemDB::Itemclass);
		rec.subclass = r.getInt(itemDB::Subclass);
		rec.type = r.getInt(itemDB::InventorySlot);
		switch (r.getInt(itemDB::Sheath))
		{
		case SHEATHETYPE_MAINHAND: rec.sheath = ATT_LEFT_BACK_SHEATH; break;
		case SHEATHETYPE_LARGEWEAPON: rec.sheath = ATT_RIGHT_BACK_SHEATH; break;
		case SHEATHETYPE_HIPWEAPON: rec.sheath = ATT_LEFT_HIP_SHEATH; break;
		case SHEATHETYPE_SHIELD: rec.sheath = ATT_MIDDLE_BACK_SHEATH; break;
		default: rec.sheath = SHEATHETYPE_NONE;
		}

		swprintf_s(rec.name, DEFAULT_SIZE, L"%d", id);

		items.push_back(rec);
		itemLookup[rec.id] = (s32)(items.size() -1);
	}
	*/

	delete file;

	return true;
}

SNPCRecord::SNPCRecord( const c8* line )
{
	sscanf_s(line, "%u,%u,%u,", &id, &model, &type);
	size_t len = strlen(line);
	for (size_t i=len-2; i>1; i--) {
		if (line[i]==',') {
			utf8to16(line+i+1, name, DEFAULT_SIZE);
			break;
		}
	}

	u32 l = wcslen(name);
	if ( l > 0 )
		name[l-1] = L'\0';
}

bool NPCCollections::open( const c8* filename )
{
	IFileSystem* fs = g_Engine->getFileSystem();
	fs->changeWorkingDirectoryTo(g_Engine->getWowEnvironment()->getLocalePath());

	IReadFile* file = fs->createAndOpenFile(filename, true);
	_ASSERT(file);
	if (!file)
		return false;

	c8 line[512];
	while (file->readLine(line, 512))
	{
		SNPCRecord rec(line);
		if (rec.type > 0 && rec.model > 0)
		{
			npcs.push_back(rec);
			npcLookup[rec.id] = (s32)(npcs.size() -1);
		}
	}

	delete file;

	return true;
}

SNPCRecord* NPCCollections::getById( s32 id )
{
	T_npcLookup::iterator itr = npcLookup.find(id);
	if (itr != npcLookup.end()) 
	{
		s32 i = itr->second;
		return &npcs[i];
	}
	return NULL;
}

const SStartOutfitEntry* StartOutfitClassCollections::get( u32 race, bool female, u32 idx )
{
	u32 count = 0;
	for (u32 i=0; i<startOutfits.size(); ++i)
	{
		if(startOutfits[i].race == race && 
			startOutfits[i].female == female)
		{
			if(count == idx)
				return &startOutfits[i];
			++count;
		}
	}
	return NULL;
}

u32 StartOutfitClassCollections::getNumStartOutfits( u32 race, bool female )
{
	u32 count = 0;
	for (u32 i=0; i<startOutfits.size(); ++i)
	{
		if(startOutfits[i].race == race && startOutfits[i].female == female)
			++count;
	}
	return count;
}


SMapRecord* MapCollections::getMapById( s32 id )
{
	T_mapLookup::iterator itr = mapLookup.find(id);
	if (itr == mapLookup.end())
		return NULL;

	s32 index = itr->second;
	return &maps[index];
}

SArea* MapCollections::getAreaById( s32 id )
{
	T_areaLookup::iterator itr = areaLookup.find(id);
	if (itr == areaLookup.end())
		return NULL;

	s32 index = itr->second;
	return &areas[index];
}

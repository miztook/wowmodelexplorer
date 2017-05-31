#include "stdafx.h"
#include "wow_dbc.h"
#include "mywow.h"
#include <tuple>

dbc::dbc( wowEnvironment* env, const c8* filename, bool tmp )
	: minorVersion(0), IsSparse(false)
{
	_recordStart = NULL_PTR;
	_stringStart = NULL_PTR;

	nRecords = 0;
	nFields = 0;
	RecordSize = 0;
	StringSize = 0;
	nActualRecords = 0;

	Fields = NULL_PTR;
	OffsetMaps = NULL_PTR;
	IDs = NULL_PTR;
	CopyTables = NULL_PTR;
	CommonColumns = NULL_PTR;

	HasDataOffsetBlock = false;
	HasIndex = false;
	HasUnknownStuff = false;

	string512 path = filename;
	IMemFile* file = env->openFile(path.c_str());

	if (!file)
	{
		if (hasFileExtensionA(path.c_str(), "dbc"))
			path.changeExt(".dbc", ".db2");

		file = env->openFile(path.c_str());
	}

	ASSERT(file);

	IFileSystem* fs = env->getFileSystem();
	
	if (!file)
	{
		fs->writeLog(ELOG_RES, "cannot open db file: %s", path.c_str());
		return;
	}

	const c8* magic = (const c8*)file->getBuffer();
	int db_type = 0;

	if (strncmp(magic, "WDBC", 4) == 0)
		db_type = 1;
	else if (strncmp(magic, "WDB2", 4) == 0)
		db_type = 2;
	else if (strncmp(magic, "WDB5", 4) == 0)
		db_type = 5;
	else if (strncmp(magic, "WDB6", 4) == 0)
		db_type = 6;
	else
		ASSERT(false);

	if (db_type == 1)		//dbc
		readWDBC(env, file, tmp);
	else if(db_type == 2)		//db2
		readWDB2(env, file, tmp);
	else if (db_type == 5)		//db5
		readWDB5(env, file, tmp);
	else if (db_type == 6)	//db6
		readWDB6(env, file, tmp);

	delete file;
}

dbc::~dbc()
{
	delete[] CommonColumns;
	delete[] CopyTables;
	delete[] IDs;
	delete[] OffsetMaps;
	delete[] Fields;
	delete[] _stringStart;
	delete[] _recordStart;
}

void dbc::readWDBC(wowEnvironment* env, IMemFile* file, bool tmp)
{
	IFileSystem* fs = env->getFileSystem();

	dbcHeader header;
	file->read(&header, sizeof(header));

	nRecords = header._nRecords;
	nFields = header._nFields;
	RecordSize = header._recordSize;
	StringSize = header._stringsize;
	nActualRecords = nRecords;
	
	fs->writeLog(ELOG_RES, "db file %s: field num %d, record size %d, record num %d, string size %d", 
		file->getFileName(), nFields, RecordSize, nActualRecords, StringSize);

	u32 current = file->getPos();
	_recordStart = new u8[RecordSize * nActualRecords];
	_stringStart = new u8[StringSize];

	file->read(_recordStart, RecordSize * nActualRecords);			//records
	file->seek(current + RecordSize * nRecords);
	file->read(_stringStart, StringSize);		//string

	ASSERT(file->getPos() == file->getSize());

	//build map
	if (!tmp)		//临时文件不写map
	{
		for (u32 i=0; i<nActualRecords; ++i)
		{
			u32 id =  *reinterpret_cast<u32*>(_recordStart + i * RecordSize);
			RecordLookup32[id] = i;
		}
	}

	fs->writeLog(ELOG_RES, "successfully loaded db file: %s", file->getFileName());
}

void dbc::readWDB2(wowEnvironment* env, IMemFile* file, bool tmp)
{
	IFileSystem* fs = env->getFileSystem();

	db2Header header;
	file->read(&header, sizeof(header));

	minorVersion = header.clientminor;
	nRecords = header._nRecords;
	nFields = header._nFields;
	RecordSize = header._recordSize;
	StringSize = header._stringsize;
	nActualRecords = nRecords;

	IsSparse = (header.firstrow != 0 && header.lastrow != 0);
	if (IsSparse)
	{
		nActualRecords = 0;

		for (u32 i= header.firstrow; i <= header.lastrow; ++i)
		{
			u32 avail;
			file->read(&avail, 4);
			if (avail == 0)
				continue;
			if(!tmp)		//临时文件不写map
			{
				RecordSparseLookup32[nActualRecords] = i;
			}
			++nActualRecords;
		}

		//skip
		file->seek((header.lastrow - header.firstrow + 1) * 2, true);
	}
	
	fs->writeLog(ELOG_RES, "db file %s: field num %d, record size %d, record num %d, string size %d", 
		file->getFileName(), nFields, RecordSize, nActualRecords, StringSize);

	u32 current = file->getPos();
	_recordStart = new u8[RecordSize * nActualRecords];
	_stringStart = new u8[StringSize];

	file->read(_recordStart, RecordSize * nActualRecords);			//records
	file->seek(current + RecordSize * nRecords);
	file->read(_stringStart, StringSize);		//string

	ASSERT(file->getPos() == file->getSize());

	//build map
	if (!IsSparse && !tmp)		//临时文件不写map
	{
		for (u32 i=0; i<nActualRecords; ++i)
		{
			u32 id =  *reinterpret_cast<u32*>(_recordStart + i * RecordSize);
			RecordLookup32[id] = i;
		}
	}

	fs->writeLog(ELOG_RES, "successfully loaded db file: %s", file->getFileName());
}

void dbc::readWDB5(wowEnvironment* env, IMemFile* file, bool tmp)
{
	IFileSystem* fs = env->getFileSystem();

	db5Header header;
	file->read(&header, sizeof(header));

	nRecords = header._nRecords;
	nFields = header._nFields;
	RecordSize = header._recordSize;
	StringSize = header._stringsize;
	nActualRecords = nRecords;

	HasDataOffsetBlock = (header.fileflags & WDB5_FLAG_DATAOFFSET) != 0;
	HasUnknownStuff = (header.fileflags & WDB5_FLAG_UNKNOWN) != 0;
	HasIndex = (header.fileflags & WDB5_FLAG_INDEX) != 0;

	Fields = new SField[nFields];
	for (u32 i=0; i<nFields; ++i)
	{
		s16 size;
		file->read(&size, (u32)sizeof(s16));
		u16 offset;
		file->read(&offset, (u32)sizeof(u16));

		Fields[i].size = (32 - size) / 8;
		Fields[i].position = offset;

	}

	//TOOD file count

	if (HasDataOffsetBlock)
	{
		u32 curPos = file->getPos();
		file->seek(header._stringsize);

		std::map<u32, u16, std::less<u32>, qzone_allocator<std::pair<u32, u16> > > dataOffsetMap;
		u32 nTotalSize = 0;
		while((u32)dataOffsetMap.size() < header._nRecords)
		{
			u32 offset;
			u16 length;
			file->read(&offset, sizeof(u32));
			file->read(&length, sizeof(u16));
			if (offset > 0 && dataOffsetMap.find(offset) == dataOffsetMap.end())
			{
				ASSERT(offset >= curPos);
				dataOffsetMap[offset] = length;
				nTotalSize += length;
			}

			if (file->isEof())
			{	
				fs->writeLog(ELOG_RES, "dataoffset load error: %s", file->getFileName());
				return;
			}
		}

		u32 indexDataStart = file->getPos();

		int x = 0;
		OffsetMaps = new SOffsetMapEntry[header._nRecords];
		_recordStart = new u8[nTotalSize];
		u32 curOffset = 0;
		int count = 0;
		for (auto itr = dataOffsetMap.begin(); itr != dataOffsetMap.end(); ++itr)
		{
			u32 off = itr->first;
			u16 len = itr->second;
			
			file->seek(off);
			file->read(&_recordStart[curOffset], len);
			
			OffsetMaps[count].offset = curOffset;
			OffsetMaps[count].length = len;

			curOffset += len;
			++count;
		}
		_stringStart = NULL;

		file->seek(indexDataStart);
	}
	else
	{
		u32 current = file->getPos();
		_recordStart = new u8[RecordSize * nRecords];
		_stringStart = new u8[StringSize];

		file->read(_recordStart, RecordSize * nRecords);			//records
		file->seek(current + RecordSize * nRecords);
		file->read(_stringStart, StringSize);		//string
	}

	s32 indexDataSize = nRecords * sizeof(s32);
	if(HasUnknownStuff)
		file->seek(indexDataSize, true);

	if (HasIndex)
	{
		IDs = new u32[nRecords];
		file->read(IDs, sizeof(u32) * nRecords);
	}

	if (header.refdatasize > 0)
	{
		ASSERT(header.refdatasize % sizeof(SCopyTableEntry) == 0);
		//int nCount = header.refdatasize / sizeof(SCopyTableEntry);
		//CopyTables = new SCopyTableEntry[nCount];
		//file->read(CopyTables, header.refdatasize);
		file->seek(header.refdatasize, true);
	}

	ASSERT(file->getPos() == file->getSize());

	//build map
	if (!tmp && HasIndex)		//临时文件不写map
	{
		for (u32 i=0; i<nRecords; ++i)
		{
			u32 id = IDs[i];
			RecordLookup32[id] = i;
		}
	}

	fs->writeLog(ELOG_RES, "successfully loaded db file: %s", file->getFileName());
}

void dbc::readWDB6(wowEnvironment* env, IMemFile* file, bool tmp)
{
	IFileSystem* fs = env->getFileSystem();

	db6Header header;
	file->read(&header, sizeof(header));

	nRecords = header._nRecords;
	nFields = header._nFields;
	RecordSize = header._recordSize;
	StringSize = header._stringsize;
	nActualRecords = nRecords;

	HasDataOffsetBlock = (header.fileflags & WDB5_FLAG_DATAOFFSET) != 0;
	HasUnknownStuff = (header.fileflags & WDB5_FLAG_UNKNOWN) != 0;
	HasIndex = (header.fileflags & WDB5_FLAG_INDEX) != 0;

	Fields = new SField[nFields];
	for (u32 i = 0; i < nFields; ++i)
	{
		s16 size;
		file->read(&size, (u32)sizeof(s16));
		u16 offset;
		file->read(&offset, (u32)sizeof(u16));

		Fields[i].size = (32 - size) / 8;
		Fields[i].position = offset;

	}

	//TOOD file count

	if (HasDataOffsetBlock)
	{
		u32 curPos = file->getPos();
		file->seek(header._stringsize);

		std::map<u32, u16, std::less<u32>, qzone_allocator<std::pair<u32, u16> > > dataOffsetMap;
		u32 nTotalSize = 0;
		while ((u32)dataOffsetMap.size() < header._nRecords)
		{
			u32 offset;
			u16 length;
			file->read(&offset, sizeof(u32));
			file->read(&length, sizeof(u16));
			if (offset > 0 && dataOffsetMap.find(offset) == dataOffsetMap.end())
			{
				ASSERT(offset >= curPos);
				dataOffsetMap[offset] = length;
				nTotalSize += length;
			}

			if (file->isEof())
			{
				fs->writeLog(ELOG_RES, "dataoffset load error: %s", file->getFileName());
				return;
			}
		}

		u32 indexDataStart = file->getPos();

		int x = 0;
		OffsetMaps = new SOffsetMapEntry[header._nRecords];
		_recordStart = new u8[nTotalSize];
		u32 curOffset = 0;
		int count = 0;
		for (auto itr = dataOffsetMap.begin(); itr != dataOffsetMap.end(); ++itr)
		{
			u32 off = itr->first;
			u16 len = itr->second;

			file->seek(off);
			file->read(&_recordStart[curOffset], len);

			OffsetMaps[count].offset = curOffset;
			OffsetMaps[count].length = len;

			curOffset += len;
			++count;
		}
		_stringStart = NULL;

		file->seek(indexDataStart);
	}
	else
	{
		u32 current = file->getPos();
		_recordStart = new u8[RecordSize * nRecords];
		_stringStart = new u8[StringSize];

		file->read(_recordStart, RecordSize * nRecords);			//records
		file->seek(current + RecordSize * nRecords);
		file->read(_stringStart, StringSize);		//string
	}

	s32 indexDataSize = nRecords * sizeof(s32);
	if (HasUnknownStuff)
		file->seek(indexDataSize, true);

	if (HasIndex)
	{
		IDs = new u32[nRecords];
		file->read(IDs, sizeof(u32) * nRecords);
	}

	if (header.refdatasize > 0)
	{
		ASSERT(header.refdatasize % sizeof(SCopyTableEntry) == 0);
		//int nCount = header.refdatasize / sizeof(SCopyTableEntry);
		//CopyTables = new SCopyTableEntry[nCount];
		//file->read(CopyTables, header.refdatasize);
		file->seek(header.refdatasize, true);
	}

	if (header.nonzero_column_table_size > 0)
	{
		u32 ncolumns;
		file->read(&ncolumns, sizeof(ncolumns));

		CommonColumns = new SCommonColumn[ncolumns];
		for (u32 c = 0; c < ncolumns; ++c)
		{
			u32 nrecs;
			file->read(&nrecs, sizeof(nrecs));

			u8 type;
			file->read(&type, sizeof(type));

			if (nrecs == 0)
				continue;

			u32 size = 4;
			if (type == 1)
				size = 2;
			else if (type == 2)
				size = 1;

			std::map < u32, u32> recmap;
			for (u32 i = 0; i < nrecs; ++i)
			{
				u32 id;
				file->read(&id, sizeof(id));

				u32 val;
				file->read(&val, size);

				recmap[id] = val;
			}

			SCommonColumn commonColumn;
			commonColumn.recordmap = std::move(recmap);
			commonColumn.type = type;
			CommonColumns[c] = std::move(commonColumn);
		}
	}

	ASSERT(file->getPos() == file->getSize());

	//build map
	if (!tmp && HasIndex)		//临时文件不写map
	{
		for (u32 i = 0; i < nRecords; ++i)
		{
			u32 id = IDs[i];
			RecordLookup32[id] = i;
		}
	}

	fs->writeLog(ELOG_RES, "successfully loaded db file: %s", file->getFileName());
}

dbc::record charFacialHairDB::getByParams(unsigned int race, unsigned int gender, unsigned int style) const
{
	for (u32 i=0; i<nRecords; ++i)
	{
		dbc::record r = getRecord(i);
		if (r.getUInt(RaceV400) == race &&
			r.getUInt(GenderV400) == gender &&
			r.getUInt(StyleV400) == style)					//id是record的第一个字段
			return r;
	}
	return dbc::record::EMPTY();
}

u32 charFacialHairDB::getStylesFor( unsigned int race, unsigned int gender) const
{
	u32 n = 0;

	for (u32 i=0; i<nRecords; ++i)
	{
		dbc::record r = getRecord(i);
		if (r.getUInt(RaceV400) == race &&
			r.getUInt(GenderV400) == gender)
			++n;
	}
	return n;
}

dbc::record charHairGeosetsDB::getByParams( unsigned int race, unsigned int gender, unsigned int section ) const
{
	for (u32 i=0; i<nRecords; ++i)
	{
		dbc::record r = getRecord(i);
		if (r.getUInt(Race) == race &&
			r.getUInt(Gender) == gender &&
			r.getUInt(Section) == section)					//id是record的第一个字段
			return r;
	}
	return dbc::record::EMPTY();
}

u32 charHairGeosetsDB::getGeosetsFor( unsigned int race, unsigned int gender ) const
{
	u32 n = 0;

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
		if(Q_stricmp(r.getString(Name),name) == 0)
			return r;
	}
	return dbc::record::EMPTY();
}

dbc::record charSectionsDB::getByParams( u32 race, u32 gender, u32 type, u32 section, u32 color ) const
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
	return dbc::record::EMPTY();
}

u32 charSectionsDB::getColorsFor( u32 race, u32 gender, u32 type, u32 section ) const
{
	u32 n = 0;

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

u32 charSectionsDB::getSectionsFor( u32 race, u32 gender, u32 type, u32 color ) const
{
	u32 n = 0;

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
	return dbc::record::EMPTY();
}

void itemModifiedAppearanceDB::buildItemLookup()
{
	for (u32 i=0; i<nActualRecords; ++i)
	{
		u32 itemid = getRecord(i).getUInt(itemModifiedAppearanceDB::ItemId);
		ItemLookup32[itemid] = i;
	}
}

const SItemRecord* ItemCollections::getById( s32 id ) const
{
	T_itemLookup::const_iterator itr = itemLookup.find(id);
	if (itr != itemLookup.end()) 
	{
		s32 i = itr->second;
		return &items[i];
	}
	return NULL_PTR;
}

void ItemCollections::build( itemDB* itemDb, itemSparseDB* itemSparseDb)
{
	u32 numRecords = itemSparseDb->getNumActualRecords();
	items.clear();
	items.reserve(numRecords);

	for (u32 i=0; i<numRecords; ++i)
	{
		dbc::record rs = itemSparseDb->getRecord(i);
		if(!rs.isValid())
			continue;

		SItemRecord rec;
#ifdef WOW70
		rec.id = rs.getID();
#else
		rec.id = itemSparseDb->getRecordSparseRow(i);
#endif
		if (rec.id == -1)
			continue;

		//item db
		dbc::record r = itemDb->getByID(rec.id);		
		if (!r.isValid())
			continue;

		rec.itemclass = r.getInt(itemDB::Itemclass);
		rec.subclass = r.getInt(itemDB::Subclass);
		rec.type = r.getInt(itemDB::InventorySlot);

#ifdef WOW70
		Q_sprintf(rec.name, DEFAULT_SIZE * 2, "Item-%d", rec.id);
#else
		const c8* str = rs.getString(itemSparseDb->getItemNameField());
		Q_strcpy(rec.name, DEFAULT_SIZE * 2, str);
#endif
		
		items.emplace_back(rec);
		itemLookup[rec.id] = (s32)(items.size() -1);
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

		Q_sprintf(rec.name, DEFAULT_SIZE, "%d", id);

		items.emplace_back(rec);
		itemLookup[rec.id] = (s32)(items.size() -1);
	}
	*/
}

SNPCRecord::SNPCRecord( const c8* line )
{
	s32 id;

	Q_sscanf(line, "%d,%d,%d", &id, &model, &type);

	size_t len = strlen(line);
	for (size_t i=len-2; i>1; i--) {
		if (line[i]==',') 
		{
			Q_strcpy(name, DEFAULT_SIZE * 2, line+i+1);
			break;
		}
	}

	u32 l = (u32)Q_strlen(name);
	if ( l > 0 )
		name[l-1] = '\0';
}

bool NPCCollections::open( const c8* filename )
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
			npcLookup[rec.model] = (s32)(npcs.size() -1);

			//id_set.insert(rec.id);
		}
	}

	delete file;

	//build other npcs (unknown name)
	const creatureDisplayInfoDB* db = g_Engine->getWowDatabase()->getCreatureDisplayInfoDB();
	for (u32 i=0; i<db->getNumActualRecords(); ++i)
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
		npcLookup[rec.model] = (s32)(npcs.size() -1);
	}

	//npcs.shrink_to_fit();

	return true;
}

const SNPCRecord* NPCCollections::getById( s32 id ) const
{
	T_npcLookup::const_iterator itr = npcLookup.find(id);
	if (itr != npcLookup.end()) 
	{
		s32 i = itr->second;
		return &npcs[i];
	}
	return NULL_PTR;
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
	return NULL_PTR;
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


const SMapRecord* MapCollections::getMapById( s32 id ) const
{
	T_mapLookup::const_iterator itr = mapLookup.find(id);
	if (itr == mapLookup.end())
		return NULL_PTR;

	s32 index = itr->second;
	return &maps[index];
}

SMapRecord* MapCollections::getMapById( s32 id )
{
	T_mapLookup::iterator itr = mapLookup.find(id);
	if (itr == mapLookup.end())
		return NULL_PTR;

	s32 index = itr->second;
	return &maps[index];
}

const SArea* MapCollections::getAreaById( s32 id ) const
{
	T_areaLookup::const_iterator itr = areaLookup.find(id);
	if (itr == areaLookup.end())
		return NULL_PTR;

	s32 index = itr->second;
	return &areas[index];
}

SArea* MapCollections::getAreaById( s32 id )
{
	T_areaLookup::iterator itr = areaLookup.find(id);
	if (itr == areaLookup.end())
		return NULL_PTR;

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


SRidable::SRidable( const c8* line )
{
	Q_sscanf(line, "%d,%d", &npcid, &mountflag);
}

bool RidableCollections::open( const c8* filename, const NPCCollections& npcs )
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
		if (rec.npcid != 0 || npcs.getById(rec.npcid) != NULL_PTR)
		{
			ridables.emplace_back(rec);
		}
	}

	delete file;

	//ridables.shrink_to_fit();

	return true;
}

void fileDataDB::saveListFile(const char* szPath)
{
	FILE* file = fopen(szPath, "wt");
	if (!file)
		return;

	for (u32 i=0; i<getNumActualRecords(); ++i)
	{
		dbc::record r = getRecord(i);
		if (!r.isValid())
			continue;
		
		char path[MAX_PATH];
		Q_strcpy(path, MAX_PATH, r.getString(fileDataDB::FilePath));
		Q_strcat(path, MAX_PATH, r.getString(fileDataDB::FileName));
		Q_strcat(path, MAX_PATH, "\n");

		fputs(path, file);
	}

	fclose(file);
}

dbc::record itemDisplayInfoMaterialResDB::getByItemDisplayInfoIDAndSlot(u32 itemDisplayId, u32 slot) const
{
	for (u32 i=0; i<nRecords; ++i)
	{
		dbc::record r = getRecord(i);
		if(r.getUInt(itemDisplayInfoMaterialResDB::ItemDisplayInfoID) == itemDisplayId &&
			r.getUInt(itemDisplayInfoMaterialResDB::TextureSlot) == slot)
			return r;
	}
	return dbc::record::EMPTY();
}

void itemDisplayInfoMaterialResDB::getTexturePath(u32 itemDisplayId, u32 slot, c8* path, u32 size) const
{
	dbc::record r = getByItemDisplayInfoIDAndSlot(itemDisplayId, slot);
	if (!r.isValid())
	{
		memset(path, 0, size);
		return;
	}
	int texId = r.getInt(itemDisplayInfoMaterialResDB::TextureFileDataID);
	g_Engine->getWowDatabase()->getTextureFilePath(texId, path, size);
}

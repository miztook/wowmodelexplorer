#include "stdafx.h"
#include "wow_dbc.h"
#include "mywow.h"

namespace WowClassic
{
	dbc::dbc(const wowEnvironment* env, const char* filename, bool tmp)
		: DBType(WowDBType::Unknown)
		, minorVersion(0)
	{
		_recordStart = nullptr;
		_stringStart = nullptr;

		nRecords = 0;
		nFields = 0;
		RecordSize = 0;
		StringSize = 0;
		nActualRecords = 0;

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

		const char* magic = (const char*)file->getBuffer();
		if (strncmp(magic, "WDBC", 4) == 0)
			DBType = WowDBType::WDBC;
		else if (strncmp(magic, "WDB2", 4) == 0)
			DBType = WowDBType::WDB2;
		else if (strncmp(magic, "WDB5", 4) == 0)
			DBType = WowDBType::WDB5;
		else if (strncmp(magic, "WDB6", 4) == 0)
			DBType = WowDBType::WDB6;
		else if (strncmp(magic, "WDC1", 4) == 0)
			DBType = WowDBType::WDC1;
		else if (strncmp(magic, "WDC2", 4) == 0)
			DBType = WowDBType::WDC2;
		else
			ASSERT(false);

		if (DBType == WowDBType::WDBC)		//dbc
			readWDBC(env, file, tmp);
		else if (DBType == WowDBType::WDB2)		//db2
			readWDB2(env, file, tmp);
		else
			ASSERT(false);

		delete file;
	}

	dbc::~dbc()
	{
		delete[] _stringStart;
		delete[] _recordStart;
	}

	void dbc::readWDBC(const wowEnvironment* env, IMemFile* file, bool tmp)
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

		uint32_t current = file->getPos();
		_recordStart = new uint8_t[RecordSize * nActualRecords];
		_stringStart = new uint8_t[StringSize];

		file->read(_recordStart, RecordSize * nActualRecords);			//records
		file->seek(current + RecordSize * nRecords);
		file->read(_stringStart, StringSize);		//string

		ASSERT(file->getPos() == file->getSize());

		//build map
		if (!tmp)		//临时文件不写map
		{
			for (uint32_t i = 0; i < nActualRecords; ++i)
			{
				uint32_t id = *reinterpret_cast<uint32_t*>(_recordStart + i * RecordSize);
				RecordLookup32[id] = i;
			}
		}

		fs->writeLog(ELOG_RES, "successfully loaded db file: %s", file->getFileName());
	}

	void dbc::readWDB2(const wowEnvironment* env, IMemFile* file, bool tmp)
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

		bool isSparse = (header.min_id != 0 && header.max_id != 0);
		if (isSparse)
		{
			nActualRecords = 0;
			for (uint32_t i = header.min_id; i <= header.max_id; ++i)
			{
				uint32_t avail;
				file->read(&avail, 4);
				if (avail == 0)
					continue;
				if (!tmp)		//临时文件不写map
				{
					RecordSparseLookup32[nActualRecords] = i;
				}
				++nActualRecords;
			}

			//skip
			file->seek((header.max_id - header.min_id + 1) * 2, true);
		}

		fs->writeLog(ELOG_RES, "db file %s: field num %d, record size %d, record num %d, string size %d",
			file->getFileName(), nFields, RecordSize, nActualRecords, StringSize);

		uint32_t current = file->getPos();
		_recordStart = new uint8_t[RecordSize * nActualRecords];
		_stringStart = new uint8_t[StringSize];

		file->read(_recordStart, RecordSize * nActualRecords);			//records
		file->seek(current + RecordSize * nRecords);
		file->read(_stringStart, StringSize);		//string

		ASSERT(file->getPos() == file->getSize());

		//build map
		if (!isSparse && !tmp)		//临时文件不写map
		{
			for (uint32_t i = 0; i < nActualRecords; ++i)
			{
				uint32_t id = *reinterpret_cast<uint32_t*>(_recordStart + i * RecordSize);
				RecordLookup32[id] = i;
			}
		}

		fs->writeLog(ELOG_RES, "successfully loaded db file: %s", file->getFileName());
	}

	/*
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
	HasRelationshipData = (header.fileflags & WDB5_FLAG_UNKNOWN) != 0;
	HasIndex = (header.fileflags & WDB5_FLAG_INDEX) != 0;
	HasCopyData = header.copydatasize > 0;

	Fields = new SField[nFields];
	for (uint32_t i=0; i<nFields; ++i)
	{
	int16_t size;
	file->read(&size, (uint32_t)sizeof(int16_t));
	uint16_t offset;
	file->read(&offset, (uint32_t)sizeof(uint16_t));

	Fields[i].size = (32 - size) / 8;
	Fields[i].position = offset;
	}

	if (HasDataOffsetBlock)
	{
	uint32_t curPos = file->getPos();
	file->seek(header._stringsize);

	nActualRecords = 0;
	uint32_t nTotalSize = 0;
	for (uint32_t i = header.min_id; i <= header.max_id; ++i)
	{
	SOffsetMapEntry entry;
	file->read(&entry, sizeof(SOffsetMapEntry));

	if (entry.offset > 0 && entry.length > 0)
	{
	ASSERT(entry.offset >= curPos);
	IDs.push_back(i);
	OffsetMaps.push_back(entry);
	nTotalSize += entry.length;

	++nActualRecords;
	}
	}

	//整合recordstart
	const uint32_t indexDataStart = file->getPos();
	_recordStart = new uint8_t[nTotalSize];
	uint32_t curOffset = 0;
	for (auto& entry : OffsetMaps)
	{
	file->seek(entry.offset);
	file->read(&_recordStart[curOffset], entry.length);

	entry.offset = curOffset;
	curOffset += entry.length;
	}
	file->seek(indexDataStart);
	_stringStart = nullptr;
	}
	else   //no dataoffset
	{
	uint32_t current = file->getPos();
	_recordStart = new uint8_t[RecordSize * nRecords];
	_stringStart = new uint8_t[StringSize];

	file->read(_recordStart, RecordSize * nRecords);			//records
	file->read(_stringStart, StringSize);		//string

	//IDs
	if (HasIndex)
	{
	IDs.resize(nRecords);
	file->read(IDs.data(), nRecords * sizeof(uint32_t));
	}
	else
	{
	uint16_t indexPos = Fields[header.idindex].position;
	uint16_t indexSize = Fields[header.idindex].size;
	uint32_t indexMask = 0xFFFFFFFF;
	if (indexSize == 1)
	indexMask = 0x000000FF;
	else if (indexSize == 2)
	indexMask = 0x0000FFFF;
	else if (indexSize == 3)
	indexMask = 0x00FFFFFF;

	for (uint32_t i = 0; i < nRecords; ++i)
	{
	uint8_t* ofs = _recordStart + i * RecordSize;
	uint32_t val;
	memcpy(&val, ofs + indexPos, indexSize);
	val &= indexMask;
	IDs.push_back(val);
	};
	}

	for (uint32_t i = 0; i < nActualRecords; ++i)
	{
	SOffsetMapEntry entry;
	entry.offset = i * RecordSize;
	entry.length = RecordSize;
	OffsetMaps.push_back(entry);
	}
	}

	//relationship
	if (HasRelationshipData)
	{
	int32_t relationshipDataSize = nRecords * sizeof(int32_t);
	file->seek(nRecords * sizeof(int32_t), true);
	}

	//copy table
	if (HasCopyData)
	{
	ASSERT(header.copydatasize % sizeof(SCopyTableEntry) == 0);
	uint32_t nbEntries = (uint32_t)(header.copydatasize / sizeof(SCopyTableEntry));

	std::vector<SCopyTableEntry> copyTables;
	copyTables.resize(nbEntries);
	file->read(copyTables.data(), (uint32_t)(copyTables.size() * sizeof(SCopyTableEntry)));

	IDs.reserve(nActualRecords + nbEntries);
	OffsetMaps.reserve(nActualRecords + nbEntries);

	std::map<uint32_t, uint32_t, std::less<uint32_t>, qzone_allocator<std::pair<uint32_t, uint32_t>>> IdToIndexMap;
	for (uint32_t i = 0; i < nActualRecords; ++i)
	{
	IdToIndexMap[IDs[i]] = i;
	}

	for (const auto& entry : copyTables)
	{
	IDs.push_back(entry.id_new_row);
	uint32_t idx = IdToIndexMap[entry.id_copied_row];
	OffsetMaps.push_back(OffsetMaps[idx]);
	}

	nActualRecords += nbEntries;
	}

	ASSERT(file->getPos() == file->getSize());

	//build map
	if (!tmp)		//临时文件不写map
	{
	for (uint32_t i = 0; i < (uint32_t)IDs.size(); ++i)
	{
	uint32_t id = IDs[i];
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
	HasRelationshipData = (header.fileflags & WDB5_FLAG_UNKNOWN) != 0;
	HasIndex = (header.fileflags & WDB5_FLAG_INDEX) != 0;

	Fields = new SField[nFields];
	for (uint32_t i = 0; i < nFields; ++i)
	{
	int16_t size;
	file->read(&size, (uint32_t)sizeof(int16_t));
	uint16_t offset;
	file->read(&offset, (uint32_t)sizeof(uint16_t));

	Fields[i].size = (32 - size) / 8;
	Fields[i].position = offset;
	}

	if (HasDataOffsetBlock)
	{
	uint32_t curPos = file->getPos();
	file->seek(header._stringsize);

	nActualRecords = 0;
	uint32_t nTotalSize = 0;
	for (uint32_t i = header.min_id; i <= header.max_id; ++i)
	{
	SOffsetMapEntry entry;
	file->read(&entry, sizeof(SOffsetMapEntry));

	if (entry.offset > 0 && entry.length > 0)
	{
	ASSERT(entry.offset >= curPos);
	IDs.push_back(i);
	OffsetMaps.push_back(entry);
	nTotalSize += entry.length;

	++nActualRecords;
	}
	}

	//整合recordstart
	const uint32_t indexDataStart = file->getPos();
	_recordStart = new uint8_t[nTotalSize];
	uint32_t curOffset = 0;
	for (auto& entry : OffsetMaps)
	{
	file->seek(entry.offset);
	file->read(&_recordStart[curOffset], entry.length);

	entry.offset = curOffset;
	curOffset += entry.length;
	}
	file->seek(indexDataStart);
	_stringStart = nullptr;
	}
	else   //no dataoffset
	{
	uint32_t current = file->getPos();
	_recordStart = new uint8_t[RecordSize * nRecords];
	_stringStart = new uint8_t[StringSize];

	file->read(_recordStart, RecordSize * nRecords);			//records
	file->read(_stringStart, StringSize);		//string

	//IDs
	if (HasIndex)
	{
	IDs.resize(nRecords);
	file->read(IDs.data(), nRecords * sizeof(uint32_t));
	}
	else
	{
	uint16_t indexPos = Fields[header.idindex].position;
	uint16_t indexSize = Fields[header.idindex].size;
	uint32_t indexMask = 0xFFFFFFFF;
	if (indexSize == 1)
	indexMask = 0x000000FF;
	else if (indexSize == 2)
	indexMask = 0x0000FFFF;
	else if (indexSize == 3)
	indexMask = 0x00FFFFFF;

	for (uint32_t i = 0; i < nRecords; ++i)
	{
	uint8_t* ofs = _recordStart + i * RecordSize;
	uint32_t val;
	memcpy(&val, ofs + indexPos, indexSize);
	val &= indexMask;
	IDs.push_back(val);
	};
	}

	for (uint32_t i = 0; i < nRecords; ++i)
	{
	SOffsetMapEntry entry;
	entry.offset = i * RecordSize;
	entry.length = RecordSize;
	OffsetMaps.push_back(entry);
	}
	}

	//relationship
	if (HasRelationshipData)
	{
	int32_t relationshipDataSize = nRecords * sizeof(int32_t);
	file->seek(nRecords * sizeof(int32_t), true);
	}

	//copy table
	if (HasCopyData)
	{
	ASSERT(header.copydatasize % sizeof(SCopyTableEntry) == 0);
	uint32_t nbEntries = (uint32_t)(header.copydatasize / sizeof(SCopyTableEntry));

	std::vector<SCopyTableEntry> copyTables;
	copyTables.resize(nbEntries);
	file->read(copyTables.data(), (uint32_t)(copyTables.size() * sizeof(SCopyTableEntry)));

	IDs.reserve(nActualRecords + nbEntries);
	OffsetMaps.reserve(nActualRecords + nbEntries);

	std::map<uint32_t, uint32_t, std::less<uint32_t>, qzone_allocator<std::pair<uint32_t, uint32_t>>> IdToIndexMap;
	for (uint32_t i = 0; i < nActualRecords; ++i)
	{
	IdToIndexMap[IDs[i]] = i;
	}

	for (const auto& entry : copyTables)
	{
	IDs.push_back(entry.id_new_row);
	uint32_t idx = IdToIndexMap[entry.id_copied_row];
	OffsetMaps.push_back(OffsetMaps[idx]);
	}

	nActualRecords += nbEntries;
	}

	if (header.nonzero_column_table_size > 0)
	{
	uint32_t ncolumns;
	file->read(&ncolumns, sizeof(ncolumns));

	CommonColumns = new SCommonColumn[ncolumns];
	for (uint32_t c = 0; c < ncolumns; ++c)
	{
	uint32_t nrecs;
	file->read(&nrecs, sizeof(nrecs));

	uint8_t type;
	file->read(&type, sizeof(type));

	if (nrecs == 0)
	continue;

	uint32_t size = 4;
	if (type == 1)
	size = 2;
	else if (type == 2)
	size = 1;

	std::map < uint32_t, uint32_t> recmap;
	for (uint32_t i = 0; i < nrecs; ++i)
	{
	uint32_t id;
	file->read(&id, sizeof(id));

	uint32_t val;
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
	for (uint32_t i = 0; i < nRecords; ++i)
	{
	uint32_t id = IDs[i];
	RecordLookup32[id] = i;
	}
	}

	fs->writeLog(ELOG_RES, "successfully loaded db file: %s", file->getFileName());
	}

	void dbc::readWDC2(wowEnvironment* env, IMemFile* file, bool tmp)
	{
	IFileSystem* fs = env->getFileSystem();

	dc2Header header;
	file->read(&header, sizeof(header));

	nRecords = header._nRecords;
	nFields = header._nFields;
	RecordSize = header._recordSize;
	StringSize = header._stringsize;
	nActualRecords = nRecords;

	HasDataOffsetBlock = (header.fileflags & WDB5_FLAG_DATAOFFSET) != 0;
	HasRelationshipData = (header.fileflags & WDB5_FLAG_UNKNOWN) != 0;
	HasIndex = (header.fileflags & WDB5_FLAG_INDEX) != 0;

	//section
	std::vector<SSectionHeader> sectionHeaders;
	sectionHeaders.resize(header.section_count);
	file->read(sectionHeaders.data(), header.section_count * sizeof(SSectionHeader));

	//field
	Fields = new SField[nFields];
	file->read(Fields, nFields * sizeof(SField));

	//field_storage_info
	uint32_t nFieldStorage = header.field_storage_info_size / sizeof(SFieldStorageInfo);
	FieldStorageInfos = new SFieldStorageInfo[nFieldStorage];
	file->read(FieldStorageInfos, nFieldStorage * sizeof(SFieldStorageInfo));

	//
	uint32_t palletBlockOffset = file->getPos();
	uint32_t commonBlockOffset = palletBlockOffset + header.pallet_data_size;

	uint32_t stringSize = HasDataOffsetBlock ? 0 : header._stringsize;
	uint32_t IdBlockOffset = file->getPos() + nRecords * RecordSize;
	uint32_t copyBlockOffset = IdBlockOffset + sectionHeaders[0].id_list_size;
	uint32_t relationshipDataOffset = copyBlockOffset + sectionHeaders[0].copy_table_size;

	if (HasDataOffsetBlock)
	{
	file->seek(sectionHeaders[0].offset_map_offset);

	nActualRecords = 0;
	uint32_t nTotalSize = 0;
	for (uint32_t i = header.min_id; i <= header.max_id; ++i)
	{
	SOffsetMapEntry entry;
	file->read(&entry, sizeof(SOffsetMapEntry));

	if (entry.offset > 0 && entry.length > 0)
	{
	IDs.push_back(i);
	OffsetMaps.push_back(entry);
	nTotalSize += entry.length;

	++nActualRecords;
	}
	}

	//整合recordstart
	const uint32_t indexDataStart = file->getPos();
	_recordStart = new uint8_t[nTotalSize];
	uint32_t curOffset = 0;
	for (auto& entry : OffsetMaps)
	{
	file->seek(entry.offset);
	file->read(&_recordStart[curOffset], entry.length);

	entry.offset = curOffset;
	curOffset += entry.length;
	}
	file->seek(indexDataStart);
	_stringStart = nullptr;
	}
	else
	{
	uint32_t current = file->getPos();
	_recordStart = new uint8_t[RecordSize * nRecords];
	_stringStart = new uint8_t[StringSize];

	file->read(_recordStart, RecordSize * nRecords);			//records
	file->read(_stringStart, StringSize);		//string

	//IDs
	if (HasIndex)
	{
	IDs.resize(nRecords);
	file->read(IDs.data(), nRecords * sizeof(uint32_t));
	}
	else
	{
	ASSERT(StringSize == 0);

	const SFieldStorageInfo& info = FieldStorageInfos[header.idindex];
	const uint8_t* data = file->getPointer();

	for (uint32_t i = 0; i < nRecords; ++i)
	{
	const uint8_t* recordOffset = data + i * RecordSize;
	switch (info.storage_type)
	{
	case FIELD_COMPRESSION::NONE:
	{
	uint8_t* val = new uint8_t[info.field_size_bits / 8];
	memcpy(&val, recordOffset + info.field_offset_bits / 8, info.field_size_bits / 8);
	IDs.push_back((*reinterpret_cast<uint32_t*>(val)));
	delete[] val;
	}
	break;
	case FIELD_COMPRESSION::BITPACKED:
	{
	uint32_t id = readBitpackedValue(info, recordOffset);
	IDs.push_back(id);
	}
	break;
	case FIELD_COMPRESSION::BITPACKED_INDEXED:
	case FIELD_COMPRESSION::BITPACKED_SIGNED:
	{
	uint32_t id = readBitpackedValue2(info, recordOffset);
	IDs.push_back(id);
	}
	break;
	case FIELD_COMPRESSION::COMMON_DATA:
	case FIELD_COMPRESSION::BITPACKED_INDEXED_ARRAY:
	default:
	ASSERT(false);
	break;
	}
	}
	}

	for (uint32_t i = 0; i < nRecords; ++i)
	{
	SOffsetMapEntry entry;
	entry.offset = i * RecordSize;
	entry.length = RecordSize;
	OffsetMaps.push_back(entry);
	}
	}

	if (sectionHeaders[0].copy_table_size > 0)
	{
	file->seek(copyBlockOffset);
	uint32_t nbEntries = (uint32_t)(sectionHeaders[0].copy_table_size / sizeof(SCopyTableEntry));

	std::vector<SCopyTableEntry> copyTables;
	copyTables.resize(nbEntries);
	file->read(copyTables.data(), (uint32_t)(copyTables.size() * sizeof(SCopyTableEntry)));

	IDs.reserve(nActualRecords + nbEntries);
	OffsetMaps.reserve(nActualRecords + nbEntries);

	std::map<uint32_t, uint32_t, std::less<uint32_t>, qzone_allocator<std::pair<uint32_t, uint32_t>>> IdToIndexMap;
	for (uint32_t i = 0; i < nActualRecords; ++i)
	{
	IdToIndexMap[IDs[i]] = i;
	}

	for (const auto& entry : copyTables)
	{
	IDs.push_back(entry.id_new_row);
	uint32_t idx = IdToIndexMap[entry.id_copied_row];
	OffsetMaps.push_back(OffsetMaps[idx]);
	}

	nActualRecords += nbEntries;
	}

	ASSERT(file->getPos() == file->getSize());

	//build map
	if (!tmp && HasIndex)		//临时文件不写map
	{
	for (uint32_t i = 0; i < nRecords; ++i)
	{
	uint32_t id = IDs[i];
	RecordLookup32[id] = i;
	}
	}

	fs->writeLog(ELOG_RES, "successfully loaded db file: %s", file->getFileName());
	}

	*/

	dbc::record charFacialHairDB::getByParams(unsigned int race, unsigned int gender, unsigned int style) const
	{
		for (uint32_t i = 0; i < nRecords; ++i)
		{
			dbc::record r = getRecord(i);
			if (r.getUInt(RaceV400) == race &&
				r.getUInt(GenderV400) == gender &&
				r.getUInt(StyleV400) == style)					//id是record的第一个字段
				return r;
		}
		return dbc::record::EMPTY();
	}

	uint32_t charFacialHairDB::getStylesFor(unsigned int race, unsigned int gender) const
	{
		uint32_t n = 0;

		for (uint32_t i = 0; i < nRecords; ++i)
		{
			dbc::record r = getRecord(i);
			if (r.getUInt(RaceV400) == race &&
				r.getUInt(GenderV400) == gender)
				++n;
		}
		return n;
	}

	dbc::record charHairGeosetsDB::getByParams(unsigned int race, unsigned int gender, unsigned int section) const
	{
		for (uint32_t i = 0; i < nRecords; ++i)
		{
			dbc::record r = getRecord(i);
			if (r.getUInt(Race) == race &&
				r.getUInt(Gender) == gender &&
				r.getUInt(Section) == section)					//id是record的第一个字段
				return r;
		}
		return dbc::record::EMPTY();
	}

	uint32_t charHairGeosetsDB::getGeosetsFor(unsigned int race, unsigned int gender) const
	{
		uint32_t n = 0;

		for (uint32_t i = 0; i < nRecords; ++i)
		{
			dbc::record r = getRecord(i);
			if (r.getUInt(Race) == race &&
				r.getUInt(Gender) == gender)
				++n;
		}
		return n;
	}

	dbc::record charRacesDB::getByName(const char* name)
	{
		for (uint32_t i = 0; i < nRecords; ++i)
		{
			dbc::record r = getRecord(i);
			if (Q_stricmp(r.getString(Name), name) == 0)
				return r;
		}
		return dbc::record::EMPTY();
	}

	dbc::record charSectionsDB::getByParams(uint32_t race, uint32_t gender, uint32_t type, uint32_t section, uint32_t color) const
	{
		for (uint32_t i = 0; i < nRecords; ++i)
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

	uint32_t charSectionsDB::getColorsFor(uint32_t race, uint32_t gender, uint32_t type, uint32_t section) const
	{
		uint32_t n = 0;

		for (uint32_t i = 0; i < nRecords; ++i)
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

	uint32_t charSectionsDB::getSectionsFor(uint32_t race, uint32_t gender, uint32_t type, uint32_t color) const
	{
		uint32_t n = 0;

		for (uint32_t i = 0; i < nRecords; ++i)
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

	dbc::record itemSubClassDB::getById(int id, int subid)
	{
		for (uint32_t i = 0; i < nRecords; ++i)
		{
			dbc::record r = getRecord(i);
			if (r.getInt(ClassIDV400) == id && r.getInt(SubClassIDV400) == subid)
				return r;
		}
		return dbc::record::EMPTY();
	}

	void itemModifiedAppearanceDB::buildItemLookup()
	{
		for (uint32_t i = 0; i < nActualRecords; ++i)
		{
			uint32_t itemid = getRecord(i).getUInt(itemModifiedAppearanceDB::ItemId);
			ItemLookup32[itemid] = i;
		}
	}

	void fileDataDB::saveListFile(const char* szPath)
	{
		FILE* file = fopen(szPath, "wt");
		if (!file)
			return;

		for (uint32_t i = 0; i < getNumActualRecords(); ++i)
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

	dbc::record itemDisplayInfoMaterialResDB::getByItemDisplayInfoIDAndSlot(uint32_t itemDisplayId, uint32_t slot) const
	{
		for (uint32_t i = 0; i < nRecords; ++i)
		{
			dbc::record r = getRecord(i);
			if (r.getUInt(itemDisplayInfoMaterialResDB::ItemDisplayInfoID) == itemDisplayId &&
				r.getUInt(itemDisplayInfoMaterialResDB::TextureSlot) == slot)
				return r;
		}
		return dbc::record::EMPTY();
	}

	void itemDisplayInfoMaterialResDB::getTexturePath(uint32_t itemDisplayId, uint32_t slot, char* path, uint32_t size) const
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

	void buildItemCollections(ItemCollections& itemCollections, const itemDB* itemDb, const itemSparseDB* itemSparseDb)
	{
		uint32_t numRecords = itemSparseDb->getNumActualRecords();
		itemCollections.items.clear();
		itemCollections.items.reserve(numRecords);

		for (uint32_t i = 0; i < numRecords; ++i)
		{
			dbc::record rs = itemSparseDb->getRecord(i);
			if (!rs.isValid())
				continue;

			SItemRecord rec;
#if WOW_VER >= 70
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

#if WOW_VER >= 70
			Q_sprintf(rec.name, DEFAULT_SIZE * 2, "Item-%d", rec.id);
#else
			const char* str = rs.getString(itemSparseDb->getItemNameField());
			Q_strcpy(rec.name, DEFAULT_SIZE * 2, str);
#endif

			itemCollections.items.emplace_back(rec);
			itemCollections.itemLookup[rec.id] = (int32_t)(itemCollections.items.size() - 1);
		}

		/*
		//未翻译的
		for (uint32_t i=0; i<itemDb->getNumRecords(); ++i)
		{
		dbc::record r = itemDb->getRecord(i);
		int32_t id = r.getInt(itemDB::ID);
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
		itemLookup[rec.id] = (int32_t)(items.size() -1);
		}
		*/
	}
};
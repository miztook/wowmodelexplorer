#include "stdafx.h"
#include "wow_dbc70.h"
#include "mywow.h"

namespace WowLegion
{
	dbc::dbc(const wowEnvironment* env, const wowDatabase* database, const c8* filename, bool tmp /*= false*/)
		: DBType(WowDBType::Unknown)
		, IsSparse(false)
	{
		_buffer = nullptr;
		_recordStart = nullptr;
		_stringStart = nullptr;

		nRecords = 0;
		nFields = 0;
		RecordSize = 0;
		StringSize = 0;
		nActualRecords = 0;

		string512 path = filename;

		c8 name[MAX_PATH];
		getFileNameNoExtensionA(filename, name, MAX_PATH);
		TableStructure = database->getTableStructure(name);
		ASSERT(TableStructure);

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

		_buffer = new u8[file->getSize()];
		memcpy(_buffer, file->getBuffer(), file->getSize());

		const c8* magic = (const c8*)file->getBuffer();
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

		if (DBType == WowDBType::WDB5)		
			readWDB5(env, file, tmp);
		else if (DBType == WowDBType::WDB6)	
			readWDB6(env, file, tmp);
		else if (DBType == WowDBType::WDC1)	
			readWDC1(env, file, tmp);
		else if (DBType == WowDBType::WDC2)	
			readWDC2(env, file, tmp);
		else
			ASSERT(false);

		delete file;
	}

	dbc::~dbc()
	{
		delete _buffer;
	}

	void dbc::readWDB5(const wowEnvironment* env, IMemFile* file, bool tmp)
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
		IsSparse = HasDataOffsetBlock;

		std::vector<SField> Fields;
		Fields.resize(nFields);
		file->read(Fields.data(), sizeof(SField)* nFields);

		for (u32 i = 0; i < nFields; ++i)
		{
			FieldSizeMap[Fields[i].position] = Fields[i].size;
		}

		if (HasDataOffsetBlock)		//sparse
		{
			file->seek(StringSize);

			nActualRecords = 0;
			for (u32 i = 0; i < (header.max_id - header.min_id); ++i)
			{
				u32 offset;
				u16 length;

				file->read(&offset, sizeof(offset));
				file->read(&length, sizeof(length));

				if ((offset == 0) || (length == 0))
					continue;

				IDs.push_back(header.min_id + i);
				RecordOffsets.push_back(_buffer + offset);
				++nActualRecords;
			}
		}
		else
		{
			_recordStart = file->getPointer();
			_stringStart = _recordStart + RecordSize * nRecords;
			file->seek(RecordSize * nRecords + StringSize, true);
			if (HasIndex)
			{
				IDs.resize(nRecords);
				file->read(IDs.data(), sizeof(u32) * nRecords);
			}
			else
			{
				u32 indexPos = Fields[header.idindex].position;
				u32 indexSize = (32 - Fields[header.idindex].size) / 8;
				u32 indexMask = 0xFFFFFFFF;
				if (indexSize == 1)
					indexMask = 0x000000FF;
				else if (indexSize == 2)
					indexMask = 0x0000FFFF;
				else if (indexSize == 3)
					indexMask = 0x00FFFFFF;

				for (u32 i = 0; i < nRecords; ++i)
				{
					const u8* recordOffset = _recordStart + i * RecordSize;
					u32 v;
					memcpy(&v, recordOffset + indexPos, indexSize);
					IDs.push_back(v & indexMask);
				}
			}

			for (u32 i = 0; i < nRecords; ++i)
				RecordOffsets.push_back(_recordStart + i * RecordSize);
		}

		//relationship
		if (HasRelationshipData)
		{
			s32 relationshipDataSize = nRecords * sizeof(s32);
			file->seek(nRecords * sizeof(s32), true);
		}

		if (header.copydatasize > 0)
		{
			u32 nCopys = header.copydatasize / sizeof(SCopyTableEntry);
			std::vector<SCopyTableEntry> copyTables;
			copyTables.resize(nCopys);
			file->read(copyTables.data(), sizeof(SCopyTableEntry)* nCopys);

			std::map<int, u8*>  IDToOffsetMap;
			for (u32 i = 0; i < nActualRecords; ++i)
			{
				IDToOffsetMap[IDs[i]] = RecordOffsets[i];
			}

			for (u32 i = 0; i < nCopys; ++i)
			{
				IDs.push_back(copyTables[i].id_new_row);
				RecordOffsets.push_back(IDToOffsetMap[copyTables[i].id_copied_row]);
			}

			nActualRecords += nCopys;
		}

		ASSERT(file->getPos() == file->getSize());

		//build map
		for (u32 i = 0; i < nActualRecords; ++i)
		{
			RecordLookup32[IDs[i]] = i;
		}
		
		fs->writeLog(ELOG_RES, "successfully loaded db file: %s", file->getFileName());
	}

	void dbc::readWDB6(const wowEnvironment* env, IMemFile* file, bool tmp)
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
		IsSparse = HasDataOffsetBlock;

		std::vector<SField> Fields;
		Fields.resize(nFields);
		file->read(Fields.data(), sizeof(SField)* nFields);

		for (u32 i = 0; i < nFields; ++i)
		{
			FieldSizeMap[Fields[i].position] = Fields[i].size;
		}

		if (HasDataOffsetBlock)		//sparse
		{
			file->seek(StringSize);

			nActualRecords = 0;
			for (u32 i = 0; i < (header.max_id - header.min_id); ++i)
			{
				u32 offset;
				u16 length;

				file->read(&offset, sizeof(offset));
				file->read(&length, sizeof(length));

				if ((offset == 0) || (length == 0))
					continue;

				IDs.push_back(header.min_id + i);
				RecordOffsets.push_back(_buffer + offset);
				++nActualRecords;
			}
		}
		else
		{
			_recordStart = file->getPointer();
			_stringStart = _recordStart + RecordSize * nRecords;
			file->seek(RecordSize * nRecords + StringSize, true);
			if (HasIndex)
			{
				IDs.resize(nRecords);
				file->read(IDs.data(), sizeof(u32)* nRecords);
			}
			else
			{
				u32 indexPos = Fields[header.idindex].position;
				u32 indexSize = (32 - Fields[header.idindex].size) / 8;
				u32 indexMask = 0xFFFFFFFF;
				if (indexSize == 1)
					indexMask = 0x000000FF;
				else if (indexSize == 2)
					indexMask = 0x0000FFFF;
				else if (indexSize == 3)
					indexMask = 0x00FFFFFF;

				for (u32 i = 0; i < nRecords; ++i)
				{
					const u8* recordOffset = _recordStart + i * RecordSize;
					u32 v;
					memcpy(&v, recordOffset + indexPos, indexSize);
					IDs.push_back(v & indexMask);
				}
			}

			for (u32 i = 0; i < nRecords; ++i)
				RecordOffsets.push_back(_recordStart + i * RecordSize);
		}

		//relationship
		if (HasRelationshipData)
		{
			s32 relationshipDataSize = nRecords * sizeof(s32);
			file->seek(nRecords * sizeof(s32), true);
		}

		if (header.copydatasize > 0)
		{
			u32 nCopys = header.copydatasize / sizeof(SCopyTableEntry);
			std::vector<SCopyTableEntry> copyTables;
			copyTables.resize(nCopys);
			file->read(copyTables.data(), sizeof(SCopyTableEntry)* nCopys);

			std::map<int, u8*>  IDToOffsetMap;
			for (u32 i = 0; i < nActualRecords; ++i)
			{
				IDToOffsetMap[IDs[i]] = RecordOffsets[i];
			}

			for (u32 i = 0; i < nCopys; ++i)
			{
				IDs.push_back(copyTables[i].id_new_row);
				RecordOffsets.push_back(IDToOffsetMap[copyTables[i].id_copied_row]);
			}

			nActualRecords += nCopys;
		}

		//CommonData read
		if (header.nonzero_column_table_size > 0)
		{
			u32 nColumns;
			file->read(&nColumns, sizeof(u32));

			for (u32 c = 0; c < nColumns; c++)
			{
				// read number of records
				u32 nbrecords;
				file->read(&nbrecords, sizeof(nbrecords));

				// read type
				u8 type;
				file->read(&type, sizeof(type));

				if (nbrecords == 0)
					continue;

				SCommonColumn column;
				column.type = type;

				u32 size = 4;
				// starting from 7.3 version, data in common data is stored in 4 bytes, not dynamic size anymore
				if (false) //(!GAMEDIRECTORY.version().contains("7.3"))
				{
					if (type == 1)
						size = 2;
					else if (type == 2)
						size = 1;
				}

				for (u32 i = 0; i < nbrecords; i++)
				{
					u32 id;
					file->read(&id, sizeof(id));

					u32 val = 0;
					file->read(&val, size);

					column.recordmap[id] = val;
				}

				CommonDataMap[c] = column;
			}
		}

		ASSERT(file->getPos() == file->getSize());

		//build map
		for (u32 i = 0; i < nActualRecords; ++i)
		{
			RecordLookup32[IDs[i]] = i;
		}

		fs->writeLog(ELOG_RES, "successfully loaded db file: %s", file->getFileName());
	}

	void dbc::readWDC1(const wowEnvironment* env, IMemFile* file, bool tmp)
	{
		IFileSystem* fs = env->getFileSystem();

		dc1Header header;
		file->read(&header, sizeof(header));

		nRecords = header._nRecords;
		nFields = header._nFields;
		RecordSize = header._recordSize;
		StringSize = header._stringsize;
		nActualRecords = nRecords;

		HasDataOffsetBlock = (header.fileflags & WDB5_FLAG_DATAOFFSET) != 0;
		HasRelationshipData = (header.fileflags & WDB5_FLAG_UNKNOWN) != 0;
		HasIndex = (header.fileflags & WDB5_FLAG_INDEX) != 0;
		IsSparse = HasDataOffsetBlock;

		std::vector<SField> Fields;
		Fields.resize(nFields);
		file->read(Fields.data(), sizeof(SField)* nFields);

		for (u32 i = 0; i < nFields; ++i)
		{
			FieldSizeMap[Fields[i].position] = Fields[i].size;
		}

		u8* recordOffsetStart = file->getPointer();
		u32 stringTableOffset = file->getPos() + RecordSize * nRecords;
		if (HasDataOffsetBlock)
		{
			StringSize = 0;
			stringTableOffset = header.offset_map_offset + 6 * (header.max_id - header.min_id + 1);
		}

		file->seek(stringTableOffset);
		u32 idBlockOffset = stringTableOffset + StringSize;
		u32 copyBlockOffset = idBlockOffset;

		if (HasIndex)			//skip index
			copyBlockOffset += (nRecords * 4);

		u32 fieldStorageInfoOffset = copyBlockOffset + header.copydatasize;
		u32 palletBlockOffset = fieldStorageInfoOffset + header.field_storage_info_size;
		u32 commonBlockOffset = palletBlockOffset + header.pallet_data_size;
		u32 relationshipDataOffset = commonBlockOffset + header.common_data_size;

		//read storage info
		if (header.field_storage_info_size > 0)
		{
			file->seek(fieldStorageInfoOffset);
			u32 nFieldStorageInfo = header.field_storage_info_size / sizeof(SFieldStorageInfo);
			for (u32 i = 0; i < nFieldStorageInfo; ++i)
			{
				SFieldStorageInfo info;
				file->read(&info, sizeof(info));
				FieldStorageInfos.push_back(info);
			}
		}

		if (HasDataOffsetBlock)		//sparse
		{
			file->seek(header.offset_map_offset);

			nActualRecords = 0;
			for (u32 i = 0; i < (header.max_id - header.min_id); ++i)
			{
				u32 offset;
				u16 length;

				file->read(&offset, sizeof(offset));
				file->read(&length, sizeof(length));

				if ((offset == 0) || (length == 0))
					continue;

				IDs.push_back(header.min_id + i);
				RecordOffsets.push_back(_buffer + offset);
				++nActualRecords;
			}
		}
		else
		{
			if (HasIndex)
			{
				file->seek(idBlockOffset);
				IDs.resize(nRecords);
				file->read(IDs.data(), sizeof(u32)* nRecords);
			}
			else
			{
				const SFieldStorageInfo& info = FieldStorageInfos[header.idindex];
				for (u32 i = 0; i < nRecords; ++i)
				{
					const u8* recordOffset = recordOffsetStart + i * RecordSize;
					switch (info.storage_type)
					{
					case FIELD_COMPRESSION::NONE:
					{
						u32 size = info.field_offset_bits / 8;
						u8* val = new u8[size];
						memcpy(val, recordOffset, size);
						IDs.push_back((*reinterpret_cast<u32*>(val)));
						delete[] val;
					}
						break;
					case FIELD_COMPRESSION::BITPACKED:
					{
						u32 size = (info.field_size_bits + (info.field_offset_bits & 7) + 7) / 8;
						u32 offset = info.field_offset_bits / 8;
						u8* val = new u8[size];
						memcpy(val, recordOffset, size);
						u32 id = (*reinterpret_cast<u32*>(val));
						id = id & ((1ull << info.field_size_bits) - 1);
						IDs.push_back(id);
						delete[] val;
					}
						break;
					case FIELD_COMPRESSION::COMMON_DATA:
						ASSERT(false);
						break;
					case FIELD_COMPRESSION::BITPACKED_INDEXED:
						ASSERT(false);
						break;
					case FIELD_COMPRESSION::BITPACKED_INDEXED_ARRAY:
						ASSERT(false);
						break;
					default:
						ASSERT(false);
						break;;
					}
				}
			}

			for (u32 i = 0; i < nRecords; ++i)
				RecordOffsets.push_back(recordOffsetStart + i * RecordSize);
		}

		//copy table
		if (header.copydatasize > 0)
		{
			u32 nCopys = header.copydatasize / sizeof(SCopyTableEntry);
			std::vector<SCopyTableEntry> copyTables;
			copyTables.resize(nCopys);
			file->read(copyTables.data(), sizeof(SCopyTableEntry)* nCopys);

			std::map<int, u8*>  IDToOffsetMap;
			for (u32 i = 0; i < nActualRecords; ++i)
			{
				IDToOffsetMap[IDs[i]] = RecordOffsets[i];
			}

			for (u32 i = 0; i < nCopys; ++i)
			{
				IDs.push_back(copyTables[i].id_new_row);
				RecordOffsets.push_back(IDToOffsetMap[copyTables[i].id_copied_row]);
			}

			nActualRecords += nCopys;
		}

		//common data
		if (header.common_data_size > 0)
		{
			u32 fieldId = 0;
			for (auto info : FieldStorageInfos)
			{
				if (info.storage_type == FIELD_COMPRESSION::COMMON_DATA && info.additional_data_size != 0)
				{
					file->seek(commonBlockOffset);

					SCommonColumn column;
					column.type = 0;
					for(u32 i = 0; i < info.additional_data_size / 8; ++i)
					{
						u32 id;
						file->read(&id, sizeof(id));
						u32 val = 0;
						file->read(&val, 4);

						column.recordmap[id] = val;
					}

					CommonDataMap[fieldId] = column;
					commonBlockOffset += info.additional_data_size;
				}
				++fieldId;
			}
		}

		//pallet data
		if (header.pallet_data_size > 0)
		{
			u32 fieldId = 0;
			for (auto info : FieldStorageInfos)
			{
				if (info.storage_type == FIELD_COMPRESSION::BITPACKED_INDEXED &&
					info.storage_type == FIELD_COMPRESSION::BITPACKED_INDEXED_ARRAY &&
					info.additional_data_size != 0)
				{
					PalletBlockOffsets[fieldId] = palletBlockOffset;
					palletBlockOffset += info.additional_data_size;
				}
				++fieldId;
			}
		}

		//relationship data
		if (header.relationship_data_size > 0)
		{
			file->seek(relationshipDataOffset);
			u32 nbEntries;
			file->read(&nbEntries, sizeof(u32));

			file->seek(8, true);
			for (u32 i = 0; i < nbEntries; ++i)
			{
				u32 foreignKey;
				u32 recordIndex;
				file->read(&foreignKey, sizeof(u32));
				file->read(&recordIndex, sizeof(u32));
				RelationShipData[recordIndex] = foreignKey;
			}
		}

		//build map
		for (u32 i = 0; i < nActualRecords; ++i)
		{
			RecordLookup32[IDs[i]] = i;
		}

		fs->writeLog(ELOG_RES, "successfully loaded db file: %s", file->getFileName());
	}

	void dbc::readWDC2(const wowEnvironment* env, IMemFile* file, bool tmp)
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
		IsSparse = HasDataOffsetBlock;

		std::vector<SSectionHeader> sectionHeaders;
		sectionHeaders.resize(header.section_count);
		file->read(sectionHeaders.data(), sizeof(SSectionHeader)* header.section_count);

		std::vector<SField> Fields;
		Fields.resize(nFields);
		file->read(Fields.data(), sizeof(SField)* nFields);

		for (u32 i = 0; i < nFields; ++i)
		{
			FieldSizeMap[Fields[i].position] = Fields[i].size;
		}

		//read storage info
		if (header.field_storage_info_size > 0)
		{
			u32 nFieldStorageInfo = header.field_storage_info_size / sizeof(SFieldStorageInfo);
			for (u32 i = 0; i < nFieldStorageInfo; ++i)
			{
				SFieldStorageInfo info;
				file->read(&info, sizeof(info));
				FieldStorageInfos.push_back(info);
			}
		}

		u32 palletBlockOffset = file->getPos();
		u32 commonBlockOffset = palletBlockOffset + header.pallet_data_size;

		file->seek(sectionHeaders[0].file_offset);

		u8* recordOffsetStart = file->getPointer();

		u32 stringTableOffset = file->getPos() + RecordSize * nRecords;
		if (HasDataOffsetBlock)
		{
			StringSize = 0;
		}

		file->seek(stringTableOffset);
		_stringStart = file->getPointer();

		u32 idBlockOffset = stringTableOffset + StringSize;
		u32 copyBlockOffset = idBlockOffset + sectionHeaders[0].id_list_size;
		u32 relationshipDataOffset = copyBlockOffset + sectionHeaders[0].copy_table_size;

		if (HasDataOffsetBlock)
		{
			file->seek(sectionHeaders[0].offset_map_offset);

			nActualRecords = 0;
			for (u32 i = 0; i < (header.max_id - header.min_id); ++i)
			{
				u32 offset;
				u16 length;

				file->read(&offset, sizeof(offset));
				file->read(&length, sizeof(length));

				if ((offset == 0) || (length == 0))
					continue;

				IDs.push_back(header.min_id + i);
				RecordOffsets.push_back(_buffer + offset);
				++nActualRecords;
			}
		}
		else
		{
			if (HasIndex)
			{
				file->seek(idBlockOffset);
				IDs.resize(nRecords);
				file->read(IDs.data(), sizeof(u32)* nRecords);
			}
			else
			{
				const SFieldStorageInfo& info = FieldStorageInfos[header.idindex];
				for (u32 i = 0; i < nRecords; ++i)
				{
					const u8* recordOffset = recordOffsetStart + i * RecordSize;
					switch (info.storage_type)
					{
					case FIELD_COMPRESSION::NONE:
					{
						u32 size = info.field_offset_bits / 8;
						u8* val = new u8[size];
						memcpy(val, recordOffset, size);
						IDs.push_back((*reinterpret_cast<u32*>(val)));
						delete[] val;
					}
						break;
					case FIELD_COMPRESSION::BITPACKED:
					{
						u32 size = (info.field_size_bits + (info.field_offset_bits & 7) + 7) / 8;
						u32 offset = info.field_offset_bits / 8;
						u8* val = new u8[size];
						memcpy(val, recordOffset, size);
						u32 id = (*reinterpret_cast<u32*>(val));
						id = id & ((1ull << info.field_size_bits) - 1);
						IDs.push_back(id);
						delete[] val;
					}
						break;
					case FIELD_COMPRESSION::COMMON_DATA:
						ASSERT(false);
						break;
					case FIELD_COMPRESSION::BITPACKED_INDEXED:
						ASSERT(false);
						break;
					case FIELD_COMPRESSION::BITPACKED_INDEXED_ARRAY:
						ASSERT(false);
						break;
					default:
						ASSERT(false);
						break;;
					}
				}
			}

			for (u32 i = 0; i < nRecords; ++i)
				RecordOffsets.push_back(recordOffsetStart + i * RecordSize);
		}

		//copy table
		if (sectionHeaders[0].copy_table_size > 0)
		{
			u32 nCopys = sectionHeaders[0].copy_table_size / sizeof(SCopyTableEntry);
			std::vector<SCopyTableEntry> copyTables;
			copyTables.resize(nCopys);
			file->read(copyTables.data(), sizeof(SCopyTableEntry)* nCopys);

			std::map<int, u8*>  IDToOffsetMap;
			for (u32 i = 0; i < nActualRecords; ++i)
			{
				IDToOffsetMap[IDs[i]] = RecordOffsets[i];
			}

			for (u32 i = 0; i < nCopys; ++i)
			{
				IDs.push_back(copyTables[i].id_new_row);
				RecordOffsets.push_back(IDToOffsetMap[copyTables[i].id_copied_row]);
			}

			nActualRecords += nCopys;
		}

		//common data
		if (header.common_data_size > 0)
		{
			u32 fieldId = 0;
			for (auto info : FieldStorageInfos)
			{
				if (info.storage_type == FIELD_COMPRESSION::COMMON_DATA && info.additional_data_size != 0)
				{
					file->seek(commonBlockOffset);

					SCommonColumn column;
					column.type = 0;
					for (u32 i = 0; i < info.additional_data_size / 8; ++i)
					{
						u32 id;
						file->read(&id, sizeof(id));
						u32 val = 0;
						file->read(&val, 4);

						column.recordmap[id] = val;
					}

					CommonDataMap[fieldId] = column;
					commonBlockOffset += info.additional_data_size;
				}
				++fieldId;
			}
		}

		//pallet data
		if (header.pallet_data_size > 0)
		{
			u32 fieldId = 0;
			for (auto info : FieldStorageInfos)
			{
				if (info.storage_type == FIELD_COMPRESSION::BITPACKED_INDEXED &&
					info.storage_type == FIELD_COMPRESSION::BITPACKED_INDEXED_ARRAY &&
					info.additional_data_size != 0)
				{
					PalletBlockOffsets[fieldId] = palletBlockOffset;
					palletBlockOffset += info.additional_data_size;
				}
				++fieldId;
			}
		}

		//relationship data
		if (sectionHeaders[0].relationship_data_size > 0)
		{
			file->seek(relationshipDataOffset);
			u32 nbEntries;
			file->read(&nbEntries, sizeof(u32));

			file->seek(8, true);
			for (u32 i = 0; i < nbEntries; ++i)
			{
				u32 foreignKey;
				u32 recordIndex;
				file->read(&foreignKey, sizeof(u32));
				file->read(&recordIndex, sizeof(u32));
				RelationShipData[recordIndex] = foreignKey;
			}
		}

		//build map
		for (u32 i = 0; i < nActualRecords; ++i)
		{
			RecordLookup32[IDs[i]] = i;
		}

		fs->writeLog(ELOG_RES, "successfully loaded db file: %s", file->getFileName());
	}

	bool dbc::readFieldValue(u32 recordIndex, u32 fieldIndex, u32 arrayIndex, u32 arraySize, u32& result) const
	{
		return true;
	}

	u32 dbc::readBitpackedValue(const SFieldStorageInfo& info, const u8* recordOffset) const
	{
		u32 size = (info.field_size_bits + (info.field_offset_bits & 7) + 7) / 8;
		u32 offset = info.field_offset_bits / 8;
		u8* v = new u8[size];
		memcpy(v, recordOffset + offset, size);
		u32 result = (*reinterpret_cast<u32*>(v));
		delete v;

		result = result & ((1ull << info.field_size_bits) - 1);
		return result;
	}

	u32 dbc::readBitpackedValue2(const SFieldStorageInfo& info, const u8* recordOffset) const
	{
		u32 size = (info.field_size_bits + (info.field_offset_bits & 7) + 7) / 8;
		u32 offset = info.field_offset_bits / 8;
		u8* v = new u8[size];
		memcpy(v, recordOffset + offset, size);
		u32 result = (*reinterpret_cast<u32*>(v));
		delete v;

		result = result >> (info.field_offset_bits & 7);
		result = result & ((1ull << info.field_size_bits) - 1);
		return result;
	}
}
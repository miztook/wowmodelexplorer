#include "stdafx.h"
#include "wow_dbc70.h"
#include "mywow.h"

namespace WowLegion
{
	dbc::dbc(const wowEnvironment* env, const wowDatabase* database, const char* filename, bool tmp /*= false*/)
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

		char name[MAX_PATH];
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

		_buffer = new uint8_t[file->getSize()];
		memcpy(_buffer, file->getBuffer(), file->getSize());

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

		for (uint32_t i = 0; i < nFields; ++i)
		{
			FieldSizeMap[Fields[i].position] = Fields[i].size;
		}

		if (HasDataOffsetBlock)		//sparse
		{
			file->seek(StringSize);

			nActualRecords = 0;
			for (uint32_t i = 0; i < (header.max_id - header.min_id); ++i)
			{
				uint32_t offset;
				uint16_t length;

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
				file->read(IDs.data(), sizeof(uint32_t) * nRecords);
			}
			else
			{
				uint32_t indexPos = Fields[header.idindex].position;
				uint32_t indexSize = (32 - Fields[header.idindex].size) / 8;
				uint32_t indexMask = 0xFFFFFFFF;
				if (indexSize == 1)
					indexMask = 0x000000FF;
				else if (indexSize == 2)
					indexMask = 0x0000FFFF;
				else if (indexSize == 3)
					indexMask = 0x00FFFFFF;

				for (uint32_t i = 0; i < nRecords; ++i)
				{
					const uint8_t* recordOffset = _recordStart + i * RecordSize;
					uint32_t v;
					memcpy(&v, recordOffset + indexPos, indexSize);
					IDs.push_back(v & indexMask);
				}
			}

			for (uint32_t i = 0; i < nRecords; ++i)
				RecordOffsets.push_back(_recordStart + i * RecordSize);
		}

		//relationship
		if (HasRelationshipData)
		{
			int32_t relationshipDataSize = nRecords * sizeof(int32_t);
			file->seek(nRecords * sizeof(int32_t), true);
		}

		if (header.copydatasize > 0)
		{
			uint32_t nCopys = header.copydatasize / sizeof(SCopyTableEntry);
			std::vector<SCopyTableEntry> copyTables;
			copyTables.resize(nCopys);
			file->read(copyTables.data(), sizeof(SCopyTableEntry)* nCopys);

			std::map<int, uint8_t*>  IDToOffsetMap;
			for (uint32_t i = 0; i < nActualRecords; ++i)
			{
				IDToOffsetMap[IDs[i]] = RecordOffsets[i];
			}

			for (uint32_t i = 0; i < nCopys; ++i)
			{
				IDs.push_back(copyTables[i].id_new_row);
				RecordOffsets.push_back(IDToOffsetMap[copyTables[i].id_copied_row]);
			}

			nActualRecords += nCopys;
		}

		ASSERT(file->getPos() == file->getSize());

		//build map
		for (uint32_t i = 0; i < nActualRecords; ++i)
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

		for (uint32_t i = 0; i < nFields; ++i)
		{
			FieldSizeMap[Fields[i].position] = Fields[i].size;
		}

		if (HasDataOffsetBlock)		//sparse
		{
			file->seek(StringSize);

			nActualRecords = 0;
			for (uint32_t i = 0; i < (header.max_id - header.min_id); ++i)
			{
				uint32_t offset;
				uint16_t length;

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
				file->read(IDs.data(), sizeof(uint32_t)* nRecords);
			}
			else
			{
				uint32_t indexPos = Fields[header.idindex].position;
				uint32_t indexSize = (32 - Fields[header.idindex].size) / 8;
				uint32_t indexMask = 0xFFFFFFFF;
				if (indexSize == 1)
					indexMask = 0x000000FF;
				else if (indexSize == 2)
					indexMask = 0x0000FFFF;
				else if (indexSize == 3)
					indexMask = 0x00FFFFFF;

				for (uint32_t i = 0; i < nRecords; ++i)
				{
					const uint8_t* recordOffset = _recordStart + i * RecordSize;
					uint32_t v;
					memcpy(&v, recordOffset + indexPos, indexSize);
					IDs.push_back(v & indexMask);
				}
			}

			for (uint32_t i = 0; i < nRecords; ++i)
				RecordOffsets.push_back(_recordStart + i * RecordSize);
		}

		//relationship
		if (HasRelationshipData)
		{
			int32_t relationshipDataSize = nRecords * sizeof(int32_t);
			file->seek(nRecords * sizeof(int32_t), true);
		}

		if (header.copydatasize > 0)
		{
			uint32_t nCopys = header.copydatasize / sizeof(SCopyTableEntry);
			std::vector<SCopyTableEntry> copyTables;
			copyTables.resize(nCopys);
			file->read(copyTables.data(), sizeof(SCopyTableEntry)* nCopys);

			std::map<int, uint8_t*>  IDToOffsetMap;
			for (uint32_t i = 0; i < nActualRecords; ++i)
			{
				IDToOffsetMap[IDs[i]] = RecordOffsets[i];
			}

			for (uint32_t i = 0; i < nCopys; ++i)
			{
				IDs.push_back(copyTables[i].id_new_row);
				RecordOffsets.push_back(IDToOffsetMap[copyTables[i].id_copied_row]);
			}

			nActualRecords += nCopys;
		}

		//CommonData read
		if (header.nonzero_column_table_size > 0)
		{
			uint32_t nColumns;
			file->read(&nColumns, sizeof(uint32_t));

			for (uint32_t c = 0; c < nColumns; c++)
			{
				// read number of records
				uint32_t nbrecords;
				file->read(&nbrecords, sizeof(nbrecords));

				// read type
				uint8_t type;
				file->read(&type, sizeof(type));

				if (nbrecords == 0)
					continue;

				SCommonColumn column;
				column.type = type;

				uint32_t size = 4;
				// starting from 7.3 version, data in common data is stored in 4 bytes, not dynamic size anymore
				if (false) //(!GAMEDIRECTORY.version().contains("7.3"))
				{
					if (type == 1)
						size = 2;
					else if (type == 2)
						size = 1;
				}

				for (uint32_t i = 0; i < nbrecords; i++)
				{
					uint32_t id;
					file->read(&id, sizeof(id));

					uint32_t val = 0;
					file->read(&val, size);

					column.recordmap[id] = val;
				}

				CommonDataMap[c] = column;
			}
		}

		ASSERT(file->getPos() == file->getSize());

		//build map
		for (uint32_t i = 0; i < nActualRecords; ++i)
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

		for (uint32_t i = 0; i < nFields; ++i)
		{
			FieldSizeMap[Fields[i].position] = Fields[i].size;
		}

		uint8_t* recordOffsetStart = file->getPointer();
		uint32_t stringTableOffset = file->getPos() + RecordSize * nRecords;
		if (HasDataOffsetBlock)
		{
			StringSize = 0;
			stringTableOffset = header.offset_map_offset + 6 * (header.max_id - header.min_id + 1);
		}

		file->seek(stringTableOffset);
		uint32_t idBlockOffset = stringTableOffset + StringSize;
		uint32_t copyBlockOffset = idBlockOffset;

		if (HasIndex)			//skip index
			copyBlockOffset += (nRecords * 4);

		uint32_t fieldStorageInfoOffset = copyBlockOffset + header.copydatasize;
		uint32_t palletBlockOffset = fieldStorageInfoOffset + header.field_storage_info_size;
		uint32_t commonBlockOffset = palletBlockOffset + header.pallet_data_size;
		uint32_t relationshipDataOffset = commonBlockOffset + header.common_data_size;

		//read storage info
		if (header.field_storage_info_size > 0)
		{
			file->seek(fieldStorageInfoOffset);
			uint32_t nFieldStorageInfo = header.field_storage_info_size / sizeof(SFieldStorageInfo);
			for (uint32_t i = 0; i < nFieldStorageInfo; ++i)
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
			for (uint32_t i = 0; i < (header.max_id - header.min_id); ++i)
			{
				uint32_t offset;
				uint16_t length;

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
				file->read(IDs.data(), sizeof(uint32_t)* nRecords);
			}
			else
			{
				const SFieldStorageInfo& info = FieldStorageInfos[header.idindex];
				for (uint32_t i = 0; i < nRecords; ++i)
				{
					const uint8_t* recordOffset = recordOffsetStart + i * RecordSize;
					switch (info.storage_type)
					{
					case FIELD_COMPRESSION::NONE:
					{
						uint32_t size = info.field_offset_bits / 8;
						uint8_t* val = new uint8_t[size];
						memcpy(val, recordOffset, size);
						IDs.push_back((*reinterpret_cast<uint32_t*>(val)));
						delete[] val;
					}
						break;
					case FIELD_COMPRESSION::BITPACKED:
					{
						uint32_t size = (info.field_size_bits + (info.field_offset_bits & 7) + 7) / 8;
						uint32_t offset = info.field_offset_bits / 8;
						uint8_t* val = new uint8_t[size];
						memcpy(val, recordOffset, size);
						uint32_t id = (*reinterpret_cast<uint32_t*>(val));
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

			for (uint32_t i = 0; i < nRecords; ++i)
				RecordOffsets.push_back(recordOffsetStart + i * RecordSize);
		}

		//copy table
		if (header.copydatasize > 0)
		{
			uint32_t nCopys = header.copydatasize / sizeof(SCopyTableEntry);
			std::vector<SCopyTableEntry> copyTables;
			copyTables.resize(nCopys);
			file->read(copyTables.data(), sizeof(SCopyTableEntry)* nCopys);

			std::map<int, uint8_t*>  IDToOffsetMap;
			for (uint32_t i = 0; i < nActualRecords; ++i)
			{
				IDToOffsetMap[IDs[i]] = RecordOffsets[i];
			}

			for (uint32_t i = 0; i < nCopys; ++i)
			{
				IDs.push_back(copyTables[i].id_new_row);
				RecordOffsets.push_back(IDToOffsetMap[copyTables[i].id_copied_row]);
			}

			nActualRecords += nCopys;
		}

		//common data
		if (header.common_data_size > 0)
		{
			uint32_t fieldId = 0;
			for (auto info : FieldStorageInfos)
			{
				if (info.storage_type == FIELD_COMPRESSION::COMMON_DATA && info.additional_data_size != 0)
				{
					file->seek(commonBlockOffset);

					SCommonColumn column;
					column.type = 0;
					for(uint32_t i = 0; i < info.additional_data_size / 8; ++i)
					{
						uint32_t id;
						file->read(&id, sizeof(id));
						uint32_t val = 0;
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
			uint32_t fieldId = 0;
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
			uint32_t nbEntries;
			file->read(&nbEntries, sizeof(uint32_t));

			file->seek(8, true);
			for (uint32_t i = 0; i < nbEntries; ++i)
			{
				uint32_t foreignKey;
				uint32_t recordIndex;
				file->read(&foreignKey, sizeof(uint32_t));
				file->read(&recordIndex, sizeof(uint32_t));
				RelationShipData[recordIndex] = foreignKey;
			}
		}

		//build map
		for (uint32_t i = 0; i < nActualRecords; ++i)
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

		for (uint32_t i = 0; i < nFields; ++i)
		{
			FieldSizeMap[Fields[i].position] = Fields[i].size;
		}

		//read storage info
		if (header.field_storage_info_size > 0)
		{
			uint32_t nFieldStorageInfo = header.field_storage_info_size / sizeof(SFieldStorageInfo);
			for (uint32_t i = 0; i < nFieldStorageInfo; ++i)
			{
				SFieldStorageInfo info;
				file->read(&info, sizeof(info));
				FieldStorageInfos.push_back(info);
			}
		}

		uint32_t palletBlockOffset = file->getPos();
		uint32_t commonBlockOffset = palletBlockOffset + header.pallet_data_size;

		file->seek(sectionHeaders[0].file_offset);

		uint8_t* recordOffsetStart = file->getPointer();

		uint32_t stringTableOffset = file->getPos() + RecordSize * nRecords;
		if (HasDataOffsetBlock)
		{
			StringSize = 0;
		}

		file->seek(stringTableOffset);
		_stringStart = file->getPointer();

		uint32_t idBlockOffset = stringTableOffset + StringSize;
		uint32_t copyBlockOffset = idBlockOffset + sectionHeaders[0].id_list_size;
		uint32_t relationshipDataOffset = copyBlockOffset + sectionHeaders[0].copy_table_size;

		if (HasDataOffsetBlock)
		{
			file->seek(sectionHeaders[0].offset_map_offset);

			nActualRecords = 0;
			for (uint32_t i = 0; i < (header.max_id - header.min_id); ++i)
			{
				uint32_t offset;
				uint16_t length;

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
				file->read(IDs.data(), sizeof(uint32_t)* nRecords);
			}
			else
			{
				const SFieldStorageInfo& info = FieldStorageInfos[header.idindex];
				for (uint32_t i = 0; i < nRecords; ++i)
				{
					const uint8_t* recordOffset = recordOffsetStart + i * RecordSize;
					switch (info.storage_type)
					{
					case FIELD_COMPRESSION::NONE:
					{
						uint32_t size = info.field_offset_bits / 8;
						uint8_t* val = new uint8_t[size];
						memcpy(val, recordOffset, size);
						IDs.push_back((*reinterpret_cast<uint32_t*>(val)));
						delete[] val;
					}
						break;
					case FIELD_COMPRESSION::BITPACKED:
					{
						uint32_t size = (info.field_size_bits + (info.field_offset_bits & 7) + 7) / 8;
						uint32_t offset = info.field_offset_bits / 8;
						uint8_t* val = new uint8_t[size];
						memcpy(val, recordOffset, size);
						uint32_t id = (*reinterpret_cast<uint32_t*>(val));
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

			for (uint32_t i = 0; i < nRecords; ++i)
				RecordOffsets.push_back(recordOffsetStart + i * RecordSize);
		}

		//copy table
		if (sectionHeaders[0].copy_table_size > 0)
		{
			uint32_t nCopys = sectionHeaders[0].copy_table_size / sizeof(SCopyTableEntry);
			std::vector<SCopyTableEntry> copyTables;
			copyTables.resize(nCopys);
			file->read(copyTables.data(), sizeof(SCopyTableEntry)* nCopys);

			std::map<int, uint8_t*>  IDToOffsetMap;
			for (uint32_t i = 0; i < nActualRecords; ++i)
			{
				IDToOffsetMap[IDs[i]] = RecordOffsets[i];
			}

			for (uint32_t i = 0; i < nCopys; ++i)
			{
				IDs.push_back(copyTables[i].id_new_row);
				RecordOffsets.push_back(IDToOffsetMap[copyTables[i].id_copied_row]);
			}

			nActualRecords += nCopys;
		}

		//common data
		if (header.common_data_size > 0)
		{
			uint32_t fieldId = 0;
			for (auto info : FieldStorageInfos)
			{
				if (info.storage_type == FIELD_COMPRESSION::COMMON_DATA && info.additional_data_size != 0)
				{
					file->seek(commonBlockOffset);

					SCommonColumn column;
					column.type = 0;
					for (uint32_t i = 0; i < info.additional_data_size / 8; ++i)
					{
						uint32_t id;
						file->read(&id, sizeof(id));
						uint32_t val = 0;
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
			uint32_t fieldId = 0;
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
			uint32_t nbEntries;
			file->read(&nbEntries, sizeof(uint32_t));

			file->seek(8, true);
			for (uint32_t i = 0; i < nbEntries; ++i)
			{
				uint32_t foreignKey;
				uint32_t recordIndex;
				file->read(&foreignKey, sizeof(uint32_t));
				file->read(&recordIndex, sizeof(uint32_t));
				RelationShipData[recordIndex] = foreignKey;
			}
		}

		//build map
		for (uint32_t i = 0; i < nActualRecords; ++i)
		{
			RecordLookup32[IDs[i]] = i;
		}

		fs->writeLog(ELOG_RES, "successfully loaded db file: %s", file->getFileName());
	}

	bool dbc::readFieldValue(uint32_t recordIndex, uint32_t fieldIndex, uint32_t arrayIndex, uint32_t arraySize, uint32_t& result) const
	{
		return true;
	}

	uint32_t dbc::readBitpackedValue(const SFieldStorageInfo& info, const uint8_t* recordOffset) const
	{
		uint32_t size = (info.field_size_bits + (info.field_offset_bits & 7) + 7) / 8;
		uint32_t offset = info.field_offset_bits / 8;
		uint8_t* v = new uint8_t[size];
		memcpy(v, recordOffset + offset, size);
		uint32_t result = (*reinterpret_cast<uint32_t*>(v));
		delete v;

		result = result & ((1ull << info.field_size_bits) - 1);
		return result;
	}

	uint32_t dbc::readBitpackedValue2(const SFieldStorageInfo& info, const uint8_t* recordOffset) const
	{
		uint32_t size = (info.field_size_bits + (info.field_offset_bits & 7) + 7) / 8;
		uint32_t offset = info.field_offset_bits / 8;
		uint8_t* v = new uint8_t[size];
		memcpy(v, recordOffset + offset, size);
		uint32_t result = (*reinterpret_cast<uint32_t*>(v));
		delete v;

		result = result >> (info.field_offset_bits & 7);
		result = result & ((1ull << info.field_size_bits) - 1);
		return result;
	}
}
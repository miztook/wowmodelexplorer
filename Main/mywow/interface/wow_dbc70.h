#pragma once

#include "base.h"
#include "fixstring.h"
#include "wow_enums.h"
#include <map>
#include <vector>

class wowEnvironment;
class IMemFile;

namespace WowLegion
{
	class wowDatabase;
	class CTableStructure;
}

namespace WowLegion
{
#	pragma pack (4)

#define WDB5_FLAG_DATAOFFSET 1
#define WDB5_FLAG_UNKNOWN 2
#define WDB5_FLAG_INDEX 4

	struct db5Header
	{
		/*0x00*/    char      _magic[4];	// "WDB5"
		/*0x04*/    uint32_t  _nRecords;        //
		/*0x08*/    uint32_t  _nFields;      	//
		/*0x0C*/    uint32_t  _recordSize;      //
		/*0x10*/    uint32_t  _stringsize;

		uint32_t tablehash;
		uint32_t layouthash;
		uint32_t min_id;
		uint32_t max_id;
		int32_t localecode;
		uint32_t copydatasize;
		uint16_t fileflags;
		uint16_t idindex;
	};

	struct db6Header
	{
		/*0x00*/    char      _magic[4];	// "WDB6"
		/*0x04*/    uint32_t  _nRecords;        //
		/*0x08*/    uint32_t  _nFields;      	//
		/*0x0C*/    uint32_t  _recordSize;      //
		/*0x10*/    uint32_t  _stringsize;

		uint32_t tablehash;
		uint32_t layouthash;
		uint32_t min_id;
		uint32_t max_id;
		int32_t localecode;
		uint32_t copydatasize;
		uint16_t fileflags;
		uint16_t idindex;
		uint32_t	total_field_count;
		uint32_t nonzero_column_table_size;
	};

	struct dc1Header
	{
		/*0x00*/    char      _magic[4];	// "WDC1"
		/*0x04*/    uint32_t  _nRecords;        //
		/*0x08*/    uint32_t  _nFields;      	//
		/*0x0C*/    uint32_t  _recordSize;      //
		/*0x10*/    uint32_t  _stringsize;

		uint32_t tablehash;
		uint32_t layouthash;
		uint32_t min_id;
		uint32_t max_id;
		int32_t localecode;
		uint32_t copydatasize;
		uint16_t fileflags;
		uint16_t idindex;
		uint32_t total_field_count;      // in WDC1 this value seems to always be the same as the 'field_count' value
		uint32_t bitpacked_data_offset;  // relative position in record where bitpacked data begins; not important for parsing the file
		uint32_t lookup_column_count;
		uint32_t offset_map_offset;      // Offset to array of struct {uint32 offset; uint16_t size;}[max_id - min_id + 1];
		uint32_t id_list_size;           // List of ids present in the DB file
		uint32_t field_storage_info_size;
		uint32_t common_data_size;
		uint32_t pallet_data_size;
		uint32_t relationship_data_size;
	};

	struct dc2Header
	{
		/*0x00*/    char      _magic[4];	// "WDC2"
		/*0x04*/    uint32_t  _nRecords;        //
		/*0x08*/    uint32_t  _nFields;      	//
		/*0x0C*/    uint32_t  _recordSize;      //
		/*0x10*/    uint32_t  _stringsize;

		uint32_t tablehash;
		uint32_t layouthash;
		uint32_t min_id;
		uint32_t max_id;
		int32_t localecode;
		uint16_t fileflags;
		uint16_t idindex;			// this is the index of the field containing ID values; this is ignored if flags & 0x04 != 0
		uint32_t total_field_count;      // in WDC1 this value seems to always be the same as the 'field_count' value
		uint32_t bitpacked_data_offset;  // relative position in record where bitpacked data begins; not important for parsing the file
		uint32_t lookup_column_count;
		uint32_t field_storage_info_size;
		uint32_t common_data_size;
		uint32_t pallet_data_size;
		uint32_t section_count;          // new to WDC2, this is number of sections of data (records + copy table + id list + relationship map = a section)
	};

#	pragma pack ()

	class dbc
	{
	private:
		DISALLOW_COPY_AND_ASSIGN(dbc);

	public:
		dbc(const wowEnvironment* env, const wowDatabase* database, const char* filename, bool tmp = false);
		virtual ~dbc();

	protected:
		void readWDB5(const wowEnvironment* env, IMemFile* file, bool tmp);
		void readWDB6(const wowEnvironment* env, IMemFile* file, bool tmp);
		void readWDC1(const wowEnvironment* env, IMemFile* file, bool tmp);
		void readWDC2(const wowEnvironment* env, IMemFile* file, bool tmp);

	public:
		class record
		{
		public:
		};

	protected:
		WowDBType DBType;

		const CTableStructure*  TableStructure;

		typedef std::map<uint32_t, uint32_t, std::less<uint32_t>, qzone_allocator<std::pair<uint32_t, uint32_t>>> T_RecordLookup32;

		T_RecordLookup32		RecordLookup32;

		uint8_t*		_buffer;
		uint8_t*		_recordStart;
		uint8_t*		_stringStart;

	protected:
		uint32_t		nFields;
		uint32_t		nRecords;
		uint32_t		RecordSize;
		uint32_t		StringSize;
		uint32_t		nActualRecords;
		bool	IsSparse;

	protected:	//WDB5
		bool	HasDataOffsetBlock;
		bool	HasRelationshipData;
		bool	HasIndex;

		struct SField
		{
			uint16_t  size;	// size in bits as calculated by: byteSize = (32 - size) / 8
			uint16_t position;  //position of the field within the record
		};

		struct SCopyTableEntry
		{
			uint32_t id_new_row;
			uint32_t id_copied_row;
		};

		std::vector<uint32_t> IDs;		//if (flags & 0x04 != 0) [header.record_count]
		std::vector<uint8_t*>  RecordOffsets;
		std::map<int, int>  FieldSizeMap;
	

	protected:	//WDB6

		struct SCommonColumn
		{
			std::map<uint32_t, uint32_t> recordmap;
			uint8_t type;
		};

		std::map<uint32_t, SCommonColumn>   CommonDataMap;

	protected:		//WDC1
		struct SFieldStorageInfo
		{
			uint16_t field_offset_bits;
			uint16_t field_size_bits;
			uint32_t additional_data_size;
			uint32_t storage_type;
			uint32_t val1;
			uint32_t val2;
			uint32_t val3;
		};

		enum FIELD_COMPRESSION : int
		{
			NONE = 0,
			BITPACKED,
			COMMON_DATA,
			BITPACKED_INDEXED,
			BITPACKED_INDEXED_ARRAY,
		};

		std::vector<SFieldStorageInfo> FieldStorageInfos;

		std::map<uint32_t, uint32_t> PalletBlockOffsets;
		std::map<uint32_t, uint32_t> RelationShipData;

	protected:		//WDC2
		struct SSectionHeader
		{
			uint32_t wdc2_unk_header1;       // always 0 in Battle (8.0.1.26231) and unnamed in client binary
			uint32_t wdc2_unk_header2;       // always 0 in Battle (8.0.1.26231) and unnamed in client binary
			uint32_t file_offset;            // absolute position to the beginning of the section
			uint32_t record_count;           // 'record_count' for the section
			uint32_t string_table_size;      // 'string_table_size' for the section
			uint32_t copy_table_size;
			uint32_t offset_map_offset;      // Offset to array of struct {uint32_t offset; uint16_t size;}[max_id - min_id + 1];
			uint32_t id_list_size;           // List of ids present in the DB file
			uint32_t relationship_data_size;
		};

		bool readFieldValue(uint32_t recordIndex, uint32_t fieldIndex, uint32_t arrayIndex, uint32_t arraySize, uint32_t& result) const;
		uint32_t readBitpackedValue(const SFieldStorageInfo& info, const uint8_t* recordOffset) const;
		uint32_t readBitpackedValue2(const SFieldStorageInfo& info, const uint8_t* recordOffset) const;
	};

	class animDB : public dbc
	{
	public:
		animDB(const wowEnvironment* env, const wowDatabase* database) : dbc(env, database, "DBFilesClient\\AnimationData.dbc") {}

	public:
		static const uint32_t Name = 0;		// string
	};

	class charClassesDB : public dbc
	{
	public:
		charClassesDB(const wowEnvironment* env, const wowDatabase* database) : dbc(env, database, "DBFilesClient\\ChrClasses.dbc") {}

		static const uint32_t NameV400 = 3;		//string
		static const uint32_t ShortName = 4;
	};
};
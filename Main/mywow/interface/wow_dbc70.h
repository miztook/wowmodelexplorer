#pragma once

#include "base.h"
#include "fixstring.h"
#include <map>
#include <vector>

class wowEnvironment;
class IMemFile;

namespace WowLegion
{
#	pragma pack (4)

#define WDB5_FLAG_DATAOFFSET 1
#define WDB5_FLAG_UNKNOWN 2
#define WDB5_FLAG_INDEX 4

	struct db5Header
	{
		/*0x00*/    char      _magic[4];	// "WDB5"
		/*0x04*/    u32  _nRecords;        //
		/*0x08*/    u32  _nFields;      	//
		/*0x0C*/    u32  _recordSize;      //
		/*0x10*/    u32  _stringsize;

		u32 tablehash;
		u32 layouthash;
		u32 min_id;
		u32 max_id;
		s32 localecode;
		u32 copydatasize;
		u16 fileflags;
		u16 idindex;
	};

	struct db6Header
	{
		/*0x00*/    char      _magic[4];	// "WDB6"
		/*0x04*/    u32  _nRecords;        //
		/*0x08*/    u32  _nFields;      	//
		/*0x0C*/    u32  _recordSize;      //
		/*0x10*/    u32  _stringsize;

		u32 tablehash;
		u32 layouthash;
		u32 min_id;
		u32 max_id;
		s32 localecode;
		u32 copydatasize;
		u16 fileflags;
		u16 idindex;
		u32	total_field_count;
		u32 nonzero_column_table_size;
	};

	struct dc1Header
	{
		/*0x00*/    char      _magic[4];	// "WDC1"
		/*0x04*/    u32  _nRecords;        //
		/*0x08*/    u32  _nFields;      	//
		/*0x0C*/    u32  _recordSize;      //
		/*0x10*/    u32  _stringsize;

		u32 tablehash;
		u32 layouthash;
		u32 min_id;
		u32 max_id;
		s32 localecode;
		u32 copydatasize;
		u16 fileflags;
		u16 idindex;
		u32 total_field_count;      // in WDC1 this value seems to always be the same as the 'field_count' value
		u32 bitpacked_data_offset;  // relative position in record where bitpacked data begins; not important for parsing the file
		u32 lookup_column_count;
		u32 offset_map_offset;      // Offset to array of struct {uint32 offset; uint16_t size;}[max_id - min_id + 1];
		u32 id_list_size;           // List of ids present in the DB file
		u32 field_storage_info_size;
		u32 common_data_size;
		u32 pallet_data_size;
		u32 relationship_data_size;
	};

	struct dc2Header
	{
		/*0x00*/    char      _magic[4];	// "WDC2"
		/*0x04*/    u32  _nRecords;        //
		/*0x08*/    u32  _nFields;      	//
		/*0x0C*/    u32  _recordSize;      //
		/*0x10*/    u32  _stringsize;

		u32 tablehash;
		u32 layouthash;
		u32 min_id;
		u32 max_id;
		s32 localecode;
		u16 fileflags;
		u16 idindex;			// this is the index of the field containing ID values; this is ignored if flags & 0x04 != 0
		u32 total_field_count;      // in WDC1 this value seems to always be the same as the 'field_count' value
		u32 bitpacked_data_offset;  // relative position in record where bitpacked data begins; not important for parsing the file
		u32 lookup_column_count;
		u32 field_storage_info_size;
		u32 common_data_size;
		u32 pallet_data_size;
		u32 section_count;          // new to WDC2, this is number of sections of data (records + copy table + id list + relationship map = a section)
	};

#	pragma pack ()

	class dbc70
	{
	private:
		DISALLOW_COPY_AND_ASSIGN(dbc70);

	public:
		dbc70(wowEnvironment* env, const c8* filename, bool tmp = false);
		virtual ~dbc70();

	protected:
		void readWDB5(wowEnvironment* env, IMemFile* file, bool tmp);
		void readWDB6(wowEnvironment* env, IMemFile* file, bool tmp);
		void readWDC1(wowEnvironment* env, IMemFile* file, bool tmp);
		void readWDC2(wowEnvironment* env, IMemFile* file, bool tmp);

	public:
		class record
		{
		public:
		};

	protected:	//WDB5
		bool	HasDataOffsetBlock;
		bool	HasRelationshipData;
		bool	HasIndex;
		bool	HasCopyData;

		struct SField
		{
			u16  size;	// size in bits as calculated by: byteSize = (32 - size) / 8
			u16 position;  //position of the field within the record
		};

		struct SOffsetMapEntry
		{
			u32 offset;
			u16 length;
		};

		struct SCopyTableEntry
		{
			u32 id_new_row;
			u32 id_copied_row;
		};

		SField* Fields;		//[header.field_count]
		std::vector<SOffsetMapEntry>	OffsetMaps;  //if (flags & 0x01 != 0) [header.max_id - header.min_id + 1];
		std::vector<u32> IDs;		//if (flags & 0x04 != 0) [header.record_count]
		//SCopyTableEntry* CopyTables;

	protected:	//WDB6

		struct SCommonColumn
		{
			std::map<u32, u32> recordmap;
			u8 type;
		};

		SCommonColumn*   CommonColumns;

	protected:		//WDC1

	protected:		//WDC2
		struct SSectionHeader
		{
			u32 wdc2_unk_header1;       // always 0 in Battle (8.0.1.26231) and unnamed in client binary
			u32 wdc2_unk_header2;       // always 0 in Battle (8.0.1.26231) and unnamed in client binary
			u32 file_offset;            // absolute position to the beginning of the section
			u32 record_count;           // 'record_count' for the section
			u32 string_table_size;      // 'string_table_size' for the section
			u32 copy_table_size;
			u32 offset_map_offset;      // Offset to array of struct {uint32_t offset; uint16_t size;}[max_id - min_id + 1];
			u32 id_list_size;           // List of ids present in the DB file
			u32 relationship_data_size;
		};

		enum FIELD_COMPRESSION
		{
			NONE,
			BITPACKED,
			COMMON_DATA,
			BITPACKED_INDEXED,
			BITPACKED_INDEXED_ARRAY,
			BITPACKED_SIGNED
		};

		struct SFieldStorageInfo
		{
			u16 field_offset_bits;
			u16 field_size_bits;
			u32 additional_data_size;
			u32 storage_type;
			u32 val1;
			u32 val2;
			u32 val3;
		};

		SFieldStorageInfo* FieldStorageInfos;

		bool readFieldValue(u32 recordIndex, u32 fieldIndex, u32 arrayIndex, u32 arraySize, u32& result) const;
		u32 readBitpackedValue(const SFieldStorageInfo& info, const u8* recordOffset) const;
		u32 readBitpackedValue2(const SFieldStorageInfo& info, const u8* recordOffset) const;
	};
};
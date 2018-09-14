#include "stdafx.h"
#include "wow_dbc70.h"
#include "mywow.h"

namespace WowLegion
{
	dbc70::dbc70(wowEnvironment* env, const c8* filename, bool tmp /*= false*/)
	{
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
		else if (strncmp(magic, "WDC1", 4) == 0)
			db_type = 7;
		else if (strncmp(magic, "WDC2", 4) == 0)
			db_type = 8;
		else
			ASSERT(false);

		if (db_type == 5)		//db5
			readWDB5(env, file, tmp);
		else if (db_type == 6)	//db6
			readWDB6(env, file, tmp);
		else if (db_type == 6)	//db7
			readWDC1(env, file, tmp);
		else if (db_type == 8)	//dc2
			readWDC2(env, file, tmp);
		else
			ASSERT(false);

		delete file;
	}

	dbc70::~dbc70()
	{
	}

	bool dbc70::readFieldValue(u32 recordIndex, u32 fieldIndex, u32 arrayIndex, u32 arraySize, u32& result) const
	{
		return true;
	}

	u32 dbc70::readBitpackedValue(const SFieldStorageInfo& info, const u8* recordOffset) const
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

	u32 dbc70::readBitpackedValue2(const SFieldStorageInfo& info, const u8* recordOffset) const
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
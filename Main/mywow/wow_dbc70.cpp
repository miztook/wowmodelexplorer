#include "stdafx.h"
#include "wow_dbc70.h"
#include "mywow.h"

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

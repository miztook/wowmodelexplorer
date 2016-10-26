#pragma once

#include "core.h"

class IReadFile;

namespace Json
{
	class Value;
}

//blizzard api related

struct SCharArmoryInfo
{
	c8		Name[NAME_SIZE*4];

	u32		Race;
	u32		Gender;
	u32		ClassId;

	u32		SkinColor;
	u32		FaceType;
	u32		HairColor;
	u32		HairStyle;
	u32		FacialHair;

	s32		Head;
	s32		Shoulder;
	s32		Boots;
	s32		Belt;
	s32		Shirt;
	s32		Pants;
	s32		Chest;
	s32		Bracers;
	s32		Gloves;
	s32		HandRight;
	s32		HandLeft;
	s32		Cape;
	s32		Tabard;
};

class wow_armory
{
private:
	DISALLOW_COPY_AND_ASSIGN(wow_armory);

public:
	wow_armory();
	~wow_armory();

public:
	bool parseCharacterArmoryInfo(const c8* filename, SCharArmoryInfo* charInfo);
	bool parseCharacterArmoryInfo(IReadFile* file, SCharArmoryInfo* charInfo);

private:
	bool parseCharInfoFromJson(Json::Value& root, SCharArmoryInfo* charInfo);
	s32 parseItem(Json::Value& items, const c8* key);
};


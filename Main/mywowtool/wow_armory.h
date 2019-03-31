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
	char		Name[NAME_SIZE*4];

	uint32_t		Race;
	uint32_t		Gender;
	uint32_t		ClassId;

	uint32_t		SkinColor;
	uint32_t		FaceType;
	uint32_t		HairColor;
	uint32_t		HairStyle;
	uint32_t		FacialHair;

	int32_t		Head;
	int32_t		Shoulder;
	int32_t		Boots;
	int32_t		Belt;
	int32_t		Shirt;
	int32_t		Pants;
	int32_t		Chest;
	int32_t		Bracers;
	int32_t		Gloves;
	int32_t		HandRight;
	int32_t		HandLeft;
	int32_t		Cape;
	int32_t		Tabard;
};

class wow_armory
{
private:
	DISALLOW_COPY_AND_ASSIGN(wow_armory);

public:
	wow_armory();
	~wow_armory();

public:
	bool parseCharacterArmoryInfo(const char* filename, SCharArmoryInfo* charInfo);
	bool parseCharacterArmoryInfo(IReadFile* file, SCharArmoryInfo* charInfo);

private:
	bool parseCharInfoFromJson(Json::Value& root, SCharArmoryInfo* charInfo);
	int32_t parseItem(Json::Value& items, const char* key);
};


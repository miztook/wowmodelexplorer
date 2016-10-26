#include "stdafx.h"
#include "wow_armory.h"
#include "mywow.h"

wow_armory::wow_armory()
{

}

wow_armory::~wow_armory()
{

}

bool wow_armory::parseCharacterArmoryInfo( const c8* filename, SCharArmoryInfo* charInfo )
{
	string256 path = g_Engine->getFileSystem()->getBaseDirectory();
	path.normalizeDir();
	path.append(filename);

	IReadFile* rfile = g_Engine->getFileSystem()->createAndOpenFile(path.c_str(), false);
	if (!rfile)
		return false;

	bool ret = parseCharacterArmoryInfo(rfile, charInfo);

	delete rfile;
	return ret;
}

bool wow_armory::parseCharacterArmoryInfo( IReadFile* file, SCharArmoryInfo* charInfo )
{
#ifdef MW_PLATFORM_WINDOWS
    
	u32 size  = file->getSize();
	if (size == 0)
		return false;

	bool parsingSuccessful = false;

	++size;				//比文件长度大1
	c8* buffer = (c8*)Z_AllocateTempMemory(size);
	file->seek(0);
	u32 read = file->readText(buffer, size);
	_ASSERT(read + 1== size);

	//parse
	Json::Features features;
	Json::Value root;
	Json::Reader reader( features );
	parsingSuccessful = reader.parse(buffer, buffer + size - 1, root, false );

	if (parsingSuccessful)
	{
		parsingSuccessful = parseCharInfoFromJson(root, charInfo);
	}

	Z_FreeTempMemory(buffer);

	return parsingSuccessful;
    
#else
    _ASSERT(false);
    return false;
#endif
}

bool wow_armory::parseCharInfoFromJson( Json::Value& root, SCharArmoryInfo* charInfo )
{
#ifdef MW_PLATFORM_WINDOWS
	if (root.empty())
		return false;

	Json::Value appearance = root["appearance"];
	if(appearance.empty())
		return false;

	Json::Value items = root["items"];
	if (items.empty())
		return false;

	string128 name = root["name"].asCString();
	strcpy_s(charInfo->Name, NAME_SIZE, name.c_str());
	charInfo->Race = root["race"].asInt();
	charInfo->Gender = root["gender"].asInt();
	charInfo->ClassId = root["class"].asInt();

	charInfo->SkinColor = appearance["skinColor"].asInt();
	charInfo->FaceType = appearance["faceVariation"].asInt();
	charInfo->HairColor = appearance["hairColor"].asInt();
	charInfo->HairStyle = appearance["hairVariation"].asInt();
	charInfo->FacialHair = appearance["featureVariation"].asInt();

	charInfo->Head = parseItem(items, "head");
	charInfo->Shoulder = parseItem(items, "shoulder");
	charInfo->Boots = parseItem(items, "feet");
	charInfo->Belt = parseItem(items, "waist");
	charInfo->Shirt = parseItem(items, "shirt");
	charInfo->Pants = parseItem(items, "legs");
	charInfo->Chest = parseItem(items, "chest");
	charInfo->Bracers = parseItem(items, "wrist");
	charInfo->Gloves = parseItem(items, "hands");
	charInfo->HandRight = parseItem(items, "mainHand");
	charInfo->HandLeft = parseItem(items, "offHand");
	charInfo->Cape = parseItem(items, "back");
	charInfo->Tabard = parseItem(items, "tabard");

	return true;
    
#else
    _ASSERT(false);
    return false;
#endif
}

s32 wow_armory::parseItem( Json::Value& items, const c8* key )
{
	s32 ret = 0;

#ifdef MW_PLATFORM_WINDOWS    
	Json::Value v = items[key];
	if (!v.empty())
	{
		if (v["tooltipParams"].isMember("transmogItem"))
			ret = v["tooltipParams"]["transmogItem"].asInt();
		else
			ret = v["id"].asInt();
	}
#endif
	
	return ret;
}

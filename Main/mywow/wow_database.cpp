#include "stdafx.h"
#include "wow_database.h"
#include "mywow.h"

namespace WowClassic
{
	void g_callbackWMO(const char* filename, void* param)
	{
		if (strstr(filename, "internal"))
		{
			return;
		}

		uint32_t len = (uint32_t)strlen(filename);
		if (len > 8 &&
			filename[len - 8] == '_' &&
			isdigit((int)filename[len - 7]) &&
			isdigit((int)filename[len - 6]) &&
			isdigit((int)filename[len - 5]))		//_xxx.wmo
		{
			return;
		}

		WMOCollections* collection = (WMOCollections*)param;
		collection->wmos.push_back(filename);
	}

	void g_callbackBLP(const char* filename, void* param)
	{
		TextureCollections* collection = (TextureCollections*)param;
		collection->textures.push_back(filename);
	}

	void g_callbackWorldM2(const char* filename, void* param)
	{
		WorldModelCollections* collection = (WorldModelCollections*)param;
		collection->models.push_back(filename);
	}

	wowDatabase::wowDatabase(const wowEnvironment* env)
		: Environment(env)
	{
	}

	wowDatabase::~wowDatabase()
	{
	}

	bool wowDatabase::init()
	{
		AnimDB = std::make_unique<animDB>(Environment);
		AreaTableDB = std::make_unique< areaTableDB>(Environment);
		CharClassesDB = std::make_unique< charClassesDB>(Environment);
		CharFacialHairDB = std::make_unique< charFacialHairDB>(Environment);
		CharHairGeosetsDB = std::make_unique< charHairGeosetsDB>(Environment);
		CharRacesDB = std::make_unique< charRacesDB>(Environment);
		CharSectionsDB = std::make_unique< charSectionsDB>(Environment);
		CreatureTypeDB = std::make_unique< creatureTypeDB>(Environment);
		CreatureModelDB = std::make_unique< creatureModelDB>(Environment);
		CreatureDisplayInfoDB = std::make_unique< creatureDisplayInfoDB>(Environment);
		CreatureDisplayInfoExtraDB = std::make_unique< creatureDisplayInfoExtraDB>(Environment);
		HelmGeosetDB = std::make_unique< helmGeosetDB>(Environment);
		ItemDisplayDB = std::make_unique< itemDisplayDB>(Environment);
		ItemSetDB = std::make_unique< itemSetDB>(Environment);
		ItemSubClassDB = std::make_unique< itemSubClassDB>(Environment);
		StartOutFitDB = std::make_unique< startOutfitDB>(Environment);

		ItemDB = std::make_unique< itemDB>(Environment);

#if WOW_VER >= 70
		NpcModelItemSlotDisplayInfoDB = std::make_unique< npcModelItemSlotDisplayInfoDB>(Environment);
		ItemDisplayInfoMaterialResDB = std::make_unique< itemDisplayInfoMaterialResDB>(Environment);
		ItemModifiedAppearanceDB = std::make_unique< itemModifiedAppearanceDB>(Environment);
		ItemAppearanceDB = std::make_unique< itemAppearanceDB>(Environment);
		TextureFileDataDB = std::make_unique< textureFileDataDB>(Environment);
		ModelFileDataDB = std::make_unique< modelFileDataDB>(Environment);
#elif WOW_VER >= 60
		NpcModelItemSlotDisplayInfoDB = nullptr;
		ItemDisplayInfoMaterialResDB = nullptr;
		ItemModifiedAppearanceDB = std::make_unique< itemModifiedAppearanceDB>(Environment);
		ItemAppearanceDB = std::make_unique< itemAppearanceDB>(Environment);
		TextureFileDataDB = std::make_unique< textureFileDataDB>(Environment);
		ModelFileDataDB = nullptr;
#else
		NpcModelItemSlotDisplayInfoDB = nullptr;
		ItemDisplayInfoMaterialResDB = nullptr;
		ItemModifiedAppearanceDB = nullptr;
		ItemAppearanceDB = nullptr;
		TextureFileDataDB = nullptr;
		ModelFileDataDB = nullptr;
#endif

#if WOW_VER == 60
		FileDataDB = std::make_unique<fileDataDB>(Environment);
#else
		FileDataDB = nullptr;
#endif

		MapDB = std::make_unique<mapDB>(Environment);

		return true;
	}

	void wowDatabase::buildItems()
	{
		itemSparseDB* sparseDB = new itemSparseDB(Environment);
		buildItemCollections(itemCollections, ItemDB.get(), sparseDB);
		delete sparseDB;
	}

	bool wowDatabase::buildNpcs(const char* filename)
	{
		if (!npcCollections.open(filename))
			return false;

		return true;
	}

	bool wowDatabase::buildRidables(const char* filename)
	{
		if (!ridableCollections.open(filename, npcCollections))
			return false;

		return true;
	}

	void wowDatabase::buildStartOutfitClass()
	{
		uint32_t numtotal = StartOutFitDB->getNumRecords();
		startOutfitClassCollections.startOutfits.resize(numtotal);

		for (uint32_t i = 0; i < numtotal; ++i)
		{
			auto r = StartOutFitDB->getRecord(i);
			if (r.isValid())
			{
#if WOW_VER >= 70
				uint8_t race = (uint8_t)r.getUInt(startOutfitDB::Race);
				uint8_t id = (uint8_t)r.getUInt(startOutfitDB::Class);
				uint8_t gender =	(uint8_t)r.getUInt(startOutfitDB::Gender);
#else
				uint8_t race = r.getByte(startOutfitDB::Race);
				uint8_t id = r.getByte(startOutfitDB::Class);
				uint8_t gender = r.getByte(startOutfitDB::Gender);
#endif
				auto rec = CharClassesDB->getByID(id);
				if (rec.isValid())
				{
					SStartOutfitEntry entry;
					utf8to16(rec.getString(charClassesDB::NameV400), entry.name, NAME_SIZE);
					utf8to16(rec.getString(charClassesDB::ShortName), entry.shortname, NAME_SIZE);
					entry.id = r.getID();
					entry.race = race;
					entry.female = gender == 1;

					startOutfitClassCollections.startOutfits[i] = entry;
				}
			}
		}

		//startOutfitClassCollections.startOutfits.shrink_to_fit();
	}

	void wowDatabase::buildMaps()
	{
		uint32_t numMaps = MapDB->getNumRecords();
		for (uint32_t i = 0; i < numMaps; ++i)
		{
			auto r = MapDB->getRecord(i);

			SMapRecord m;
			m.id = r.getID();
			m.type = r.getInt(mapDB::AreaType);
			Q_strcpy(m.name, DEFAULT_SIZE, r.getString(mapDB::Name));

			mapCollections.mapLookup[m.id] = (uint32_t)mapCollections.maps.size();
			mapCollections.maps.emplace_back(m);
		}

		//mapCollections.maps.shrink_to_fit();
	}

	void wowDatabase::buildWmos()
	{
		wmoCollections.wmos.clear();

		Environment->iterateFiles("world", "wmo", g_callbackWMO, &wmoCollections);

		//wmoCollections.wmos.shrink_to_fit();
	}

	void wowDatabase::buildWorldModels()
	{
		worldModelCollections.models.clear();

		Environment->iterateFiles("world", "m2", g_callbackWorldM2, &worldModelCollections);
	}

	void wowDatabase::buildTextures()
	{
		textureCollections.textures.clear();

		Environment->iterateFiles("blp", g_callbackBLP, &textureCollections);
	}

	bool wowDatabase::getRaceGender(const char* filename, uint32_t& race, uint32_t& gender, bool& isHD)
	{
		if (Q_stricmp(filename, "OrcFemale_HD_Shadowmoon") == 0 ||
			Q_stricmp(filename, "OrcMale_HD_Shadowmoon") == 0)
		{
			return false;
		}

		char n[DEFAULT_SIZE];
		Q_strcpy(n, DEFAULT_SIZE, filename);
		Q_strlwr(n);

		char* s;
		isHD = ((s = strstr(n, "_hd")) && s[3] == '\0');

		uint32_t len = (uint32_t)strlen(n);
		if ((s = strstr(n, "female")))
		{
			gender = 1;
			len = (uint32_t)(s - n);
		}
		else if ((s = strstr(n, "male")))
		{
			gender = 0;
			len = (uint32_t)(s - n);
		}
		else
			return false;

		char raceName[DEFAULT_SIZE];
		Q_strncpy(raceName, DEFAULT_SIZE, n, len);
		raceName[len] = '\0';

		auto r = CharRacesDB->getByName(raceName);
		if (!r.isValid())
			return false;
		race = r.getID();

		return true;
	}

	bool wowDatabase::getRaceId(const char* raceName, uint32_t& race)
	{
		auto r = CharRacesDB->getByName(raceName);
		if (!r.isValid())
			return false;
		race = r.getID();
		return true;
	}

	const char* wowDatabase::getRaceName(uint32_t race)
	{
		auto r = CharRacesDB->getByID(race);
		if (!r.isValid())
			return "";
		return r.getString(charRacesDB::Name);
	}

	bool wowDatabase::getCharacterPath(const char* raceName, bool female, bool isHD, char* path, uint32_t size)
	{
		auto r = CharRacesDB->getByName(raceName);
		if (!r.isValid())
			return false;

		char genderString[10];
		if (female)
			Q_strcpy(genderString, 10, "Female");
		else
			Q_strcpy(genderString, 10, "Male");

		string256 modelpath;

		modelpath = "Character\\";
		modelpath.append(raceName);
		modelpath.append("\\");
		modelpath.append(genderString);
		modelpath.append("\\");
		modelpath.append(raceName);
		modelpath.append(genderString);

		if (isHD)
			modelpath.append("_HD");

		modelpath.append(".m2");

		if (modelpath.length() > size)
			return false;

		Q_strcpy(path, size, modelpath.c_str());
		normalizeFileName(path);
		Q_strlwr(path);

		return true;
	}

	int32_t wowDatabase::getNpcModelId(int32_t npcid)
	{
		auto rSkin = CreatureDisplayInfoDB->getByID(npcid);
		if (!rSkin.isValid())
			return -1;

		uint32_t modelId = rSkin.getUInt(creatureDisplayInfoDB::ModelID);
		if (!modelId)
			return -1;

		return (int32_t)modelId;
	}

	bool wowDatabase::getNpcPath(int32_t npcid, bool isHD, char* path, uint32_t size)
	{
		path[0] = '\0';

		int32_t modelId = getNpcModelId(npcid);
		if (-1 == modelId)
			return false;

		auto r = CreatureModelDB->getByID((uint32_t)modelId);
		if (!r.isValid())
			return false;

#if WOW_VER >= 70
		int32_t fileId = r.getByte3(creatureModelDB::FileNameID);
		char filename[512];
		getFilePath(fileId, filename, 512);
#elif WOW_VER >= 60
		int32_t fileId = r.getInt(creatureModelDB::FileNameID);
		char filename[512];
		getFilePath(fileId, filename, 512);
#else
		const char* filename = r.getString(creatureModelDB::Filename);
#endif
		if (strlen(filename) < 4 || strlen(filename) > 255)
			return false;

		char ext[10];
		getFileExtensionA(filename, ext, 10);

		string256 modelpath;
		modelpath = filename;

		if (getM2Type(filename) == MT_CHARACTER && isHD)
		{
			modelpath.makeHDFileName(filename);
		}

		if (Q_stricmp(ext, "m2") != 0)
		{
			string256 tmppath;
			modelpath.subString(0, modelpath.length() - (uint32_t)strlen(ext), tmppath);
			tmppath.append("M2");
			modelpath = tmppath;
		}

		if (size <= modelpath.length())
			return false;

		Q_strcpy(path, size, modelpath.c_str());
		normalizeFileName(path);
		Q_strlwr(path);

		return true;
	}

	const char* wowDatabase::getAnimationName(uint32_t id)
	{
		auto r = AnimDB->getByID(id);
		if (r.isValid())
		{
			return r.getString(animDB::Name);
		}
		else
		{
			return "";
		}
	}

	uint32_t wowDatabase::getMaxSkinColor(uint32_t race, bool female, bool isHD)
	{
		bool bHD = isRaceHasHD(race) && isHD;
		uint32_t num = CharSectionsDB->getColorsFor(race, (female ? 1 : 0), (bHD ? charSectionsDB::SkinTypeHD : charSectionsDB::SkinType), 0);
		ASSERT(num);
		return num - 1;
	}

	uint32_t wowDatabase::getMaxFaceType(uint32_t race, bool female, bool isHD)
	{
		bool bHD = isRaceHasHD(race) && isHD;
		uint32_t num = CharSectionsDB->getSectionsFor(race, (female ? 1 : 0), (bHD ? charSectionsDB::FaceTypeHD : charSectionsDB::FaceType), 0);
		ASSERT(num);
		return num - 1;
	}

	uint32_t wowDatabase::getMaxHairColor(uint32_t race, bool female, bool isHD)
	{
		bool bHD = isRaceHasHD(race) && isHD;
		uint32_t num = CharSectionsDB->getColorsFor(race, (female ? 1 : 0), (bHD ? charSectionsDB::HairTypeHD : charSectionsDB::HairType), 0);
		ASSERT(num);
		return num - 1;
	}

	uint32_t wowDatabase::getMaxHairStyle(uint32_t race, bool female, bool isHD)
	{
		bool bHD = isRaceHasHD(race) && isHD;
		uint32_t num = CharSectionsDB->getSectionsFor(race, (female ? 1 : 0), (bHD ? charSectionsDB::HairTypeHD : charSectionsDB::HairType), 0);
		ASSERT(num);
		return num - 1;
	}

	uint32_t wowDatabase::getMaxFacialHairStyle(uint32_t race, bool female)
	{
		uint32_t num = CharFacialHairDB->getStylesFor(race, female ? 1 : 0);
		ASSERT(num);
		return num - 1;
	}

	void wowDatabase::getSubClassName(int32_t id, int32_t subid, char16_t* outname, uint32_t size)
	{
		auto  r = ItemSubClassDB->getById(id, subid);
		if (r.isValid() && id > 0)
		{
			const char* n = r.getString(itemSubClassDB::NameV400);
			utf8to16(n, outname, size);
		}
		else
		{
			memset(outname, 0, size * sizeof(uint16_t));
		}
	}

	void wowDatabase::getNpcTypeName(int32_t id, char16_t* outname, uint32_t size)
	{
		auto r = CreatureTypeDB->getByID(id);
		if (r.isValid())
		{
			const char* n = r.getString(creatureTypeDB::Name);
			utf8to16(n, outname, size);
		}
		else
		{
			const char* n = "Unknown";
			utf8to16(n, outname, size);
		}
	}

	const char* wowDatabase::getClassShortName(uint32_t classId)
	{
		auto r = CharClassesDB->getByID(classId);
		if (!r.isValid())
			return "";
		return r.getString(charClassesDB::ShortName);
	}

	bool wowDatabase::getClassInfo(const char* shortname, char16_t* classname, uint32_t size, uint32_t* id)
	{
		for (uint32_t i = 0; i < CharClassesDB->getNumActualRecords(); ++i)
		{
			auto r = CharClassesDB->getRecord(i);
			if (!r.isValid())
				continue;

			if (Q_stricmp(r.getString(charClassesDB::ShortName), shortname) == 0)
			{
				*id = r.getID();
				utf8to16(r.getString(charClassesDB::NameV400), classname, size);
				return true;
			}
		}

		*id = 0;
		memset(classname, 0, sizeof(char16_t)* size);
		return false;
	}

	bool wowDatabase::getSet(uint32_t index, int32_t& setid, char16_t* outname, uint32_t size)
	{
		auto r = ItemSetDB->getRecord(index);
		if (!r.isValid())
			return false;

		setid = r.getInt(0);
		const char* n = r.getString(itemSetDB::Name);
		if (size <= strlen(n))
			return false;
		utf8to16(n, outname, size);
		return true;
	}

	const SMapRecord* wowDatabase::getMap(uint32_t idx) const
	{
		if (idx >= mapCollections.maps.size())
			return nullptr;
		return &mapCollections.maps[idx];
	}

#ifdef MW_EDITOR

	const SItemRecord* wowDatabase::getItem(uint32_t idx) const
	{
		if (itemCollections.items.size() <= idx)
			return nullptr;
		return &itemCollections.items[idx];
	}

	const SNPCRecord* wowDatabase::getNPC(uint32_t idx) const
	{
		if (npcCollections.npcs.size() <= idx)
			return nullptr;
		return &npcCollections.npcs[idx];
	}

	uint32_t wowDatabase::getNumStartOutfits(uint32_t race, bool female)
	{
		return startOutfitClassCollections.getNumStartOutfits(race, female);
	}

	const SStartOutfitEntry* wowDatabase::getStartOutfit(uint32_t race, bool female, uint32_t idx)
	{
		return startOutfitClassCollections.get(race, female, idx);
	}

	const char* wowDatabase::getWmoFileName(uint32_t index) const
	{
		if (index >= wmoCollections.wmos.size())
			return nullptr;
		return wmoCollections.wmos[index].c_str();
	}

	const char* wowDatabase::getWorldModelFileName(uint32_t index) const
	{
		if (index >= worldModelCollections.models.size())
			return nullptr;
		return worldModelCollections.models[index].c_str();
	}

	const char* wowDatabase::getTextureFileName(uint32_t index) const
	{
		if (index >= textureCollections.textures.size())
			return nullptr;
		return textureCollections.textures[index].c_str();
	}

	const SRidable* wowDatabase::getRidable(uint32_t idx) const
	{
		if (idx >= ridableCollections.ridables.size())
			return nullptr;
		return &ridableCollections.ridables[idx];
	}

#endif

	int32_t wowDatabase::getItemDisplayId(int32_t itemid) const
	{
#if WOW_VER >= 60
		auto r = getItemModifiedAppearanceDB()->getByItemID(itemid);
		if (!r.isValid())
			return -1;

		uint32_t appearanceId = r.getUInt(itemModifiedAppearanceDB::ItemAppearanceId);
		r = getItemAppearanceDB()->getByID(appearanceId);
		if (!r.isValid())
			return -1;

		return r.getInt(itemAppearanceDB::ItemDisplay);

#else
		auto r = g_Engine->getWowDatabase()->getItemDB()->getByID(itemid);
		if (!r.isValid())
			return -1;
		return r.getInt(itemDB::ItemDisplayInfo);
#endif
	}

	void wowDatabase::getFilePath(int32_t fileId, string256& path) const
	{
#if WOW_VER >= 70
		if(fileId == 0)
		{
			path.clear();
			return;
		}

		const char* filename = g_Engine->getWowEnvironment()->getFileNameByFileDataId(fileId);
		if (strlen(filename) == 0)
		{
			path.format("File%08X.unk", fileId);
		}
		else
		{
			path = filename;
		}
		path.normalize();
		path.make_lower();
#elif WOW_VER >= 60
		auto r = getFileDataDB()->getByID(fileId);
		if (!r.isValid())
		{
			path.clear();
			return;
		}
		path = r.getString(fileDataDB::FilePath);
		path.append(r.getString(fileDataDB::FileName));

		path.normalize();
		path.make_lower();
#else
		ASSERT(false);
		path.clear();
#endif
	}

	void wowDatabase::getFilePath(int32_t fileId, char* path, uint32_t size) const
	{
#if WOW_VER >= 70
		if(fileId == 0)
		{
			memset(path, 0, size);
			return;
		}

		const char* filename = g_Engine->getWowEnvironment()->getFileNameByFileDataId(fileId);
		if (strlen(filename) == 0)
		{
			Q_sprintf(path, size, "File%08X.unk", fileId);
		}
		else
		{
			Q_strcpy(path, size, filename);
		}
		normalizeFileName(path);
		Q_strlwr(path);
#elif WOW_VER >= 60
		auto r = getFileDataDB()->getByID(fileId);
		if (!r.isValid())
		{
			memset(path, 0, size);
			return;
		}
		Q_strcpy(path, size, r.getString(fileDataDB::FilePath));
		Q_strcat(path, size, r.getString(fileDataDB::FileName));

		normalizeFileName(path);
		Q_strlwr(path);

#else
		ASSERT(false);
		memset(path, 0, size);
#endif
	}

	void wowDatabase::getTextureFilePath(int32_t texId, string256& path) const
	{
#if WOW_VER >= 70
		auto r = getTextureFileDataDB()->getByTextureId(texId);
		if (!r.isValid())
		{
			path.clear();
			return;
		}
		int32_t fileId = r.getByte3(textureFileDataDB::Path);
		getFilePath(fileId, path);
#elif WOW_VER >= 60
		auto r = getTextureFileDataDB()->getByTextureId(texId);
		if (!r.isValid())
		{
			path.clear();
			return;
		}
		int32_t fileId = r.getInt(textureFileDataDB::Path);
		getFilePath(fileId, path);
#else
		ASSERT(false);
		path.clear();
#endif
	}

	void wowDatabase::getTextureFilePath(int32_t texId, char* path, uint32_t size) const
	{
#if WOW_VER >= 70
		auto r = getTextureFileDataDB()->getByTextureId(texId);
		if (!r.isValid())
		{
			memset(path, 0, size);
			return;
		}
		int32_t fileId = r.getByte3(textureFileDataDB::Path);
		getFilePath(fileId, path, size);
#elif WOW_VER >= 60
		auto r = getTextureFileDataDB()->getByTextureId(texId);
		if (!r.isValid())
		{
			memset(path, 0, size);
			return;
		}
		int32_t fileId = r.getInt(textureFileDataDB::Path);
		getFilePath(fileId, path, size);
#else
		ASSERT(false);
		memset(path, 0, size);
#endif
	}

	void wowDatabase::getModelFilePath(int32_t modelId, string256& path) const
	{
#if WOW_VER >= 70
		auto r = getModelFileDataDB()->getByModelId(modelId);
		if (!r.isValid())
		{
			path.clear();
			return;
		}
		int32_t fileId = r.getByte3(modelFileDataDB::Path);
		getFilePath(fileId, path);
#else
		ASSERT(false);
		path.clear();
#endif
	}

	void wowDatabase::getModelFilePath(int32_t modelId, char* path, uint32_t size) const
	{
#if WOW_VER >= 70
		auto r = getModelFileDataDB()->getByModelId(modelId);
		if (!r.isValid())
		{
			memset(path, 0, size);
			return;
		}
		int32_t fileId = r.getByte3(modelFileDataDB::Path);
		getFilePath(fileId, path, size);
#else
		ASSERT(false);
		memset(path, 0, size);
#endif
	}

	bool wowDatabase::isRaceHasHD(uint32_t race)
	{
		return race == RACE_HUMAN ||
			race == RACE_ORC ||
			race == RACE_DWARF ||
			race == RACE_NIGHTELF ||
			race == RACE_UNDEAD ||
			race == RACE_TAUREN ||
			race == RACE_GNOME ||
			race == RACE_TROLL ||
			race == RACE_DRAENEI ||
			race == RACE_BLOODELF;
	}

	bool wowDatabase::getItemPath(int32_t itemid, char* modelpath, uint32_t modelSize, char* texturepath, uint32_t texSize)
	{
		uint32_t displayid = getItemDisplayId(itemid);
		auto display = getItemDisplayDB()->getByID(displayid);
		if (!display.isValid())
			return false;

		string256 model1;
		string256 skin1;
#if WOW_VER >= 70
		getModelFilePath((int32_t)display.getInArray<uint16_t>(itemDisplayDB::Model, 0), model1);
		getTextureFilePath((int32_t)display.getByte3InArray(itemDisplayDB::Skin, 0), skin1);
#elif WOW_VER >= 60
		model1 = display.getString(itemDisplayDB::Model);
		int32_t skin1Id = display.getInt(itemDisplayDB::Skin);
		getTextureFilePath(skin1Id, skin1);
#else
		model1 = display.getString(itemDisplayDB::Model);
		skin1 = display.getString(itemDisplayDB::Skin);
#endif

		if (modelSize < (model1.length() + 1) || texSize < (skin1.length() + 1))
			return false;

		model1.changeExt("mdx", "m2");

		getFileNameA(model1.c_str(), modelpath, modelSize);
		getFileNameA(skin1.c_str(), texturepath, texSize);

		return true;
	}
};
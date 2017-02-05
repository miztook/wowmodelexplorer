#include "stdafx.h"
#include "wow_database.h"
#include "mywow.h"

void g_callbackWMO(const c8* filename, void* param)
{
	if (strstr(filename, "internal"))
	{
		return;
	}

	u32 len = (u32)strlen(filename);
	if (len > 8 && 
		filename[len-8] == '_' &&
		isdigit((int)filename[len-7]) &&
		isdigit((int)filename[len-6]) &&
		isdigit((int)filename[len-5]) )		//_xxx.wmo
	{
		return;
	}

	WMOCollections* collection = (WMOCollections*)param;
	collection->wmos.push_back(filename);
}

void g_callbackBLP(const c8* filename, void* param)
{
	TextureCollections* collection = (TextureCollections*)param;
	collection->textures.push_back(filename);
}

void g_callbackWorldM2(const c8* filename, void* param)
{
	WorldModelCollections* collection = (WorldModelCollections*)param;
	collection->models.push_back(filename);
}

wowDatabase::wowDatabase( wowEnvironment* env )
	: Environment(env)
{
	AnimDB = new animDB(env);
	AreaTableDB = new areaTableDB(env);
	CharClassesDB = new charClassesDB(env);
	CharFacialHairDB = new charFacialHairDB(env);
	CharHairGeosetsDB = new charHairGeosetsDB(env);
	CharRacesDB = new charRacesDB(env);
	CharSectionsDB = new charSectionsDB(env);
	CreatureTypeDB = new creatureTypeDB(env);
	CreatureModelDB = new creatureModelDB(env);
	CreatureDisplayInfoDB = new creatureDisplayInfoDB(env);
	CreatureDisplayInfoExtraDB = new creatureDisplayInfoExtraDB(env);
	HelmGeosetDB = new helmGeosetDB(env);
	ItemDisplayDB = new itemDisplayDB(env);
	ItemSetDB = new itemSetDB(env);
	ItemSubClassDB = new itemSubClassDB(env);
	StartOutFitDB = new startOutfitDB(env);

#if 0
	LightDB = new lightDB(env);
	ItemVisualsDB = new itemVisualsDB(env);
	ItemVisualEffectDB = new itemVisualEffectDB(env);
	WmoAreaTableDB = new wmoAreaTableDB(env);
	WorldMapAreaDB = new worldMapAreaDB(env);
	LightSkyboxDB = new lightSkyboxDB(env);
#else
	LightDB = NULL_PTR;
	ItemVisualsDB = NULL_PTR;
	ItemVisualEffectDB = NULL_PTR;
	WmoAreaTableDB = NULL_PTR;
	WorldMapAreaDB = NULL_PTR;
	LightSkyboxDB = NULL_PTR;
#endif

	ItemDB = new itemDB(env);

#if defined(WOW70)
	NpcModelItemSlotDisplayInfoDB = new npcModelItemSlotDisplayInfoDB(env);
	ItemDisplayInfoMaterialResDB = new itemDisplayInfoMaterialResDB(env);
	ItemModifiedAppearanceDB = new itemModifiedAppearanceDB(env);
	ItemAppearanceDB = new itemAppearanceDB(env);
	TextureFileDataDB = new textureFileDataDB(env);
	ModelFileDataDB = new modelFileDataDB(env);
#elif defined(WOW60)
	NpcModelItemSlotDisplayInfoDB = NULL_PTR;
	ItemDisplayInfoMaterialResDB = NULL_PTR;
	ItemModifiedAppearanceDB = new itemModifiedAppearanceDB(env);
	ItemAppearanceDB = new itemAppearanceDB(env);
	TextureFileDataDB = new textureFileDataDB(env);
	ModelFileDataDB = NULL_PTR;
#else
	NpcModelItemSlotDisplayInfoDB = NULL_PTR;
	ItemDisplayInfoMaterialResDB = NULL_PTR;
	ItemModifiedAppearanceDB = NULL_PTR;
	ItemAppearanceDB = NULL_PTR;
	TextureFileDataDB = NULL_PTR;
	ModelFileDataDB = NULL_PTR;
#endif

#ifdef WOW60
	FileDataDB = new fileDataDB(env);
#else
	FileDataDB = NULL_PTR;
#endif

	MapDB = new mapDB(env);
	
	SpellVisualEffectNameDB = new spellVisualEffectNameDB(env);

#if 0
	SpellVisualKitDB = new spellVisualKitDB(env);
	SpellVisualDB = new spellVisualDB(env);
	SpellDB = new spellDB(env);
#else
	SpellVisualKitDB = NULL_PTR; 
	SpellVisualDB = NULL_PTR;
	SpellDB = NULL_PTR; 
#endif
}

wowDatabase::~wowDatabase()
{
	delete SpellDB;
	delete SpellVisualDB;
	delete SpellVisualKitDB;
	delete SpellVisualEffectNameDB;
	delete MapDB;
	delete FileDataDB;
	delete ModelFileDataDB;
	delete TextureFileDataDB;
	delete ItemAppearanceDB;
	delete ItemModifiedAppearanceDB;
	delete ItemDisplayInfoMaterialResDB;
	delete NpcModelItemSlotDisplayInfoDB;
	delete ItemDB;
	delete LightSkyboxDB;
	delete LightDB;

	delete WorldMapAreaDB;
	delete WmoAreaTableDB;
	delete ItemVisualEffectDB;
	delete ItemVisualsDB;
	delete StartOutFitDB;
	delete ItemSubClassDB;
	delete ItemSetDB;
	delete ItemDisplayDB;
	delete HelmGeosetDB;
	delete CreatureDisplayInfoExtraDB;
	delete CreatureDisplayInfoDB;
	delete CreatureModelDB;
	delete CreatureTypeDB;
	delete CharSectionsDB;
	delete CharRacesDB;
	delete CharHairGeosetsDB;
	delete CharFacialHairDB;
	delete CharClassesDB;
	delete AreaTableDB;
	delete AnimDB;
}

void wowDatabase::buildItems( )
{
	itemSparseDB* sparseDB = new itemSparseDB(Environment);
	itemCollections.build(ItemDB, sparseDB);
	delete sparseDB;
}

bool wowDatabase::buildNpcs( const c8* filename )
{
	if (!npcCollections.open(filename))
		return false;

	return true;
}

bool wowDatabase::buildRidables( const c8* filename )
{
	if (!ridableCollections.open(filename, npcCollections))
		return false;

	return true;
}

void wowDatabase::buildStartOutfitClass()
{
	u32 numtotal = StartOutFitDB->getNumRecords();
	startOutfitClassCollections.startOutfits.resize(numtotal);

	for (u32 i=0; i<numtotal; ++i)
	{
		dbc::record r = StartOutFitDB->getRecord(i);
		if (r.isValid())
		{
#ifdef WOW70
			u8 race = (u8)r.getUInt(startOutfitDB::Race);
			u8 id = (u8)r.getUInt(startOutfitDB::Class);
			u8 gender =	(u8)r.getUInt(startOutfitDB::Gender);
#else
			u8 race = r.getByte(startOutfitDB::Race);
			u8 id = r.getByte(startOutfitDB::Class);
			u8 gender =	r.getByte(startOutfitDB::Gender);
#endif
			dbc::record rec = CharClassesDB->getByID(id);
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
	u32 numMaps = MapDB->getNumRecords();
	for (u32 i=0; i<numMaps; ++i)
	{
		dbc::record r = MapDB->getRecord(i);

		SMapRecord m;
		m.id = r.getID();
		m.type = r.getInt(mapDB::AreaType);
		Q_strcpy(m.name, DEFAULT_SIZE, r.getString(mapDB::Name));

		mapCollections.mapLookup[m.id] = (u32)mapCollections.maps.size();
		mapCollections.maps.emplace_back(m);
	}

	u32 numAreas = AreaTableDB->getNumRecords();
	for (u32 i=0; i<numAreas; ++i)
	{
		dbc::record r = AreaTableDB->getRecord(i);

		SArea area;
		area.id = r.getID();
		area.mapId = r.getInt(areaTableDB::MapID);
		area.parentId = r.getInt(areaTableDB::ParentAreaTableID);
		utf8to16(r.getString(areaTableDB::Name), area.name, DEFAULT_SIZE);

		mapCollections.areaLookup[area.id] = (u32)mapCollections.areas.size();
		mapCollections.areas.emplace_back(area);
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

bool wowDatabase::getRaceGender( const c8* filename, u32& race, u32& gender, bool& isHD )
{
	if (Q_stricmp(filename, "OrcFemale_HD_Shadowmoon") == 0 ||
		Q_stricmp(filename, "OrcMale_HD_Shadowmoon") == 0)
	{
		return false;
	}

	char n[DEFAULT_SIZE];
	Q_strcpy(n, DEFAULT_SIZE, filename);
	Q_strlwr(n);

	c8* s;
	isHD = ((s = strstr(n, "_hd")) && s[3] == '\0');

	u32 len = (u32)strlen(n);
	if((s = strstr(n, "female")))
	{
		gender = 1;
		len = (u32)(s-n);
	}
	else if ((s = strstr(n, "male")))
	{
		gender = 0;
		len = (u32)(s-n);
	}
	else
		return false;

	char raceName[DEFAULT_SIZE];
	Q_strncpy(raceName, DEFAULT_SIZE, n, len);
	raceName[len] = '\0';

	dbc::record r = CharRacesDB->getByName(raceName);
	if (!r.isValid())
		return false;
	race = r.getID();

	return true;
}

bool wowDatabase::getRaceId(const c8* raceName, u32& race)
{
	dbc::record r = CharRacesDB->getByName(raceName);
	if (!r.isValid())
		return false;
	race = r.getID();
	return true;
}

const c8* wowDatabase::getRaceName( u32 race )
{
	dbc::record r = CharRacesDB->getByID(race);
	if (!r.isValid())
		return "";
	return r.getString(charRacesDB::Name);
}

bool wowDatabase::getCharacterPath( const c8* raceName, bool female, bool isHD, c8* path, u32 size )
{
	dbc::record r = CharRacesDB->getByName(raceName);
	if (!r.isValid())
		return false;

	c8 genderString[10];
	if(female)
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

s32 wowDatabase::getNpcModelId( s32 npcid )
{
	dbc::record rSkin = CreatureDisplayInfoDB->getByID(npcid);
	if (!rSkin.isValid())
		return -1;

	u32 modelId = rSkin.getUInt(creatureDisplayInfoDB::ModelID);
	if (!modelId)
		return -1;

	return (s32)modelId;
}

bool wowDatabase::getNpcPath(s32 npcid, bool isHD, c8* path, u32 size)
{
	path[0] = '\0';

	s32 modelId = getNpcModelId(npcid);
	if (-1 == modelId)
		return false;

	dbc::record r = CreatureModelDB->getByID((u32)modelId);
	if (!r.isValid())
		return false;

#if defined(WOW70)
	s32 fileId = r.getByte3(creatureModelDB::FileNameID);
	c8 filename[512];
	g_Engine->getWowDatabase()->getFilePath(fileId, filename, 512);
#elif defined(WOW60)
	s32 fileId = r.getInt(creatureModelDB::FileNameID);
	c8 filename[512];
	g_Engine->getWowDatabase()->getFilePath(fileId, filename, 512);
#else
	const c8* filename = r.getString(creatureModelDB::Filename);
#endif
	if (strlen(filename) < 4 || strlen(filename) > 255)
		return false;

	c8 ext[10];
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
		modelpath.subString(0, modelpath.length()-(u32)strlen(ext), tmppath);
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

const c8* wowDatabase::getAnimationName( u32 id )
{
	dbc::record r = AnimDB->getByID(id);
	if (r.isValid())
	{
		return r.getString(animDB::Name);
	}
	else
	{
		return "";
	}
}

u32 wowDatabase::getMaxSkinColor( u32 race, bool female, bool isHD )
{
	bool bHD = isRaceHasHD(race) && isHD;
	u32 num = CharSectionsDB->getColorsFor(race, (female ? 1 : 0), (bHD ? charSectionsDB::SkinTypeHD : charSectionsDB::SkinType), 0);
	ASSERT(num);
	return num-1;
}

u32 wowDatabase::getMaxFaceType( u32 race, bool female, bool isHD )
{
	bool bHD = isRaceHasHD(race) && isHD;
	u32 num = CharSectionsDB->getSectionsFor(race, (female ? 1 : 0), (bHD ? charSectionsDB::FaceTypeHD : charSectionsDB::FaceType), 0);
	ASSERT(num);
	return num-1;
}

u32 wowDatabase::getMaxHairColor( u32 race, bool female, bool isHD )
{
	bool bHD = isRaceHasHD(race) && isHD;
	u32 num = CharSectionsDB->getColorsFor(race, (female ? 1 : 0), (bHD ? charSectionsDB::HairTypeHD : charSectionsDB::HairType), 0);
	ASSERT(num);
	return num-1;
}

u32 wowDatabase::getMaxHairStyle( u32 race, bool female, bool isHD )
{
	bool bHD = isRaceHasHD(race) && isHD;
	u32 num = CharSectionsDB->getSectionsFor(race, (female ? 1 : 0), (bHD ? charSectionsDB::HairTypeHD : charSectionsDB::HairType), 0);
	ASSERT(num);
	return num-1;
}

u32 wowDatabase::getMaxFacialHairStyle( u32 race, bool female )
{
	u32 num = CharFacialHairDB->getStylesFor(race, female ? 1 : 0);
	ASSERT(num);
	return num-1;
}

void wowDatabase::getSubClassName( s32 id, s32 subid, c16* outname, u32 size )
{
	dbc::record  r = ItemSubClassDB->getById(id, subid);
	if (r.isValid() && id>0)
	{
		const c8* n = r.getString(itemSubClassDB::NameV400);
		utf8to16(n, outname, size);
	}
	else
	{
		memset(outname, 0, size * sizeof(u16));
	}
}

void wowDatabase::getNpcTypeName(s32 id, c16* outname, u32 size)
{
	dbc::record r = CreatureTypeDB->getByID(id);
	if (r.isValid())
	{
		const c8* n = r.getString(creatureTypeDB::Name);
		utf8to16(n, outname, size);
	}
	else
	{
		const c8* n = "Unknown";
		utf8to16(n, outname, size);
	}
}

const c8* wowDatabase::getClassShortName( u32 classId )
{
	dbc::record r = CharClassesDB->getByID(classId);
	if (!r.isValid())
		return "";
	return r.getString(charClassesDB::ShortName);
}

bool wowDatabase::getClassInfo( const c8* shortname, c16* classname, u32 size, u32* id )
{
	for (u32 i=0; i<CharClassesDB->getNumActualRecords(); ++i)
	{
		dbc::record r = CharClassesDB->getRecord(i);
		if(!r.isValid())
			continue;

		if (Q_stricmp(r.getString(charClassesDB::ShortName), shortname) == 0)
		{
			*id = r.getID();
			utf8to16(r.getString(charClassesDB::NameV400), classname, size);
			return true;
		}
	}

	*id = 0;
	memset(classname, 0, sizeof(c16) * size);
	return false;
}

bool wowDatabase::getSet( u32 index, s32& setid, c16* outname, u32 size )
{
	dbc::record r = ItemSetDB->getRecord(index);
	if (!r.isValid())
		return false;

	setid = r.getInt(0);
	const c8* n = r.getString(itemSetDB::Name);
	if (size <= strlen(n))
		return false;
	utf8to16(n, outname, size);
	return true;
}

bool wowDatabase::getItemVisualPath( s32 visualId, c8* path, u32 size )
{
	dbc::record rEff = ItemVisualEffectDB->getByID(visualId);
	if (!rEff.isValid())
		return false;

	const c8* filename = rEff.getString(itemVisualEffectDB::Model);
	if (!filename || strlen(filename) < 4 || strlen(filename) > 255)
		return false;

	string256 modelpath, tmppath;
	modelpath = filename;
	modelpath.subString(0, modelpath.length()-4, tmppath);
	tmppath.append(".M2");
	if (size <= tmppath.length())
		return false;

	Q_strcpy(path, size, tmppath.c_str());
	normalizeFileName(path);
	Q_strlwr(path);

	return true;
}

bool wowDatabase::getEffectVisualPath( s32 visualId, c8* path, u32 size )
{
	dbc::record rEff = SpellVisualEffectNameDB->getByID(visualId);
	if (!rEff.isValid())
		return false;

	const c8* filename = rEff.getString(spellVisualEffectNameDB::Model);
	if (!filename || strlen(filename) < 4 || strlen(filename) > 255)
		return false;

	string256 modelpath, tmppath;
	modelpath = filename;
	modelpath.subString(0, modelpath.length()-4, tmppath);
	tmppath.append(".M2");
	if (size <= tmppath.length())
		return false;

	Q_strcpy(path, size, tmppath.c_str());
	normalizeFileName(path);
	Q_strlwr(path);

	return true;
}

const SMapRecord* wowDatabase::getMap( u32 idx ) const
{
	if (idx >= mapCollections.maps.size())
		return NULL_PTR;
	return &mapCollections.maps[idx];
}

#ifdef MW_EDITOR

const SItemRecord* wowDatabase::getItem( u32 idx ) const
{
	if(itemCollections.items.size() <= idx)
		return NULL_PTR;
	return &itemCollections.items[idx]; 
}

const SNPCRecord* wowDatabase::getNPC( u32 idx ) const
{
	if(npcCollections.npcs.size() <= idx)
		return NULL_PTR;
	return &npcCollections.npcs[idx]; 
}

u32 wowDatabase::getNumStartOutfits( u32 race, bool female )
{
	return startOutfitClassCollections.getNumStartOutfits(race, female);
}

const SStartOutfitEntry* wowDatabase::getStartOutfit( u32 race, bool female, u32 idx )
{
	return startOutfitClassCollections.get(race, female, idx);
}

const c8* wowDatabase::getWmoFileName( u32 index ) const
{
	if (index >= wmoCollections.wmos.size())
		return NULL_PTR;
	return wmoCollections.wmos[index].c_str();
}

const c8* wowDatabase::getWorldModelFileName(u32 index) const
{
	if (index >= worldModelCollections.models.size())
		return NULL_PTR;
	return worldModelCollections.models[index].c_str();
}

const c8* wowDatabase::getTextureFileName( u32 index ) const
{
	if (index >= textureCollections.textures.size())
		return NULL_PTR;
	return textureCollections.textures[index].c_str();
}

const SRidable* wowDatabase::getRidable( u32 idx ) const
{
	if (idx >= ridableCollections.ridables.size())
		return NULL_PTR;
	return &ridableCollections.ridables[idx];
}

#endif

s32 wowDatabase::getItemDisplayId( s32 itemid ) const
{
#if defined(WOW60) || defined(WOW70)
	dbc::record r = getItemModifiedAppearanceDB()->getByItemID(itemid);
	if (!r.isValid())
		return -1;

	u32 appearanceId = r.getUInt(itemModifiedAppearanceDB::ItemAppearanceId);
	r = getItemAppearanceDB()->getByID(appearanceId);
	if (!r.isValid())
		return -1;

	return r.getInt(itemAppearanceDB::ItemDisplay);

#else
	dbc::record r = g_Engine->getWowDatabase()->getItemDB()->getByID(itemid);
	if (!r.isValid())
		return -1;
	return r.getInt(itemDB::ItemDisplayInfo);
#endif
}

void wowDatabase::getFilePath( s32 fileId, string256& path ) const
{
#if defined(WOW70)
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
#elif defined(WOW60)
	dbc::record r = getFileDataDB()->getByID(fileId);
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

void wowDatabase::getFilePath( s32 fileId, c8* path, u32 size ) const
{
#if defined(WOW70)
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
#elif defined(WOW60)
	dbc::record r = getFileDataDB()->getByID(fileId);
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

void wowDatabase::getTextureFilePath( s32 texId, string256& path ) const
{
#if defined(WOW70)
	dbc::record r = getTextureFileDataDB()->getByTextureId(texId);
	if (!r.isValid())
	{
		path.clear();
		return;
	}
	s32 fileId = r.getByte3(textureFileDataDB::Path);
	getFilePath(fileId, path);
#elif defined(WOW60)
	dbc::record r = getTextureFileDataDB()->getByTextureId(texId);
	if (!r.isValid())
	{
		path.clear();
		return;
	}
	s32 fileId = r.getInt(textureFileDataDB::Path);
	getFilePath(fileId, path);
#else
	ASSERT(false);
	path.clear();
#endif
}

void wowDatabase::getTextureFilePath( s32 texId, c8* path, u32 size ) const
{
#if defined(WOW70)
	dbc::record r = getTextureFileDataDB()->getByTextureId(texId);
	if (!r.isValid())
	{
		memset(path, 0, size);
		return;
	}
	s32 fileId = r.getByte3(textureFileDataDB::Path);
	getFilePath(fileId, path, size);
#elif defined(WOW60) 
	dbc::record r = getTextureFileDataDB()->getByTextureId(texId);
	if (!r.isValid())
	{
		memset(path, 0, size);
		return;
	}
	s32 fileId = r.getInt(textureFileDataDB::Path);
	getFilePath(fileId, path, size);
#else
	ASSERT(false);
	memset(path, 0, size);
#endif
}

void wowDatabase::getModelFilePath(s32 modelId, string256& path) const
{
#if defined(WOW70)
	dbc::record r = getModelFileDataDB()->getByModelId(modelId);
	if (!r.isValid())
	{
		path.clear();
		return;
	}
	s32 fileId = r.getByte3(modelFileDataDB::Path);
	getFilePath(fileId, path);
#else
	ASSERT(false);
	path.clear();
#endif
}

void wowDatabase::getModelFilePath(s32 modelId, c8* path, u32 size) const
{
#if defined(WOW70)
	dbc::record r = getModelFileDataDB()->getByModelId(modelId);
	if (!r.isValid())
	{
		memset(path, 0, size);
		return;
	}
	s32 fileId = r.getByte3(modelFileDataDB::Path);
	getFilePath(fileId, path, size);
#else
	ASSERT(false);
	memset(path, 0, size);
#endif
}

bool wowDatabase::isRaceHasHD( u32 race )
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

bool wowDatabase::getItemPath(s32 itemid, c8* modelpath, u32 modelSize, c8* texturepath, u32 texSize)
{
	u32 displayid = getItemDisplayId(itemid);
	dbc::record display = getItemDisplayDB()->getByID(displayid);
	if (!display.isValid())
		return false;

	string256 model1;
	string256 skin1;
#if defined(WOW70)
	getModelFilePath((s32)display.getInArray<u16>(itemDisplayDB::Model, 0), model1);
	getTextureFilePath((s32)display.getByte3InArray(itemDisplayDB::Skin, 0), skin1);
#elif defined(WOW60)
	model1 = display.getString(itemDisplayDB::Model);
	s32 skin1Id = display.getInt(itemDisplayDB::Skin);
	getTextureFilePath(skin1Id, skin1);
#else
	model1 = display.getString(itemDisplayDB::Model);
	skin1 = display.getString(itemDisplayDB::Skin);
#endif

	if(modelSize < (model1.length() + 1) || texSize < (skin1.length() + 1))
		return false;

	model1.changeExt("mdx", "m2");

	getFileNameA(model1.c_str(), modelpath, modelSize);
	getFileNameA(skin1.c_str(), texturepath, texSize);

	return true;
}



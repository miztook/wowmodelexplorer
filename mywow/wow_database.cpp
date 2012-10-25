#include "stdafx.h"
#include "mywow.h"

void g_callbackDBC(const c8* filename, void* param)
{
	dbc* db = new dbc((wowEnvironment*)param, filename);

	c8* buf = (c8*)Hunk_AllocateTempMemory(db->StringSize + 1);
	memcpy_s(buf, db->StringSize+1, db->_stringStart, db->StringSize);
	buf[db->StringSize] = '\0';

	char *p=buf;
	while (p<buf+db->StringSize) {

		const c8* str = p;
		if(strstr(str, "Durotar"))
		{
			MessageBoxA(NULL, str, filename, 0);
			break;
		}

		p+=strlen(p)+1;
	}
	Hunk_FreeTempMemory(buf);

	delete db;
}

wowDatabase::wowDatabase( wowEnvironment* env )
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
	ItemVisualsDB = new itemVisualsDB(env);
	ItemVisualEffectDB = new itemVisualEffectDB(env);
	StartOutFitDB = new startOutfitDB(env);
	WmoAreaTableDB = new wmoAreaTableDB(env);
	WorldMapAreaDB = new worldMapAreaDB(env);
	LightDB = new lightDB(env);
	LightSkyboxDB = new lightSkyboxDB(env);

	ItemDB = new itemDB(env);
	MapDB = new mapDB(env);
	SpellVisualEffectNameDB = new spellVisualEffectNameDB(env);

	//
	//env->iterateFiles("dbc", g_callbackDBC, env);
}

wowDatabase::~wowDatabase()
{
	delete SpellVisualEffectNameDB;
	delete MapDB;
	delete ItemDB;
	delete LightSkyboxDB;
	delete LightDB;

	delete WorldMapAreaDB;
	delete WmoAreaTableDB;
	delete StartOutFitDB;
	delete ItemVisualEffectDB;
	delete ItemVisualsDB;
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

bool wowDatabase::buildItems( const c8* filename )
{
	if (!itemCollections.open(ItemDB, ItemDisplayDB, filename))
		return false;

	return true;
}

bool wowDatabase::buildNpcs( const c8* filename )
{
	if (!npcCollections.open(filename))
		return false;

	return true;
}

bool wowDatabase::getRaceGender( const c8* filename, u32& race, u32& gender )
{
	char n[DEFAULT_SIZE];
	strcpy_s(n, DEFAULT_SIZE, filename);
	_strlwr_s(n, DEFAULT_SIZE);

	u32 len = strlen(n);

	if(strstr(n, "female"))
	{
		gender = 1;
		len -= 6;
	}
	else if (strstr(n, "male"))
	{
		gender = 0;
		len -= 4;
	}
	else
		return false;

	char raceName[DEFAULT_SIZE];
	strncpy_s(raceName, DEFAULT_SIZE, n, len);

	dbc::record r = CharRacesDB->getByName(raceName);
	if (!r.isValid())
		return false;
	race = r.getUInt(charRacesDB::ID);

	return true;
}

bool wowDatabase::getRaceId(const c8* raceName, u32& race)
{
	dbc::record r = CharRacesDB->getByName(raceName);
	if (!r.isValid())
		return false;
	race = r.getUInt(charRacesDB::ID);
	return true;
}

bool wowDatabase::getCharacterPath( const c8* raceName, bool female, c8* path, u32 size )
{
	dbc::record r = CharRacesDB->getByName(raceName);
	if (!r.isValid())
		return false;

	c8 genderString[10];
	if(female)
		strcpy_s(genderString, 10, "Female");
	else
		strcpy_s(genderString, 10, "Male");

	string256 modelpath;

	modelpath = "Character\\";
	modelpath.append(raceName);
	modelpath.append("\\");
	modelpath.append(genderString);
	modelpath.append("\\");
	modelpath.append(raceName);
	modelpath.append(genderString);
	modelpath.append(".m2");

	if (modelpath.length() > size)
		return false;

	strcpy_s(path, size, modelpath.c_str());

	return true;
}

bool wowDatabase::getNpcPath(s32 npcid, c8* path, u32 size)
{
	path[0] = '\0';

	const SNPCRecord* npcRec = npcCollections.getById(npcid);
	if (!npcRec)
		return false;

	dbc::record rSkin = CreatureDisplayInfoDB->getByID(npcRec->model);
	if (!rSkin.isValid())
		return false;

	u32 modelId = rSkin.getUInt(creatureDisplayInfoDB::ModelID);
	if (!modelId)
		return false;

	dbc::record r = CreatureModelDB->getByID(modelId);
	if (!r.isValid())
		return false;

	const c8* filename = r.getString(creatureModelDB::Filename);
	if (!filename || strlen(filename) < 4 || strlen(filename) > 255)
		return false;

	c8 ext[10];
	getFileExtensionA(filename, ext, 10);

	string256 modelpath;
	modelpath = filename;
	if (_stricmp(ext, "m2") != 0)
	{
		string256 tmppath;
		modelpath.subString(0, modelpath.length()-strlen(ext), tmppath);
		tmppath.append("M2");
		modelpath = tmppath;
	}

	if (size <= modelpath.length())
		return false;

	strcpy_s(path, size, modelpath.c_str());
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

u32 wowDatabase::getMaxSkinColor( u32 race, bool female )
{
	u32 num = CharSectionsDB->getColorsFor(race, female ? 1 : 0, charSectionsDB::SkinType, 0);
	_ASSERT(num);
	return num-1;
}

u32 wowDatabase::getMaxFaceType( u32 race, bool female )
{
	u32 num = CharSectionsDB->getSectionsFor(race, female ? 1 : 0, charSectionsDB::FaceType, 0);
	_ASSERT(num);
	return num-1;
}

u32 wowDatabase::getMaxHairColor( u32 race, bool female )
{
	u32 num = CharSectionsDB->getColorsFor(race, female ? 1 : 0, charSectionsDB::HairType, 0);
	_ASSERT(num);
	return num-1;
}

u32 wowDatabase::getMaxHairStyle( u32 race, bool female )
{
	u32 num = CharSectionsDB->getSectionsFor(race, female ? 1 : 0, charSectionsDB::HairType, 0);
	_ASSERT(num);
	return num-1;
}

u32 wowDatabase::getMaxFacialHairStyle( u32 race, bool female )
{
	u32 num = CharFacialHairDB->getStylesFor(race, female ? 1 : 0);
	_ASSERT(num);
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
		memset(outname, 0, size * sizeof(u16));
	}
}

SItemRecord* wowDatabase::getItem( u32 idx )
{
	if(itemCollections.items.size() <= idx)
		return NULL;
	return &itemCollections.items[idx]; 
}

SNPCRecord* wowDatabase::getNPC( u32 idx )
{
	if(npcCollections.npcs.size() <= idx)
		return NULL;
	return &npcCollections.npcs[idx]; 
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
			u8 race = r.getByte(startOutfitDB::Race);
			u8 gender =	r.getByte(startOutfitDB::Gender);

			u8 id = r.getByte(startOutfitDB::Class);
			dbc::record rec = CharClassesDB->getByID(id);
			if (rec.isValid())
			{
				SStartOutfitEntry entry;
				utf8to16(rec.getString(charClassesDB::NameV400), entry.name, DEFAULT_SIZE);
				entry.id = r.getUInt(startOutfitDB::ID);
				entry.race = race;
				entry.female = gender == 1;

				startOutfitClassCollections.startOutfits[i] = entry;
			}
		}
	}
}

void wowDatabase::buildMaps()
{
	u32 numMaps = MapDB->getNumRecords();
	for (u32 i=0; i<numMaps; ++i)
	{
		dbc::record r = MapDB->getRecord(i);

		SMapRecord m;
		m.id = r.getInt(dbc::ID);
		strcpy_s(m.internalname, DEFAULT_SIZE, r.getString(mapDB::InternalName));
		m.type = r.getInt(mapDB::AreaType);
		utf8to16(r.getString(mapDB::Name), m.name, DEFAULT_SIZE);

		mapCollections.mapLookup[m.id] = mapCollections.maps.size();
		mapCollections.maps.push_back(m);
	}

	u32 numAreas = AreaTableDB->getNumRecords();
	for (u32 i=0; i<numAreas; ++i)
	{
		dbc::record r = AreaTableDB->getRecord(i);

		SArea area;
		area.id = r.getInt(dbc::ID);
		area.mapId = r.getInt(areaTableDB::MapID);
		area.parentId = r.getInt(areaTableDB::ParentAreaTableID);
		utf8to16(r.getString(areaTableDB::Name), area.name, DEFAULT_SIZE);

		mapCollections.areaLookup[area.id] = mapCollections.areas.size();
		mapCollections.areas.push_back(area);

		SMapRecord* m = getMapById(area.mapId);
		if (m)
			m->areaList.push_back(area.id);
		else
			mapCollections.freeAreaList.push_back(area.id);
	}
}

u32 wowDatabase::getNumStartOutfits( u32 race, bool female )
{
	return startOutfitClassCollections.getNumStartOutfits(race, female);
}

const SStartOutfitEntry* wowDatabase::getStartOutfit( u32 race, bool female, u32 idx )
{
	return startOutfitClassCollections.get(race, female, idx);
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

	strcpy_s(path, size, tmppath.c_str());
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

	strcpy_s(path, size, tmppath.c_str());
	return true;
}

const SArea* wowDatabase::getMapArea( SMapRecord* map, u32 index )
{
	if (index >= map->areaList.size())
		return NULL;
	int areaId = map->areaList[index];
	return mapCollections.getAreaById(areaId);
}

SMapRecord* wowDatabase::getMap( u32 idx )
{
	if (idx >= mapCollections.maps.size())
		return NULL;
	return &mapCollections.maps[idx];
}

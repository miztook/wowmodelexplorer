#pragma once

#include "base.h"
#include <map>
#include <vector>

class wowEnvironment;

struct dbcHeader {
	/*0x00*/    char      _magic[4];	// "WDBC"
	/*0x04*/    u32  _nRecords;        // 	  
	/*0x08*/    u32  _nFields;      	// 
	/*0x0C*/    u32  _recordSize;      // 
	/*0x10*/    u32  _stringsize;   
};

class dbc 
{
public:
	dbc(wowEnvironment* env, const c8* filename);
	virtual ~dbc();

	static const size_t ID = 0;

public:
	class record
	{
	public:
		bool isValid() const { return _ofs != NULL; }

		f32 getFloat(u32 idx) const { return *reinterpret_cast<float*>(_ofs+idx*4); }
		u32 getUInt(u32 idx) const { return *reinterpret_cast<u32*>(_ofs+idx*4); }
		s32 getInt(u32 idx) const { return *reinterpret_cast<s32*>(_ofs+idx*4); }
		u8 getByte(u32 idx) const { return *reinterpret_cast<u8*>(_ofs+idx); }
		const c8* getString(u32 idx) const
		{
			_ASSERT(idx < _dbc->nFields);
			u32 offset = getUInt(idx);
			_ASSERT(offset < _dbc->StringSize);
					return reinterpret_cast<c8*>(_dbc->_stringStart + offset);
		}
	public:
		record(const dbc* d, u8* ofs) : _dbc(d), _ofs(ofs) {}
	private:
		friend class dbc;	
		u8* _ofs;
		const dbc* _dbc;
	};

public:
	dbc::record getRecord(u32 idx) const
	{
		_ASSERT(idx < nRecords);
		return dbc::record(this, _recordStart + idx * RecordSize);
	}

	dbc::record getByID(u32 id)
	{
		T_RecordLookup::iterator itr = RecordLookup.find(id);
		if ( itr == RecordLookup.end())
			return dbc::record(NULL, NULL);

		int i = itr->second;
		return getRecord(i);
	}

	u32 getNumRecords() const { return nRecords; }

protected:
	u8*		_data;
	u8*		_recordStart;
	u8*		_stringStart;

	u32		nFields;
	u32		nRecords;
	u32		RecordSize;
	u32		StringSize;

	typedef std::map<u32, u32, std::less<u32>, qzone_allocator<std::pair<u32, u32>>> T_RecordLookup;
	T_RecordLookup	RecordLookup;
};

/*
客户端用到的dbc
AnimationData
AreaPOI
AreaTable
AreaTrigger
AttackAnimKits
AttackAnimTypes
AuctionHouse
BankBagSlotPrices
BattlemasterList
CameraShakes
Cfg_Categories
Cfg_Configs
CharBaseInfo
CharHairGeosets
CharSections
CharStartOutfit
CharTitles
CharVariations
CharacterFacialHairStyles
ChatChannels
ChatProfanity
ChrClasses
ChrRaces
CinematicCamera
CinematicSequences
CreatureDisplayInfo
CreatureDisplayInfoExtra
CreatureFamily
CreatureModelData
CreatureSoundData
CreatureSpellData
CreatureType
DeathThudLookups
DurabilityCosts
DurabilityQuality
Emotes
EmotesText
EmotesTextData
EmotesTextSound
EnvironmentalDamage
Exhaustion
Faction
FactionGroup
FactionTemplate
FootprintTextures
FootstepTerrainLookup
GameObjectArtKit
GameObjectDisplayInfo
GameTables
GameTips
GemProperties
GMSurveyCurrentSurvey
GMSurveyQuestions
GMSurveySurveys
GMTicketCategory
GroundEffectDoodad
GroundEffectTexture
gtCombatRatings
gtChanceToMeleeCrit
gtChanceToMeleeCritBase
gtChanceToSpellCrit
gtChanceToSpellCritBase
gtOCTRegenHP
gtOCTRegenMP
gtRegenHPPerSpt
gtRegenMPPerSpt
HelmetGeosetVisData
ItemBagFamily
ItemClass
ItemDisplayInfo
ItemExtendedCost
ItemGroupSounds
ItemPetFood
ItemRandomProperties
ItemRandomSuffix
ItemSet
ItemSubClass
ItemSubClassMask
ItemVisualEffects
ItemVisuals
LanguageWords
Languages
LfgDungeons
Light
LightFloatBand
LightIntBand
LightParams
LightSkybox
LiquidType
LoadingScreens
LoadingScreenTaxiSplines
Lock
LockType
MailTemplate
Map
Material
NameGen
NpcSounds
NamesProfanity
NamesReserved
Package
PageTextMaterial
PaperDollItemFrame
PetLoyalty
PetPersonality
QuestInfo
QuestSort
Resistances
RandPropPoints
ServerMessages
SheatheSoundLookups
SkillCostsData
SkillLineAbility
SkillLineCategory
SkillLine
SkillRaceClassInfo
SkillTiers
SoundAmbience
SoundEntries
SoundProviderPreferences
SoundSamplePreferences
SoundWaterType
SpamMessages
SpellCastTimes
SpellCategory
SpellChainEffects
Spell
SpellDispelType
SpellDuration
SpellEffectCameraShakes
SpellFocusObject
SpellIcon
SpellItemEnchantment
SpellItemEnchantmentCondition
SpellMechanic
SpellMissleMotion
SpellRadius
SpellRange
SpellShapeshiftForm
SpellVisual
SpellVisualEffectName
SpellVisualKit
StableSlotPrices
Stationery
StringLookups
Talent
TalentTab
TaxiNodes
TaxiPath
TaxiPathNode
TerrainType
TerrainTypeSounds
TotemCategory
TransportAnimation
UISoundLookups
UnitBlood
UnitBloodLevels
VocalUISounds
WMOAreaTable
WeaponSwingSounds
Weather
WorldMapArea
WorldMapTransforms
WorldMapContinent
WorldMapOverlay
WorldSafeLocs
WorldStateUI
ZoneIntroMusicTable
ZoneMusic

*/

class animDB;							//			AnimationData.dbc
class areaTableDB;					//			AreaTable.dbc
class charClassesDB;					//			ChrClasses.dbc
class charFacialHairDB;					//			CharacterFacialHairStyles.dbc
class charHairGeosetsDB;				//			CharHairGeosets.dbc
class charRacesDB;						//			ChrRaces.dbc
class charSectionsDB;					//			CharSections.dbc
class creatureTypeDB;					//			CreatureType.dbc
class creatureModelDB;				//			CreatureModelData.dbc
class creatureDisplayInfoDB;					//			CreatureDisplayInfo.dbc
class creatureDisplayInfoExtraDB;			//			CreatureDisplayInfoExtra.dbc
class helmGeosetDB;					//			HelmetGeosetVisData.dbc
class itemDisplayDB;				//			ItemDisplayInfo.dbc
class itemSetDB;					//			ItemSet.dbc
class itemSubClassDB;					//			ItemSubClass.dbc
class itemVisualsDB;						//			ItemVisuals.dbc
class itemVisualEffectDB;			//			ItemVisualEffects.dbc	
class startOutfitDB;					//			CharStartOutfit.dbc
class lightSkyboxDB;						//		LightSkybox.dbc
class itemVisualsDB;					//		ItemVisuals.dbc
class itemDB;									//		Item.dbc
class mapDB;									//		Map.dbc
class spellVisualEffectNameDB;					//		SpellVisualEffectName.dbc
class wmoAreaTableDB;				//		WmoAreaTable.dbc
class worldMapAreaDB;					//WorldMapArea.dbc

class animDB : public dbc
{
public:
	animDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\AnimationData.dbc") {}

public:
	static const size_t AnimID = 0;		// uint
	static const size_t Name = 1;		// string
	static const size_t WeaponFlags = 2;	//32 = pull weapons out during animation. 16 and 4 weapons are put back.
};

class areaTableDB : public dbc
{
public:
	areaTableDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\AreaTable.dbc") {}

	static const size_t MapID = 1;
	static const size_t ParentAreaTableID = 2;
	static const size_t ExplorationLevel = 10;
	static const size_t Name = 11;

};

class charClassesDB: public dbc
{
public:
	charClassesDB(wowEnvironment* env): dbc(env, "DBFilesClient\\ChrClasses.dbc") {}

	/// Fields
	static const size_t Name = 4;		// string, localization - english name
	static const size_t NameV400 = 3;	// string, localization - english name
};

class charFacialHairDB : public dbc
{
public:
	charFacialHairDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\CharacterFacialHairStyles.dbc") {}

	dbc::record getByParams(unsigned int race, unsigned int gender, unsigned int style);
	size_t getStylesFor(unsigned int race, unsigned int gender);

	static const size_t RaceV400 = 1;				// uint
	static const size_t GenderV400 = 2;				// uint
	static const size_t StyleV400 = 3;				// uint
	static const size_t Geoset100V400 = 4;			// uint
	static const size_t Geoset300V400 = 5;			// uint
	static const size_t Geoset200V400 = 6;			// uint
};

class charHairGeosetsDB : public dbc
{
public:
	charHairGeosetsDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\CharHairGeosets.dbc") {}

	/// Fields
	static const size_t Race = 1;				// uint
	static const size_t Gender = 2;				// uint, 0 = Male, 1 = Female
	static const size_t Section = 3;			// uint, ID unique between race, and gender.
	static const size_t Geoset = 4;				// uint, Defines hairstyle, each number should be unique for that race / gender combo.

	dbc::record getByParams(unsigned int race, unsigned int gender, unsigned int section);
	size_t getGeosetsFor(unsigned int race, unsigned int gender);
};

class charRacesDB : public dbc
{
public:
	charRacesDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\ChrRaces.dbc") {}

	static const size_t maleModeID = 4;		// unit
	static const size_t femaleModeID = 5;	// unit
	static const size_t ShortName = 6;		// string, Name, represented by only 2 chars
	static const size_t Name = 11;			// string, Model name, 10048 to 11

	dbc::record getByName(const c8* name);
};

class charSectionsDB : public dbc
{
public:
	charSectionsDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\CharSections.dbc") {}
	static const size_t Race = 1;		// uint
	static const size_t Gender = 2;		// uint
	static const size_t Type = 3;		// uint

	static const size_t Tex1 = 4;		// string
	static const size_t Tex2 = 5;		// string
	static const size_t Tex3 = 6;		// string
	static const size_t IsNPC = 7;		// Flags, uint, IsNPC = 0x1 ?, IsDeathKnight?
	static const size_t Section = 8;	// uint
	static const size_t Color = 9;		// uint

	/// Types
	static const size_t SkinType = 0;
	static const size_t FaceType = 1;
	static const size_t FacialHairType = 2;
	static const size_t HairType = 3;
	static const size_t UnderwearType = 4;

	dbc::record getByParams(size_t race, size_t gender, size_t type, size_t section, size_t color);
	size_t getColorsFor(size_t race, size_t gender, size_t type, size_t section);
	size_t getSectionsFor(size_t race, size_t gender, size_t type, size_t color);
};

class creatureTypeDB : public dbc
{
public:
	creatureTypeDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\CreatureType.dbc") {}

	static const size_t Name = 1;		// string
};

class creatureModelDB : public dbc
{
public:
	creatureModelDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\CreatureModelData.dbc") {}

	/// Fields
	static const size_t Type = 1;			// uint
	static const size_t Filename = 2;		// string
};

class creatureDisplayInfoDB : public dbc
{
public:
	creatureDisplayInfoDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\CreatureDisplayInfo.dbc") {}

	/// Fields
	static const size_t ModelID = 1;		// uint
	static const size_t NPCID = 3;			// uint CreatureDisplayInfoExtraID
	static const size_t Skin = 6;			// string
};

class creatureDisplayInfoExtraDB : public dbc
{
public:
	creatureDisplayInfoExtraDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\CreatureDisplayInfoExtra.dbc") {}

	/// Fields
	static const size_t RaceID = 1;			// uint
	static const size_t Gender = 2;			// bool
	static const size_t SkinColor = 3;		// uint
	static const size_t Face = 4;			// uint
	static const size_t HairStyle = 5;		// uint
	static const size_t HairColor = 6;		// uint
	static const size_t FacialHair = 7;		// uint
	static const size_t HelmID = 8;			// uint, Slot1
	static const size_t ShoulderID = 9;		// uint, Slot3
	static const size_t ShirtID = 10;		// uint, Slot4
	static const size_t ChestID = 11;		// uint, Slot5
	static const size_t BeltID = 12;		// uint, Slot6
	static const size_t PantsID = 13;		// uint, Slot7
	static const size_t BootsID = 14;		// uint, Slot8
	static const size_t BracersID = 15;		// uint, Slot9
	static const size_t GlovesID = 16;		// uint, Slot10
	static const size_t TabardID = 17;		// uint, Slot19
	static const size_t CapeID = 18;		// uint, Slot16
	//static const size_t CanEquip = 19;		// bool
	static const size_t Filename = 20;		// string. an index offset to the filename.

};

class helmGeosetDB : public dbc
{
public:
	helmGeosetDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\HelmetGeosetVisData.dbc") {}

	static const size_t Hair = 1;		// int Hair, 0 = show, anything else = don't show? eg: a value of 1020 won't hide night elf ears, but 999999 or -1 will.
	static const size_t Facial1Flags = 2;		// int Beard or Tusks
	static const size_t Facial2Flags = 3;		// int Earring
	static const size_t Facial3Flags = 4;		// int, See ChrRaces, column 24 to 26 for information on what is what.
	static const size_t EarsFlags = 5;		// int Ears
};

class itemDisplayDB : public dbc
{
public:
	itemDisplayDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\ItemDisplayInfo.dbc") { }

	static const size_t Model = 1;			// string, modelleft
	static const size_t Model2 = 2;			// string, modelright
	static const size_t Skin = 3;			// string, textureleft
	static const size_t Skin2 = 4;			// string, textureright
	//static const size_t Icon = 5;			// string
	//static const size_t Texture = 6;			// string
	static const size_t GloveGeosetFlags = 7;		// uint, (0,1,2,3,4,5)
	static const size_t BracerGeosetFlags = 8;		// uint, (0,1,2,3)
	static const size_t RobeGeosetFlags = 9;		// uint, (0,1)
	static const size_t BootsGeosetFlags = 10;		// uint, (0,1,2,4,6)
	//static const size_t Unknown = 11;		// uint
	//static const size_t ItemGroupSounds = 12;			// uint
	static const size_t GeosetVisID1 = 13;	// uint, HelmetGeosetVisData.dbc
	static const size_t GeosetVisID2 = 14;	// uint, HelmetGeosetVisData.dbc
	static const size_t TexArmUpper = 15;	// string
	static const size_t TexArmLower = 16;	// string
	static const size_t TexHands = 17;		// string
	static const size_t TexChestUpper = 18;	// string
	static const size_t TexChestLower = 19;	// string
	static const size_t TexLegUpper = 20;	// string
	static const size_t TexLegLower = 21;	// string
	static const size_t TexFeet = 22;		// string
	static const size_t Visuals = 23;		// uint
	//static const size_t ParticleColor = 24;	// uint

};

class itemSetDB : public dbc
{
public:
	itemSetDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\ItemSet.dbc") {}

	static const size_t NumItems = 17;

	/// Fields
	static const size_t Name = 1;	// string, Localization
	static const size_t ItemIDBase = 18; // 10 * uint
	static const size_t ItemIDBaseV400 = 2; // 10 * uint
};

class itemSubClassDB : public dbc
{
public:
	itemSubClassDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\ItemSubClass.dbc") {}

	static const size_t ClassIDV400 = 1;	// int
	static const size_t SubClassIDV400 = 2;	// int
	static const size_t HandsV400 = 10;		// int
	static const size_t NameV400 = 11;		// string

	dbc::record getById(int id, int subid);
};

class startOutfitDB : public dbc
{
public:
	startOutfitDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\CharStartOutfit.dbc") {}

	static const size_t NumItems = 24;

	/// Fields
	static const size_t Race = 4;	// byte offset
	static const size_t Class = 5;	// byte offset
	static const size_t Gender = 6;	// byte offset
	static const size_t ItemIDBase = 2; // 24 * uint
};

class lightSkyboxDB : public dbc
{
public:
	lightSkyboxDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\LightSkybox.dbc") {}

	static const size_t Name = 1;		// string
};

class itemVisualsDB : public dbc
{
public:
	itemVisualsDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\ItemVisuals.dbc") {}

};

class itemVisualEffectDB : public dbc
{
public:
	itemVisualEffectDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\ItemVisualEffects.dbc") {}

	static const size_t Model = 1;		// string
};

class itemDB : public dbc
{
public:
	itemDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\Item.dbc") { }

	static const size_t MaxItem = 100000;

	// Fields
	static const size_t Itemclass = 1;	// unit
	static const size_t Subclass = 2;	// unit
	// static const size_t materialid = 4;	// uint
	static const size_t ItemDisplayInfo = 5;	// unit
	static const size_t InventorySlot = 6;	// unit
	static const size_t Sheath = 7;	// unit
};

class mapDB : public dbc
{
public:
	mapDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\Map.dbc") {}

	static const size_t InternalName = 1;		// string
	static const size_t AreaType = 2;		// int
	static const size_t AreaTableID = 4;			// int
	static const size_t Name = 6;
};

class spellVisualEffectNameDB : public dbc
{
public:
	spellVisualEffectNameDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\SpellVisualEffectName.dbc") {}

	static const size_t Name = 1;		// string
	static const size_t Model = 2;
};

class wmoAreaTableDB : public dbc
{
public:
	wmoAreaTableDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\WmoAreaTable.dbc") {}
	
	static const size_t WMOID = 1;
	static const size_t WMOGroupID = 2;
	static const size_t Name = 11;
};

class worldMapAreaDB : public dbc
{
public:
	worldMapAreaDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\WorldMapArea.dbc") {}

	static const size_t AreaTableID = 2;
	static const size_t Name = 3;
	static const size_t Left = 4;
	static const size_t Right = 5;
	static const size_t Top = 6;
	static const size_t Bottom = 7;
};

//物品和npc需要建两个单独的内存数据
struct SItemRecord
{
	c16 name[DEFAULT_SIZE];
	s32 id, itemclass, subclass, type, model, sheath, quality;

	bool init(itemDB* itemDb, const c8* line);

	const bool operator<(const SItemRecord& r) const
	{
		if (type == r.type)
			return _wcsicmp(name, r.name) < 0;
		else
			return type < r.type;
	}
};

class ItemCollections
{
public:
	std::vector<SItemRecord>	items;

	typedef std::map<s32, s32, std::less<s32>, qzone_allocator<std::pair<s32, s32>>>		T_itemLookup;
	T_itemLookup		itemLookup;				//item id 到 index

	ItemCollections()
	{
		items.reserve(1024);
	}

	bool open(itemDB* itemDb, itemDisplayDB* itemDisplay, const c8* filename);

	SItemRecord* getById(s32 id);
};


struct SNPCRecord
{
	c16 name[DEFAULT_SIZE];
	s32 id, model, type;

	SNPCRecord(const c8* line);

	const bool operator<(const SNPCRecord& r) const
	{
		if (type == r.type)
			return _wcsicmp(name, r.name) < 0;
		else
			return type < r.type;
	}
};

class NPCCollections
{
public:
	NPCCollections() { npcs.reserve(1024); }

public:
	std::vector<SNPCRecord>	npcs;

	typedef std::map<s32, s32, std::less<s32>, qzone_allocator<std::pair<s32, s32>>>		T_npcLookup;
	T_npcLookup		npcLookup;				//npc id 到 index

	bool open(const c8* filename);

	SNPCRecord* getById(s32 id);
};

struct SStartOutfitEntry
{
	u32 id;
	u8 race;
	bool female;
	c16 name[DEFAULT_SIZE];
};

class StartOutfitClassCollections
{
public:
	u32 getNumStartOutfits(u32 race, bool female);
	const SStartOutfitEntry* get(u32 race, bool female, u32 idx);

	std::vector<SStartOutfitEntry> startOutfits;
};

struct SArea
{
	int id;
	int mapId;
	int parentId;
	c16 name[DEFAULT_SIZE];
};

struct SWowMapArea
{
	int		id;
	int		areaId;
	c8			name[DEFAULT_SIZE];
	f32		locLeft;
	f32		locRight;
	f32		locTop;
	f32		locBottom;
};

struct SMapRecord
{
	s32 id;
	c8 internalname[DEFAULT_SIZE];
	c16 name[DEFAULT_SIZE];
	int type;

	std::vector<s32>	areaList;	
};

class MapCollections
{
public:
	std::vector<SMapRecord>	maps;
	std::vector<SArea>		areas;

	typedef std::map<s32, s32, std::less<s32>, qzone_allocator<std::pair<s32, s32>>>		T_mapLookup;
	T_mapLookup		mapLookup;

	typedef std::map<s32, s32, std::less<s32>, qzone_allocator<std::pair<s32, s32>>>		T_areaLookup;
	T_areaLookup		areaLookup;

	SMapRecord* getMapById(s32 id);
	SArea* getAreaById(s32 id);

	std::vector<s32>		freeAreaList;				//不属于map的area
};

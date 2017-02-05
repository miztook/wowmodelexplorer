#pragma once

#include "base.h"
#include "fixstring.h"
#include <unordered_map>
#include <map>
#include <vector>

class wowEnvironment;
class IMemFile;

#	pragma pack (4)

struct dbcHeader {
	/*0x00*/    char      _magic[4];	// "WDBC"
	/*0x04*/    u32  _nRecords;        // 	  
	/*0x08*/    u32  _nFields;      	// 
	/*0x0C*/    u32  _recordSize;      // 
	/*0x10*/    u32  _stringsize;   
};

struct db2Header
{
	/*0x00*/    char      _magic[4];	// "WDB2"
	/*0x04*/    u32  _nRecords;        // 	  
	/*0x08*/    u32  _nFields;      	// 
	/*0x0C*/    u32  _recordSize;      // 
	/*0x10*/    u32  _stringsize;   
	u32 hash;
	u32 clientminor;		//version
	u32 unknown1;
	u32 firstrow;
	u32 lastrow;
	u32 localecode;
	u32 refdatasize;
};

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
	u32 firstrow;
	u32 lastrow;
	s32 localecode;
	u32 refdatasize;
	u16 fileflags;		
	u16 idindex;
};

#	pragma pack ()

class dbc 
{
private:
	DISALLOW_COPY_AND_ASSIGN(dbc);

public:
	dbc(wowEnvironment* env, const c8* filename, bool tmp=false);
	virtual ~dbc();

protected:
	void readWDBC(wowEnvironment* env, IMemFile* file, bool tmp);
	void readWDB2(wowEnvironment* env, IMemFile* file, bool tmp);
	void readWDB5(wowEnvironment* env, IMemFile* file, bool tmp);

public:
	class record
	{
	public:
		static record EMPTY() { static record m(NULL_PTR, -1, NULL_PTR, 0); return m; }

		bool isValid() const { return _ofs != NULL_PTR; }

		u32 getID() const
		{
			if (!_dbc->Fields)
				return getUInt(0);

			ASSERT(_dbc->hasIndex() && _index >= 0);
			return _dbc->getIDs()[_index];
		}

		f32 getFloat(u32 idx) const 
		{
			ASSERT(idx < _dbc->getNumFields());
			if(!_dbc->Fields)
				return *reinterpret_cast<float*>(_ofs+idx*4); 

			ASSERT(idx < _dbc->getNumFields());
			ASSERT(_dbc->Fields[idx].size == sizeof(float));
			return *reinterpret_cast<float*>(_ofs + _dbc->Fields[idx].position);
		}

		u32 getUInt(u32 idx) const 
		{
			ASSERT(idx < _dbc->getNumFields());

			if(!_dbc->Fields)
				return *reinterpret_cast<u32*>(_ofs+idx*4); 

			ASSERT(idx < _dbc->getNumFields());
			u32 ret = 0;
			if(_dbc->Fields[idx].size == sizeof(u32))
				ret = *reinterpret_cast<u32*>(_ofs + _dbc->Fields[idx].position);
			else if (_dbc->Fields[idx].size == sizeof(u16))
				ret = *reinterpret_cast<u16*>(_ofs + _dbc->Fields[idx].position); 
			else if (_dbc->Fields[idx].size == sizeof(u8))
				ret = *reinterpret_cast<u8*>(_ofs + _dbc->Fields[idx].position);
			else
				ASSERT(false);
			return ret;
		}

		s32 getByte3(u32 idx) const
		{
			ASSERT(_dbc->Fields); 
			ASSERT(_dbc->Fields[idx].size == 3);

			u8* bytes = getRawPointer<u8>(idx);
			return (s32)((u32)bytes[0] + ((u32)bytes[1] << 8) + ((u32)bytes[2] << 16));
		}

		s32 getByte3InArray(u32 idx, u32 arrayIndex) const
		{
			ASSERT(_dbc->Fields); 

			u8* bytes = getRawPointer<u8>(idx);
			bytes += arrayIndex * 3;
			return (s32)((u32)bytes[0] + ((u32)bytes[1] << 8) + ((u32)bytes[2] << 16));
		}

		s32 getInt(u32 idx) const 
		{ 
			ASSERT(idx < _dbc->getNumFields());
			
			if(!_dbc->Fields)
				return *reinterpret_cast<s32*>(_ofs+idx*4); 

			ASSERT(idx < _dbc->getNumFields());
			s32 ret = 0;
			if(_dbc->Fields[idx].size == sizeof(s32))
				ret = *reinterpret_cast<s32*>(_ofs + _dbc->Fields[idx].position);
			else if (_dbc->Fields[idx].size == sizeof(s16))
				ret = *reinterpret_cast<s16*>(_ofs + _dbc->Fields[idx].position); 
			else if (_dbc->Fields[idx].size == sizeof(s8))
				ret = *reinterpret_cast<s8*>(_ofs + _dbc->Fields[idx].position);
			else
				ASSERT(false);
			return ret;
		}

		u8 getByte(u32 idx) const
		{
			ASSERT(idx < _dbc->getNumFields());

			if(!_dbc->Fields)
				return *reinterpret_cast<u8*>(_ofs+ idx); 

			ASSERT(idx < _dbc->getNumFields());
			ASSERT(_dbc->Fields[idx].size == sizeof(u8));
			return *reinterpret_cast<u8*>(_ofs + _dbc->Fields[idx].position);
		}

		template<class T>
		T* getRawPointer(u32 idx) const
		{
			ASSERT(idx < _dbc->getNumFields());

			if(!_dbc->Fields)
				return reinterpret_cast<T*>(_ofs + idx * sizeof(T)); 

			ASSERT(idx < _dbc->getNumFields());
			return reinterpret_cast<T*>(_ofs + _dbc->Fields[idx].position);
		}

		template<class T>
		T getInArray(u32 idx, u32 arrayIndex) const
		{
			ASSERT(idx < _dbc->getNumFields());

			if(!_dbc->Fields)
				return (reinterpret_cast<T*>(_ofs + idx * sizeof(T)))[arrayIndex]; 

			ASSERT(idx < _dbc->getNumFields());
			return (reinterpret_cast<T*>(_ofs + _dbc->Fields[idx].position))[arrayIndex];
		}

		const c8* getString(u32 idx) const
		{
			ASSERT(idx < _dbc->getNumFields());
			
			u32 offset = getUInt(idx);
			ASSERT(offset < _dbc->getStringSize());
			return reinterpret_cast<c8*>(_dbc->_stringStart + offset);
		}

	public:
		record(const dbc* d, u32 index, u8* ofs, u16 size) : _ofs(ofs), _index(index), _size(size), _dbc(d)  {}
	private:
		friend class dbc;	
		u8* _ofs;
		u16 _size;
		u32 _index;
		const dbc* _dbc;
	};

public:
	dbc::record getRecord(u32 idx) const
	{
		ASSERT(idx < nRecords);

		if(HasDataOffsetBlock)
			return record(this, idx, _recordStart + OffsetMaps[idx].offset, OffsetMaps[idx].length);
		else
			return record(this, idx, _recordStart + idx * RecordSize, RecordSize);
	}

	dbc::record getByID(u32 id) const
	{
		T_RecordLookup32::const_iterator itr = RecordLookup32.find(id);
		if ( itr == RecordLookup32.end())
			return record::EMPTY();

		u32 i = itr->second;
		return getRecord(i);
	}

	u32 getNumRecords() const { return nRecords; }
	u32 getNumActualRecords() const { return nActualRecords; }
	u32 getNumFields() const { return nFields; }
	u32 getRecordSize() const { return RecordSize; }
	u32 getStringSize() const { return HasDataOffsetBlock ? 0 : StringSize; }
	u8* getStringStart() const { return _stringStart; }
	u16 getFieldSize(int idx) const { return Fields ? Fields[idx].size : 4; }
	const u32* getIDs() const { return IDs; }
	bool isSparse() const { return IsSparse; }
	bool hasIndex() const { return HasIndex; }

	//for sparse db2
	s32 getRecordSparseRow(u32 index) const
	{
		T_RecordSparseLookup32::const_iterator itr = RecordSparseLookup32.find(index);
		if (itr == RecordSparseLookup32.end())
			return -1;

		return (s32)itr->second;
	}

protected:
	u8*		_recordStart;
	u8*		_stringStart;

	u32		nFields;
	u32		nRecords;
	u32		RecordSize;
	u32		StringSize;
	u32		nActualRecords;

	u32		minorVersion;

#ifdef USE_QALLOCATOR
	typedef std::map<u32, u32, std::less<u32>, qzone_allocator<std::pair<u32, u32>>> T_RecordLookup32;
	typedef std::map<u32, u32, std::less<u32>, qzone_allocator<std::pair<u32, u32>>> T_RecordSparseLookup32;
#else
	typedef std::unordered_map<u32, u32> T_RecordLookup32;
	typedef std::unordered_map<u32, u32> T_RecordSparseLookup32;
#endif

	T_RecordLookup32		RecordLookup32;
protected:		//WDB2
	T_RecordSparseLookup32			RecordSparseLookup32;
	bool	IsSparse;

protected:	//WDB5
	bool	HasDataOffsetBlock;
	bool	HasUnknownStuff;
	bool	HasIndex;

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
	SOffsetMapEntry*	OffsetMaps;  //if (flags & 0x01 != 0) [header.max_id - header.min_id + 1];
	u32* IDs;		//if (flags & 0x04 != 0) [header.record_count]
	SCopyTableEntry* CopyTables;		//if (header.refdatasize > 0)
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
class charComponentTextureLayoutsDB;		//		CharComponentTextureLayouts.dbc
class charComponentTextureSectionsDB;		//		CharComponentTextureSections.dbc
class charRacesDB;						//			ChrRaces.dbc
class charSectionsDB;					//			CharSections.dbc
class creatureTypeDB;					//			CreatureType.dbc
class creatureModelDB;				//			CreatureModelData.dbc
class creatureDisplayInfoDB;					//			CreatureDisplayInfo.dbc
class creatureDisplayInfoExtraDB;			//			CreatureDisplayInfoExtra.dbc
class npcModelItemSlotDisplayInfoDB;			//		NpcModelItemSlotDisplayInfo.dbc
class helmGeosetDB;					//			HelmetGeosetVisData.dbc
class itemDisplayDB;				//			ItemDisplayInfo.dbc
class itemDisplayInfoMaterialResDB;		//		ItemDisplayInfoMaterialRes.dbc
class itemSetDB;					//			ItemSet.dbc
class itemSubClassDB;					//			ItemSubClass.dbc
class itemVisualsDB;						//			ItemVisuals.dbc
class itemVisualEffectDB;			//			ItemVisualEffects.dbc	
class startOutfitDB;					//			CharStartOutfit.dbc
class lightDB;								//			Light.dbc
class lightIntBandDB;					//		LightIntBand.dbc
class lightSkyboxDB;						//		LightSkybox.dbc
class itemVisualsDB;					//		ItemVisuals.dbc
class itemDB;									//		Item.db2
class itemSparseDB;							//		Item-Sparse.db2
class itemModifiedAppearanceDB;			//ItemModifiedAppearance.db2
class itemAppearanceDB;			//ItemAppearance.db2
class textureFileDataDB;			//TextureFieldData.db2
class modelFileDataDB;				//ModelFileData
class fileDataDB;				//FileData.dbc
class mapDB;									//		Map.dbc
class spellVisualEffectNameDB;					//		SpellVisualEffectName.dbc
class wmoAreaTableDB;				//		WmoAreaTable.dbc
class worldMapAreaDB;					//WorldMapArea.dbc

class animDB : public dbc
{
public:
	animDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\AnimationData.dbc") {}

public:

#ifdef WOW70
	static const u32 Name = 0;		// string
#else
	static const u32 Name = 1;		// string
#endif
};

class areaTableDB : public dbc
{
public:
	areaTableDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\AreaTable.dbc") {}

	static const u32 MapID = 1;
	static const u32 ParentAreaTableID = 2;
	static const u32 ExplorationLevel = 10;
	static const u32 Name = 13;

};

class charClassesDB: public dbc
{
public:
	charClassesDB(wowEnvironment* env): dbc(env, "DBFilesClient\\ChrClasses.dbc") {}

#ifdef WOW70
	static const u32 NameV400 = 3;		//string
	static const u32 ShortName = 4;
#else
	static const u32 NameV400 = 3;
	static const u32 ShortName = 6;
#endif
};

class charFacialHairDB : public dbc
{
public:
	charFacialHairDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\CharacterFacialHairStyles.dbc") {}

	dbc::record getByParams(unsigned int race, unsigned int gender, unsigned int style) const;
	u32 getStylesFor(unsigned int race, unsigned int gender) const;



#ifdef WOW70
	static const u32 RaceV400 = 1;				// byte
	static const u32 GenderV400 = 2;				// byte
	static const u32 StyleV400 = 3;				// byte
	static const u32 Geoset100V400 = 0;			// uint array
	static const u32 Geoset300V400 = 1;			// uint
	static const u32 Geoset200V400 = 2;			// uint
#else
	static const u32 RaceV400 = 1;				// uint
	static const u32 GenderV400 = 2;				// uint
	static const u32 StyleV400 = 3;				// uint
	static const u32 Geoset100V400 = 4;			// uint array
	static const u32 Geoset300V400 = 1;			// uint
	static const u32 Geoset200V400 = 2;			// uint
#endif
};

class charHairGeosetsDB : public dbc
{
public:
	charHairGeosetsDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\CharHairGeosets.dbc") {}

#ifdef WOW70
	static const u32 Race = 1;				// byte
	static const u32 Gender = 2;				// byte
	static const u32 Section = 3;			// byte
	static const u32 Geoset = 5;				// byte
	static const u32 ShowScalp = 7;			// byte
#else
	static const u32 Race = 1;				// uint
	static const u32 Gender = 2;				// uint, 0 = Male, 1 = Female
	static const u32 Section = 3;			// uint, ID unique between race, and gender.
	static const u32 Geoset = 5;				// uint, Defines hairstyle, each number should be unique for that race / gender combo.
	static const u32 ShowScalp = 7;			// uint
#endif

	dbc::record getByParams(unsigned int race, unsigned int gender, unsigned int section) const;
	u32 getGeosetsFor(unsigned int race, unsigned int gender) const;
};

class charComponentTextureLayoutsDB : public dbc
{
public:
	charComponentTextureLayoutsDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\CharComponentTextureLayouts.dbc") {}

#ifdef WOW70
	static const u32 Width = 0;
	static const u32 Height = 1;
#else
	static const u32 Width = 1;
	static const u32 Height = 2;
#endif
};

class charComponentTextureSectionsDB : public dbc
{
public:
	charComponentTextureSectionsDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\CharComponentTextureSections.dbc") {}

#ifdef WOW70
	static const u32 LayoutID = 4;
	static const u32 Section = 5;
	static const u32 X = 0;
	static const u32 Y = 1;
	static const u32 Width = 2;
	static const u32 Height = 3;
#else
	static const u32 LayoutID = 1;
	static const u32 Section = 2;
	static const u32 X = 3;
	static const u32 Y = 4;
	static const u32 Width = 5;
	static const u32 Height = 6;
#endif

};

class charRacesDB : public dbc
{
public:
	charRacesDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\ChrRaces.dbc") {}

#ifdef WOW70
	static const u32 ShortName = 1;		// string, Name, represented by only 2 chars
	static const u32 Name = 2;			// string, Model name, 10048 to 11
	static const u32 maleModelID = 15;
	static const u32 femaleModelID = 16;
	static const u32 CharComponentTexLayoutID = 27;		//byte
	static const u32 maleHDModelID = 32;
	static const u32 femaleHDModelID = 33;
	static const u32 CharComponentHDTexLayoutID = 31;		//byte
#else
	static const u32 maleModelID = 4;		// unit
	static const u32 femaleModelID = 5;	// unit
	static const u32 ShortName = 6;		// string, Name, represented by only 2 chars
	static const u32 Name = 11;			// string, Model name, 10048 to 11
	static const u32 CharComponentTexLayoutID = 24;
#endif

	dbc::record getByName(const c8* name);
};

class charSectionsDB : public dbc
{
public:
	charSectionsDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\CharSections.dbc") {}
	
#ifdef WOW70
	static const u32 Race = 2;		// byte
	static const u32 Gender = 3;		// byte
	static const u32 Type = 4;		// byte

	static const u32 Tex1 = 0;		// uint array 3

	static const u32 IsNPC = 1;		// Flags, u16, IsNPC = 0x1 ?, IsDeathKnight?
	static const u32 Section = 5;	// byte
	static const u32 Color = 6;		// byte
#else
	static const u32 Race = 1;		// uint
	static const u32 Gender = 2;		// uint
	static const u32 Type = 3;		// uint

	static const u32 Tex1 = 4;		// string array 3

	static const u32 IsNPC = 7;		// Flags, uint, IsNPC = 0x1 ?, IsDeathKnight?
	static const u32 Section = 8;	// uint
	static const u32 Color = 9;		// uint
#endif

	/// Types
	static const u32 SkinType = 0;
	static const u32 FaceType = 1;
	static const u32 FacialHairType = 2;
	static const u32 HairType = 3;
	static const u32 UnderwearType = 4;
	static const u32 SkinTypeHD = 5;
	static const u32 FaceTypeHD = 6;
	static const u32 FacialHairTypeHD = 7;
	static const u32 HairTypeHD = 8;
	static const u32 UnderwearTypeHD = 9;

	dbc::record getByParams(u32 race, u32 gender, u32 type, u32 section, u32 color) const;
	u32 getColorsFor(u32 race, u32 gender, u32 type, u32 section) const;
	u32 getSectionsFor(u32 race, u32 gender, u32 type, u32 color) const;
};

class creatureTypeDB : public dbc
{
public:
	creatureTypeDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\CreatureType.dbc") {}

#ifdef WOW70
	static const u32 Name = 0;		// string
#else
	static const u32 Name = 1;		// string
#endif
};

class creatureModelDB : public dbc
{
public:
	creatureModelDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\CreatureModelData.dbc") {}

	/// Fields
	//static const u32 Type = 1;			// uint

#ifdef WOW70
	static const u32 FileNameID = 19;		// uint fileDataId
#else
	static const u32 FileNameID = 2;		// uint FileData
#endif
};

class creatureDisplayInfoDB : public dbc
{
public:
	creatureDisplayInfoDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\CreatureDisplayInfo.dbc") {}

#if defined(WOW70)
	/// Fields
	static const u32 ModelID = 9;		// uint
	static const u32 NPCExtraID = 0;			// uint CreatureDisplayInfoExtraID
	static const u32 Scale = 1;		//float 
	static const u32 Alpha = 2;		//uint
	static const u32 Texture1 = 3;			// uint array 3
#else
	/// Fields
	static const u32 ModelID = 1;		// uint
	static const u32 NPCExtraID = 3;			// uint CreatureDisplayInfoExtraID
	static const u32 Scale = 4;		//float 
	static const u32 Alpha = 5;		//uint
	static const u32 Texture1 = 7;			// string array 3
#endif
	
};

class creatureDisplayInfoExtraDB : public dbc
{
public:
	creatureDisplayInfoExtraDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\CreatureDisplayInfoExtra.dbc") {}

#ifdef WOW70
	static const u32 SkinColor = 5;		// byte
	static const u32 Face = 6;			// byte
	static const u32 HairStyle = 7;		// byte
	static const u32 HairColor = 8;		// byte
	static const u32 FacialHair = 9;		// byte

	static const u32 HelmID = 0;			// uint, Slot1
	static const u32 ShoulderID = 1;		// uint, Slot3
	static const u32 ShirtID = 2;		// uint, Slot4
	static const u32 ChestID = 3;		// uint, Slot5
	static const u32 BeltID = 4;		// uint, Slot6
	static const u32 PantsID = 5;		// uint, Slot7
	static const u32 BootsID = 6;		// uint, Slot8
	static const u32 BracersID = 7;		// uint, Slot9
	static const u32 GlovesID = 8;		// uint, Slot10
	static const u32 TabardID = 9;		// uint, Slot19
	static const u32 CapeID = 10;		// uint, Slot16

#else
	static const u32 RaceID = 1;			// uint
	static const u32 Gender = 2;			// bool
	static const u32 SkinColor = 3;		// uint
	static const u32 Face = 4;			// uint
	static const u32 HairStyle = 5;		// uint
	static const u32 HairColor = 6;		// uint
	static const u32 FacialHair = 7;		// uint

	static const u32 HelmID = 8;			// uint, Slot1
	static const u32 ShoulderID = 9;		// uint, Slot3
	static const u32 ShirtID = 10;		// uint, Slot4
	static const u32 ChestID = 11;		// uint, Slot5
	static const u32 BeltID = 12;		// uint, Slot6
	static const u32 PantsID = 13;		// uint, Slot7
	static const u32 BootsID = 14;		// uint, Slot8
	static const u32 BracersID = 15;		// uint, Slot9
	static const u32 GlovesID = 16;		// uint, Slot10
	static const u32 TabardID = 17;		// uint, Slot19
	static const u32 CapeID = 18;		// uint, Slot16
#endif

#if defined(WOW60)
	static const u32 FileNameID = 20;
	static const u32 HDFileNameID = 21;
#else
	static const u32 Filename = 20;		// string. an index offset to the filename.
#endif
};

class npcModelItemSlotDisplayInfoDB : public dbc
{
public:
	npcModelItemSlotDisplayInfoDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\NpcModelItemSlotDisplayInfo.dbc") {}

	static const u32 NPCExtraID = 0;
	static const u32 ItemDisplayInfoID = 1;
	static const u32 ItemType = 2;
};

class helmGeosetDB : public dbc
{
public:
	helmGeosetDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\HelmetGeosetVisData.dbc") {}

#ifdef WOW70
	static const u32 HideHair = 0;		//int array
#else
	static const u32 HideHair = 1;		//int array
#endif
};

class itemDisplayDB : public dbc
{
public:
	itemDisplayDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\ItemDisplayInfo.dbc") { }

#if defined(WOW70)
	static const u32 Model = 0;			// u16 array, modelleft, modelright
	static const u32 Skin = 1;			// byte3 array, textureleft, textureright
#else
	static const u32 Model = 1;			// string array, modelleft, modelright
	static const u32 Skin = 3;			// string array, textureleft, textureright
#endif

#if defined(WOW60) || defined(WOW70)
	static const u32 GeosetGroup = 5;		// uint array 3, (0,1,2,3,4,5)
	//static const u32 BracerGeosetFlags = 6;		// uint, (0,1,2,3)
	//static const u32 RobeGeosetFlags = 7;		// uint, (0,1)
	static const u32 GeosetVisID = 10;	// uint array 2, HelmetGeosetVisData.dbc
	static const u32 TexArmUpper = 12;	// uint TextureFieldData.db2
	static const u32 TexArmLower = 13;	// uint TextureFieldData.db2
	static const u32 TexHands = 14;		// uint TextureFieldData.db2
	static const u32 TexChestUpper = 15;	// uint TextureFieldData.db2
	static const u32 TexChestLower = 16;	// uint TextureFieldData.db2
	static const u32 TexLegUpper = 17;	// uint TextureFieldData.db2
	static const u32 TexLegLower = 18;	// uint TextureFieldData.db2
	static const u32 TexFeet = 19;		// uint TextureFieldData.db2
	static const u32 Visuals = 20;		// uint
	//static const u32 ParticleColor = 21;	// uint
#else
	//static const u32 Icon = 5;			// string
	//static const u32 Texture = 6;			// string
	static const u32 GloveGeosetFlags = 7;		// uint, (0,1,2,3,4,5)
	static const u32 BracerGeosetFlags = 8;		// uint, (0,1,2,3)
	static const u32 RobeGeosetFlags = 9;		// uint, (0,1)
	static const u32 BootsGeosetFlags = 10;		// uint, (0,1,2,4,6)
	//static const u32 Unknown = 11;		// uint
	//static const u32 ItemGroupSounds = 12;			// uint
	static const u32 GeosetVisID1 = 13;	// uint, HelmetGeosetVisData.dbc
	static const u32 GeosetVisID2 = 14;	// uint, HelmetGeosetVisData.dbc
	static const u32 TexArmUpper = 15;	// string
	static const u32 TexArmLower = 16;	// string
	static const u32 TexHands = 17;		// string
	static const u32 TexChestUpper = 18;	// string
	static const u32 TexChestLower = 19;	// string
	static const u32 TexLegUpper = 20;	// string
	static const u32 TexLegLower = 21;	// string
	static const u32 TexFeet = 22;		// string
	static const u32 Visuals = 23;		// uint
	//static const u32 ParticleColor = 24;	// uint
#endif

};

class itemDisplayInfoMaterialResDB : public dbc
{
public:
	itemDisplayInfoMaterialResDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\ItemDisplayInfoMaterialRes.dbc") {}

	static const u32 ItemDisplayInfoID = 0;		//uint
	static const u32 TextureFileDataID = 1;
	static const u32 TextureSlot = 2;

	//slot definition
	static const u32 UpperArm = 0;
	static const u32 LowerArm = 1;
	static const u32 Hands = 2;
	static const u32 UpperTorso = 3;
	static const u32 LowerTorso = 4;
	static const u32 UpperLeg = 5;
	static const u32 LowerLeg = 6;
	static const u32 Foot = 7;

	dbc::record getByItemDisplayInfoIDAndSlot(u32 itemDisplayId, u32 slot) const;
	void getTexturePath(u32 itemDisplayId, u32 slot, c8* path, u32 size) const;
};

class itemSetDB : public dbc
{
public:
	itemSetDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\ItemSet.dbc") {}

	static const u32 NumItems = 17;

#if defined(WOW70)
	/// Fields
	static const u32 Name = 0;	// string, Localization
	static const u32 ItemIDBaseV400 = 1; // int array 17
#else
	/// Fields
	static const u32 Name = 1;	// string, Localization
	static const u32 ItemIDBaseV400 = 2; // 10 * uint
#endif

};

class itemSubClassDB : public dbc
{
public:
	itemSubClassDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\ItemSubClass.dbc") {}

#if defined(WOW70)
	static const u32 ClassIDV400 = 3;	// byte
	static const u32 SubClassIDV400 = 4;	// byte
	static const u32 NameV400 = 0;		// string
	static const u32 VerboseNameV400 = 1;		// string
#else
	static const u32 ClassIDV400 = 1;	// int
	static const u32 SubClassIDV400 = 2;	// int
	static const u32 NameV400 = 11;		// string
	static const u32 VerboseNameV400 = 12;		// string
#endif

	dbc::record getById(int id, int subid);
};

class startOutfitDB : public dbc
{
public:
	startOutfitDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\CharStartOutfit.dbc") {}

	static const u32 NumItems = 24;

#if defined(WOW70)
	static const u32 Race = 2;	// byte offset
	static const u32 Class = 3;		// byte offset
	static const u32 Gender = 4;	// byte offset
	static const u32 ItemIDBase = 0; // uint array, 24
#elif defined(WOW60)
	static const u32 Race = 4;	// byte offset
	static const u32 Class = 5;		// byte offset
	static const u32 Gender = 6;	// byte offset
	static const u32 ItemIDBase = 2; // 24 * uint
#else
	static const u32 Race = 1;	// byte offset
	static const u32 Class = 2;	// byte offset
	static const u32 Gender = 3;	// byte offset
	static const u32 ItemIDBase = 2; // 24 * uint
#endif
};

class lightDB : public dbc
{
public:
	lightDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\Light.dbc") {}

	static const u32 Map = 1;			// uint
	static const u32 PositionX = 2;		// float
	static const u32 PositionY = 3;		// float
	static const u32 PositionZ = 4;		// float
	static const u32 RadiusInner = 5;	// float
	static const u32 RadiusOuter = 6;	// float
	static const u32 DataIDs = 7;		// uint[8]
};

class lightSkyboxDB : public dbc
{
public:
	lightSkyboxDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\LightSkybox.dbc") {}

	static const u32 Name = 1;		// string
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

	static const u32 Model = 1;		// string
};

class itemDB : public dbc
{
public:
	itemDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\Item.dbc") { }

#if defined(WOW70)
	static const u32 Itemclass = 1;	// byte
	static const u32 Subclass = 2;	// byte
	static const u32 InventorySlot = 5;	// byte
	static const u32 Sheath = 6;		// byte
#elif defined(WOW60) 
	static const u32 Itemclass = 1;	// unit
	static const u32 Subclass = 2;	// unit
	static const u32 InventorySlot = 5;	// unit
	static const u32 Sheath = 6;		// unit
#else
	static const u32 Itemclass = 1;	// unit
	static const u32 Subclass = 2;	// unit
	static const u32 ItemDisplayInfo = 5;	// unit
	static const u32 InventorySlot = 6;	// unit
	static const u32 Sheath = 7;	// unit
#endif
};

class itemSparseDB : public dbc
{
public:
	itemSparseDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\Item-Sparse.dbc") { }

#if defined(WOW70)
	static const u32 Quality = 42;
#elif defined(WOW60)
	static const u32 Quality = 1;
#endif

	u32 getItemNameField() const
	{
#if defined(WOW70)
		return 13;
#elif defined(WOW60)
		return 70;
#else
		if (minorVersion >= 19000)
			return 70;
		else if (minorVersion >= 16309)
			return 100;
		else
			return 99;
#endif
	}
};

class itemModifiedAppearanceDB : public dbc
{
public:
	itemModifiedAppearanceDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\ItemModifiedAppearance.dbc", true)
	{
		buildItemLookup();
	}

#ifdef WOW70
	static const u32 ItemId = 0;	// uint
	static const u32 ItemAppearanceId = 1;	//uint
#else
	static const u32 ItemId = 1;	// unit
	static const u32 ItemAppearanceId = 3;	//uint
#endif

private:
	void buildItemLookup();

private:
#ifdef USE_QALLOCATOR
	typedef std::map<u32, u32, std::less<u32>, qzone_allocator<std::pair<u32, u32>>> T_ItemLookup32;
#else
	typedef std::unordered_map<u32, u32> T_ItemLookup32;
#endif
	
	T_ItemLookup32 ItemLookup32;

public:
	dbc::record getByItemID(u32 itemid) const
	{
		T_ItemLookup32::const_iterator itr = ItemLookup32.find(itemid);
		if ( itr == ItemLookup32.end())
			return record::EMPTY();

		u32 i = itr->second;
		return getRecord(i);
	}
};

class itemAppearanceDB : public dbc
{
public:
	itemAppearanceDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\ItemAppearance.dbc") {}

#ifdef WOW70
	static const u32 ItemDisplay = 0;		// unit
	static const u32 Icon = 1;		// uint filedata
#else
	static const u32 ItemDisplay = 1;		// unit
	static const u32 Icon = 2;		// uint filedata
#endif
};

class textureFileDataDB : public dbc
{
public:
	textureFileDataDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\TextureFileData.dbc", true)
	{
		buildItemLookup();
	}

#ifdef WOW70
	static const u32 TextureId = 0;
	static const u32 Path = 2;		//byte3 fileid
#else	
	static const u32 TextureId = 1;
	static const u32 Path = 4;		//uint filedata
#endif

private:
	void buildItemLookup()
	{
		for (u32 i=0; i<nActualRecords; ++i)
		{
			u32 itemid = getRecord(i).getUInt(textureFileDataDB::TextureId);
			ItemLookup32[itemid] = i;
		}
	}

private:
#ifdef USE_QALLOCATOR
	typedef std::map<u32, u32, std::less<u32>, qzone_allocator<std::pair<u32, u32>>> T_ItemLookup32;
#else
	typedef std::unordered_map<u32, u32> T_ItemLookup32;
#endif

	T_ItemLookup32 ItemLookup32;

public:
	dbc::record getByTextureId(u32 texid) const
	{
		T_ItemLookup32::const_iterator itr = ItemLookup32.find(texid);
		if ( itr == ItemLookup32.end())
			return record::EMPTY();

		u32 i = itr->second;
		return getRecord(i);
	}
};

class modelFileDataDB : public dbc
{
public:
	modelFileDataDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\ModelFileData.dbc", true)
	{
		buildItemLookup();
	}

	static const u32 Path = 1;		//byte3 fileid	
	static const u32 ModelId = 2;

private:
	void buildItemLookup()
	{
		for (u32 i=0; i<nActualRecords; ++i)
		{
			u32 itemid = getRecord(i).getUInt(modelFileDataDB::ModelId);
			ItemLookup32[itemid] = i;
		}
	}

private:
#ifdef USE_QALLOCATOR
	typedef std::map<u32, u32, std::less<u32>, qzone_allocator<std::pair<u32, u32>>> T_ItemLookup32;
#else
	typedef std::unordered_map<u32, u32> T_ItemLookup32;
#endif

	T_ItemLookup32 ItemLookup32;

public:
	dbc::record getByModelId(u32 modelId) const
	{
		T_ItemLookup32::const_iterator itr = ItemLookup32.find(modelId);
		if ( itr == ItemLookup32.end())
			return record::EMPTY();

		u32 i = itr->second;
		return getRecord(i);
	}
};


class fileDataDB : public dbc
{
public:
	fileDataDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\FileData.dbc") {}

	void saveListFile(const char* szPath);

	static const u32 FileName = 1;
	static const u32 FilePath = 2;
};


class mapDB : public dbc
{
public:
	mapDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\Map.dbc") {}

#ifdef WOW70
	static const u32 Name = 0;		// string
	static const u32 AreaType = 15;		// int
#else
	static const u32 Name = 1;		// string
	static const u32 AreaType = 2;		// int
#endif
};

class spellVisualEffectNameDB : public dbc
{
public:
	spellVisualEffectNameDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\SpellVisualEffectName.dbc") {}

#ifdef WOW70
	static const u32 Model = 0;		// string
#else
	static const u32 Model = 1;		// string
#endif
};

class spellVisualKitDB : public dbc
{
public:
	spellVisualKitDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\SpellVisualKit.dbc") {}

	static const u32 PrecastAnimation = 1;
	static const u32 CastAnimation = 2;
	static const u32 Head = 4;
	static const u32 Chest = 5;
	static const u32 Ground = 6;
	static const u32 RightHand = 7;
	static const u32 LeftHand = 8;
	static const u32 AOE = 9;
};

class spellVisualDB : public dbc
{
public:
	spellVisualDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\SpellVisual.dbc") {}

};

class spellDB : public dbc
{
public:
	spellDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\Spell.dbc") {}

	static const u32 Name = 1;
};

class wmoAreaTableDB : public dbc
{
public:
	wmoAreaTableDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\WmoAreaTable.dbc") {}
	
	static const u32 WMOID = 1;
	static const u32 WMOGroupID = 2;
	static const u32 Name = 11;
};

class worldMapAreaDB : public dbc
{
public:
	worldMapAreaDB(wowEnvironment* env) : dbc(env, "DBFilesClient\\WorldMapArea.dbc") {}

	static const u32 MapID = 1;
	static const u32 AreaTableID = 2;
	static const u32 Name = 3;
	static const u32 Left = 4;
	static const u32 Right = 5;
	static const u32 Top = 6;
	static const u32 Bottom = 7;
};

//物品和npc需要建两个单独的内存数据
struct SItemRecord
{
	c8 name[DEFAULT_SIZE * 2];
	s32 id, itemclass, subclass, type;

	const bool operator<(const SItemRecord& r) const
	{
		if (type == r.type)
			return Q_stricmp(name, r.name) < 0;
		else
			return type < r.type;
	}
};

class ItemCollections
{
public:
#ifdef USE_QALLOCATOR
	typedef std::map<s32, s32, std::less<s32>, qzone_allocator<std::pair<s32, s32>>>		T_itemLookup;
#else
	ItemCollections() : itemLookup(1024) {}
	typedef std::unordered_map<s32, s32>		T_itemLookup;
#endif

public:
	std::vector<SItemRecord>	items;
	T_itemLookup		itemLookup;				//item id 到 index

	void build(itemDB* itemDb,  itemSparseDB* itemSparseDb);

	const SItemRecord* getById(s32 id) const;
};


struct SNPCRecord
{
	c8 name[DEFAULT_SIZE * 2];
	s32 model;
	s32 type;

	SNPCRecord() : model(0), type(0)
	{
		memset(name, 0, sizeof(name));
	}

	explicit SNPCRecord(const c8* line);

	bool operator<(const SNPCRecord& r) const
	{
		if (type == r.type)
			return Q_stricmp(name, r.name) < 0;
		else
			return type < r.type;
	}
};

class NPCCollections
{
public:
#ifdef USE_QALLOCATOR
	typedef std::map<s32, s32, std::less<s32>, qzone_allocator<std::pair<s32, s32>>>		T_npcLookup;
#else
	NPCCollections() : npcLookup(1024) { }
	typedef std::unordered_map<s32, s32>		T_npcLookup;
#endif
	
public:
	std::vector<SNPCRecord>	npcs;
	T_npcLookup		npcLookup;				//npc model id 到 index

	bool open(const c8* filename);

	const SNPCRecord* getById(s32 id) const;
};

struct SStartOutfitEntry
{
	u32 id;
	c16 name[NAME_SIZE];
	c16 shortname[NAME_SIZE];
	u8 race;
	bool female;
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
	s32 id;
	s32 mapId;
	s32 parentId;
	c16 name[DEFAULT_SIZE];
};

struct SWowMapArea
{
	s32		id;
	s32		areaId;
	f32		locLeft;
	f32		locRight;
	f32		locTop;
	f32		locBottom;
	c8		name[DEFAULT_SIZE];
};

struct SMapRecord
{
	s32 id;
	s32 type;
	c8 name[DEFAULT_SIZE];
};

class MapCollections
{
public:
#ifdef USE_QALLOCATOR
	typedef std::map<s32, s32, std::less<s32>, qzone_allocator<std::pair<s32, s32>>>		T_mapLookup;
	typedef std::map<s32, s32, std::less<s32>, qzone_allocator<std::pair<s32, s32>>>		T_areaLookup;
#else
	typedef std::unordered_map<s32, s32>		T_mapLookup;
	typedef std::unordered_map<s32, s32>		T_areaLookup;
#endif

public:
	std::vector<SMapRecord>	maps;
	std::vector<SArea>		areas;

	T_mapLookup		mapLookup;
	T_areaLookup		areaLookup;

	const SMapRecord* getMapById(s32 id) const;
	SMapRecord* getMapById(s32 id);
	const SArea* getAreaById(s32 id) const;
	SArea* getAreaById(s32 id);
};

class WMOCollections
{
public:
	WMOCollections();

	std::vector<string256>		wmos;
};

class WorldModelCollections
{
public: 
	WorldModelCollections();

	std::vector<string256>		models;
};

class TextureCollections
{
public:
	TextureCollections();

	std::vector<string256>		textures;
};

struct SRidable
{
	s32		npcid;
	s32		mountflag;

	explicit SRidable(const c8* line);
};

class RidableCollections
{
public:
	RidableCollections();

	std::vector<SRidable>	ridables;

	bool open(const c8* filename, const NPCCollections& npcs);
};

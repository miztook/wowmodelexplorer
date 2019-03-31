#pragma once

#include "base.h"
#include "fixstring.h"
#include "wow_enums.h"
#include <map>
#include <vector>

class wowEnvironment;
class IMemFile;
class ItemCollections;

namespace WowClassic
{
#	pragma pack (4)

	struct dbcHeader {
		/*0x00*/    char      _magic[4];	// "WDBC"
		/*0x04*/    uint32_t  _nRecords;        //
		/*0x08*/    uint32_t  _nFields;      	//
		/*0x0C*/    uint32_t  _recordSize;      //
		/*0x10*/    uint32_t  _stringsize;
	};

	struct db2Header
	{
		/*0x00*/    char      _magic[4];	// "WDB2"
		/*0x04*/    uint32_t  _nRecords;        //
		/*0x08*/    uint32_t  _nFields;      	//
		/*0x0C*/    uint32_t  _recordSize;      //
		/*0x10*/    uint32_t  _stringsize;
		uint32_t hash;
		uint32_t clientminor;		//version
		uint32_t unknown1;
		uint32_t min_id;
		uint32_t max_id;
		uint32_t localecode;
		uint32_t refdatasize;
	};

#	pragma pack ()

	class dbc
	{
	private:
		DISALLOW_COPY_AND_ASSIGN(dbc);

	public:
		dbc(const wowEnvironment* env, const char* filename, bool tmp = false);
		virtual ~dbc();

	protected:
		void readWDBC(const wowEnvironment* env, IMemFile* file, bool tmp);
		void readWDB2(const wowEnvironment* env, IMemFile* file, bool tmp);

	public:
		class record
		{
		public:
			static record EMPTY() { static record m(nullptr, -1, nullptr, 0); return m; }

			bool isValid() const { return _ofs != nullptr; }

			uint32_t getID() const
			{
				return getUInt(0);
			}

			float getFloat(uint32_t idx) const
			{
				ASSERT(idx < _dbc->getNumFields());
				return *reinterpret_cast<float*>(_ofs + idx * 4);
			}

			uint32_t getUInt(uint32_t idx) const
			{
				ASSERT(idx < _dbc->getNumFields());
				return *reinterpret_cast<uint32_t*>(_ofs + idx * 4);
			}

			int32_t getInt(uint32_t idx) const
			{
				ASSERT(idx < _dbc->getNumFields());
				return *reinterpret_cast<int32_t*>(_ofs + idx * 4);
			}

			uint8_t getByte(uint32_t idx) const
			{
				ASSERT(idx < _dbc->getNumFields());
				return *reinterpret_cast<uint8_t*>(_ofs + idx);
			}

			template<class T>
			T* getRawPointer(uint32_t idx) const
			{
				ASSERT(idx < _dbc->getNumFields());
				return reinterpret_cast<T*>(_ofs + idx * sizeof(T));
			}

			template<class T>
			T getInArray(uint32_t idx, uint32_t arrayIndex) const
			{
				ASSERT(idx < _dbc->getNumFields());
				return (reinterpret_cast<T*>(_ofs + idx * sizeof(T)))[arrayIndex];
			}

			const char* getString(uint32_t idx) const
			{
				ASSERT(idx < _dbc->getNumFields());
				uint32_t offset = getUInt(idx);
				ASSERT(offset < _dbc->getStringSize());
				return reinterpret_cast<char*>(_dbc->_stringStart + offset);
			}

		public:
			record(const dbc* d, uint32_t index, uint8_t* ofs, uint16_t size) : _ofs(ofs), _index(index), _size(size), _dbc(d)  {}
		private:
			friend class dbc;
			uint8_t* _ofs;
			uint16_t _size;
			uint32_t _index;
			const dbc* _dbc;
		};

	public:
		dbc::record getRecord(uint32_t idx) const
		{
			ASSERT(idx < nActualRecords);
			return record(this, idx, _recordStart + idx * RecordSize, RecordSize);
		}

		dbc::record getByID(uint32_t id) const
		{
			auto itr = RecordLookup32.find(id);
			if (itr == RecordLookup32.end())
				return record::EMPTY();

			uint32_t i = itr->second;
			return getRecord(i);
		}

		uint32_t getNumRecords() const { return nRecords; }
		uint32_t getNumActualRecords() const { return nActualRecords; }
		uint32_t getNumFields() const { return nFields; }
		uint32_t getRecordSize() const { return RecordSize; }
		uint32_t getStringSize() const { return StringSize; }
		uint8_t* getStringStart() const { return _stringStart; }
		uint16_t getFieldSize(int idx) const { return 4; }

		//for sparse db2
		int32_t getRecordSparseRow(uint32_t index) const
		{
			auto itr = RecordSparseLookup32.find(index);
			if (itr == RecordSparseLookup32.end())
				return -1;

			return (int32_t)itr->second;
		}

	protected:
		WowDBType DBType;

		typedef std::map<uint32_t, uint32_t, std::less<uint32_t>, qzone_allocator<std::pair<uint32_t, uint32_t>>> T_RecordLookup32;
		typedef std::map<uint32_t, uint32_t, std::less<uint32_t>, qzone_allocator<std::pair<uint32_t, uint32_t>>> T_RecordSparseLookup32;

		T_RecordLookup32		RecordLookup32;

		uint8_t*		_recordStart;
		uint8_t*		_stringStart;

	protected:
		uint32_t		nFields;
		uint32_t		nRecords;
		uint32_t		RecordSize;
		uint32_t		StringSize;
		uint32_t		nActualRecords;

	protected:		//WDB2
		uint32_t		minorVersion;
		T_RecordSparseLookup32			RecordSparseLookup32;
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
		animDB(const wowEnvironment* env) : dbc(env, "DBFilesClient\\AnimationData.dbc") {}

	public:

#if WOW_VER >= 70
		static const uint32_t Name = 0;		// string
#else
		static const uint32_t Name = 1;		// string
#endif
	};

	class areaTableDB : public dbc
	{
	public:
		areaTableDB(const wowEnvironment* env) : dbc(env, "DBFilesClient\\AreaTable.dbc") {}

		static const uint32_t MapID = 1;
		static const uint32_t ParentAreaTableID = 2;
		static const uint32_t ExplorationLevel = 10;
		static const uint32_t Name = 13;
	};

	class charClassesDB : public dbc
	{
	public:
		charClassesDB(const wowEnvironment* env) : dbc(env, "DBFilesClient\\ChrClasses.dbc") {}

#if WOW_VER >= 70
		static const uint32_t NameV400 = 3;		//string
		static const uint32_t ShortName = 4;
#else
		static const uint32_t NameV400 = 3;
		static const uint32_t ShortName = 6;
#endif
	};

	class charFacialHairDB : public dbc
	{
	public:
		charFacialHairDB(const wowEnvironment* env) : dbc(env, "DBFilesClient\\CharacterFacialHairStyles.dbc") {}

		dbc::record getByParams(unsigned int race, unsigned int gender, unsigned int style) const;
		uint32_t getStylesFor(unsigned int race, unsigned int gender) const;

#if WOW_VER >= 70
		static const uint32_t RaceV400 = 1;				// byte
		static const uint32_t GenderV400 = 2;				// byte
		static const uint32_t StyleV400 = 3;				// byte
		static const uint32_t Geoset100V400 = 0;			// uint array
		static const uint32_t Geoset300V400 = 1;			// uint
		static const uint32_t Geoset200V400 = 2;			// uint
#else
		static const uint32_t RaceV400 = 1;				// uint
		static const uint32_t GenderV400 = 2;				// uint
		static const uint32_t StyleV400 = 3;				// uint
		static const uint32_t Geoset100V400 = 4;			// uint array
		static const uint32_t Geoset300V400 = 1;			// uint
		static const uint32_t Geoset200V400 = 2;			// uint
#endif
	};

	class charHairGeosetsDB : public dbc
	{
	public:
		charHairGeosetsDB(const wowEnvironment* env) : dbc(env, "DBFilesClient\\CharHairGeosets.dbc") {}

#if WOW_VER >= 70
		static const uint32_t Race = 1;				// byte
		static const uint32_t Gender = 2;				// byte
		static const uint32_t Section = 3;			// byte
		static const uint32_t Geoset = 5;				// byte
		static const uint32_t ShowScalp = 7;			// byte
#else
		static const uint32_t Race = 1;				// uint
		static const uint32_t Gender = 2;				// uint, 0 = Male, 1 = Female
		static const uint32_t Section = 3;			// uint, ID unique between race, and gender.
		static const uint32_t Geoset = 5;				// uint, Defines hairstyle, each number should be unique for that race / gender combo.
		static const uint32_t ShowScalp = 7;			// uint
#endif

		dbc::record getByParams(unsigned int race, unsigned int gender, unsigned int section) const;
		uint32_t getGeosetsFor(unsigned int race, unsigned int gender) const;
	};

	class charComponentTextureLayoutsDB : public dbc
	{
	public:
		charComponentTextureLayoutsDB(const wowEnvironment* env) : dbc(env, "DBFilesClient\\CharComponentTextureLayouts.dbc") {}

#if WOW_VER >= 70
		static const uint32_t Width = 0;
		static const uint32_t Height = 1;
#else
		static const uint32_t Width = 1;
		static const uint32_t Height = 2;
#endif
	};

	class charComponentTextureSectionsDB : public dbc
	{
	public:
		charComponentTextureSectionsDB(const wowEnvironment* env) : dbc(env, "DBFilesClient\\CharComponentTextureSections.dbc") {}

#if WOW_VER >= 70
		static const uint32_t LayoutID = 4;
		static const uint32_t Section = 5;
		static const uint32_t X = 0;
		static const uint32_t Y = 1;
		static const uint32_t Width = 2;
		static const uint32_t Height = 3;
#else
		static const uint32_t LayoutID = 1;
		static const uint32_t Section = 2;
		static const uint32_t X = 3;
		static const uint32_t Y = 4;
		static const uint32_t Width = 5;
		static const uint32_t Height = 6;
#endif
	};

	class charRacesDB : public dbc
	{
	public:
		charRacesDB(const wowEnvironment* env) : dbc(env, "DBFilesClient\\ChrRaces.dbc") {}

#if WOW_VER >= 70
		static const uint32_t ShortName = 1;		// string, Name, represented by only 2 chars
		static const uint32_t Name = 2;			// string, Model name, 10048 to 11
		static const uint32_t maleModelID = 15;
		static const uint32_t femaleModelID = 16;
		static const uint32_t CharComponentTexLayoutID = 27;		//byte
		static const uint32_t maleHDModelID = 32;
		static const uint32_t femaleHDModelID = 33;
		static const uint32_t CharComponentHDTexLayoutID = 31;		//byte
#else
		static const uint32_t maleModelID = 4;		// unit
		static const uint32_t femaleModelID = 5;	// unit
		static const uint32_t ShortName = 6;		// string, Name, represented by only 2 chars
		static const uint32_t Name = 11;			// string, Model name, 10048 to 11
		static const uint32_t CharComponentTexLayoutID = 24;
#endif

		dbc::record getByName(const char* name);
	};

	class charSectionsDB : public dbc
	{
	public:
		charSectionsDB(const wowEnvironment* env) : dbc(env, "DBFilesClient\\CharSections.dbc") {}

#if WOW_VER >= 70
		static const uint32_t Race = 2;		// byte
		static const uint32_t Gender = 3;		// byte
		static const uint32_t Type = 4;		// byte

		static const uint32_t Tex1 = 0;		// uint array 3

		static const uint32_t IsNPC = 1;		// Flags, uint16_t, IsNPC = 0x1 ?, IsDeathKnight?
		static const uint32_t Section = 5;	// byte
		static const uint32_t Color = 6;		// byte
#else
		static const uint32_t Race = 1;		// uint
		static const uint32_t Gender = 2;		// uint
		static const uint32_t Type = 3;		// uint

		static const uint32_t Tex1 = 4;		// string array 3

		static const uint32_t IsNPC = 7;		// Flags, uint, IsNPC = 0x1 ?, IsDeathKnight?
		static const uint32_t Section = 8;	// uint
		static const uint32_t Color = 9;		// uint
#endif

		/// Types
		static const uint32_t SkinType = 0;
		static const uint32_t FaceType = 1;
		static const uint32_t FacialHairType = 2;
		static const uint32_t HairType = 3;
		static const uint32_t UnderwearType = 4;
		static const uint32_t SkinTypeHD = 5;
		static const uint32_t FaceTypeHD = 6;
		static const uint32_t FacialHairTypeHD = 7;
		static const uint32_t HairTypeHD = 8;
		static const uint32_t UnderwearTypeHD = 9;

		dbc::record getByParams(uint32_t race, uint32_t gender, uint32_t type, uint32_t section, uint32_t color) const;
		uint32_t getColorsFor(uint32_t race, uint32_t gender, uint32_t type, uint32_t section) const;
		uint32_t getSectionsFor(uint32_t race, uint32_t gender, uint32_t type, uint32_t color) const;
	};

	class creatureTypeDB : public dbc
	{
	public:
		creatureTypeDB(const wowEnvironment* env) : dbc(env, "DBFilesClient\\CreatureType.dbc") {}

#if WOW_VER >= 70
		static const uint32_t Name = 0;		// string
#else
		static const uint32_t Name = 1;		// string
#endif
	};

	class creatureModelDB : public dbc
	{
	public:
		creatureModelDB(const wowEnvironment* env) : dbc(env, "DBFilesClient\\CreatureModelData.dbc") {}

		/// Fields
		//static const uint32_t Type = 1;			// uint

#if WOW_VER >= 70
		static const uint32_t FileNameID = 19;		// uint fileDataId
#else
		static const uint32_t FileNameID = 2;		// uint FileData
#endif
	};

	class creatureDisplayInfoDB : public dbc
	{
	public:
		creatureDisplayInfoDB(const wowEnvironment* env) : dbc(env, "DBFilesClient\\CreatureDisplayInfo.dbc") {}

#if WOW_VER >= 70
		/// Fields
		static const uint32_t ModelID = 9;		// uint
		static const uint32_t NPCExtraID = 0;			// uint CreatureDisplayInfoExtraID
		static const uint32_t Scale = 1;		//float
		static const uint32_t Alpha = 2;		//uint
		static const uint32_t Texture1 = 3;			// uint array 3
#else
		/// Fields
		static const uint32_t ModelID = 1;		// uint
		static const uint32_t NPCExtraID = 3;			// uint CreatureDisplayInfoExtraID
		static const uint32_t Scale = 4;		//float
		static const uint32_t Alpha = 5;		//uint
		static const uint32_t Texture1 = 7;			// string array 3
#endif
	};

	class creatureDisplayInfoExtraDB : public dbc
	{
	public:
		creatureDisplayInfoExtraDB(const wowEnvironment* env) : dbc(env, "DBFilesClient\\CreatureDisplayInfoExtra.dbc") {}

#if WOW_VER >= 70
		static const uint32_t SkinColor = 5;		// byte
		static const uint32_t Face = 6;			// byte
		static const uint32_t HairStyle = 7;		// byte
		static const uint32_t HairColor = 8;		// byte
		static const uint32_t FacialHair = 9;		// byte

		static const uint32_t HelmID = 0;			// uint, Slot1
		static const uint32_t ShoulderID = 1;		// uint, Slot3
		static const uint32_t ShirtID = 2;		// uint, Slot4
		static const uint32_t ChestID = 3;		// uint, Slot5
		static const uint32_t BeltID = 4;		// uint, Slot6
		static const uint32_t PantsID = 5;		// uint, Slot7
		static const uint32_t BootsID = 6;		// uint, Slot8
		static const uint32_t BracersID = 7;		// uint, Slot9
		static const uint32_t GlovesID = 8;		// uint, Slot10
		static const uint32_t TabardID = 9;		// uint, Slot19
		static const uint32_t CapeID = 10;		// uint, Slot16

#else
		static const uint32_t RaceID = 1;			// uint
		static const uint32_t Gender = 2;			// bool
		static const uint32_t SkinColor = 3;		// uint
		static const uint32_t Face = 4;			// uint
		static const uint32_t HairStyle = 5;		// uint
		static const uint32_t HairColor = 6;		// uint
		static const uint32_t FacialHair = 7;		// uint

		static const uint32_t HelmID = 8;			// uint, Slot1
		static const uint32_t ShoulderID = 9;		// uint, Slot3
		static const uint32_t ShirtID = 10;		// uint, Slot4
		static const uint32_t ChestID = 11;		// uint, Slot5
		static const uint32_t BeltID = 12;		// uint, Slot6
		static const uint32_t PantsID = 13;		// uint, Slot7
		static const uint32_t BootsID = 14;		// uint, Slot8
		static const uint32_t BracersID = 15;		// uint, Slot9
		static const uint32_t GlovesID = 16;		// uint, Slot10
		static const uint32_t TabardID = 17;		// uint, Slot19
		static const uint32_t CapeID = 18;		// uint, Slot16
#endif

#if WOW_VER >= 60
		static const uint32_t FileNameID = 20;
		static const uint32_t HDFileNameID = 21;
#else
		static const uint32_t Filename = 20;		// string. an index offset to the filename.
#endif
	};

	class npcModelItemSlotDisplayInfoDB : public dbc
	{
	public:
		npcModelItemSlotDisplayInfoDB(const wowEnvironment* env) : dbc(env, "DBFilesClient\\NpcModelItemSlotDisplayInfo.dbc") {}

		static const uint32_t NPCExtraID = 0;
		static const uint32_t ItemDisplayInfoID = 1;
		static const uint32_t ItemType = 2;
	};

	class helmGeosetDB : public dbc
	{
	public:
		helmGeosetDB(const wowEnvironment* env) : dbc(env, "DBFilesClient\\HelmetGeosetVisData.dbc") {}

#if WOW_VER >= 70
		static const uint32_t HideHair = 0;		//int array
#else
		static const uint32_t HideHair = 1;		//int array
#endif
	};

	class itemDisplayDB : public dbc
	{
	public:
		itemDisplayDB(const wowEnvironment* env) : dbc(env, "DBFilesClient\\ItemDisplayInfo.dbc") { }

#if WOW_VER >= 70
		static const uint32_t Model = 0;			// uint16_t array, modelleft, modelright
		static const uint32_t Skin = 1;			// byte3 array, textureleft, textureright
#else
		static const uint32_t Model = 1;			// string array, modelleft, modelright
		static const uint32_t Skin = 3;			// string array, textureleft, textureright
#endif

#if WOW_VER >= 60
		static const uint32_t GeosetGroup = 5;		// uint array 3, (0,1,2,3,4,5)
		//static const uint32_t BracerGeosetFlags = 6;		// uint, (0,1,2,3)
		//static const uint32_t RobeGeosetFlags = 7;		// uint, (0,1)
		static const uint32_t GeosetVisID = 10;	// uint array 2, HelmetGeosetVisData.dbc
		static const uint32_t TexArmUpper = 12;	// uint TextureFieldData.db2
		static const uint32_t TexArmLower = 13;	// uint TextureFieldData.db2
		static const uint32_t TexHands = 14;		// uint TextureFieldData.db2
		static const uint32_t TexChestUpper = 15;	// uint TextureFieldData.db2
		static const uint32_t TexChestLower = 16;	// uint TextureFieldData.db2
		static const uint32_t TexLegUpper = 17;	// uint TextureFieldData.db2
		static const uint32_t TexLegLower = 18;	// uint TextureFieldData.db2
		static const uint32_t TexFeet = 19;		// uint TextureFieldData.db2
		static const uint32_t Visuals = 20;		// uint
		//static const uint32_t ParticleColor = 21;	// uint
#else
		//static const uint32_t Icon = 5;			// string
		//static const uint32_t Texture = 6;			// string
		static const uint32_t GloveGeosetFlags = 7;		// uint, (0,1,2,3,4,5)
		static const uint32_t BracerGeosetFlags = 8;		// uint, (0,1,2,3)
		static const uint32_t RobeGeosetFlags = 9;		// uint, (0,1)
		static const uint32_t BootsGeosetFlags = 10;		// uint, (0,1,2,4,6)
		//static const uint32_t Unknown = 11;		// uint
		//static const uint32_t ItemGroupSounds = 12;			// uint
		static const uint32_t GeosetVisID1 = 13;	// uint, HelmetGeosetVisData.dbc
		static const uint32_t GeosetVisID2 = 14;	// uint, HelmetGeosetVisData.dbc
		static const uint32_t TexArmUpper = 15;	// string
		static const uint32_t TexArmLower = 16;	// string
		static const uint32_t TexHands = 17;		// string
		static const uint32_t TexChestUpper = 18;	// string
		static const uint32_t TexChestLower = 19;	// string
		static const uint32_t TexLegUpper = 20;	// string
		static const uint32_t TexLegLower = 21;	// string
		static const uint32_t TexFeet = 22;		// string
		static const uint32_t Visuals = 23;		// uint
		//static const uint32_t ParticleColor = 24;	// uint
#endif
	};

	class itemDisplayInfoMaterialResDB : public dbc
	{
	public:
		itemDisplayInfoMaterialResDB(const wowEnvironment* env) : dbc(env, "DBFilesClient\\ItemDisplayInfoMaterialRes.dbc") {}

		static const uint32_t ItemDisplayInfoID = 0;		//uint
		static const uint32_t TextureFileDataID = 1;
		static const uint32_t TextureSlot = 2;

		//slot definition
		static const uint32_t UpperArm = 0;
		static const uint32_t LowerArm = 1;
		static const uint32_t Hands = 2;
		static const uint32_t UpperTorso = 3;
		static const uint32_t LowerTorso = 4;
		static const uint32_t UpperLeg = 5;
		static const uint32_t LowerLeg = 6;
		static const uint32_t Foot = 7;

		dbc::record getByItemDisplayInfoIDAndSlot(uint32_t itemDisplayId, uint32_t slot) const;
		void getTexturePath(uint32_t itemDisplayId, uint32_t slot, char* path, uint32_t size) const;
	};

	class itemSetDB : public dbc
	{
	public:
		itemSetDB(const wowEnvironment* env) : dbc(env, "DBFilesClient\\ItemSet.dbc") {}

		static const uint32_t NumItems = 17;

#if WOW_VER >= 70
		/// Fields
		static const uint32_t Name = 0;	// string, Localization
		static const uint32_t ItemIDBaseV400 = 1; // int array 17
#else
		/// Fields
		static const uint32_t Name = 1;	// string, Localization
		static const uint32_t ItemIDBaseV400 = 2; // 10 * uint
#endif
	};

	class itemSubClassDB : public dbc
	{
	public:
		itemSubClassDB(const wowEnvironment* env) : dbc(env, "DBFilesClient\\ItemSubClass.dbc") {}

#if WOW_VER >= 70
		static const uint32_t ClassIDV400 = 3;	// byte
		static const uint32_t SubClassIDV400 = 4;	// byte
		static const uint32_t NameV400 = 0;		// string
		static const uint32_t VerboseNameV400 = 1;		// string
#else
		static const uint32_t ClassIDV400 = 1;	// int
		static const uint32_t SubClassIDV400 = 2;	// int
		static const uint32_t NameV400 = 11;		// string
		static const uint32_t VerboseNameV400 = 12;		// string
#endif

		dbc::record getById(int id, int subid);
	};

	class startOutfitDB : public dbc
	{
	public:
		startOutfitDB(const wowEnvironment* env) : dbc(env, "DBFilesClient\\CharStartOutfit.dbc") {}

		static const uint32_t NumItems = 24;

#if WOW_VER >= 70
		static const uint32_t Race = 2;	// byte offset
		static const uint32_t Class = 3;		// byte offset
		static const uint32_t Gender = 4;	// byte offset
		static const uint32_t ItemIDBase = 0; // uint array, 24
#elif WOW_VER >= 60
		static const uint32_t Race = 4;	// byte offset
		static const uint32_t Class = 5;		// byte offset
		static const uint32_t Gender = 6;	// byte offset
		static const uint32_t ItemIDBase = 2; // 24 * uint
#else
		static const uint32_t Race = 1;	// byte offset
		static const uint32_t Class = 2;	// byte offset
		static const uint32_t Gender = 3;	// byte offset
		static const uint32_t ItemIDBase = 2; // 24 * uint
#endif
	};

	class lightDB : public dbc
	{
	public:
		lightDB(const wowEnvironment* env) : dbc(env, "DBFilesClient\\Light.dbc") {}

		static const uint32_t Map = 1;			// uint
		static const uint32_t PositionX = 2;		// float
		static const uint32_t PositionY = 3;		// float
		static const uint32_t PositionZ = 4;		// float
		static const uint32_t RadiusInner = 5;	// float
		static const uint32_t RadiusOuter = 6;	// float
		static const uint32_t DataIDs = 7;		// uint[8]
	};

	class lightSkyboxDB : public dbc
	{
	public:
		lightSkyboxDB(const wowEnvironment* env) : dbc(env, "DBFilesClient\\LightSkybox.dbc") {}

		static const uint32_t Name = 1;		// string
	};

	class itemVisualsDB : public dbc
	{
	public:
		itemVisualsDB(const wowEnvironment* env) : dbc(env, "DBFilesClient\\ItemVisuals.dbc") {}
	};

	class itemVisualEffectDB : public dbc
	{
	public:
		itemVisualEffectDB(const wowEnvironment* env) : dbc(env, "DBFilesClient\\ItemVisualEffects.dbc") {}

		static const uint32_t Model = 1;		// string
	};

	class itemDB : public dbc
	{
	public:
		itemDB(const wowEnvironment* env) : dbc(env, "DBFilesClient\\Item.dbc") { }

#if WOW_VER >= 70
		static const uint32_t Itemclass = 1;	// byte
		static const uint32_t Subclass = 2;	// byte
		static const uint32_t InventorySlot = 5;	// byte
		static const uint32_t Sheath = 6;		// byte
#elif WOW_VER >= 60
		static const uint32_t Itemclass = 1;	// unit
		static const uint32_t Subclass = 2;	// unit
		static const uint32_t InventorySlot = 5;	// unit
		static const uint32_t Sheath = 6;		// unit
#else
		static const uint32_t Itemclass = 1;	// unit
		static const uint32_t Subclass = 2;	// unit
		static const uint32_t ItemDisplayInfo = 5;	// unit
		static const uint32_t InventorySlot = 6;	// unit
		static const uint32_t Sheath = 7;	// unit
#endif
	};

	class itemSparseDB : public dbc
	{
	public:
		itemSparseDB(const wowEnvironment* env) : dbc(env, "DBFilesClient\\Item-Sparse.dbc") { }

#if WOW_VER >= 70
		static const uint32_t Quality = 42;
#elif WOW_VER >= 60
		static const uint32_t Quality = 1;
#endif

		uint32_t getItemNameField() const
		{
#if WOW_VER >= 70
			return 13;
#elif WOW_VER >= 60
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
		itemModifiedAppearanceDB(const wowEnvironment* env) : dbc(env, "DBFilesClient\\ItemModifiedAppearance.dbc", true)
		{
			buildItemLookup();
		}

#if WOW_VER >= 70
		static const uint32_t ItemId = 0;	// uint
		static const uint32_t ItemAppearanceId = 1;	//uint
#else
		static const uint32_t ItemId = 1;	// unit
		static const uint32_t ItemAppearanceId = 3;	//uint
#endif

	private:
		void buildItemLookup();

	private:
#ifdef USE_QALLOCATOR
		typedef std::map<uint32_t, uint32_t, std::less<uint32_t>, qzone_allocator<std::pair<uint32_t, uint32_t>>> T_ItemLookup32;
#else
		typedef std::unordered_map<uint32_t, uint32_t> T_ItemLookup32;
#endif

		T_ItemLookup32 ItemLookup32;

	public:
		dbc::record getByItemID(uint32_t itemid) const
		{
			T_ItemLookup32::const_iterator itr = ItemLookup32.find(itemid);
			if (itr == ItemLookup32.end())
				return record::EMPTY();

			uint32_t i = itr->second;
			return getRecord(i);
		}
	};

	class itemAppearanceDB : public dbc
	{
	public:
		itemAppearanceDB(const wowEnvironment* env) : dbc(env, "DBFilesClient\\ItemAppearance.dbc") {}

#if WOW_VER >= 70
		static const uint32_t ItemDisplay = 0;		// unit
		static const uint32_t Icon = 1;		// uint filedata
#else
		static const uint32_t ItemDisplay = 1;		// unit
		static const uint32_t Icon = 2;		// uint filedata
#endif
	};

	class textureFileDataDB : public dbc
	{
	public:
		textureFileDataDB(const wowEnvironment* env) : dbc(env, "DBFilesClient\\TextureFileData.dbc", true)
		{
			buildItemLookup();
		}

#if WOW_VER >= 70
		static const uint32_t TextureId = 0;
		static const uint32_t Path = 2;		//byte3 fileid
#else
		static const uint32_t TextureId = 1;
		static const uint32_t Path = 4;		//uint filedata
#endif

	private:
		void buildItemLookup()
		{
			for (uint32_t i = 0; i < nActualRecords; ++i)
			{
				uint32_t itemid = getRecord(i).getUInt(textureFileDataDB::TextureId);
				ItemLookup32[itemid] = i;
			}
		}

	private:
#ifdef USE_QALLOCATOR
		typedef std::map<uint32_t, uint32_t, std::less<uint32_t>, qzone_allocator<std::pair<uint32_t, uint32_t>>> T_ItemLookup32;
#else
		typedef std::unordered_map<uint32_t, uint32_t> T_ItemLookup32;
#endif

		T_ItemLookup32 ItemLookup32;

	public:
		dbc::record getByTextureId(uint32_t texid) const
		{
			T_ItemLookup32::const_iterator itr = ItemLookup32.find(texid);
			if (itr == ItemLookup32.end())
				return record::EMPTY();

			uint32_t i = itr->second;
			return getRecord(i);
		}
	};

	class modelFileDataDB : public dbc
	{
	public:
		modelFileDataDB(const wowEnvironment* env) : dbc(env, "DBFilesClient\\ModelFileData.dbc", true)
		{
			buildItemLookup();
		}

		static const uint32_t Path = 1;		//byte3 fileid
		static const uint32_t ModelId = 2;

	private:
		void buildItemLookup()
		{
			for (uint32_t i = 0; i < nActualRecords; ++i)
			{
				uint32_t itemid = getRecord(i).getUInt(modelFileDataDB::ModelId);
				ItemLookup32[itemid] = i;
			}
		}

	private:
#ifdef USE_QALLOCATOR
		typedef std::map<uint32_t, uint32_t, std::less<uint32_t>, qzone_allocator<std::pair<uint32_t, uint32_t>>> T_ItemLookup32;
#else
		typedef std::unordered_map<uint32_t, uint32_t> T_ItemLookup32;
#endif

		T_ItemLookup32 ItemLookup32;

	public:
		dbc::record getByModelId(uint32_t modelId) const
		{
			T_ItemLookup32::const_iterator itr = ItemLookup32.find(modelId);
			if (itr == ItemLookup32.end())
				return record::EMPTY();

			uint32_t i = itr->second;
			return getRecord(i);
		}
	};

	class fileDataDB : public dbc
	{
	public:
		fileDataDB(const wowEnvironment* env) : dbc(env, "DBFilesClient\\FileData.dbc") {}

		void saveListFile(const char* szPath);

		static const uint32_t FileName = 1;
		static const uint32_t FilePath = 2;
	};

	class mapDB : public dbc
	{
	public:
		mapDB(const wowEnvironment* env) : dbc(env, "DBFilesClient\\Map.dbc") {}

#if WOW_VER >= 70
		static const uint32_t Name = 0;		// string
		static const uint32_t AreaType = 15;		// int
#else
		static const uint32_t Name = 1;		// string
		static const uint32_t AreaType = 2;		// int
#endif
	};

	class spellVisualEffectNameDB : public dbc
	{
	public:
		spellVisualEffectNameDB(const wowEnvironment* env) : dbc(env, "DBFilesClient\\SpellVisualEffectName.dbc") {}

#if WOW_VER >= 70
		static const uint32_t Model = 0;		// string
#else
		static const uint32_t Model = 1;		// string
#endif
	};

	class spellVisualKitDB : public dbc
	{
	public:
		spellVisualKitDB(const wowEnvironment* env) : dbc(env, "DBFilesClient\\SpellVisualKit.dbc") {}

		static const uint32_t PrecastAnimation = 1;
		static const uint32_t CastAnimation = 2;
		static const uint32_t Head = 4;
		static const uint32_t Chest = 5;
		static const uint32_t Ground = 6;
		static const uint32_t RightHand = 7;
		static const uint32_t LeftHand = 8;
		static const uint32_t AOE = 9;
	};

	class spellVisualDB : public dbc
	{
	public:
		spellVisualDB(const wowEnvironment* env) : dbc(env, "DBFilesClient\\SpellVisual.dbc") {}
	};

	class spellDB : public dbc
	{
	public:
		spellDB(const wowEnvironment* env) : dbc(env, "DBFilesClient\\Spell.dbc") {}

		static const uint32_t Name = 1;
	};

	class wmoAreaTableDB : public dbc
	{
	public:
		wmoAreaTableDB(const wowEnvironment* env) : dbc(env, "DBFilesClient\\WmoAreaTable.dbc") {}

		static const uint32_t WMOID = 1;
		static const uint32_t WMOGroupID = 2;
		static const uint32_t Name = 11;
	};

	class worldMapAreaDB : public dbc
	{
	public:
		worldMapAreaDB(const wowEnvironment* env) : dbc(env, "DBFilesClient\\WorldMapArea.dbc") {}

		static const uint32_t MapID = 1;
		static const uint32_t AreaTableID = 2;
		static const uint32_t Name = 3;
		static const uint32_t Left = 4;
		static const uint32_t Right = 5;
		static const uint32_t Top = 6;
		static const uint32_t Bottom = 7;
	};

	void buildItemCollections(ItemCollections& itemCollections, const itemDB* itemDb, const itemSparseDB* itemSparseDb);
};
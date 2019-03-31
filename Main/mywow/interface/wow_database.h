#pragma once

#include "base.h"
#include "wow_def.h"
#include "wow_dbc.h"

class wowEnvironment;

namespace WowClassic
{
	class wowDatabase
	{
	public:
		explicit wowDatabase(const wowEnvironment* env);
		~wowDatabase();

		bool init();

		void buildItems();
		bool buildNpcs(const char* filename);
		void buildStartOutfitClass();
		void buildMaps();
		void buildWmos();
		void buildWorldModels();
		void buildTextures();
		bool buildRidables(const char* filename);

	public:			//Êý¾Ý²éÑ¯
		bool getRaceGender(const char* filename, uint32_t& race, uint32_t& gender, bool& isHD);
		bool getRaceId(const char* raceName, uint32_t& race);
		const char* getRaceName(uint32_t race);
		bool getCharacterPath(const char* name, bool female, bool isHD, char* path, uint32_t size);
		bool getNpcPath(int32_t npcid, bool isHD, char* path, uint32_t size);
		int32_t getNpcModelId(int32_t npcid);
		const char* getAnimationName(uint32_t id);
		void getSubClassName(int32_t id, int32_t subid, char16_t* outname, uint32_t size);
		void getNpcTypeName(int32_t id, char16_t* outname, uint32_t size);
		const char* getClassShortName(uint32_t classId);
		bool getClassInfo(const char* shortname, char16_t* classname, uint32_t size, uint32_t* id);

		uint32_t getMaxSkinColor(uint32_t race, bool female, bool isHD);
		uint32_t getMaxFaceType(uint32_t race, bool female, bool isHD);
		uint32_t getMaxHairColor(uint32_t race, bool female, bool isHD);
		uint32_t getMaxHairStyle(uint32_t race, bool female, bool isHD);
		uint32_t getMaxFacialHairStyle(uint32_t race, bool female);

		bool getSet(uint32_t index, int32_t& setid, char16_t* outname, uint32_t size);

		uint32_t getNumMaps() const { return (uint32_t)mapCollections.maps.size(); }
		const SMapRecord* getMap(uint32_t idx) const;
		const SMapRecord* getMapById(int32_t id) const { return mapCollections.getMapById(id); }
		int32_t getItemDisplayId(int32_t itemid) const;
		void getFilePath(int32_t fileId, string256& path) const;
		void getFilePath(int32_t fileId, char* path, uint32_t size) const;
		void getTextureFilePath(int32_t texId, string256& path) const;
		void getTextureFilePath(int32_t texId, char* path, uint32_t size) const;
		void getModelFilePath(int32_t modelId, string256& path) const;
		void getModelFilePath(int32_t modelId, char* path, uint32_t size) const;

		bool isRaceHasHD(uint32_t race);

#ifdef MW_EDITOR
		uint32_t getItemCount() const { return (uint32_t)itemCollections.items.size(); }
		uint32_t getNpcCount() const { return (uint32_t)npcCollections.npcs.size(); }
		const SItemRecord* getItemById(int32_t id) const { return itemCollections.getById(id); }
		const SNPCRecord* getNPCById(int32_t id) const { return npcCollections.getById(id); }
		const SItemRecord* getItem(uint32_t idx) const;
		const SNPCRecord* getNPC(uint32_t idx) const;

		uint32_t getNumStartOutfits(uint32_t race, bool female);
		const SStartOutfitEntry* getStartOutfit(uint32_t race, bool female, uint32_t idx);

		uint32_t getNumWmos() const { return (uint32_t)wmoCollections.wmos.size(); }
		const char* getWmoFileName(uint32_t index) const;
		uint32_t getNumWorldModels() const { return (uint32_t)worldModelCollections.models.size(); }
		const char* getWorldModelFileName(uint32_t index) const;
		uint32_t getNumTextures() const { return (uint32_t)textureCollections.textures.size(); }
		const char* getTextureFileName(uint32_t index) const;

		uint32_t getNumRidables() const { return (uint32_t)ridableCollections.ridables.size(); }
		const SRidable* getRidable(uint32_t idx) const;

		bool getItemPath(int32_t itemid, char* modelpath, uint32_t modelSize, char* texturepath, uint32_t texSize);

#endif

	public:
		const animDB* getAnimDB() const { return AnimDB.get(); }
		const areaTableDB*	getAreaTableDB() const { return AreaTableDB.get(); }
		const charClassesDB*		getCharClassesDB() const { return CharClassesDB.get(); }
		const charHairGeosetsDB*	getCharHairGeosetDB() const { return CharHairGeosetsDB.get(); }
		const charFacialHairDB*		getCharFacialHairDB() const { return CharFacialHairDB.get(); }
		const charRacesDB*		getCharRacesDB() const { return CharRacesDB.get(); }
		const charSectionsDB*			getCharSectionDB() const { return CharSectionsDB.get(); }
		const creatureTypeDB*			getCreatureTypeDB() const { return CreatureTypeDB.get(); }
		const creatureModelDB*		getCreatureModelDB() const { return CreatureModelDB.get(); }
		const creatureDisplayInfoDB*		getCreatureDisplayInfoDB() const { return CreatureDisplayInfoDB.get(); }
		const creatureDisplayInfoExtraDB*		getCreatureDisplayInfoExtraDB() const { return CreatureDisplayInfoExtraDB.get(); }
		const npcModelItemSlotDisplayInfoDB*	getNpcModelItemSlotDisplayInfoDB() const { return NpcModelItemSlotDisplayInfoDB.get(); }
		const helmGeosetDB*	getHelmGeosetDB() const { return HelmGeosetDB.get(); }
		const itemDisplayDB*	getItemDisplayDB() const { return ItemDisplayDB.get(); }
		const itemDisplayInfoMaterialResDB*  getItemDisplayInfoMaterialResDB() const { return ItemDisplayInfoMaterialResDB.get(); }
		const itemSetDB*	getItemSetDB() const { return ItemSetDB.get(); }
		const itemSubClassDB*	 getItemSubClassDB() const { return ItemSubClassDB.get(); }
		const startOutfitDB*	getStartOutfitDB() const { return	StartOutFitDB.get(); }
		const itemDB*		getItemDB() const	{ return ItemDB.get(); }
		const itemModifiedAppearanceDB*		getItemModifiedAppearanceDB() const { return ItemModifiedAppearanceDB.get(); }
		const itemAppearanceDB* getItemAppearanceDB() const { return ItemAppearanceDB.get(); }
		const textureFileDataDB* getTextureFileDataDB() const { return TextureFileDataDB.get(); }
		const modelFileDataDB*  getModelFileDataDB() const { return ModelFileDataDB.get(); }
		const fileDataDB* getFileDataDB() const { return FileDataDB.get(); }
		const mapDB*		getMapDB() const	{ return MapDB.get(); }
		const spellDB*	getSpellDB() const { return SpellDB.get(); }

	private:
		const wowEnvironment*		Environment;

		std::unique_ptr<animDB>		AnimDB;
		std::unique_ptr<areaTableDB>	AreaTableDB;
		std::unique_ptr<charClassesDB>		CharClassesDB;
		std::unique_ptr<charFacialHairDB>		CharFacialHairDB;
		std::unique_ptr<charHairGeosetsDB>		CharHairGeosetsDB;
		std::unique_ptr<charRacesDB>			CharRacesDB;
		std::unique_ptr<charSectionsDB>					CharSectionsDB;
		std::unique_ptr<creatureTypeDB>					CreatureTypeDB;
		std::unique_ptr<creatureModelDB>				CreatureModelDB;
		std::unique_ptr<creatureDisplayInfoDB>				CreatureDisplayInfoDB;
		std::unique_ptr<creatureDisplayInfoExtraDB>		CreatureDisplayInfoExtraDB;
		std::unique_ptr<npcModelItemSlotDisplayInfoDB>	NpcModelItemSlotDisplayInfoDB;
		std::unique_ptr<helmGeosetDB>					HelmGeosetDB;
		std::unique_ptr<itemDisplayDB>			ItemDisplayDB;
		std::unique_ptr<itemDisplayInfoMaterialResDB>	ItemDisplayInfoMaterialResDB;
		std::unique_ptr<itemSetDB>					ItemSetDB;
		std::unique_ptr<itemSubClassDB>					ItemSubClassDB;
		std::unique_ptr<startOutfitDB>					StartOutFitDB;
		std::unique_ptr<itemDB>									ItemDB;
		std::unique_ptr<itemModifiedAppearanceDB>		ItemModifiedAppearanceDB;
		std::unique_ptr<itemAppearanceDB>			ItemAppearanceDB;
		std::unique_ptr<textureFileDataDB>			TextureFileDataDB;
		std::unique_ptr<modelFileDataDB>			ModelFileDataDB;
		std::unique_ptr<fileDataDB>			FileDataDB;
		std::unique_ptr<mapDB>									MapDB;
		std::unique_ptr<spellDB>			SpellDB;

		//
		ItemCollections		itemCollections;
		NPCCollections		npcCollections;
		StartOutfitClassCollections		startOutfitClassCollections;
		MapCollections		mapCollections;
		WMOCollections	wmoCollections;
		WorldModelCollections		worldModelCollections;
		TextureCollections	textureCollections;
		RidableCollections	ridableCollections;
	};
};
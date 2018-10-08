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
		bool buildNpcs(const c8* filename);
		void buildStartOutfitClass();
		void buildMaps();
		void buildWmos();
		void buildWorldModels();
		void buildTextures();
		bool buildRidables(const c8* filename);

	public:			//Êý¾Ý²éÑ¯
		bool getRaceGender(const c8* filename, u32& race, u32& gender, bool& isHD);
		bool getRaceId(const c8* raceName, u32& race);
		const c8* getRaceName(u32 race);
		bool getCharacterPath(const c8* name, bool female, bool isHD, c8* path, u32 size);
		bool getNpcPath(s32 npcid, bool isHD, c8* path, u32 size);
		s32 getNpcModelId(s32 npcid);
		const c8* getAnimationName(u32 id);
		void getSubClassName(s32 id, s32 subid, c16* outname, u32 size);
		void getNpcTypeName(s32 id, c16* outname, u32 size);
		const c8* getClassShortName(u32 classId);
		bool getClassInfo(const c8* shortname, c16* classname, u32 size, u32* id);

		u32 getMaxSkinColor(u32 race, bool female, bool isHD);
		u32 getMaxFaceType(u32 race, bool female, bool isHD);
		u32 getMaxHairColor(u32 race, bool female, bool isHD);
		u32 getMaxHairStyle(u32 race, bool female, bool isHD);
		u32 getMaxFacialHairStyle(u32 race, bool female);

		bool getSet(u32 index, s32& setid, c16* outname, u32 size);

		u32 getNumMaps() const { return (u32)mapCollections.maps.size(); }
		const SMapRecord* getMap(u32 idx) const;
		const SMapRecord* getMapById(s32 id) const { return mapCollections.getMapById(id); }
		s32 getItemDisplayId(s32 itemid) const;
		void getFilePath(s32 fileId, string256& path) const;
		void getFilePath(s32 fileId, c8* path, u32 size) const;
		void getTextureFilePath(s32 texId, string256& path) const;
		void getTextureFilePath(s32 texId, c8* path, u32 size) const;
		void getModelFilePath(s32 modelId, string256& path) const;
		void getModelFilePath(s32 modelId, c8* path, u32 size) const;

		bool isRaceHasHD(u32 race);

#ifdef MW_EDITOR
		u32 getItemCount() const { return (u32)itemCollections.items.size(); }
		u32 getNpcCount() const { return (u32)npcCollections.npcs.size(); }
		const SItemRecord* getItemById(s32 id) const { return itemCollections.getById(id); }
		const SNPCRecord* getNPCById(s32 id) const { return npcCollections.getById(id); }
		const SItemRecord* getItem(u32 idx) const;
		const SNPCRecord* getNPC(u32 idx) const;

		u32 getNumStartOutfits(u32 race, bool female);
		const SStartOutfitEntry* getStartOutfit(u32 race, bool female, u32 idx);

		u32 getNumAreas() const { return (u32)mapCollections.areas.size(); }
		const SArea* getAreaById(s32 id) const { return mapCollections.getAreaById(id); }
		u32 getNumWmos() const { return (u32)wmoCollections.wmos.size(); }
		const c8* getWmoFileName(u32 index) const;
		u32 getNumWorldModels() const { return (u32)worldModelCollections.models.size(); }
		const c8* getWorldModelFileName(u32 index) const;
		u32 getNumTextures() const { return (u32)textureCollections.textures.size(); }
		const c8* getTextureFileName(u32 index) const;

		u32 getNumRidables() const { return (u32)ridableCollections.ridables.size(); }
		const SRidable* getRidable(u32 idx) const;

		bool getItemPath(s32 itemid, c8* modelpath, u32 modelSize, c8* texturepath, u32 texSize);

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
#pragma once

#include "base.h"
#include "wow_dbc.h"

class wowEnvironment;

class wowDatabase
{
public:
	explicit wowDatabase(wowEnvironment* env);
	~wowDatabase();

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
	bool getItemVisualPath(s32 visualId, c8* path, u32 size);
	bool getEffectVisualPath(s32 visualId, c8* path, u32 size);

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
	const animDB* getAnimDB() const { return AnimDB; }
	const areaTableDB*	getAreaTableDB() const { return AreaTableDB; }
	const charClassesDB*		getCharClassesDB() const { return CharClassesDB; }
	const charHairGeosetsDB*	getCharHairGeosetDB() const { return CharHairGeosetsDB; }
	const charFacialHairDB*		getCharFacialHairDB() const { return CharFacialHairDB; }
	const charRacesDB*		getCharRacesDB() const { return CharRacesDB; }
	const charSectionsDB*			getCharSectionDB() const { return CharSectionsDB; }
	const creatureTypeDB*			getCreatureTypeDB() const { return CreatureTypeDB; }
	const creatureModelDB*		getCreatureModelDB() const { return CreatureModelDB; }
	const creatureDisplayInfoDB*		getCreatureDisplayInfoDB() const { return CreatureDisplayInfoDB; }
	const creatureDisplayInfoExtraDB*		getCreatureDisplayInfoExtraDB() const { return CreatureDisplayInfoExtraDB; }
	const npcModelItemSlotDisplayInfoDB*	getNpcModelItemSlotDisplayInfoDB() const { return NpcModelItemSlotDisplayInfoDB; }
	const helmGeosetDB*	getHelmGeosetDB() const { return HelmGeosetDB; }
	const itemDisplayDB*	getItemDisplayDB() const { return ItemDisplayDB; }
	const itemDisplayInfoMaterialResDB*  getItemDisplayInfoMaterialResDB() const { return ItemDisplayInfoMaterialResDB; }
	const itemSetDB*	getItemSetDB() const { return ItemSetDB; }
	const itemSubClassDB*	 getItemSubClassDB() const { return ItemSubClassDB; }
	const itemVisualsDB*	 getItemVisualsDB() const 	{ return ItemVisualsDB; }
	const itemVisualEffectDB*	 getItemVisualEffectDB() const { return ItemVisualEffectDB; }
	const startOutfitDB*	getStartOutfitDB() const { return	StartOutFitDB; }
	const lightDB*	getLightDB() const { return LightDB; }
	const lightSkyboxDB*	getLightSkyboxDB() const  { return LightSkyboxDB; }
	const itemDB*		getItemDB() const	{ return ItemDB; }
	const itemModifiedAppearanceDB*		getItemModifiedAppearanceDB() const { return ItemModifiedAppearanceDB; }
	const itemAppearanceDB* getItemAppearanceDB() const { return ItemAppearanceDB; }
	const textureFileDataDB* getTextureFileDataDB() const { return TextureFileDataDB; }
	const modelFileDataDB*  getModelFileDataDB() const { return ModelFileDataDB; }
	const fileDataDB* getFileDataDB() const { return FileDataDB; }
	const mapDB*		getMapDB() const	{ return MapDB; }
	const spellVisualEffectNameDB*	 getSpellVisualEffectNameDB() const { return SpellVisualEffectNameDB; }
 	const spellVisualDB*		getSpellVisualDB() const { return SpellVisualDB; }
 	const spellVisualKitDB*		getSpellVisualKitDB() const { return SpellVisualKitDB; }
 	const spellDB*	getSpellDB() const { return SpellDB; }
 	const wmoAreaTableDB*		getWmoAreaTableDB() const { return WmoAreaTableDB; }
 	const worldMapAreaDB*		getWorldMapAreaDB() const { return WorldMapAreaDB; }

private:
	wowEnvironment*		Environment;

	animDB*		AnimDB;
	areaTableDB*	AreaTableDB;
	charClassesDB*		CharClassesDB;
	charFacialHairDB*		CharFacialHairDB;
	charHairGeosetsDB*		CharHairGeosetsDB;
	charRacesDB*			CharRacesDB;
	charSectionsDB*					CharSectionsDB;
	creatureTypeDB*					CreatureTypeDB;
	creatureModelDB*				CreatureModelDB;
	creatureDisplayInfoDB*				CreatureDisplayInfoDB;
	creatureDisplayInfoExtraDB*		CreatureDisplayInfoExtraDB;
	npcModelItemSlotDisplayInfoDB*	NpcModelItemSlotDisplayInfoDB;
	helmGeosetDB*					HelmGeosetDB;
	itemDisplayDB*			ItemDisplayDB;
	itemDisplayInfoMaterialResDB*	ItemDisplayInfoMaterialResDB;
	itemSetDB*					ItemSetDB;
	itemSubClassDB*					ItemSubClassDB;
	itemVisualsDB*						ItemVisualsDB;
	itemVisualEffectDB*			ItemVisualEffectDB;
	startOutfitDB*					StartOutFitDB;
	lightDB*				LightDB;
	lightSkyboxDB*				LightSkyboxDB;
	itemDB*									ItemDB;
	itemModifiedAppearanceDB*		ItemModifiedAppearanceDB;
	itemAppearanceDB*			ItemAppearanceDB;
	textureFileDataDB*			TextureFileDataDB;
	modelFileDataDB*			ModelFileDataDB;
	fileDataDB*			FileDataDB;
	mapDB*									MapDB;
 	spellVisualEffectNameDB*					SpellVisualEffectNameDB;
 	spellVisualKitDB*			SpellVisualKitDB;
 	spellVisualDB*		SpellVisualDB;
 	spellDB*			SpellDB;
	wmoAreaTableDB*			WmoAreaTableDB;
	worldMapAreaDB*			WorldMapAreaDB;

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

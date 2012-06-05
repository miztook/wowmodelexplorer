#pragma once

#include "base.h"
#include "wow_dbc.h"

class wowEnvironment;

class wowDatabase
{
public:
	wowDatabase(wowEnvironment* env);
	~wowDatabase();

	bool buildItems(const c8* filename);
	void buildAvailableSets();
	bool buildNpcs(const c8* filename);
	void buildStartOutfitClass();
	void buildMaps();

public:			//Êý¾Ý²éÑ¯
	bool getRaceGender(const c8* filename, u32& race, u32& gender);
	bool getRaceId(const c8* raceName, u32& race);
	bool getCharacterPath(const c8* name, bool female, c8* path, u32 size);
	bool getNpcPath(s32 npcid, c8* path, u32 size);
	const c8* getAnimationName(u32 id);
	void getSubClassName(s32 id, s32 subid, c16* outname, u32 size);
	void getNpcTypeName(s32 id, c16* outname, u32 size);

	u32 getMaxSkinColor(u32 race, bool female);
	u32 getMaxFaceType(u32 race, bool female);
	u32 getMaxHairColor(u32 race, bool female);
	u32 getMaxHairStyle(u32 race, bool female);
	u32 getMaxFacialHairStyle(u32 race, bool female);

	u32 getItemCount() const { return itemCollections.items.size(); }
	u32 getNpcCount() const { return npcCollections.npcs.size(); }
	SItemRecord* getItemById(s32 id) { return itemCollections.getById(id); }
	SNPCRecord* getNPCById(s32 id) { return npcCollections.getById(id); }
	SItemRecord* getItem(u32 idx);
	SNPCRecord* getNPC(u32 idx);

	u32 getNumStartOutfits(u32 race, bool female);
	const SStartOutfitEntry* getStartOutfit(u32 race, bool female, u32 idx);

	bool getSet(u32 index, s32& setid, c16* outname, u32 size);

	bool getItemVisualPath(s32 visualId, c8* path, u32 size);
	bool getEffectVisualPath(s32 visualId, c8* path, u32 size);

	const SArea* getMapArea(SMapRecord* map, u32 index);
	u32 getNumMaps() const { return mapCollections.maps.size(); }
	SMapRecord* getMap(u32 idx);
	SMapRecord* getMapById(s32 id) { return mapCollections.getMapById(id); }
	u32 getNumAreas() const { return mapCollections.areas.size(); }
	SArea* getAreaById(s32 id) { return mapCollections.getAreaById(id); }

public:
	animDB* getAnimDB() const { return AnimDB; }
	areaTableDB*	getAreaTableDB() const { return AreaTableDB; }
	charClassesDB*		getCharClassesDB() const { return CharClassesDB; }
	charHairGeosetsDB*	getCharHairGeosetDB() const { return CharHairGeosetsDB; }
	charFacialHairDB*		getCharFacialHairDB() const { return CharFacialHairDB; }
	charRacesDB*		getCharRacesDB() const { return CharRacesDB; }
	charSectionsDB*			getCharSectionDB() const { return CharSectionsDB; }
	creatureTypeDB*			getCreatureTypeDB() const { return CreatureTypeDB; }
	creatureModelDB*		getCreatureModelDB() const { return CreatureModelDB; }
	creatureDisplayInfoDB*		getCreatureDisplayInfoDB() const { return CreatureDisplayInfoDB; }
	creatureDisplayInfoExtraDB*		getCreatureDisplayInfoExtraDB() const { return CreatureDisplayInfoExtraDB; }
	helmGeosetDB*	getHelmGeosetDB() const { return HelmGeosetDB; }
	itemDisplayDB*	getItemDisplayDB() const { return ItemDisplayDB; }
	itemSetDB*	getItemSetDB() const { return ItemSetDB; }
	itemSubClassDB*	 getItemSubClassDB() const { return ItemSubClassDB; }
	itemVisualsDB*	 getItemVisualsDB() const 	{ return ItemVisualsDB; }
	itemVisualEffectDB*	 getItemVisualEffectDB() const { return ItemVisualEffectDB; }
	startOutfitDB*	getStartOutfitDB() const { return	StartOutFitDB; }
	//lightSkyboxDB*	getLightSkyboxDB() const  { return LightSkyboxDB; }
	itemDB*		getItemDB() const	{ return ItemDB; }
	mapDB*		getMapDB() const	{ return MapDB; }
	spellVisualEffectNameDB*	 getSpellVisualEffectNameDB() const { return SpellVisualEffectNameDB; }
	wmoAreaTableDB*		getWmoAreaTableDB() const { return WmoAreaTableDB; }
	worldMapAreaDB*		getWorldMapAreaDB() const { return WorldMapAreaDB; }

private:
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
	helmGeosetDB*					HelmGeosetDB;
	itemDisplayDB*			ItemDisplayDB;
	itemSetDB*					ItemSetDB;
	itemSubClassDB*					ItemSubClassDB;
	itemVisualsDB*						ItemVisualsDB;
	itemVisualEffectDB*			ItemVisualEffectDB;
	startOutfitDB*					StartOutFitDB;
	//lightSkyboxDB*				LightSkyboxDB;
	itemDB*									ItemDB;
	mapDB*									MapDB;
	spellVisualEffectNameDB*					SpellVisualEffectNameDB;
	wmoAreaTableDB*			WmoAreaTableDB;
	worldMapAreaDB*			WorldMapAreaDB;

	//
	ItemCollections		itemCollections;
	NPCCollections		npcCollections;
	StartOutfitClassCollections		startOutfitClassCollections;
	MapCollections		mapCollections;
};
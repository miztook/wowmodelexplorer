#pragma once

#include "IFileWDT.h"
#include <unordered_map>
#include <map>
#include <vector>
#include "fixstring.h"

class CFileWDT : public IFileWDT
{
private:
	CFileWDT();
	~CFileWDT();

	friend class CWDTLoader;

public:
	virtual bool loadFile(IMemFile* file, s32 mapid);
	virtual bool loadFileSimple(IMemFile* file, s32 mapid);
	
	virtual STile* getTile(u8 row, u8 col);

	u8* getFileData() const { return FileData; }

	s32 getMapId() const { return MapId; }
	const c8* getMapName() const { return MapName; }
	CMapEnvironment*	 getMapEnvironment() const { return MapEnvironment; }

	bool getPositionByTile(s32 row, s32 col, vector3df& pos);
	bool getTileByPosition(vector3df pos, s32& row, s32& col);
	STile* getTile(u32 index);
	u32 getTileCount() const { return (u32)Tiles.size(); }

	bool loadADT(STile* tile, bool simple = false);
	bool loadADTTextures(STile* tile);
	bool unloadADT(STile* tile);

	void getADTFileName(u8 row, u8 col, c8* name, u32 size) const;
	const c8* getWMOFileName(u32 index) const;

private:
	bool loadADTData(STile* tile);		//no video data

	void loadWDL();
	void loadTEX();


private:
#ifdef USE_QALLOCATOR
	typedef std::map<u16, u32, std::less<u16>, qzone_allocator<std::pair<u16, u32>>> T_TileLookup;
#else
	typedef std::unordered_map<u16, u32> T_TileLookup;
#endif
	
	u8*			FileData;
	CMapEnvironment*		MapEnvironment;

	s32			MapId;

	//1st adt
	vector3df		AdtPosition;
	s32		AdtRow, AdtCol;

	c8		MapName[DEFAULT_SIZE];
	c8		Name[QMAX_PATH];

	//hibyte: row, lowbyte: col
	T_TileLookup		TileLookup;

};

inline STile* CFileWDT::getTile( u32 index )
{
	if (index >= Tiles.size())
		return NULL_PTR;
	return &Tiles[index];
}
#pragma once

#include "IFileWDT.h"
#include <map>
#include <vector>
#include "str.h"

class CWDTLoader
{
public:
	bool isALoadableFileExtension( const c8* filename ) { return hasFileExtensionA(filename, "wdt"); }

	IFileWDT* loadWDT( MPQFile* file, s32 mapid );
};

class CFileWDT : public IFileWDT
{
private:
	CFileWDT();
	~CFileWDT();

	friend class CWDTLoader;

public:
	virtual bool loadFile(MPQFile* file, s32 mapid);
	virtual u8* getFileData() const { return FileData; }

	virtual s32 getMapId() const { return MapId; }
	virtual const c16* getMapName() const { return MapName; }
	virtual CMapEnvironment*	 getMapEnvironment() const { return MapEnvironment; }

	virtual bool getPositionByTile(s32 row, s32 col, vector3df& pos);
	virtual bool getTileByPosition(vector3df pos, s32& row, s32& col);
	virtual STile* getTile(u8 row, u8 col);
	virtual STile* getTile(u32 index);
	virtual u32 getTileCount() const { return Tiles.size(); }

	virtual bool loadADT(STile* tile);
	virtual bool unloadADT(STile* tile);

	virtual void getADTFileName(u8 row, u8 col, c8* name, u32 size);
	virtual const c8* getWMOFileName(u32 index) const;

private:

	void loadWDL();
	void loadTEX();

private:
	u8*			FileData;

	s32			MapId;

	c16		MapName[DEFAULT_SIZE];
	c8		Name[MAX_PATH];

	//hibyte: row, lowbyte: col
	typedef std::map<u16, u32, std::less<u16>, qzone_allocator<std::pair<u16, u32>>> T_TileLookup;
	T_TileLookup		TileLookup;

	std::vector<STile>	Tiles;

	CMapEnvironment*		MapEnvironment;

	//1st adt
	vector3df		AdtPosition;
	s32		AdtRow, AdtCol;
};
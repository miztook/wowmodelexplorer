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
	virtual bool loadFile(IMemFile* file, int32_t mapid);
	virtual bool loadFileSimple(IMemFile* file, int32_t mapid);
	
	virtual STile* getTile(uint8_t row, uint8_t col);

	uint8_t* getFileData() const { return FileData; }

	int32_t getMapId() const { return MapId; }
	const char* getMapName() const { return MapName; }
	CMapEnvironment*	 getMapEnvironment() const { return MapEnvironment; }

	bool getPositionByTile(int32_t row, int32_t col, vector3df& pos);
	bool getTileByPosition(vector3df pos, int32_t& row, int32_t& col);
	STile* getTile(uint32_t index);
	uint32_t getTileCount() const { return (uint32_t)Tiles.size(); }

	bool loadADT(STile* tile, bool simple = false);
	bool loadADTTextures(STile* tile);
	bool unloadADT(STile* tile);

	void getADTFileName(uint8_t row, uint8_t col, char* name, uint32_t size) const;
	const char* getWMOFileName(uint32_t index) const;

private:
	bool loadADTData(STile* tile);		//no video data

	void loadWDL();
	void loadTEX();


private:
#ifdef USE_QALLOCATOR
	typedef std::map<uint16_t, uint32_t, std::less<uint16_t>, qzone_allocator<std::pair<uint16_t, uint32_t>>> T_TileLookup;
#else
	typedef std::unordered_map<uint16_t, uint32_t> T_TileLookup;
#endif
	
	uint8_t*			FileData;
	CMapEnvironment*		MapEnvironment;

	int32_t			MapId;

	//1st adt
	vector3df		AdtPosition;
	int32_t		AdtRow, AdtCol;

	char		MapName[DEFAULT_SIZE];
	char		Name[QMAX_PATH];

	//hibyte: row, lowbyte: col
	T_TileLookup		TileLookup;

};

inline STile* CFileWDT::getTile( uint32_t index )
{
	if (index >= Tiles.size())
		return nullptr;
	return &Tiles[index];
}
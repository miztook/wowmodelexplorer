#pragma once

#include "core.h"
#include "IFileWDT.h"
#include <unordered_map>
#include <map>

class CWDTSceneNode;
class CMapChunk;
class CFileWDT;

//wdt的场景管理
class wow_wdtScene
{
public:
	explicit wow_wdtScene(CWDTSceneNode* wdtNode);
	~wow_wdtScene();

public:
	void update();
	void setCameraChunk(CMapChunk* chunk) { CamChunk = chunk; }
	const CMapChunk* getCameraChunk() const { return CamChunk; }

	int32_t getRow() const { return Row; }
	int32_t getCol() const { return Col; }

	void setAdtLoadSize(E_ADT_LOAD size);

	void setCurrentTile(int32_t row, int32_t col);
	void startLoadADT(STile* tile);
	bool processADT();
	bool updateBlocksByCamera();
	void unloadOutBlocks();

	void updateTileTransform(STile* tile);
	bool isTileNeeded(STile* tile) const;		//是否在需要加载的范围内

	bool isLoading(STile* tile) const;
	void recalculateTilesNeeded(int32_t row, int32_t col);
	uint32_t getNumBlocks() const;

private:
#ifdef USE_QALLOCATOR
	typedef std::map<STile*, bool, std::less<STile*>, qzone_allocator<std::pair<STile*, bool>>>		T_TileMap;
#else
	typedef std::unordered_map<STile*, bool>		T_TileMap;
#endif

private:
	CWDTSceneNode* WdtSceneNode;
	CFileWDT*	FileWDT;

	//已加载和加载中的tile, false为加载中，true为已加载
	T_TileMap		TilesMap;

	CMapChunk*			CamChunk;
	CMapChunk*			LastCamChunk;

	int32_t	Row;			//所在中心row
	int32_t	Col;				//所在中心col;

	vector2di		Coords[25];
	
	//flags
	uint32_t TileChangeTick;
	bool TileChanging;
	
	E_ADT_LOAD		AdtLoadSize;
};

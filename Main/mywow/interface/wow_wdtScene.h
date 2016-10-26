#pragma once

#include "core.h"
#include "IFileWDT.h"
#include <unordered_map>

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

	s32 getRow() const { return Row; }
	s32 getCol() const { return Col; }

	void setAdtLoadSize(E_ADT_LOAD size);

	void setCurrentTile(s32 row, s32 col);
	void startLoadADT(STile* tile);
	bool processADT();
	bool updateBlocksByCamera();
	void unloadOutBlocks();

	void updateTileTransform(STile* tile);
	bool isTileNeeded(STile* tile) const;		//是否在需要加载的范围内

	bool isLoading(STile* tile) const;
	void recalculateTilesNeeded(s32 row, s32 col);
	u32 getNumBlocks() const;

private:
	CWDTSceneNode* WdtSceneNode;
	CFileWDT*	FileWDT;

	//已加载和加载中的tile, false为加载中，true为已加载
	typedef std::unordered_map<STile*, bool>		T_TileMap;
	T_TileMap		TilesMap;

	CMapChunk*			CamChunk;
	CMapChunk*			LastCamChunk;

	s32	Row;			//所在中心row
	s32	Col;				//所在中心col;

	vector2di		Coords[25];
	
	//flags
	u32 TileChangeTick;
	bool TileChanging;
	
	E_ADT_LOAD		AdtLoadSize;
};

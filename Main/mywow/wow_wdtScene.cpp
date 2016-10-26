#include "stdafx.h"
#include "wow_wdtScene.h"
#include "mywow.h"
#include "CWDTSceneNode.h"
#include "CFileWDT.h"

#define TIME_DELAY_TILE_CHANGED		5		//在摄像机所在tile改变后，隔多少帧开始发送新的加载请求

wow_wdtScene::wow_wdtScene( CWDTSceneNode* wdtNode )
	: WdtSceneNode(wdtNode)
{
	FileWDT = WdtSceneNode->FileWDT;

	Row = Col = -1;

	CamChunk = LastCamChunk = NULL_PTR;

	AdtLoadSize = g_Engine->getSceneRenderServices()->getAdtLoadSize();

	TileChangeTick = 0;
	TileChanging = false;
}

wow_wdtScene::~wow_wdtScene()
{
	g_Engine->getResourceLoader()->cancelAll(IResourceLoader::ET_ADT);

	for (T_TileMap::const_iterator itr = TilesMap.begin(); itr != TilesMap.end(); ++itr)
	{
		if (!itr->second)			//正在loading的跳过
			continue;

		STile* tile = itr->first;
		FileWDT->unloadADT(tile);
	}
}

void wow_wdtScene::setCurrentTile( s32 row, s32 col )
{
	Row = row;
	Col = col;

	recalculateTilesNeeded(Row, Col);

	g_Engine->getResourceLoader()->cancelAll(IResourceLoader::ET_ADT);

	for (T_TileMap::const_iterator itr = TilesMap.begin(); itr != TilesMap.end(); ++itr)
	{
		if (!itr->second)			//正在loading的跳过
			continue;

		STile* tile = itr->first;
		FileWDT->unloadADT(tile);
	}

	TilesMap.clear();

	u32 num = getNumBlocks();
	for (u32 i=0; i<num; ++i)
	{
		u8 r = (u8)Coords[i].X;
		u8 c = (u8)Coords[i].Y;
		STile* newTile = FileWDT->getTile(r,c);
		WdtSceneNode->MapBlocks[i].tile = newTile;
		if (newTile && !newTile->fileAdt && !isLoading(newTile))
		{
			bool success = FileWDT->loadADT(newTile);
			ASSERT(success);
			TilesMap[newTile] = true;
		}

		WdtSceneNode->updateMapBlock(i);
	}
}

void wow_wdtScene::startLoadADT( STile* tile )
{
	if (!tile || tile->fileAdt)
		return;

	c8 adtname[QMAX_PATH];
	FileWDT->getADTFileName(tile->row, tile->col, adtname, QMAX_PATH);

	IResourceLoader::SParamBlock block = {0};
	block.param1 = WdtSceneNode;
	block.param2 = tile;

	g_Engine->getResourceLoader()->beginLoadADT(adtname, block);

	TilesMap[tile] = false;
}

bool wow_wdtScene::processADT()
{
	IResourceLoader* loader = g_Engine->getResourceLoader();
	if(loader->adtLoadCompleted())
	{
		IResourceLoader::STask task = loader->getCurrentTask();
		ASSERT(task.type == IResourceLoader::ET_ADT);

		IFileADT* fileadt = (IFileADT*)task.file;
		if(WdtSceneNode != (IWDTSceneNode*)task.param.param1)		//不是需要的adt
			return false;
		STile* tile = (STile*)task.param.param2;

		//process
		loader->clearCurrentTask();

		ASSERT(!tile->fileAdt);

		fileadt->SetPosition(tile->row, tile->col);
		tile->fileAdt = fileadt;

		tile->fileAdt->buildVideoResources();			//优先建立显存资源
		
		loader->resumeLoading();
		
		//
		updateTileTransform(tile);

		return true;
	}

	return false;
}

bool wow_wdtScene::updateBlocksByCamera()
{
	ICamera* cam = g_Engine->getSceneManager()->getActiveCamera();
	s32 row, col;
	if(!WdtSceneNode->getTileByPosition(cam->getPosition(), row, col))
		return false;

	bool changed = false;

	if (TileChanging)
	{
		++TileChangeTick;
		if (TileChangeTick > TIME_DELAY_TILE_CHANGED)			//在tile发生改变5tick后再发起请求
		{
			//当前所在格子发生变化，更新tile
			u32 num = getNumBlocks();
			for (u32 i=0; i<num; ++i)
			{
				u8 r = (u8)Coords[i].X;
				u8 c = (u8)Coords[i].Y;
				STile* newTile = FileWDT->getTile(r,c);
				WdtSceneNode->MapBlocks[i].tile = newTile;
    			if (newTile && !newTile->fileAdt && !isLoading(newTile))
           			startLoadADT(newTile);

				//block发生变化, 更新
				WdtSceneNode->updateMapBlock(i);
			}
			TileChanging = false;

			//
			changed = true;
		}
	}

	if (Row != row || Col != col)
	{
		TileChanging = true;
		TileChangeTick = 0;

		Row = row;
		Col = col;

		recalculateTilesNeeded(Row, Col);
	}
	return changed;
}

bool wow_wdtScene::isLoading( STile* tile ) const
{
	T_TileMap::const_iterator itr = TilesMap.find(tile);
	return itr != TilesMap.end();
}

void wow_wdtScene::updateTileTransform( STile* tile )
{
	T_TileMap::iterator itr = TilesMap.find(tile);
	ASSERT(itr != TilesMap.end());
	itr->second = true;

	u32 num = getNumBlocks();
	for (u32 i=0; i<num; ++i)
	{
		if (WdtSceneNode->MapBlocks[i].tile == tile)
			WdtSceneNode->updateMapBlock(i);
	}
}

void wow_wdtScene::unloadOutBlocks()
{
	for (T_TileMap::iterator itr = TilesMap.begin(); itr != TilesMap.end(); )
	{
		if (!itr->second)	//正在loading的跳过
		{
			++itr;
			continue;
		}

		STile* tile = itr->first;			//删除不在加载范围之外的tile
		if (!isTileNeeded(tile))
		{
			FileWDT->unloadADT(tile);
			TilesMap.erase(itr++);
		}
		else
		{
			++itr;
		}
	}
}

void wow_wdtScene::recalculateTilesNeeded( s32 row, s32 col )
{
	u32 count = 0;

	if (AdtLoadSize == EAL_3X3 || AdtLoadSize == EAL_5X5 /*|| AdtLoadSize == EAL_7X7*/)
	{
			Coords[count++].set(row, col);
			//3x3 surround
			Coords[count++].set(row-1, col-1);
			Coords[count++].set(row-1, col);
			Coords[count++].set(row-1, col+1);
			Coords[count++].set(row, col-1);
			Coords[count++].set(row, col+1);
			Coords[count++].set(row+1, col-1);
			Coords[count++].set(row+1, col);
			Coords[count++].set(row+1, col+1);
	}
	
	if (AdtLoadSize == EAL_5X5 /*|| AdtLoadSize == EAL_7X7*/)
	{
		//5x5 surround
		//up
		Coords[count++].set(row-2, col-2);
		Coords[count++].set(row-2, col-1);
		Coords[count++].set(row-2, col);
		Coords[count++].set(row-2, col+1);
		Coords[count++].set(row-2, col+2);
		//left
		Coords[count++].set(row-1, col-2);
		Coords[count++].set(row, col-2);
		Coords[count++].set(row+1, col-2);
		//right
		Coords[count++].set(row-1, col+2);
		Coords[count++].set(row, col+2);
		Coords[count++].set(row+1, col+2);
		//bottom
		Coords[count++].set(row+2, col-2);
		Coords[count++].set(row+2, col-1);
		Coords[count++].set(row+2, col);
		Coords[count++].set(row+2, col+1);
		Coords[count++].set(row+2, col+2);
	}

	/*
	if (AdtLoadSize == EAL_7X7)
	{
		//7x7 surround
		//up
		Coords[count++].set(row-3, col-3);
		Coords[count++].set(row-3, col-2);
		Coords[count++].set(row-3, col-1);
		Coords[count++].set(row-3, col);
		Coords[count++].set(row-3, col+1);
		Coords[count++].set(row-3, col+2);
		Coords[count++].set(row-3, col+3);
		//left
		Coords[count++].set(row-2, col-3);
		Coords[count++].set(row-1, col-3);
		Coords[count++].set(row, col-3);
		Coords[count++].set(row+1, col-3);
		Coords[count++].set(row+2, col-3);
		//right
		Coords[count++].set(row-2, col+3);
		Coords[count++].set(row-1, col+3);
		Coords[count++].set(row, col+3);
		Coords[count++].set(row+1, col+3);
		Coords[count++].set(row+2, col+3);
		//bottom
		Coords[count++].set(row+3, col-3);
		Coords[count++].set(row+3, col-2);
		Coords[count++].set(row+3, col-1);
		Coords[count++].set(row+3, col);
		Coords[count++].set(row+3, col+1);
		Coords[count++].set(row+3, col+2);
		Coords[count++].set(row+3, col+3);
	}
	*/
}

bool wow_wdtScene::isTileNeeded( STile* tile ) const
{
	if (AdtLoadSize == EAL_3X3)
	{
		return abs((s32)tile->row - Row) <= 1 && abs((s32)tile->col - Col) <= 1;
	}
	else if (AdtLoadSize == EAL_5X5)
	{
		return abs((s32)tile->row - Row) <= 2 && abs((s32)tile->col - Col) <= 2;
	}
// 	else if (AdtLoadSize == EAL_7X7)
// 	{
// 		return abs((s32)tile->row - Row) <= 3 && abs((s32)tile->col - Col) <= 3;
// 	}
	return false;
}

u32 wow_wdtScene::getNumBlocks() const
{
	switch(AdtLoadSize)
	{
	case EAL_3X3:
		return 9;
	case EAL_5X5:
		return 25;
// 	case EAL_7X7:
// 		return 49;
	}
	return 0;
}

void wow_wdtScene::update()
{
	bool tilechanged = updateBlocksByCamera();
	if (!tilechanged)			//如果没有变化时处理
	{
		bool processed = processADT();
		if(!processed)
			unloadOutBlocks();
	}

	//显示当前位置
	if (CamChunk != LastCamChunk)
	{
		if (CamChunk)
		{
			dbc::record r = g_Engine->getWowDatabase()->getAreaTableDB()->getByID(CamChunk->areaID);
			if (r.isValid())
				Q_sprintf(g_Engine->getSceneManager()->AreaName, MAX_TEXT_LENGTH, "%s (%d, %d): %s", FileWDT->getMapName(), Row, Col, r.getString(areaTableDB::Name));
			else
				Q_sprintf(g_Engine->getSceneManager()->AreaName, MAX_TEXT_LENGTH, "%s (%d, %d)", FileWDT->getMapName(), Row, Col);
		}
		else
		{
			Q_sprintf(g_Engine->getSceneManager()->AreaName, MAX_TEXT_LENGTH, "%s (%d, %d)", FileWDT->getMapName(), Row, Col);
		}
		LastCamChunk = CamChunk;
	}
}

void wow_wdtScene::setAdtLoadSize( E_ADT_LOAD size )
{
	AdtLoadSize = size;
}

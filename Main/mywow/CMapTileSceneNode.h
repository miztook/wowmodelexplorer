#pragma once

#include "IMapTileSceneNode.h"
#include "IFileWDT.h"
#include "SMaterial.h"
#include <vector>
#include <list>

class CMapChunk;
class ICamera;
class IM2SceneNode;
class IWMOSceneNode;
class wow_tileScene;
class CFileWDT;

class CMapTileSceneNode : public IMapTileSceneNode
{
public:
	CMapTileSceneNode(IFileWDT* wdt, STile* tile, ISceneNode* parent);
	virtual ~CMapTileSceneNode();

public:
	//IMapTileSceneNode
	virtual IFileADT*	getFileADT() const;
	virtual bool getHeightNormal(f32 x, f32 z, f32* height, vector3df* normal) const;
	virtual vector3df getCenter() const;

	virtual void addM2SceneNodes();
	virtual void addWMOSceneNodes();

	virtual void startLoadingM2SceneNodes();
	virtual void startLoadingWMOSceneNodes();

	//ISceneNode
	virtual void registerSceneNode(bool frustumcheck, int sequence);
	virtual aabbox3df getBoundingBox() const;
	virtual void tick(u32 timeSinceStart, u32 timeSinceLastFrame, bool visible);
	virtual void render() const;
	virtual bool isNodeEligible() const;
	virtual void onUpdated();

protected:
	struct SDynChunk 
	{
		aabbox3df		terrianWorldBox;
		bool highres;
		bool lowres;
	};

	struct SChunkRenderUnit
	{
		SChunkRenderUnit() : row(0), col(0), chunkCount(0) {}

		u8 row;		//start
		u8 col;
		u16 chunkCount;
	};

	class CMapBlock
	{
	private:
		DISALLOW_COPY_AND_ASSIGN(CMapBlock);

	public:
		CMapBlock() : tile(NULL_PTR)
		{
			memset(DynChunks, 0, sizeof(DynChunks));
		}

		STile* tile;

		SDynChunk	DynChunks[16][16];

		typedef std::vector<SChunkRenderUnit> T_ChunkRenderList;
		T_ChunkRenderList HighResChunkRenderList;
		T_ChunkRenderList LowResChunkRenderList;
	};

protected:
	void registerVisibleChunks(ICamera* cam);
	void addChunkRenderList(bool high, u8 row, u8 col);
	void renderChunkRenderList(bool high) const;

protected:
	SMaterial		Material;

	CMapBlock	Block;

	CFileWDT*		FileWDT;
	
	wow_tileScene*			TileScene;

	friend class wow_tileScene;
};
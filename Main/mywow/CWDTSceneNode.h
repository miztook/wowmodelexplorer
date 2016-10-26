#pragma once

#include "IWDTSceneNode.h"
#include "IFileWDT.h"
#include "SMaterial.h"
#include <vector>

class CFileWDT;
class ICamera;
class ISkySceneNode;
class wow_wdtScene;

class CWDTSceneNode : public IWDTSceneNode
{
public:
	CWDTSceneNode(IFileWDT* wdt, ISceneNode* parent);
	virtual ~CWDTSceneNode();

public:
	virtual void setCurrentTile(s32 row, s32 col);
	virtual vector3df getCenter() const;
	virtual bool getHeightNormal(f32 x, f32 z, f32* height, vector3df* normal) const;
	virtual vector3df getPositionByTile(s32 row, s32 col) const;
	virtual bool getTileByPosition(vector3df pos, s32& row, s32& col) const;
	virtual void setAdtLoadSize(E_ADT_LOAD size);

	//ISceneNode
	virtual void registerSceneNode(bool frustumcheck, int sequence);
	virtual aabbox3df getBoundingBox() const { return aabbox3df::Zero(); }
	virtual void tick(u32 timeSinceStart, u32 timeSinceLastFrame, bool visible);
	virtual void render() const;
	virtual bool isNodeEligible() const { return true; }
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
	void registerVisibleChunks(u32 blockIndex, ICamera* cam);
	void updateMapBlock(u32 blockIndex);
	
	void collectBlockRenderList(u32 blockIndex);
	void renderMapBlock(u32 blockIndex) const;
	void addChunkRenderList(u32 blockIndex, bool high, u8 row, u8 col);
	void renderChunkRenderList(u32 blockIndex, bool high) const;

protected:
	SMaterial		Material;
	
	CMapBlock	MapBlocks[25];

	CFileWDT*		FileWDT;

	wow_wdtScene*	WdtScene;

	//friend
	friend class wow_wdtScene;
};
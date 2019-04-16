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
	virtual void setCurrentTile(int32_t row, int32_t col);
	virtual vector3df getCenter() const;
	virtual bool getHeightNormal(float x, float z, float* height, vector3df* normal) const;
	virtual vector3df getPositionByTile(int32_t row, int32_t col) const;
	virtual bool getTileByPosition(vector3df pos, int32_t& row, int32_t& col) const;
	virtual void setAdtLoadSize(E_ADT_LOAD size);

	//ISceneNode
	virtual void registerSceneNode(bool frustumcheck, int sequence);
	virtual aabbox3df getBoundingBox() const { return aabbox3df::Zero(); }
	virtual void tick(uint32_t timeSinceStart, uint32_t timeSinceLastFrame, bool visible);
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

		uint8_t row;		//start
		uint8_t col;
		uint16_t chunkCount;
	};

	class CMapBlock
	{
	private:
		DISALLOW_COPY_AND_ASSIGN(CMapBlock);

	public:
		CMapBlock() : tile(nullptr)
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
	void registerVisibleChunks(uint32_t blockIndex, const ICamera* cam);
	void updateMapBlock(uint32_t blockIndex);
	
	void collectBlockRenderList(uint32_t blockIndex);
	void renderMapBlock(uint32_t blockIndex) const;
	void addChunkRenderList(uint32_t blockIndex, bool high, uint8_t row, uint8_t col);
	void renderChunkRenderList(uint32_t blockIndex, bool high) const;

protected:
	SMaterial		Material;
	
	CMapBlock	MapBlocks[25];

	CFileWDT*		FileWDT;

	wow_wdtScene*	WdtScene;

	//friend
	friend class wow_wdtScene;
};
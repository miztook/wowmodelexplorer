#pragma once

#include "core.h"
#include "IFileWDT.h"

class CMapTileSceneNode;
class IM2SceneNode;
class IWMOSceneNode;
class CMapChunk;
class ICamera;
class CFileWDT;

//wdt的场景管理
class wow_tileScene
{
public:
	explicit wow_tileScene(CMapTileSceneNode* mapTileNode);
	~wow_tileScene();

public:
	void addM2SceneNodes();
	void addWMOSceneNodes();

	void startLoadingM2SceneNodes();
	void startLoadingWMOSceneNodes();

	void update();
	void registerInstances(const ICamera* cam);
	 void setCameraChunk(CMapChunk* chunk) { CamChunk = chunk; }
	 const CMapChunk* getCameraChunk() const { return CamChunk; }

private: 
	void registerVisibleM2Instances(const ICamera* cam);
	void registerVisibleWmoInstances(const ICamera* cam);

	void classifyM2Instance(uint32_t index, const matrix4& instMat);

	void processResources();

public:
	struct SDynM2Inst
	{
		IM2SceneNode*	node;
		aabbox3df box;
		float radius;
	};

	struct SDynWmoInst
	{
		IWMOSceneNode* node;
	};

private:
	CMapTileSceneNode* TileSceneNode;
	CFileWDT*	FileWDT;

	SDynM2Inst*		M2InstSceneNodes;
	bool*		M2InstVisible;

	SDynWmoInst*		WmoInstSceneNodes;
	bool*		WmoInstVisible;

	CMapChunk*			LastCamChunk;
	CMapChunk*			CamChunk;
	
private:
	typedef std::list<uint32_t, qzone_allocator<uint32_t> > T_InstanceList;

	struct SChunkM2List
	{
		T_InstanceList		m2InstList;
	};

	//小物体
	SChunkM2List		ChunkM2List[16][16];

};
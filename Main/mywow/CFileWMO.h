#pragma once

#include "IFileWMO.h"
#include <unordered_map>
#include <map>

struct SWMOBatch
{
	u32 indexStart;
	u16 indexCount;
	u16 vertexStart;
	u16 vertexEnd;
	u16 matId;

	aabbox3df box;

	u16 getVertexCount() const { return vertexEnd - vertexStart + 1; }
};

struct SWMOBspNode 
{
	u16 planetype;		//4: leaf, 0 for YZ-plane, 1 for XZ-plane, 2 for XY-plane?
	s16 left;
	s16 right;
	u16 numfaces;
	u16 firstface;
	f32 distance;

	u16 startIndex;
	u16 minVertex;
	u16 maxVertex;
};

class IFileWMO;
class IIndexBuffer;
class IVertexBuffer;

class CWMOGroup
{
private:
	DISALLOW_COPY_AND_ASSIGN(CWMOGroup);

public:
	CWMOGroup();
	~CWMOGroup();

public:

	bool loadFile(u32 index, IFileWMO* wmo);

	u32		flags;
	u32		NumBatches;
	u32		NumLights;
	u32		NumDoodads;
	u32		NumBspNodes;
	u32		NumBspTriangles;
	u32		ICount;
	u32		VCount;

	u32		VStart;
	u32		IStart;
	c8		name[DEFAULT_SIZE];

	aabbox3df		box;
	SWMOBatch*	Batches;
	u16*		Lights;
	u16*		Doodads;
	u16*		Indices;
	SVertex_PNCT2*	 Vertices;
	u16*		BspTriangles;			//for collide
	SWMOBspNode*		BspNodes;

	//bsp
	SVertex_P*	BspVertices;
	IVertexBuffer*	BspVertexBuffer;
	u16*		BspIndices;
	IIndexBuffer*		BspIndexbuffer;		//for bsp nodes

	//
	bool outdoor;
	bool	indoor;
	bool	hasVertexColor;

private:
	void buildBspVIBuffers();
};

class CFileWMO : public IFileWMO
{
private:
	CFileWMO();
	~CFileWMO();

	friend class CWMOLoader;

public:
	virtual bool loadFile(IMemFile* file);

	virtual bool buildVideoResources();
	virtual void releaseVideoResources();

	u8* getFileData() const { return FileData; }
	aabbox3df getBoundingBox() const { return Box; }
	//portal
	u32 getPortalCountAsFront(u32 frontGroupIndex) const;
	s32 getPortalIndexAsFront(u32 frontGroupIndex, u32 index) const;
	u32 getPortalCountAsBack(u32 backGroupIndex) const;
	s32 getPortalIndexAsBack(u32 backGroupIndex, u32 index) const;

	u32 getNumGroups() const { return Header.nGroups; }

private:
	void clear();

	void buildGroupBuffers();

	void calcBatchVertexCount();

	void buildPortalEntries();

public:
	u8*			FileData;
	aabbox3df	Box;

	IVertexBuffer*	PortalVertexBuffer;

	u32*		Indices;
	SVertex_PNCT2*	 Vertices;
	IVertexBuffer*	VertexBuffer;			//groups vertex buffer
	IIndexBuffer*		IndexBuffer;			//groups index buffer

private:
	struct SPortalEntry
	{
		s32 group0;
		u32 portalIndex;

		bool operator<(const SPortalEntry& other) const
		{
			return group0 < other.group0;
		}
	};

	SPortalEntry*		FrontPortalEntries;
	SPortalEntry*		BackPortalEntries;

	struct SGroupRef			//reference in portalentries
	{
		u32 start;
		u32 count;
	};

#ifdef USE_QALLOCATOR
	typedef std::map<s16, SGroupRef, std::less<s16>, qzone_allocator<std::pair<s16, SGroupRef>>> T_GroupRefMap;
#else
	typedef std::unordered_map<s16, SGroupRef> T_GroupRefMap;
#endif

	T_GroupRefMap	FrontGroupRefMap;
	T_GroupRefMap	BackGroupRefMap;
};


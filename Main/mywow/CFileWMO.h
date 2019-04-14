#pragma once

#include "IFileWMO.h"
#include <unordered_map>
#include <map>

struct SWMOBatch
{
	uint32_t indexStart;
	uint16_t indexCount;
	uint16_t vertexStart;
	uint16_t vertexEnd;
	uint16_t matId;

	aabbox3df box;

	uint16_t getVertexCount() const { return vertexEnd - vertexStart + 1; }
};

struct SWMOBspNode 
{
	uint16_t planetype;		//4: leaf, 0 for YZ-plane, 1 for XZ-plane, 2 for XY-plane?
	int16_t left;
	int16_t right;
	uint16_t numfaces;
	uint16_t firstface;
	float distance;

	uint16_t startIndex;
	uint16_t minVertex;
	uint16_t maxVertex;
};

class IFileWMO;
class CIndexBuffer;
class CVertexBuffer;

class CWMOGroup
{
private:
	DISALLOW_COPY_AND_ASSIGN(CWMOGroup);

public:
	CWMOGroup();
	~CWMOGroup();

public:

	bool loadFile(uint32_t index, IFileWMO* wmo);

	uint32_t		flags;
	uint32_t		NumBatches;
	uint32_t		NumLights;
	uint32_t		NumDoodads;
	uint32_t		NumBspNodes;
	uint32_t		NumBspTriangles;
	uint32_t		ICount;
	uint32_t		VCount;

	uint32_t		VStart;
	uint32_t		IStart;
	char		name[DEFAULT_SIZE];

	aabbox3df		box;
	SWMOBatch*	Batches;
	uint16_t*		Lights;
	uint16_t*		Doodads;
	uint16_t*		Indices;
	SVertex_PNCT2*	 Vertices;
	uint16_t*		BspTriangles;			//for collide
	SWMOBspNode*		BspNodes;

	//bsp
	SVertex_P*	BspVertices;
	CVertexBuffer*	BspVertexBuffer;
	uint16_t*		BspIndices;
	CIndexBuffer*		BspIndexbuffer;		//for bsp nodes

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

	uint8_t* getFileData() const { return FileData; }
	aabbox3df getBoundingBox() const { return Box; }
	//portal
	uint32_t getPortalCountAsFront(uint32_t frontGroupIndex) const;
	int32_t getPortalIndexAsFront(uint32_t frontGroupIndex, uint32_t index) const;
	uint32_t getPortalCountAsBack(uint32_t backGroupIndex) const;
	int32_t getPortalIndexAsBack(uint32_t backGroupIndex, uint32_t index) const;

	uint32_t getNumGroups() const { return Header.nGroups; }

private:
	void clear();

	void buildGroupBuffers();

	void buildPortalEntries();

public:
	uint8_t*			FileData;
	aabbox3df	Box;

	CVertexBuffer*	PortalVertexBuffer;

	uint32_t*		Indices;
	SVertex_PNCT2*	 Vertices;
	CVertexBuffer*	VertexBuffer;			//groups vertex buffer
	CIndexBuffer*		IndexBuffer;			//groups index buffer

private:
	struct SPortalEntry
	{
		int32_t group0;
		uint32_t portalIndex;

		bool operator<(const SPortalEntry& other) const
		{
			return group0 < other.group0;
		}
	};

	SPortalEntry*		FrontPortalEntries;
	SPortalEntry*		BackPortalEntries;

	struct SGroupRef			//reference in portalentries
	{
		uint32_t start;
		uint32_t count;
	};

#ifdef USE_QALLOCATOR
	typedef std::map<int16_t, SGroupRef, std::less<int16_t>, qzone_allocator<std::pair<int16_t, SGroupRef>>> T_GroupRefMap;
#else
	typedef std::unordered_map<int16_t, SGroupRef> T_GroupRefMap;
#endif

	T_GroupRefMap	FrontGroupRefMap;
	T_GroupRefMap	BackGroupRefMap;
};


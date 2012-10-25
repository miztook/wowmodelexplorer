#pragma once

#include "core.h"
#include "IResourceCache.h"
#include "wow_wmo_structs.h"
#include "S3DVertex.h"
#include "SColor.h"

class ITexture;

struct SWMOMaterial
{
	SWMOMaterial() : texture0(NULL), texture1(NULL) {}
	u32 flags;
	bool alphatest;
	ITexture* texture0;
	SColor color0;
	ITexture* texture1;
	SColor color1;
};

struct SWMOBatch
{
	u32 indexStart;
	u16 indexCount;
	u16 vertexStart;
	u16 vertexEnd;
	u8 matId;
};

class IFileWMO;
class IIndexBuffer;
class IVertexBuffer;

struct SWMOGroup
{
	SWMOGroup()
	{
		Indices = NULL;
		GVertices = NULL;
		Batches = NULL;
		Lights = NULL;
		Doodads = NULL;

		IndexBuffer = NULL;
		GVertexBuffer = NULL;

		NumBatches = NumLights = NumDoodads = 0;
	}

	bool loadFile(u32 index, IFileWMO* wmo);

	u32		flags;
	aabbox3df		box;
	c8*		name;
	vector3df b1, b2;
	u32		NumBatches;
	SWMOBatch*	Batches;
	u32		NumLights;
	u16*		Lights;
	u32		NumDoodads;
	u16*		Doodads;

	u32		ICount;
	u16*		Indices;
	u32		VCount;
	SGVertex_PNC*	 GVertices;
	STVertex_2T*		TVertices;

	IVertexBuffer*		GVertexBuffer;
	IVertexBuffer*		TVertexBuffer;
	IIndexBuffer*		IndexBuffer;
	
};

struct SWMOPortal
{
	u16 vStart;
	u16 vCount;
};

struct SWMOLight
{
	E_LIGHT_TYPE lighttype;
	SColor color;
	vector3df position;
	f32 intensity;
	f32 attenStart, attenEnd;
};

struct SWMODoodadSet
{
	c8 name[20];
	u32 start;
	u32 count;
};

struct SWMODoodad
{
	c8* name;
	vector3df position;
	quaternion quat;
	f32 scale;
	SColor color;
};

struct SWMOFog
{
	vector3df position;
	f32 radius1, radius2;
	f32 fogend, fogstart;
	SColor color;
};

class MPQFile;

class IFileWMO : public IReferenceCounted<IFileWMO>
{
protected:
	virtual void onRemove() 
	{
		releaseVideoResources(); 
	}

public:
	IFileWMO() : VideoBuilt(false)
	{
		TextureFileNameBlock = NULL;
		Materials = NULL;
		GroupNamesBlock = NULL;
		Groups = NULL;
		PortalVertices = NULL;
		Portals = NULL;
		Lights = NULL;
		DoodadSets = NULL;
		ModelNamesBlock = NULL;
		Doodads = NULL;
		Fogs = NULL;

		NumDoodads = NumPortalVertices = NumFogs = 0;
	}

	virtual ~IFileWMO() {}

public:
	virtual bool loadFile(MPQFile* file) = 0;
	virtual u8* getFileData() const = 0;
	virtual aabbox3df getBoundingBox() const = 0;

	virtual bool buildVideoResources() = 0;
	virtual void releaseVideoResources() = 0;

public:
	c8		Name[MAX_PATH];

	bool	VideoBuilt;

	WMO::wmoHeader		Header;
	c8*		TextureFileNameBlock;
	SWMOMaterial*		Materials;
	c8*		GroupNamesBlock;
	SWMOGroup*		Groups;
	u32		NumPortalVertices;
	vector3df*		PortalVertices;
	SWMOPortal*		Portals;
	SWMOLight*			Lights;
	c8*		ModelNamesBlock;
	SWMODoodadSet*		DoodadSets;
	u32		NumDoodads;
	SWMODoodad*		Doodads;
	u32		NumFogs;
	SWMOFog*		Fogs;
};
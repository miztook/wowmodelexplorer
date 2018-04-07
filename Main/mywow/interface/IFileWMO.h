#pragma once

#include "core.h"
#include "IResourceCache.h"
#include "wow_wmo_structs.h"
#include "S3DVertex.h"
#include "SColor.h"

class ITexture;

enum E_WMO_SHADER : int32_t
{
	Diffuse = 0,
	Specular,
	Metal,
	Env,
	Opaque,
	EnvMetal,
	TwoLayerDiffuse,
	TwoLayerEnvMetal,
	TwoLayerTerrain,
	DiffuseEmissive,
	Diffuse10,		//
	MaskedEnvMetal,
	EnvMetalEmissive,
	TwoLayerDiffuseOpaque,
	TwoLayerDiffuseEmissive,
	Diffuse15,		//
	Diffuse16,		//
};

struct SWMOMaterial
{
	SWMOMaterial() : texture0(nullptr), texture1(nullptr)
	{
		flags = 0;
		alphatest = false;
		shaderType = Diffuse;
	}
	u32 flags;
	u32	shaderType;
	ITexture* texture0;
	ITexture* texture1;
	SColor color0;
	SColor color1;
	SColor color2;	
	bool alphatest;
};

struct SWMOPortal
{
	SWMOPortal() : vStart(0), vCount(0), frontGroupIndex(-1), backGroupIndex(-1) {}

	u16 vStart;
	u16 vCount;
	s16 frontGroupIndex;
	s16 backGroupIndex;	
	plane3df	plane;
	aabbox3df	 box;

	bool isValid() const { return frontGroupIndex != -1 && backGroupIndex != -1; }
};

struct SWMOPortalRelation
{
	u16 portalIndex;
	s16 groupIndex;
	bool face;
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
	c8 name[256];
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

class IMemFile;
class CWMOGroup;

class IFileWMO : public IReferenceCounted<IFileWMO>
{
protected:
	virtual void onRemove() 
	{
		releaseVideoResources(); 
	}

	virtual ~IFileWMO() {}

public:
	IFileWMO() : VideoBuilt(false)
	{
		TextureFileNameBlock = nullptr;
		Materials = nullptr;
		GroupNamesBlock = nullptr;
		Groups = nullptr;
		PortalVertices = nullptr;
		Portals = nullptr;
		PortalRelations = nullptr;
		Lights = nullptr;
		DoodadSets = nullptr;
		ModelNamesBlock = nullptr;
		Doodads = nullptr;
		Fogs = nullptr;

		NumDoodads = NumPortalVertices = NumFogs = NumPortalRelations = GroupNameBlockSize = DoodadNameBlockSize = 0;

		::memset(Name, 0, sizeof(Name));
		::memset(&Header, 0, sizeof(Header));
	}

public:
	virtual bool loadFile(IMemFile* file) = 0;
	
	virtual bool buildVideoResources() = 0;
	virtual void releaseVideoResources() = 0;

public:
	c8		Name[QMAX_PATH];

	u32		NumPortalVertices;
	u32		NumDoodads;
	u32		NumFogs;
	u32		NumPortalRelations;
	u32		GroupNameBlockSize;
	u32		DoodadNameBlockSize;

	WMO::wmoHeader		Header;
	c8*		TextureFileNameBlock;
	SWMOMaterial*		Materials;
	c8*		GroupNamesBlock;

	CWMOGroup*		Groups;
	vector3df*		PortalVertices;
	SWMOPortal*		Portals;
	SWMOPortalRelation*		PortalRelations;
	SWMOLight*			Lights;
	c8*		ModelNamesBlock;
	SWMODoodadSet*		DoodadSets;
	SWMODoodad*		Doodads;
	SWMOFog*		Fogs;

	bool	VideoBuilt;
};
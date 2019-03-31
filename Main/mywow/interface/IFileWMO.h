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
	uint32_t flags;
	uint32_t	shaderType;
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

	uint16_t vStart;
	uint16_t vCount;
	int16_t frontGroupIndex;
	int16_t backGroupIndex;	
	plane3df	plane;
	aabbox3df	 box;

	bool isValid() const { return frontGroupIndex != -1 && backGroupIndex != -1; }
};

struct SWMOPortalRelation
{
	uint16_t portalIndex;
	int16_t groupIndex;
	bool face;
};

struct SWMOLight
{
	E_LIGHT_TYPE lighttype;
	SColor color;
	vector3df position;
	float intensity;
	float attenStart, attenEnd;
};

struct SWMODoodadSet
{
	char name[20];
	uint32_t start;
	uint32_t count;
};

struct SWMODoodad
{
	char name[256];
	vector3df position;
	quaternion quat;
	float scale;
	SColor color;
};

struct SWMOFog
{
	vector3df position;
	float radius1, radius2;
	float fogend, fogstart;
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
	char		Name[QMAX_PATH];

	uint32_t		NumPortalVertices;
	uint32_t		NumDoodads;
	uint32_t		NumFogs;
	uint32_t		NumPortalRelations;
	uint32_t		GroupNameBlockSize;
	uint32_t		DoodadNameBlockSize;

	WMO::wmoHeader		Header;
	char*		TextureFileNameBlock;
	SWMOMaterial*		Materials;
	char*		GroupNamesBlock;

	CWMOGroup*		Groups;
	vector3df*		PortalVertices;
	SWMOPortal*		Portals;
	SWMOPortalRelation*		PortalRelations;
	SWMOLight*			Lights;
	char*		ModelNamesBlock;
	SWMODoodadSet*		DoodadSets;
	SWMODoodad*		Doodads;
	SWMOFog*		Fogs;

	bool	VideoBuilt;
};
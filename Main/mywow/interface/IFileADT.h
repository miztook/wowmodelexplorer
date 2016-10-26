#pragma once

#include "core.h"
#include "IResourceCache.h"
#include "wow_adt_structs.h"
#include <list>

class IVertexBuffer;
class ITexture;

struct MHDR {
	enum MHDRFlags {
		mhdr_MFBO = 1,                // contains a MFBO chunk.
		mhdr_northrend = 2,           // is set for some northrend ones.
	};
	u32 flags;
	u32* mcin;
	u32* mtex;
	u32* mmdx;
	u32* mmid;
	u32* mwmo;
	u32* mwid;
	u32* mddf;
	u32* modf;
	u32* mfbo;                     // this is only set if flags & mhdr_MFBO.
	u32* mh2o;
	u32* mtfx;
	u32 unused[4];
};

struct SChunkSound
{
	SChunkSound() : soundID(-1) {}

	s32 soundID;			//SoundEntriesAdvanced.dbc
	vector3df pos;
	vector3df range;
};

class CMapChunk
{
private:
	DISALLOW_COPY_AND_ASSIGN(CMapChunk);

public:
	CMapChunk() : numTextures(0), numAlphaMap(0), sounds(NULL_PTR)
	{ 
		::memset(textures,0, sizeof(ITexture*)*4);
		areaID = 0;
		xbase = ybase = zbase = 0;

		::memset(mclys, 0, sizeof(mclys));

#ifdef FIXPIPELINE
		shadowmap = NULL_PTR;
		memset(alphamap, 0, sizeof(alphamap));
		data_shadowmap = new u8[64 * 64];
		for (u32 k=0; k<3; ++k)
		{
			data_alphamap[k] = new u8[64 * 64];
		}
#endif
		data_blendmap = new u8[64 * 64 * 4];
		memset(data_blendmap, 0, 64 * 64 * 4);

	}

	~CMapChunk()
	{
		delete[] sounds;

#ifdef FIXPIPELINE
		delete[] data_shadowmap;
		for (u32 k=0; k<3; ++k)
		{
			delete[] data_alphamap[k];
		}
#endif
		delete[] data_blendmap;
	}

public:
	u32	areaID;
	u32		numTextures;
	u32		numAlphaMap;

	aabbox3df	box;
	f32	xbase, ybase, zbase;
		
	u8*		data_blendmap;

	ITexture*	textures[4];

#ifdef FIXPIPELINE
	u8*		data_shadowmap;
	u8*		data_alphamap[3];
	ITexture*	shadowmap;
	ITexture*	alphamap[3];
#endif

	//sound
	SChunkSound*		sounds;	

	ADT::MCLY	mclys[4];

	bool hasSameTexture(const CMapChunk& other) const
	{
		ASSERT(numTextures <= 4);
		return (numTextures == other.numTextures &&
			memcmp(textures, other.textures, numTextures * sizeof(ITexture*)) == 0);
	}

};

class IMemFile;
class IFileM2;
class IFileWMO;

//maptile
class IFileADT : public IReferenceCounted<IFileADT>
{
protected:
	virtual void onRemove() 
	{
		releaseVideoResources(); 
	}

	virtual ~IFileADT() { }

public:
	IFileADT() : VideoBuilt(false)
	{
		WmoInstances = NULL_PTR;
		M2Instances = NULL_PTR;
		M2FileNameIndices = NULL_PTR;
		M2FileNameBlock = NULL_PTR;
		WmoFileNameIndices = NULL_PTR;
		WmoFileNameBlock = NULL_PTR;

		NumWmoInstance = NumM2Instance = 
		NumWmoFileNames = NumM2FileNames = 0;

		::memset(Name, 0, sizeof(Name));
		Row = Col = 0;
	}

public:
	struct SWmoInstance
	{
		u32 wmoIndex;
		s32 id;
		vector3df pos;
		vector3df dir;
		aabbox3df		box;
		f32		scale;
		u16		doodadset;
		u16		nameset;
	};

	struct SM2Instance
	{
		u32 m2Index;
		u32 id;
		vector3df	dir;
		vector3df	position;
		f32	scale;
	};

public:
	virtual bool loadFile(IMemFile* file) = 0;
	virtual bool loadFileSimple(IMemFile* file) = 0;
	virtual bool loadFileTextures(IMemFile* file) = 0;	

	void SetPosition(u8 row, u8 col) { Row = row; Col = col; }

#ifdef FULL_INTERFACE

	virtual u8* getFileData() const = 0;
	virtual const CMapChunk* getChunk(u8 row, u8 col) const = 0;
	virtual aabbox3df getBoundingBox() const = 0;
	virtual bool getHeight(f32 x, f32 z, f32& height) const = 0;
	virtual bool getNormal(f32 x, f32 z, vector3df& normal) const = 0;

	virtual const c8* getM2FileName(u32 index) const = 0;
	virtual const c8* getWMOFileName(u32 index) const = 0;

	virtual IVertexBuffer* getVBuffer() const = 0;
	virtual E_VERTEX_TYPE getVertexType() const = 0;
	virtual u32 getChunkVerticesOffset(u8 row, u8 col) const = 0;
	virtual ITexture* getBlendMap() const = 0;

	virtual const SM2Instance* getM2Instance(u32 index) const = 0;
	virtual const SWmoInstance* getWMOInstance(u32 index) const = 0;

#endif

	virtual bool buildVideoResources() = 0;
	virtual void releaseVideoResources() = 0;

protected:
	SM2Instance*		M2Instances;
	SWmoInstance*		WmoInstances;

	u32* M2FileNameIndices;
	c8*	M2FileNameBlock;	

	u32*  WmoFileNameIndices;
	c8*	WmoFileNameBlock;

public:
	u32 NumWmoInstance;
	
	u32 NumM2Instance;
	
	u32 NumM2FileNames;
	
	u32 NumWmoFileNames;		
	
protected:
	c8			Name[QMAX_PATH];
	u8		Row;
	u8		Col;

	//
	bool		VideoBuilt;
};
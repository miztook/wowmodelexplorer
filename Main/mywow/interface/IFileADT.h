#pragma once

#include "core.h"
#include "IResourceCache.h"
#include "wow_adt_structs.h"
#include <list>

class CVertexBuffer;
class ITexture;

struct MHDR {
	enum MHDRFlags {
		mhdr_MFBO = 1,                // contains a MFBO chunk.
		mhdr_northrend = 2,           // is set for some northrend ones.
	};
	uint32_t flags;
	uint32_t* mcin;
	uint32_t* mtex;
	uint32_t* mmdx;
	uint32_t* mmid;
	uint32_t* mwmo;
	uint32_t* mwid;
	uint32_t* mddf;
	uint32_t* modf;
	uint32_t* mfbo;                     // this is only set if flags & mhdr_MFBO.
	uint32_t* mh2o;
	uint32_t* mtfx;
	uint32_t unused[4];
};

struct SChunkSound
{
	SChunkSound() : soundID(-1) {}

	int32_t soundID;			//SoundEntriesAdvanced.dbc
	vector3df pos;
	vector3df range;
};

class CMapChunk
{
private:
	DISALLOW_COPY_AND_ASSIGN(CMapChunk);

public:
	CMapChunk() : numTextures(0), numAlphaMap(0), sounds(nullptr)
	{ 
		::memset(textures,0, sizeof(ITexture*)*4);
		areaID = 0;
		xbase = ybase = zbase = 0;

		::memset(mclys, 0, sizeof(mclys));

#ifdef FIXPIPELINE
		shadowmap = nullptr;
		memset(alphamap, 0, sizeof(alphamap));
		data_shadowmap = new uint8_t[64 * 64];
		for (uint32_t k=0; k<3; ++k)
		{
			data_alphamap[k] = new uint8_t[64 * 64];
		}
#endif
		data_blendmap = new uint8_t[64 * 64 * 4];
		memset(data_blendmap, 0, 64 * 64 * 4);

	}

	~CMapChunk()
	{
		delete[] sounds;

#ifdef FIXPIPELINE
		delete[] data_shadowmap;
		for (uint32_t k=0; k<3; ++k)
		{
			delete[] data_alphamap[k];
		}
#endif
		delete[] data_blendmap;
	}

public:
	uint32_t	areaID;
	uint32_t		numTextures;
	uint32_t		numAlphaMap;

	aabbox3df	box;
	float	xbase, ybase, zbase;
		
	uint8_t*		data_blendmap;

	ITexture*	textures[4];

#ifdef FIXPIPELINE
	uint8_t*		data_shadowmap;
	uint8_t*		data_alphamap[3];
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
		WmoInstances = nullptr;
		M2Instances = nullptr;
		M2FileNameIndices = nullptr;
		M2FileNameBlock = nullptr;
		WmoFileNameIndices = nullptr;
		WmoFileNameBlock = nullptr;

		NumWmoInstance = NumM2Instance = 
		NumWmoFileNames = NumM2FileNames = 0;

		::memset(Name, 0, sizeof(Name));
		Row = Col = 0;
	}

public:
	struct SWmoInstance
	{
		uint32_t wmoIndex;
		int32_t id;
		vector3df pos;
		vector3df dir;
		aabbox3df		box;
		float		scale;
		uint16_t		doodadset;
		uint16_t		nameset;
	};

	struct SM2Instance
	{
		uint32_t m2Index;
		uint32_t id;
		vector3df	dir;
		vector3df	position;
		float	scale;
	};

public:
	virtual bool loadFile(IMemFile* file) = 0;
	virtual bool loadFileSimple(IMemFile* file) = 0;
	virtual bool loadFileTextures(IMemFile* file) = 0;	

	void SetPosition(uint8_t row, uint8_t col) { Row = row; Col = col; }

	virtual bool buildVideoResources() = 0;
	virtual void releaseVideoResources() = 0;

protected:
	SM2Instance*		M2Instances;
	SWmoInstance*		WmoInstances;

	uint32_t* M2FileNameIndices;
	char*	M2FileNameBlock;	

	uint32_t*  WmoFileNameIndices;
	char*	WmoFileNameBlock;

public:
	uint32_t NumWmoInstance;
	
	uint32_t NumM2Instance;
	
	uint32_t NumM2FileNames;
	
	uint32_t NumWmoFileNames;		
	
protected:
	char			Name[QMAX_PATH];
	uint8_t		Row;
	uint8_t		Col;

	//
	bool		VideoBuilt;
};
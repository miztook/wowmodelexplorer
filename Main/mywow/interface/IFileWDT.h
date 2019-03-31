#pragma once

#include "core.h"
#include "IResourceCache.h"
#include "wow_adt_structs.h"

#define TILENUM		64

class IMemFile;
class IFileADT;
class CMapEnvironment;

struct STile	
{
	STile() : fileAdt(nullptr){ row = col = 0 ; }
	uint8_t  row;
	uint8_t  col;
	IFileADT*	fileAdt;
};

//WDT定义一块地图的基本信息，WDT包含多个Tile, 每个Tile可能包含ADT文件，
//不包含ADT文件的Tile就是不使用也不可见
//ADT文件描述这个Tile的具体信息: 模型,纹理,物体...
//WDL文件包含一个低分辨率的地形模型，可能是用来画远处的地形轮廓
class IFileWDT : public IReferenceCounted<IFileWDT>
{
protected:
	virtual ~IFileWDT() { }

public:
	IFileWDT() 
	{
		WmoFileNameIndices = nullptr;
		WmoFileNameBlock = nullptr;
		WmoInstances = nullptr;
		TileOffsets = nullptr;

		::memset(GlobalWmoFileName, 0, QMAX_PATH);
		::memset(&GlobalWmoInstance, 0, sizeof(SWmoInstance));

		TextureInfos = nullptr;
		TextureFileNameBlock = nullptr;

		NumTextures = NumWmoFileNames = NumWmoInstance = 0;
	}
	
public:
	struct SWmoInstance
	{
		uint32_t wmoIndex;
		int32_t id;
		vector3df pos;
		vector3df dir;
		matrix4		mat;
		aabbox3df		box;
		float		scale;
	};

public:
	virtual bool loadFile(IMemFile* file, int32_t mapid) = 0;
	virtual bool loadFileSimple(IMemFile* file, int32_t mapid) = 0;		//only tiles

	virtual CMapEnvironment*	 getMapEnvironment() const  = 0;
	virtual STile* getTile(uint8_t row, uint8_t col) = 0;

public:
	//wmo
	uint32_t		NumWmoFileNames;
	uint32_t		NumWmoInstance;
	uint32_t		NumTextures;

	uint32_t*		WmoFileNameIndices;
	char*		WmoFileNameBlock;
	
	SWmoInstance*		WmoInstances;
	int32_t*		TileOffsets;
	 
	//tex
	ADT::TXBT*	TextureInfos;
	char* TextureFileNameBlock;

	char		GlobalWmoFileName[QMAX_PATH];
	SWmoInstance		GlobalWmoInstance;

	std::vector<STile>	Tiles;
};

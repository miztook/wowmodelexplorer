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
	STile() : fileAdt(NULL_PTR){ row = col = 0 ; }
	u8  row;
	u8  col;
	IFileADT*	fileAdt;
};

//WDT定义一块地图的基本信息，WDT包含多个Tile, 每个Tile可能包含ADT文件，
//不包含ADT文件的Tile就是不使用也不可见
//ADT文件描述这个Tile的具体信息: 模型,纹理,物体...
//WDL文件包含一个低分辨率的地形模型，可能是用来画远处的地形轮廓
class IFileWDT : public IReferenceCounted<IFileWDT>
{
protected:
	virtual void onRemove() {}
	virtual ~IFileWDT() { }

public:
	IFileWDT() 
	{
		WmoFileNameIndices = NULL_PTR;
		WmoFileNameBlock = NULL_PTR;
		WmoInstances = NULL_PTR;
		TileOffsets = NULL_PTR;

		::memset(GlobalWmoFileName, 0, QMAX_PATH);
		::memset(&GlobalWmoInstance, 0, sizeof(SWmoInstance));

		TextureInfos = NULL_PTR;
		TextureFileNameBlock = NULL_PTR;

		NumTextures = NumWmoFileNames = NumWmoInstance = 0;
	}
	
public:
	struct SWmoInstance
	{
		u32 wmoIndex;
		s32 id;
		vector3df pos;
		vector3df dir;
		matrix4		mat;
		aabbox3df		box;
		f32		scale;
	};

public:
	virtual bool loadFile(IMemFile* file, s32 mapid) = 0;
	virtual bool loadFileSimple(IMemFile* file, s32 mapid) = 0;		//only tiles

#ifdef FULL_INTERFACE

	virtual u8* getFileData() const = 0;
	virtual s32 getMapId() const = 0;
	virtual const c16* getMapName() const = 0;

	virtual bool getPositionByTile(s32 row, s32 col, vector3df& pos) = 0;
	virtual bool getTileByPosition(vector3df pos, s32& row, s32& col) = 0;
	virtual STile* getTile(u32 index) = 0;
	virtual u32 getTileCount() const = 0;

	virtual void getADTFileName(u8 row, u8 col, c8* name, u32 size) const= 0;
	virtual const c8* getWMOFileName(u32 index) const = 0;

	virtual bool loadADT(STile* tile, bool simple = false) = 0;
	virtual bool loadADTTextures(STile* tile) = 0;
	virtual bool unloadADT(STile* tile) = 0;

#endif

	virtual CMapEnvironment*	 getMapEnvironment() const  = 0;
	virtual STile* getTile(u8 row, u8 col) = 0;

public:
	//wmo
	u32		NumWmoFileNames;
	u32		NumWmoInstance;
	u32		NumTextures;

	u32*		WmoFileNameIndices;
	c8*		WmoFileNameBlock;
	
	SWmoInstance*		WmoInstances;
	s32*		TileOffsets;
	 
	//tex
	ADT::TXBT*	TextureInfos;
	c8* TextureFileNameBlock;

	c8		GlobalWmoFileName[QMAX_PATH];
	SWmoInstance		GlobalWmoInstance;

	std::vector<STile>	Tiles;
};

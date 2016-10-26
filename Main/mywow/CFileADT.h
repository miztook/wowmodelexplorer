#pragma once

#include "IFileADT.h"
#include "S3DVertex.h"
#include "VertexIndexBuffer.h"

class CFileADT : public IFileADT
{
private:
	CFileADT();
	~CFileADT();

	friend class CADTLoader;

public:
	virtual bool loadFile(IMemFile* file);
	virtual bool loadFileSimple(IMemFile* file);				//load obj only
	virtual bool loadFileTextures(IMemFile* file);			//load texture only
	
	u8* getFileData() const { return FileData; }
	const CMapChunk* getChunk(u8 row, u8 col) const;
	aabbox3df getBoundingBox() const { return Box; }
	bool getHeight(f32 x, f32 z, f32& height) const;
	bool getNormal(f32 x, f32 z, vector3df& normal) const;

	const c8* getM2FileName(u32 index) const;
	const c8* getWMOFileName(u32 index) const;

	virtual bool buildVideoResources();
	virtual void releaseVideoResources();

	IVertexBuffer* getVBuffer() const { return VertexBuffer; }
	E_VERTEX_TYPE getVertexType() const { return EVT_PNCT2; }
	u32 getChunkVerticesOffset(u8 row, u8 col) const { return (row * 16 + col) * 145; }
	ITexture* getBlendMap() const { return BlendMap; }

	const SM2Instance* getM2Instance(u32 index) const { return &M2Instances[index]; }
	const SWmoInstance* getWMOInstance(u32 index) const { return &WmoInstances[index]; }

	const c8* getTextureName(u32 index) const;
	u32 getNumTextures() const { return (u32)Textures.size(); }

private:
	void readChunk( IMemFile* file, u8 row, u8 col, u32 lastpos); 
	
	void loadObj0();

	bool loadObj0Simple();

	bool loadTex(u32 n);

	void buildMaps();

	void buildTexcoords();

	void getGridPosition(u8 row, u8 col, u8 gridRow, u8 gridCol, vector3df* positions) const;
	void getGridNormal(u8 row, u8 col, u8 gridRow, u8 gridCol, vector3df* normals) const;

	u32 getVertexIndex(u8 row, u8 col) const;	//找到 9 * 9 + 8 * 8顶点的索引

private:
	struct SChunkM2List
	{
		std::vector<u32>		m2InstList;
	};

	struct STex
	{
		STex(ITexture* tex, bool spec) : texture(tex), specular(spec) {}
		ITexture* texture;
		bool	specular;
	};

private:
	u8*			FileData;

	//chunk
	SVertex_PNCT2*		Vertices;
	IVertexBuffer*		VertexBuffer;
	u32*		Data_BlendMap;
	ITexture*		BlendMap;				//整个adt的blend map,由16 X 16个chunk的map合成

	aabbox3df	Box;

	CMapChunk		Chunks[16][16];

	std::vector<STex>		Textures;	
};

inline u32 CFileADT::getVertexIndex( u8 row, u8 col ) const
{
	ASSERT(row < 17);

	if (row % 2 ==0)
	{
		ASSERT(col < 9);
		return row / 2 * (9 + 8) + col;
	}
	else
	{
		ASSERT(col < 8);
		return row / 2 * (9 + 8) + 9 + col;
	}
}
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
	
	uint8_t* getFileData() const { return FileData; }
	const CMapChunk* getChunk(uint8_t row, uint8_t col) const;
	aabbox3df getBoundingBox() const { return Box; }
	bool getHeight(float x, float z, float& height) const;
	bool getNormal(float x, float z, vector3df& normal) const;

	const char* getM2FileName(uint32_t index) const;
	const char* getWMOFileName(uint32_t index) const;

	virtual bool buildVideoResources();
	virtual void releaseVideoResources();

	CVertexBuffer* getVBuffer() const { return VertexBuffer; }
	E_VERTEX_TYPE getVertexType() const { return EVT_PNCT2; }
	uint32_t getChunkVerticesOffset(uint8_t row, uint8_t col) const { return (row * 16 + col) * 145; }
	ITexture* getBlendMap() const { return BlendMap; }

	const SM2Instance* getM2Instance(uint32_t index) const { return &M2Instances[index]; }
	const SWmoInstance* getWMOInstance(uint32_t index) const { return &WmoInstances[index]; }

	const char* getTextureName(uint32_t index) const;
	uint32_t getNumTextures() const { return (uint32_t)Textures.size(); }

private:
	void readChunk( IMemFile* file, uint8_t row, uint8_t col, uint32_t lastpos); 
	
	void loadObj0();

	bool loadObj0Simple();

	bool loadTex(uint32_t n);

	void buildMaps();

	void buildTexcoords();

	void getGridPosition(uint8_t row, uint8_t col, uint8_t gridRow, uint8_t gridCol, vector3df* positions) const;
	void getGridNormal(uint8_t row, uint8_t col, uint8_t gridRow, uint8_t gridCol, vector3df* normals) const;

	uint32_t getVertexIndex(uint8_t row, uint8_t col) const;	//找到 9 * 9 + 8 * 8顶点的索引

private:
	struct SChunkM2List
	{
		std::vector<uint32_t>		m2InstList;
	};

	struct STex
	{
		STex(ITexture* tex, bool spec) : texture(tex), specular(spec) {}
		ITexture* texture;
		bool	specular;
	};

private:
	uint8_t*			FileData;

	//chunk
	SVertex_PNCT2*		Vertices;
	CVertexBuffer*		VertexBuffer;
	uint32_t*		Data_BlendMap;
	ITexture*		BlendMap;				//整个adt的blend map,由16 X 16个chunk的map合成

	aabbox3df	Box;

	CMapChunk		Chunks[16][16];

	std::vector<STex>		Textures;	
};

inline uint32_t CFileADT::getVertexIndex( uint8_t row, uint8_t col ) const
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
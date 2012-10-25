#pragma once

#include "IFileADT.h"
#include "S3DVertex.h"
#include "VertexIndexBuffer.h"

class CADTLoader
{
public:
	bool isALoadableFileExtension( const c8* filename ) { return hasFileExtensionA(filename, "adt"); }

	IFileADT* loadADT( MPQFile* file );
};

class CFileADT : public IFileADT
{
private:
	CFileADT();
	~CFileADT();

	friend class CADTLoader;

public:
	virtual bool loadFile(MPQFile* file);
	virtual u8* getFileData() const { return FileData; }
	virtual const SMapChunk* getChunk(u8 row, u8 col) const;
	virtual aabbox3df getBoundingBox() const { return Box; }

	virtual const c8* getM2FileName(u32 index) const;
	virtual const c8* getWMOFileName(u32 index) const;

	virtual bool buildVideoResources();
	virtual void releaseVideoResources();

	virtual IVertexBuffer* getGBuffer() const { return GVertexBuffer; }
	virtual IVertexBuffer* getTBuffer() const { return TVertexBuffer; }
	virtual E_VERTEX_TYPE getVertexType() const { return EVT_2TCOORDS; }
	virtual u32 getChunkVerticesOffset(u8 row, u8 col) const { return (row * 16 + col) * 145; }

	virtual const SM2Instance* getM2Instance(u32 index) const { return &M2Instances[index]; }
	virtual const SWmoInstance* getWMOInstance(u32 index) const { return &WmoInstances[index]; }

private:
	void readChunk( MPQFile* file, u8 row, u8 col, u32 lastpos); 
	
	void loadObj0();

	bool loadTex(u32 n);

	void buildTexcoords(STVertex_2T* tVertices);

private:
	struct SChunkM2List
	{
		std::vector<u32>		m2InstList;
	};

private:
	u8*			FileData;

	SMapChunk		Chunks[16][16];

	//chunk
	SGVertex_PNC*		GVertices;
	STVertex_2T*		TVertices;

	IVertexBuffer*		GVertexBuffer;
	IVertexBuffer*		TVertexBuffer;

	struct STex
	{
		STex(ITexture* tex, bool spec) : texture(tex), specular(spec) {}
		ITexture* texture;
		bool	specular;
	};

	std::vector<STex>		Textures;

	//小物体
	SChunkM2List		LittleChunkM2List[16][16];
	//大物体
	SChunkM2List		BigChunkM2List;

	aabbox3df	Box;
};
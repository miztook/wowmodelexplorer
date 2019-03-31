#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D9

#include "IDrawServices.h"
#include "S3DVertex.h"
#include "core.h"
#include <vector>
#include <unordered_map>

class CVertexBuffer;
class CIndexBuffer;
class CD3D9Driver;
class CD3D9HardwareBufferServices;

class CD3D9DrawServices : public IDrawServices
{
private:
	DISALLOW_COPY_AND_ASSIGN(CD3D9DrawServices);

public:
	CD3D9DrawServices();
	~CD3D9DrawServices();

public:
	virtual void add2DLine(const line2di& line, SColor color);
	virtual void add2DRect(const recti& rect, SColor color);
	virtual void flushAll2DLines();

	//line
	virtual void add3DLine(const line3df& line, SColor color);
	virtual void addAABB(const aabbox3df& box, SColor color);
	virtual void add3DBox(const vector3df& vPos, const vector3df& vDir, const vector3df& vUp, const vector3df& vRight, const vector3df& vExts, SColor color, const matrix4* pMat);
	virtual void addSphere(const vector3df& center, float radius, SColor color, uint32_t hori = 10, uint32_t vert = 6);
	virtual void flushAll3DLines(ICamera* cam);

	//flat
	virtual void addAABB_Flat(const aabbox3df& box, SColor color);
	virtual void add3DBox_Flat(const vector3df& vPos, const vector3df& vDir, const vector3df& vUp, const vector3df& vRight, const vector3df& vExts, SColor color, const matrix4* pMat);
	virtual void addSphere_Flat(const vector3df& center, float radius, SColor color, uint32_t hori = 10, uint32_t vert = 6);
	virtual void add3DVertices(vector3df* verts, uint32_t numverts, uint16_t* indices, uint32_t numindices, SColor color);
	virtual void add3DVertices(vector3df* verts, uint32_t numverts, SColor color);
	virtual void flushAll3DVertices(ICamera* cam);

	virtual void add2DColor(const recti& rect, SColor color, const S2DBlendParam& blendParam = S2DBlendParam::OpaqueSource());
	virtual void add2DQuads(ITexture* texture, const SVertex_PCT* vertices, uint32_t numQuads, const S2DBlendParam& blendParam = S2DBlendParam::OpaqueSource());
	virtual void flushAll2DQuads();

	virtual void draw2DColor(const recti& rect, 
		SColor color,
		float scale = 1.0f, 
		E_BLEND_FACTOR srcBlend=EBF_ONE, 
		E_BLEND_FACTOR destBlend=EBF_ONE_MINUS_SRC_ALPHA);
	virtual void draw2DImage(ITexture* texture,
		vector2di destPos,
		const recti* sourceRect = nullptr,
		SColor color = SColor(),
		E_RECT_UVCOORDS uvcoords = ERU_00_11,
		float scale = 1.0f,
		const S2DBlendParam& blendParam = S2DBlendParam::OpaqueSource());
	virtual void draw2DImageBatch(ITexture* texture,
		const vector2di* positions,
		const recti* sourceRects[],
		uint32_t batchCount,	
		SColor color = SColor(),
		E_RECT_UVCOORDS uvcoords = ERU_00_11,
		float scale = 1.0f,
		const S2DBlendParam& blendParam = S2DBlendParam::OpaqueSource());
	virtual void draw2DImageBatch(ITexture* texture,
		const vector2di* positions,
		const recti* sourceRects[],
		const SColor* colors,
		uint32_t batchCount,	
		E_RECT_UVCOORDS uvcoords = ERU_00_11,
		float scale = 1.0f,
		const S2DBlendParam& blendParam = S2DBlendParam::OpaqueSource());
	virtual void draw2DImageRect(ITexture* texture,
		const recti* destRect,
		const recti* sourceRect = nullptr,
		SColor color = SColor(),
		E_RECT_UVCOORDS uvcoords = ERU_00_11,
		const S2DBlendParam& blendParam = S2DBlendParam::OpaqueSource());
	virtual void draw2DImageRectBatch(ITexture* texture,
		const recti* destRects[],
		const recti* sourceRects[],
		uint32_t batchCount,
		SColor color = SColor(),
		E_RECT_UVCOORDS uvcoords = ERU_00_11,
		const S2DBlendParam& blendParam = S2DBlendParam::OpaqueSource());
	virtual void draw2DImageRectBatch(ITexture* texture,
		const recti* destRects[],
		const recti* sourceRects[],
		const SColor* colors,
		uint32_t batchCount,
		E_RECT_UVCOORDS uvcoords = ERU_00_11,
		const S2DBlendParam& blendParam = S2DBlendParam::OpaqueSource());

	virtual void draw2DSquadBatch(ITexture* texture,
		const SVertex_PCT* verts,
		uint32_t numQuads,
		const S2DBlendParam& blendParam);

	virtual void draw2DVertices(ITexture* texture,
		const SVertex_PCT* verts,
		uint32_t numVerts,
		const uint16_t* indices,
		uint32_t numIndices,
		const S2DBlendParam& blendParam = S2DBlendParam::OpaqueSource());

private:
	struct SQuadDrawBatchKey
	{
		S2DBlendParam blendParam;
		ITexture* texture;

		SQuadDrawBatchKey(ITexture* tex, const S2DBlendParam& param)
			: texture(tex), blendParam(param)
		{
		}

		bool operator<(const SQuadDrawBatchKey& other) const
		{
			if (texture != other.texture)
				return texture < other.texture;
			else if (blendParam != other.blendParam)
				return blendParam < other.blendParam;
			return false;
		}

		bool operator==(const SQuadDrawBatchKey& other) const
		{
			return texture == other.texture &&
				blendParam == other.blendParam;
		}
	};

	struct SQuadBatchDraw
	{
		SQuadBatchDraw()
		{
			vertNum = 0;
		}

		SVertex_PCT	drawVerts[MAX_TEXT_LENGTH * 4];
		uint32_t		vertNum;
	};

	struct SQuadDrawBatchKey_hash
	{
		size_t operator()(const SQuadDrawBatchKey& _Keyval) const
		{
			return CRC32_BlockChecksum(&_Keyval, sizeof(_Keyval));
		}
	};

#ifdef USE_QALLOCATOR
	typedef std::map<SQuadDrawBatchKey, SQuadBatchDraw, std::less<SQuadDrawBatchKey>, qzone_allocator<std::pair<SQuadDrawBatchKey, SQuadBatchDraw>>> T_2DQuadDrawMap;
#else
	typedef std::unordered_map<SQuadDrawBatchKey, SQuadBatchDraw, SQuadDrawBatchKey_hash> T_2DQuadDrawMap;
#endif

private:
	SVertex_PC*		Line2DVertices;		//line
	SVertex_PC*		Line3DVertices;
	SVertex_PCT*		ImageVertices;		//image
	SVertex_PC*		Vertices3D;		//3d
	uint16_t*		Indices3D;
	SVertex_PCT*		Vertices2D;		//2d
	uint16_t*		Indices2D;

	CVertexBuffer*		VBLine2D;		//line
	CVertexBuffer*		VBLine3D;
	CVertexBuffer*		VBImage;		//image
	CVertexBuffer*		VB3D;		//3d
	CIndexBuffer*		IB3D;
	CVertexBuffer*		VB2D;		//2d
	CIndexBuffer*		IB2D;

	CD3D9Driver*		Driver;
	CD3D9HardwareBufferServices*		HWBufferServices;

	uint32_t		Line2DVertexLimit;	//line
	uint32_t		Line3DVertexLimit;
	uint32_t		ImageVertexLimit;		//image
	uint32_t		VertexLimit;				//3d
	uint32_t		IndexLimit;
	uint32_t		MaxImageBatch;
	uint32_t		VertexLimit2D;		//2d
	uint32_t		IndexLimit2D;

	//	
	uint32_t		Line2DVertexCount;
	uint32_t		Line3DVertexCount;
	uint32_t		CurrentVertex3DCount;
	uint32_t		CurrentIndex3DCount;

	//
	T_2DQuadDrawMap		m_2DQuadDrawMap;
};

#endif
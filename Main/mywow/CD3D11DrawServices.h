#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D11

#include "IDrawServices.h"
#include "S3DVertex.h"
#include "core.h"
#include <unordered_map>

class IVertexBuffer;
class IIndexBuffer;
class CD3D11Driver;
class CD3D11HardwareBufferServices;

class CD3D11DrawServices : public IDrawServices
{
private:
	DISALLOW_COPY_AND_ASSIGN(CD3D11DrawServices);
public:
	CD3D11DrawServices();
	~CD3D11DrawServices();

public:
	virtual void add2DLine(const line2di& line, SColor color);
	virtual void add2DRect(const recti& rect, SColor color);
	virtual void flushAll2DLines();

	//line
	virtual void add3DLine(const line3df& line, SColor color);
	virtual void addAABB(const aabbox3df& box, SColor color);
	virtual void add3DBox(const vector3df& vPos, const vector3df& vDir, const vector3df& vUp, const vector3df& vRight, const vector3df& vExts, SColor color, const matrix4* pMat);
	virtual void addSphere(const vector3df& center, float radius, SColor color, u32 hori = 10, u32 vert = 6);
	virtual void flushAll3DLines(ICamera* cam);

	//flat
	virtual void addAABB_Flat(const aabbox3df& box, SColor color);
	virtual void add3DBox_Flat(const vector3df& vPos, const vector3df& vDir, const vector3df& vUp, const vector3df& vRight, const vector3df& vExts, SColor color, const matrix4* pMat);
	virtual void addSphere_Flat(const vector3df& center, float radius, SColor color, u32 hori = 10, u32 vert = 6);
	virtual void add3DVertices(vector3df* verts, u32 numverts, u16* indices, u32 numindices, SColor color);
	virtual void add3DVertices(vector3df* verts, u32 numverts, SColor color);
	virtual void flushAll3DVertices(ICamera* cam);

	virtual void add2DColor(const recti& rect, SColor color, const S2DBlendParam& blendParam = S2DBlendParam::OpaqueSource());
	virtual void add2DQuads(ITexture* texture, const SVertex_PCT* vertices, u32 numQuads, const S2DBlendParam& blendParam = S2DBlendParam::OpaqueSource());
	virtual void flushAll2DQuads();

	virtual void draw2DColor(const recti& rect, 
		SColor color,
		float scale = 1.0f, 
		E_BLEND_FACTOR srcBlend=EBF_ONE, 
		E_BLEND_FACTOR destBlend=EBF_ONE_MINUS_SRC_ALPHA);
	virtual void draw2DImage(ITexture* texture,
		vector2di destPos,
		const recti* sourceRect = NULL_PTR,
		SColor color = SColor(),
		E_RECT_UVCOORDS uvcoords = ERU_00_11,
		float scale = 1.0f,
		const S2DBlendParam& blendParam = S2DBlendParam::OpaqueSource());
	virtual void draw2DImageBatch(ITexture* texture,
		const vector2di* positions,
		const recti* sourceRects[],
		u32 batchCount,	
		SColor color = SColor(),
		E_RECT_UVCOORDS uvcoords = ERU_00_11,
		float scale = 1.0f,
		const S2DBlendParam& blendParam = S2DBlendParam::OpaqueSource());
	virtual void draw2DImageBatch(ITexture* texture,
		const vector2di* positions,
		const recti* sourceRects[],
		const SColor* colors,
		u32 batchCount,	
		E_RECT_UVCOORDS uvcoords = ERU_00_11,
		float scale = 1.0f,
		const S2DBlendParam& blendParam = S2DBlendParam::OpaqueSource());
	virtual void draw2DImageRect(ITexture* texture,
		const recti* destRect,
		const recti* sourceRect = NULL_PTR,
		SColor color = SColor(),
		E_RECT_UVCOORDS uvcoords = ERU_00_11,
		const S2DBlendParam& blendParam = S2DBlendParam::OpaqueSource());
	virtual void draw2DImageRectBatch(ITexture* texture,
		const recti* destRects[],
		const recti* sourceRects[],
		u32 batchCount,
		SColor color = SColor(),
		E_RECT_UVCOORDS uvcoords = ERU_00_11,
		const S2DBlendParam& blendParam = S2DBlendParam::OpaqueSource());
	virtual void draw2DImageRectBatch(ITexture* texture,
		const recti* destRects[],
		const recti* sourceRects[],
		const SColor* colors,
		u32 batchCount,
		E_RECT_UVCOORDS uvcoords = ERU_00_11,
		const S2DBlendParam& blendParam = S2DBlendParam::OpaqueSource());

	virtual void draw2DSquadBatch(ITexture* texture,
		const SVertex_PCT* verts,
		u32 numQuads,
		const S2DBlendParam& blendParam);

	virtual void draw2DVertices(ITexture* texture,
		const SVertex_PCT* verts,
		u32 numVerts,
		const u16* indices,
		u32 numIndices,
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
		u32		vertNum;
	};

	struct SQuadDrawBatchKey_hash
	{
		size_t operator()(const SQuadDrawBatchKey& _Keyval) const
		{
			return CRC32_BlockChecksum(&_Keyval, sizeof(_Keyval));
		}
	};

	typedef std::unordered_map<SQuadDrawBatchKey, SQuadBatchDraw, SQuadDrawBatchKey_hash> T_2DQuadDrawMap;

private:
	SVertex_PC*		Line2DVertices;		//line
	SVertex_PC*		Line3DVertices;
	SVertex_PCT*		ImageVertices;		//image
	SVertex_PC*		Vertices3D;		//3d
	u16*		Indices3D;
	SVertex_PCT*		Vertices2D;		//2d
	u16*		Indices2D;

	IVertexBuffer*		VBLine2D;		//line
	IVertexBuffer*		VBLine3D;
	IVertexBuffer*		VBImage;		//image
	IVertexBuffer*		VB3D;		//3d
	IIndexBuffer*		IB3D;
	IVertexBuffer*		VB2D;		//2d
	IIndexBuffer*		IB2D;

	CD3D11Driver*		Driver;
	CD3D11HardwareBufferServices*		HWBufferServices;

	u32		Line2DVertexLimit;	//line
	u32		Line3DVertexLimit;
	u32		ImageVertexLimit;		//image
	u32		VertexLimit;				//3d
	u32		IndexLimit;
	u32		MaxImageBatch;
	u32		VertexLimit2D;		//2d
	u32		IndexLimit2D;

	//	
	u32		Line2DVertexCount;
	u32		Line3DVertexCount;
	u32		CurrentVertex3DCount;
	u32		CurrentIndex3DCount;

	//
	T_2DQuadDrawMap		m_2DQuadDrawMap;
};


#endif
#pragma once

#include "core.h"
#include "SColor.h"
#include "SMaterial.h"

class ICamera;
class ITexture;
struct SVertex_PCT;

class IDrawServices
{
public:
	IDrawServices() { ZTest2DQuadEnable = false; ScissorTestEnable = false; }
	virtual ~IDrawServices() {}

public:
	virtual void add2DLine(const line2di& line, SColor color) = 0;
	virtual void add2DRect(const recti& rect, SColor color) = 0;
	virtual void flushAll2DLines() = 0;

	//line
	virtual void add3DLine(const line3df& line, SColor color) = 0;
	virtual void addAABB(const aabbox3df& box, SColor color) = 0;
	virtual void add3DBox(const vector3df& vPos, const vector3df& vDir, const vector3df& vUp, const vector3df& vRight, const vector3df& vExts, SColor color, const matrix4* pMat) = 0;
	virtual void addSphere(const vector3df& center, float radius, SColor color, uint32_t hori = 10, uint32_t vert = 6) = 0;
	virtual void flushAll3DLines(ICamera* cam) = 0;

	//flat
	virtual void addAABB_Flat(const aabbox3df& box, SColor color) = 0;
	virtual void add3DBox_Flat(const vector3df& vPos, const vector3df& vDir, const vector3df& vUp, const vector3df& vRight, const vector3df& vExts, SColor color, const matrix4* pMat) = 0;
	virtual void addSphere_Flat(const vector3df& center, float radius, SColor color, uint32_t hori = 10, uint32_t vert = 6) = 0;
	virtual void add3DVertices(vector3df* verts, uint32_t numverts, uint16_t* indices, uint32_t numindices, SColor color) = 0;
	virtual void add3DVertices(vector3df* verts, uint32_t numverts, SColor color) = 0;
	virtual void flushAll3DVertices(ICamera* cam) = 0;

	virtual void add2DColor(const recti& rect, SColor color, const S2DBlendParam& blendParam = S2DBlendParam::OpaqueSource()) = 0;
	virtual void add2DQuads(ITexture* texture, const SVertex_PCT* vertices, uint32_t numQuads, const S2DBlendParam& blendParam = S2DBlendParam::OpaqueSource()) = 0;
	virtual void flushAll2DQuads() = 0;

	virtual void draw2DColor(const recti& rect, 
		SColor color,
		float scale = 1.0f, 
		E_BLEND_FACTOR srcBlend=EBF_ONE, 
		E_BLEND_FACTOR destBlend=EBF_ONE_MINUS_SRC_ALPHA) = 0;
	virtual void draw2DImage(ITexture* texture,
		vector2di destPos,
		const recti* sourceRect = nullptr,
		SColor color = SColor(),
		E_RECT_UVCOORDS uvcoords = ERU_00_11,
		float scale = 1.0f,
		const S2DBlendParam& blendParam = S2DBlendParam::OpaqueSource()) = 0;
	virtual void draw2DImageBatch(ITexture* texture,
		const vector2di* positions,
		const recti* sourceRects[],
		uint32_t batchCount,	
		SColor color = SColor(),
		E_RECT_UVCOORDS uvcoords = ERU_00_11,
		float scale = 1.0f,
		const S2DBlendParam& blendParam = S2DBlendParam::OpaqueSource()) = 0;
	virtual void draw2DImageBatch(ITexture* texture,
		const vector2di* positions,
		const recti* sourceRects[],
		const SColor* colors,
		uint32_t batchCount,	
		E_RECT_UVCOORDS uvcoords = ERU_00_11,
		float scale = 1.0f,
		const S2DBlendParam& blendParam = S2DBlendParam::OpaqueSource()) = 0;
	virtual void draw2DImageRect(ITexture* texture,
		const recti* destRect,
		const recti* sourceRect = nullptr,
		SColor color = SColor(),
		E_RECT_UVCOORDS uvcoords = ERU_00_11,
		const S2DBlendParam& blendParam = S2DBlendParam::OpaqueSource()) = 0;
	virtual void draw2DImageRectBatch(ITexture* texture,
		const recti* destRects[],
		const recti* sourceRects[],
		uint32_t batchCount,
		SColor color = SColor(),
		E_RECT_UVCOORDS uvcoords = ERU_00_11,
		const S2DBlendParam& blendParam = S2DBlendParam::OpaqueSource()) = 0;
	virtual void draw2DImageRectBatch(ITexture* texture,
		const recti* destRects[],
		const recti* sourceRects[],
		const SColor* colors,
		uint32_t batchCount,
		E_RECT_UVCOORDS uvcoords = ERU_00_11,
		const S2DBlendParam& blendParam = S2DBlendParam::OpaqueSource()) = 0;

	virtual void draw2DSquadBatch(ITexture* texture,
		const SVertex_PCT* verts,
		uint32_t numQuads,
		const S2DBlendParam& blendParam) = 0;

	virtual void draw2DVertices(ITexture* texture,
		const SVertex_PCT* verts,
		uint32_t numVerts,
		const uint16_t* indices,
		uint32_t numIndices,
		const S2DBlendParam& blendParam = S2DBlendParam::OpaqueSource()) = 0;

	SMaterial& getLineMaterial() { return LineMaterial; }

	void set2DQuadZTestEnable(bool enable) { ZTest2DQuadEnable = enable; }
	bool is2DQuadZTestEnable() const { return ZTest2DQuadEnable; }
	void setScissorTestEnable(bool enable) { ScissorTestEnable = enable; }
	bool isScissorTestEnable() const { return ScissorTestEnable; }

	void set3DLineZTestEnable(bool enable) { LineMaterial.ZBuffer = enable ? ECFN_LESSEQUAL : ECFN_ALWAYS; }
	bool is3DLineZTestEnable() const { return LineMaterial.ZBuffer == ECFN_LESSEQUAL; }

public:
	static uint32_t MAX_2DLINE_BATCH_COUNT() { return 128; }
	static uint32_t MAX_3DLINE_BATCH_COUNT() { return 1024; }
	static uint32_t MAX_IMAGE_BATCH_COUNT() { return MAX_TEXT_LENGTH; }
	static uint32_t MAX_VERTEX_COUNT() { return 4096; }
	static uint32_t MAX_INDEX_COUNT() { return 6144; }
	static uint32_t MAX_VERTEX_2D_COUNT() { return 64; }
	static uint32_t MAX_INDEX_2D_COUNT() { return 128; }

protected:
	SMaterial	LineMaterial;
	bool		ZTest2DQuadEnable;
	bool		ScissorTestEnable;
};
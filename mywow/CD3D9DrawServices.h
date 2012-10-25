#pragma once

#include "IDrawServices.h"
#include "S3DVertex.h"
#include "core.h"
#include "SMaterial.h"

class IVertexBuffer;
class IIndexBuffer;
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
	virtual void add2DLine(line2di line, SColor color);
	virtual void add2DRect(const recti& rect, SColor color);
	virtual void flushAll2DLines();

	virtual void add3DLine(line3df line, SColor color);
	virtual void add3DBox(const aabbox3df& box, SColor color);
	virtual void addSphere(vector3df center, f32 radius, SColor color, u32 hori = 10, u32 vert = 6);
	virtual void flushAll3DLines(ICamera* cam);

	virtual void add3DVertices(vector3df* verts, u32 numverts, u16* indices, u32 numindices, SColor color);
	virtual void flushAll3DVertices(ICamera* cam);

	virtual void draw2DSolid(const recti& rect, SColor color, f32 scale = 1.0f, E_BLEND_FACTOR srcBlend=EBF_ONE, E_BLEND_FACTOR destBlend=EBF_ONE_MINUS_SRC_ALPHA);
	virtual void draw2DImage(ITexture* texture, vector2di destPos, bool alphaChannel, f32 scale = 1.0f, E_BLEND_FACTOR srcBlend=EBF_ONE, E_BLEND_FACTOR destBlend=EBF_ONE_MINUS_SRC_ALPHA);
	virtual void draw2DImage(ITexture* texture,
		vector2di destPos,
		bool useAlphaChannel,
		const recti* sourceRect,
		SColor color,
		f32 scale = 1.0f,
		E_BLEND_FACTOR srcBlend=EBF_ONE, 
		E_BLEND_FACTOR destBlend=EBF_ONE_MINUS_SRC_ALPHA);
	virtual void draw2DImageBatch(ITexture* texture,
		const vector2di* positions,
		const recti** sourceRects,
		u32 batchCount,
		f32 scale = 1.0f,
		SColor color = SColor(),
		bool useAlphachannel = false,
		E_BLEND_FACTOR srcBlend=EBF_ONE, 
		E_BLEND_FACTOR destBlend=EBF_ONE_MINUS_SRC_ALPHA);

private:
	static const u32 MAX_2DLINE_BATCH_COUNT = 128;
	static const u32 MAX_3DLINE_BATCH_COUNT = 512;
	static const u32 MAX_IMAGE_BATCH_COUNT	= 512;
	static const u32 MAX_VERTEX_COUNT = 2048;
	static const u32 MAX_INDEX_COUNT = 512;

	SGVertex_PC*		Line2DVertices;		//line
	SGVertex_PC*		Line3DVertices;
	SGTVertex_PC1T*		ImageVertices;		//image
	u16* ImageIndices;
	SGVertex_PC*		Vertices;		//3d
	u16*		Indices;

	IVertexBuffer*		VBLine2D;		//line
	IVertexBuffer*		VBLine3D;
	IVertexBuffer*		VBImage;		//image
	IIndexBuffer*		IBImage;
	IVertexBuffer*		VB3D;		//3d
	IIndexBuffer*		IB3D;

	u32		Line2DVertexLimit;	//line
	u32		Line3DVertexLimit;
	u32		ImageVertexLimit;		//image
	u32		ImageIndexLimit;
	u32		VertexLimit;				//3d
	u32		IndexLimit;

	CD3D9Driver*		Driver;
	CD3D9HardwareBufferServices*		HWBufferServices;
	SMaterial	LineMaterial;

	//	
	u32		Line2DVertexCount;
	u32		Line3DVertexCount;
	u32		CurrentImageCount;
	u32		CurrentVertex3DCount;
	u32		CurrentIndex3DCount;
};
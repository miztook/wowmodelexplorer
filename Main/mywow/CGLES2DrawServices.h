#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_GLES2

#include "IDrawServices.h"
#include "S3DVertex.h"
#include "core.h"

class IVertexBuffer;
class IIndexBuffer;
class CGLES2Driver;
class CGLES2HardwareBufferServices;

class CGLES2DrawServices : public IDrawServices
{
private:
	DISALLOW_COPY_AND_ASSIGN(CGLES2DrawServices);

public:
	CGLES2DrawServices();
	~CGLES2DrawServices();

public:
	virtual void add2DLine(const line2di& line, SColor color);
	virtual void add2DRect(const recti& rect, SColor color);
	virtual void flushAll2DLines();

	virtual void add3DLine(const line3df& line, SColor color);
	virtual void add3DBox(const aabbox3df& box, SColor color);
	virtual void addSphere(vector3df center, f32 radius, SColor color, u32 hori = 10, u32 vert = 6);
	virtual void flushAll3DLines(ICamera* cam);

	virtual void add3DVertices(vector3df* verts, u32 numverts, u16* indices, u32 numindices, SColor color);
	virtual void add3DVertices(vector3df* verts, u32 numverts, SColor color);
	virtual void flushAll3DVertices(ICamera* cam);

	virtual void draw2DColor(const recti& rect, 
		SColor color, 
		E_RECT_UVCOORDS uvcoords = ERU_00_11,
		f32 scale = 1.0f, 
		E_BLEND_FACTOR srcBlend=EBF_ONE,
		E_BLEND_FACTOR destBlend=EBF_ONE_MINUS_SRC_ALPHA);

	virtual void draw2DImage(ITexture* texture,
		vector2di destPos,
		const recti* sourceRect = NULL,
		SColor color = SColor(),
		E_RECT_UVCOORDS uvcoords = ERU_00_11,
		f32 scale = 1.0f,
		bool useAlphaChannel = false,
		E_BLEND_FACTOR srcBlend=EBF_ONE, 
		E_BLEND_FACTOR destBlend=EBF_ONE_MINUS_SRC_ALPHA);
	virtual void draw2DImageBatch(ITexture* texture,
		const vector2di* positions,
		const recti* sourceRects[],
		u32 batchCount,
		SColor color = SColor(),
		E_RECT_UVCOORDS uvcoords = ERU_00_11,
		f32 scale = 1.0f,
		bool useAlphachannel = false,
		E_BLEND_FACTOR srcBlend=EBF_ONE, 
		E_BLEND_FACTOR destBlend=EBF_ONE_MINUS_SRC_ALPHA);
	virtual void draw2DImageRect(ITexture* texture,
		const recti* destRect,
		const recti* sourceRect = NULL,
		SColor color = SColor(),
		E_RECT_UVCOORDS uvcoords = ERU_00_11,
		bool useAlphaChannel = false,
		E_BLEND_FACTOR srcBlend=EBF_ONE, 
		E_BLEND_FACTOR destBlend=EBF_ONE_MINUS_SRC_ALPHA);
	virtual void draw2DImageRectBatch(ITexture* texture,
		const recti* destRects[],
		const recti* sourceRects[],
		u32 batchCount,
		SColor color = SColor(),
		E_RECT_UVCOORDS uvcoords = ERU_00_11,
		bool useAlphachannel = false,
		E_BLEND_FACTOR srcBlend=EBF_ONE, 
		E_BLEND_FACTOR destBlend=EBF_ONE_MINUS_SRC_ALPHA);

private:
	SVertex_PC*		Line2DVertices;		//line
	SVertex_PC*		Line3DVertices;
	SVertex_PCT*		ImageVertices;		//image
	SVertex_PC*		Vertices;		//3d
	u16*		Indices;

	IVertexBuffer*		VBLine2D;		//line
	IVertexBuffer*		VBLine3D;
	IVertexBuffer*		VBImage;		//image
	IVertexBuffer*		VB3D;		//3d
	IIndexBuffer*		IB3D;

	CGLES2Driver*		Driver;
	CGLES2HardwareBufferServices*		HWBufferServices;

	u32		Line2DVertexLimit;	//line
	u32		Line3DVertexLimit;
	u32		ImageVertexLimit;		//image
	u32		VertexLimit;				//3d
	u32		IndexLimit;
	u32		MaxImageBatch;

	//	
	u32		Line2DVertexCount;
	u32		Line3DVertexCount;
	u32		CurrentVertex3DCount;
	u32		CurrentIndex3DCount;
};

#endif
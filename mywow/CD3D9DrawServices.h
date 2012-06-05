#pragma once

#include "IDrawServices.h"
#include "S3DVertex.h"
#include "core.h"
#include "SMaterial.h"
#include <d3d9.h>

class IVertexBuffer;
class IIndexBuffer;
class CD3D9Driver;
class CD3D9HardwareBufferServices;

class CD3D9DrawServices : public IDrawServices
{
public:
	CD3D9DrawServices();
	~CD3D9DrawServices();

public:
	virtual void add2DLine(line2di line, SColor color);
	virtual void flushAll2DLines();

	virtual void add3DLine(line3df line, SColor color);
	virtual void add3DBox(const aabbox3df& box, SColor color);
	virtual void addSphere(vector3df center, f32 radius, SColor color, u32 hori = 10, u32 vert = 6);
	virtual void flushAll3DLines(ICamera* cam);

	virtual void draw2DImage(ITexture* texture, vector2di destPos, bool alphaChannel);
	virtual void draw2DImage(ITexture* texture,
		vector2di destPos,
		bool useAlphaChannel,
		const recti* sourceRect,
		SColor color,
		f32 scale = 1.0f);
	virtual void draw2DImageBatch(ITexture* texture,
		const vector2di* positions,
		const recti** sourceRects,
		u32 batchCount,
		f32 scale = 1.0f,
		SColor color = SColor(),
		bool useALphachannel = false);

	virtual void draw3DVerts(vector3df* verts, u32 numverts, u16* indices, u32 numindices, SColor color, const matrix4& world);

private:
	static const u32 MAX_2DLINE_BATCH_COUNT = 100;
	static const u32 MAX_3DLINE_BATCH_COUNT = 1024;
	static const u32 MAX_IMAGE_BATCH_COUNT	= 512;

	S3DVertexBasicColor*		Line2DVertices;
	S3DVertexBasicColor*		Line3DVertices;
	S3DVertexBasicTex*		ImageVertices;
	u16* ImageIndices;

	IVertexBuffer*		VBLine2D;
	IVertexBuffer*		VBLine3D;
	IVertexBuffer*		VBImage;
	IIndexBuffer*		IBImage;

	u32		Line2DVertexLimit;
	u32		Line3DVertexLimit;
	u32		ImageVertexLimit;
	u32		ImageIndexLimit;

	u32		Line2DVertexCount;
	u32		Line3DVertexCount;

	S3DVertexBasicTex			SingleImageVertices[4];

	CD3D9Driver*		Driver;
	CD3D9HardwareBufferServices*		HWBufferServices;
	SMaterial	LineMaterial;

	//
	u32			CurrentImageCount;
};
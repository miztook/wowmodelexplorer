#pragma once

#include "core.h"
#include "SColor.h"

class ICamera;
class ITexture;

class IDrawServices
{
public:
	virtual ~IDrawServices() {}

public:
	virtual void add2DLine(line2di line, SColor color) = 0;
	virtual void add2DRect(const recti& rect, SColor color) = 0;
	virtual void flushAll2DLines() = 0;

	virtual void add3DLine(line3df line, SColor color) = 0;
	virtual void add3DBox(const aabbox3df& box, SColor color) = 0;
	virtual void addSphere(vector3df center, f32 radius, SColor color, u32 hori = 10, u32 vert = 6) = 0;
	virtual void flushAll3DLines(ICamera* cam) = 0;

	virtual void add3DVertices(vector3df* verts, u32 numverts, u16* indices, u32 numindices, SColor color) = 0;
	virtual void flushAll3DVertices(ICamera* cam) = 0;

	virtual void draw2DSolid(const recti& rect, SColor color, f32 scale = 1.0f, E_BLEND_FACTOR srcBlend=EBF_ONE, E_BLEND_FACTOR destBlend=EBF_ONE_MINUS_SRC_ALPHA) = 0;
	virtual void draw2DImage(ITexture* texture, vector2di destPos, bool alphaChannel, f32 scale = 1.0f, E_BLEND_FACTOR srcBlend=EBF_ONE, 
		E_BLEND_FACTOR destBlend=EBF_ONE_MINUS_SRC_ALPHA) = 0;
	virtual void draw2DImage(ITexture* texture,
		vector2di destPos,
		bool useAlphaChannel,
		const recti* sourceRect,
		SColor color,
		f32 scale = 1.0f,
		E_BLEND_FACTOR srcBlend=EBF_ONE, 
		E_BLEND_FACTOR destBlend=EBF_ONE_MINUS_SRC_ALPHA) = 0;
	virtual void draw2DImageBatch(ITexture* texture,
		const vector2di* positions,
		const recti** sourceRects,
		u32 batchCount,
		f32 scale = 1.0f,
		SColor color = SColor(),
		bool useAlphachannel = false,
		E_BLEND_FACTOR srcBlend=EBF_ONE, 
		E_BLEND_FACTOR destBlend=EBF_ONE_MINUS_SRC_ALPHA) = 0;
};
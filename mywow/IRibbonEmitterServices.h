#pragma once

#include "base.h"
#include "wow_particle.h"
#include "S3DVertex.h"

class IVertexBuffer;
class IIndexBuffer;

//给ribbon提供segment池和硬件缓存
class IRibbonEmitterServices
{
public:
	IRibbonEmitterServices() : Vertices(NULL), CurrentOffset(0) {}
	virtual ~IRibbonEmitterServices() {}

	virtual void updateVertices(u32 numVertices, s32& baseVertIndex) = 0;
	virtual RibbonSegment* getSegment()= 0;
	virtual void putSegment(RibbonSegment* s) = 0;

	virtual u32 getActiveSegmentsCount() const = 0;

public:
	IVertexBuffer*		VertexBuffer;						//strip, 不用index buffer

	S3DVertexBasicTex*		Vertices;
	u32		CurrentOffset;
};
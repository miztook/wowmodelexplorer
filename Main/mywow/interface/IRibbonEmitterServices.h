#pragma once

#include "base.h"
#include "wow_particle.h"
#include "S3DVertex.h"
#include "VertexIndexBuffer.h"

//给ribbon提供segment池和硬件缓存
class IRibbonEmitterServices
{
public:
	IRibbonEmitterServices() { Vertices = NULL_PTR; BufferParam.clear(); BufferParam.vType = EVT_PCT; }
	virtual ~IRibbonEmitterServices() {}

public:
	virtual u32 getActiveSegmentsCount() const = 0;
	
#ifdef FULL_INTERFACE

	virtual void updateVertices(u32 numVertices) = 0;
	virtual RibbonSegment* getSegment()= 0;
	virtual void putSegment(RibbonSegment* s) = 0;

	virtual u32 getMaxVertexCount() const = 0;
#endif

public:
	SBufferParam	BufferParam;
	SVertex_PCT*		Vertices;
};
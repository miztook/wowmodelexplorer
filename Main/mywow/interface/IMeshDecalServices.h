#pragma once

#include "base.h"
#include "S3DVertex.h"
#include "VertexIndexBuffer.h"

class IMeshDecalServices
{
public:
	IMeshDecalServices() { Vertices=NULL_PTR; BufferParam.clear(); BufferParam.vType = EVT_PCT; }
	virtual ~IMeshDecalServices() {}

public:

#ifdef FULL_INTERFACE

	virtual void updateVertices(u32 numVertices) = 0;
	virtual u32 getMaxVertexCount() const = 0;
	virtual u32 getBufferQuota() const = 0;

#endif

public:
	SBufferParam		BufferParam;
	SVertex_PCT*		Vertices;
};
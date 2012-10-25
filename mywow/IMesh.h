#pragma once

#include "base.h"
#include "IResourceCache.h"
#include "VertexIndexBuffer.h"

class IMesh
{
public:
	IMesh() : PrimCount(0) { BufferParam.clear(); }
	virtual ~IMesh() { }

public:
	virtual void setBoundingBox(const aabbox3df& box) = 0;
	virtual const aabbox3df& getBoundingBox() const = 0;
	virtual void updateVertexBuffer(u32 index) = 0;
	virtual void updateIndexBuffer() = 0;

public:
	SBufferParam		BufferParam;
	E_PRIMITIVE_TYPE	PrimType;
	u32	PrimCount;
};
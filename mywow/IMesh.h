#pragma once

#include "base.h"
#include "IResourceCache.h"

class IVertexBuffer;
class IIndexBuffer;

class IMesh
{
public:
	IMesh() : VertexBuffer(NULL), IndexBuffer(NULL), PrimCount(0) { }
	virtual ~IMesh() { }

public:
	virtual void update() = 0;
	virtual void setBoundingBox(const aabbox3df& box) = 0;
	virtual const aabbox3df& getBoundingBox() const = 0;

public:
	IVertexBuffer*	VertexBuffer;
	IIndexBuffer*		IndexBuffer;
	E_PRIMITIVE_TYPE	PrimType;
	u32	PrimCount;
};
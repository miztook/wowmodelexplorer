#pragma once

#include "core.h"
#include "IMesh.h"

class CMesh : public IMesh
{
public:
	CMesh(const SBufferParam& bufferParam, E_PRIMITIVE_TYPE primType, uint32_t primCount, const aabbox3df& box);
	~CMesh();

public:
	virtual void setBoundingBox(const aabbox3df& box) { Box = box; }
	virtual const aabbox3df& getBoundingBox() const { return Box; }
	virtual void updateVertexBuffer(uint32_t index);
	virtual void updateIndexBuffer();
private:
	aabbox3df Box;
};
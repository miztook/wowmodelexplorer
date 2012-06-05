#pragma once

#include "core.h"
#include "IMesh.h"

class CMesh : public IMesh
{
public:
	CMesh(IVertexBuffer* vbuffer, IIndexBuffer* ibuffer, E_PRIMITIVE_TYPE primType, u32 primCount, const aabbox3df& box);
	~CMesh();

public:
	virtual void setBoundingBox(const aabbox3df& box) { Box = box; }
	virtual const aabbox3df& getBoundingBox() const { return Box; }
	virtual void update();

private:
	aabbox3df Box;
};
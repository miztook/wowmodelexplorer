#pragma once

#include "IMeshDecalServices.h"
#include "S3DVertex.h"

class CMeshDecalServices : public IMeshDecalServices
{
public:
	explicit CMeshDecalServices(u32 bufferQuota);
	~CMeshDecalServices();

public:
	void updateVertices(u32 numVertices);
	u32 getMaxVertexCount() const { return BufferQuota * 4; }
	u32 getBufferQuota() const { return BufferQuota; }

private:
	void createBuffer();

private:
	u32		BufferQuota;
};
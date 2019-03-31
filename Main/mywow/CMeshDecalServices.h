#pragma once

#include "IMeshDecalServices.h"
#include "S3DVertex.h"

class CMeshDecalServices : public IMeshDecalServices
{
public:
	explicit CMeshDecalServices(uint32_t bufferQuota);
	~CMeshDecalServices();

public:
	void updateVertices(uint32_t numVertices);
	uint32_t getMaxVertexCount() const { return BufferQuota * 4; }
	uint32_t getBufferQuota() const { return BufferQuota; }

private:
	void createBuffer();

private:
	uint32_t		BufferQuota;
};
#pragma once

#include "IRibbonEmitterServices.h"
#include "IResourcePool.h"
#include "S3DVertex.h"

class CRibbonEmitterServices : public IRibbonEmitterServices
{
public:
	CRibbonEmitterServices(uint32_t poolQuota, uint32_t bufferQuota);
	~CRibbonEmitterServices();

public:
	virtual uint32_t getActiveSegmentsCount() const { return SegmentPool.getUsedSize(); }
	
	void updateVertices(uint32_t numVertices);
	RibbonSegment* getSegment();
	void putSegment(RibbonSegment* s);
	uint32_t getMaxVertexCount() const  { return BufferQuota * 2; }

private:
	void createBuffer();

private:
	IResourcePool<RibbonSegment>			SegmentPool;

	uint32_t		PoolQuota;
	uint32_t		BufferQuota;
};
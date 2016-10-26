#pragma once

#include "IRibbonEmitterServices.h"
#include "IResourcePool.h"
#include "S3DVertex.h"

class CRibbonEmitterServices : public IRibbonEmitterServices
{
public:
	CRibbonEmitterServices(u32 poolQuota, u32 bufferQuota);
	~CRibbonEmitterServices();

public:
	virtual u32 getActiveSegmentsCount() const { return SegmentPool.getUsedSize(); }
	
	void updateVertices(u32 numVertices);
	RibbonSegment* getSegment();
	void putSegment(RibbonSegment* s);
	u32 getMaxVertexCount() const  { return BufferQuota * 2; }

private:
	void createBuffer();

private:
	IResourcePool<RibbonSegment>			SegmentPool;

	u32		PoolQuota;
	u32		BufferQuota;
};
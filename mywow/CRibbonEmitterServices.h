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
	virtual void updateVertices(u32 numVertices, s32& baseVertIndex);
	virtual RibbonSegment* getSegment();
	virtual void putSegment(RibbonSegment* s);

	virtual u32 getActiveSegmentsCount() const { return SegmentPool.getUsedSize(); }

private:
	void createBuffer();

private:
	IResourcePool<RibbonSegment>			SegmentPool;

	u32		PoolQuota;
	u32		BufferQuota;
};
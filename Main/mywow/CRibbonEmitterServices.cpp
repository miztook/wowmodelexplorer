#include "stdafx.h"
#include "CRibbonEmitterServices.h"
#include "mywow.h"

CRibbonEmitterServices::CRibbonEmitterServices( u32 poolQuota, u32 bufferQuota )
	: PoolQuota(poolQuota), BufferQuota(bufferQuota)
{
	SegmentPool.allocateAll(PoolQuota);

	createBuffer();
}

CRibbonEmitterServices::~CRibbonEmitterServices()
{
	g_Engine->getHardwareBufferServices()->destroyHardwareBuffers(BufferParam);
	BufferParam.destroy();
}

void CRibbonEmitterServices::createBuffer()
{
	BufferParam.clear();
	BufferParam.vType = EVT_PCT;

	u32 vsize = BufferQuota * 2;							//每个segment 2 个顶点

	//vertex buffer
	Vertices = new SVertex_PCT[vsize];
	BufferParam.vbuffer0 = new IVertexBuffer;
	BufferParam.vbuffer0->set(Vertices, EST_PCT, vsize, EMM_DYNAMIC);
	g_Engine->getHardwareBufferServices()->createHardwareBuffers(BufferParam);
}

void CRibbonEmitterServices::updateVertices( u32 numVertices )
{
	if (!numVertices)
		return;

	g_Engine->getHardwareBufferServices()->updateHardwareBuffer(BufferParam.vbuffer0, numVertices);
}

RibbonSegment* CRibbonEmitterServices::getSegment()
{
	return SegmentPool.get();
}

void CRibbonEmitterServices::putSegment( RibbonSegment* s )
{
	SegmentPool.put(s);
}

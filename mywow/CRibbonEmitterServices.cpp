#include "stdafx.h"
#include "CRibbonEmitterServices.h"
#include "mywow.h"

CRibbonEmitterServices::CRibbonEmitterServices( u32 poolQuota, u32 bufferQuota )
	: PoolQuota(poolQuota), BufferQuota(bufferQuota)
{
	SegmentPool.allocateAll(PoolQuota);

	BufferParam.clear();
	createBuffer();
}

CRibbonEmitterServices::~CRibbonEmitterServices()
{
	g_Engine->getHardwareBufferServices()->destroyHardwareBuffers(BufferParam);
	BufferParam.destroy();
}

void CRibbonEmitterServices::createBuffer()
{
	u32 vsize = BufferQuota * 2;							//每个segment 2 个顶点

	//vertex buffer
	GVertices = new SGVertex_PC[vsize];
	TVertices = new STVertex_1T[vsize];
	BufferParam.vbuffer0 = new IVertexBuffer;
	BufferParam.vbuffer1 = new IVertexBuffer;
	BufferParam.vbuffer0->set(GVertices, EST_G_PC, vsize, EMM_DYNAMIC);
	BufferParam.vbuffer1->set(TVertices, EST_T_1T, vsize, EMM_DYNAMIC);
	g_Engine->getHardwareBufferServices()->createHardwareBuffers(BufferParam);
}

void CRibbonEmitterServices::updateVertices( u32 numVertices, s32& baseVertIndex )
{
	if (!numVertices)
		return;

	if (numVertices + CurrentOffset > BufferParam.vbuffer0->Size)
	{
		baseVertIndex = 0;
		g_Engine->getHardwareBufferServices()->updateHardwareBuffer(BufferParam.vbuffer0, 0, numVertices);
		g_Engine->getHardwareBufferServices()->updateHardwareBuffer(BufferParam.vbuffer1, 0, numVertices);
		CurrentOffset = 0;	
	}
	else
	{
		baseVertIndex = CurrentOffset;
		g_Engine->getHardwareBufferServices()->updateHardwareBuffer(BufferParam.vbuffer0, CurrentOffset, numVertices);
		g_Engine->getHardwareBufferServices()->updateHardwareBuffer(BufferParam.vbuffer1, CurrentOffset, numVertices);
		CurrentOffset += numVertices;	
	}
}

RibbonSegment* CRibbonEmitterServices::getSegment()
{
	return SegmentPool.get();
}

void CRibbonEmitterServices::putSegment( RibbonSegment* s )
{
	SegmentPool.put(s);
}

#include "stdafx.h"
#include "CRibbonEmitterServices.h"
#include "mywow.h"

CRibbonEmitterServices::CRibbonEmitterServices( u32 poolQuota, u32 bufferQuota )
	: PoolQuota(poolQuota), BufferQuota(bufferQuota)
{
	SegmentPool.allocateAll(PoolQuota, true);

	createBuffer();
}

CRibbonEmitterServices::~CRibbonEmitterServices()
{
	g_Engine->getHardwareBufferServices()->destroyHardwareBuffer(VertexBuffer);
	delete VertexBuffer;
}

void CRibbonEmitterServices::createBuffer()
{
	u32 vsize = BufferQuota * 2;							//每个segment 2 个顶点

	//vertex buffer
	Vertices = new S3DVertexBasicTex[vsize];
	VertexBuffer = new IVertexBuffer;
	VertexBuffer->set(Vertices, EVT_BASICTEX, vsize, EMM_DYNAMIC);
	g_Engine->getHardwareBufferServices()->createHardwareBuffer(VertexBuffer);
}

void CRibbonEmitterServices::updateVertices( u32 numVertices, s32& baseVertIndex )
{
	if (!numVertices)
	{
		return;
	}

	if (numVertices + CurrentOffset > VertexBuffer->Size)
	{
		baseVertIndex = 0;
		g_Engine->getHardwareBufferServices()->updateHardwareBuffer(VertexBuffer, 0, numVertices);
		CurrentOffset = 0;	
	}
	else
	{
		baseVertIndex = CurrentOffset;
		g_Engine->getHardwareBufferServices()->updateHardwareBuffer(VertexBuffer, CurrentOffset, numVertices);
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

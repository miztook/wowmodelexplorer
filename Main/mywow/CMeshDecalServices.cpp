#include "stdafx.h"
#include "CMeshDecalServices.h"
#include "mywow.h"


CMeshDecalServices::CMeshDecalServices( u32 bufferQuota )
{
	BufferQuota = min_(bufferQuota, IHardwareBufferServices::MAX_QUADS());

	createBuffer();
}

CMeshDecalServices::~CMeshDecalServices()
{
	BufferParam.ibuffer = NULL_PTR;		//don't destroy common index buffer

	g_Engine->getHardwareBufferServices()->destroyHardwareBuffer(BufferParam.vbuffer0);
	BufferParam.destroy();
}

void CMeshDecalServices::updateVertices( u32 numVertices )
{
	if (!numVertices)
		return;

	g_Engine->getHardwareBufferServices()->updateHardwareBuffer(BufferParam.vbuffer0, numVertices);
}

void CMeshDecalServices::createBuffer()
{
	BufferParam.clear();
	BufferParam.vType = EVT_PCT;

	u32 vsize = BufferQuota * 4;

	//vertex buffer
	Vertices = new SVertex_PCT[vsize];
	BufferParam.vbuffer0 = new IVertexBuffer;

	BufferParam.vbuffer0->set(Vertices, EST_PCT, vsize, EMM_DYNAMIC);

	g_Engine->getHardwareBufferServices()->createHardwareBuffer(BufferParam.vbuffer0);

	//index buffer
	BufferParam.ibuffer = g_Engine->getHardwareBufferServices()->getStaticIndexBufferQuadList();
}

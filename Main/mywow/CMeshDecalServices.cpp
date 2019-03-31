#include "stdafx.h"
#include "CMeshDecalServices.h"
#include "mywow.h"


CMeshDecalServices::CMeshDecalServices( uint32_t bufferQuota )
{
	BufferQuota = min_(bufferQuota, IHardwareBufferServices::MAX_QUADS());

	createBuffer();
}

CMeshDecalServices::~CMeshDecalServices()
{
	BufferParam.ibuffer = nullptr;		//don't destroy common index buffer
	BufferParam.destroy();
}

void CMeshDecalServices::updateVertices( uint32_t numVertices )
{
	if (!numVertices)
		return;

	g_Engine->getHardwareBufferServices()->updateHardwareBuffer(BufferParam.vbuffer0, numVertices);
}

void CMeshDecalServices::createBuffer()
{
	BufferParam.clear();
	BufferParam.vType = EVT_PCT;

	uint32_t vsize = BufferQuota * 4;

	//vertex buffer
	Vertices.resize(vsize);
	BufferParam.vbuffer0 = new CVertexBuffer(false);

	BufferParam.vbuffer0->set(Vertices.data(), EST_PCT, vsize, EMM_DYNAMIC);

	g_Engine->getHardwareBufferServices()->createHardwareBuffer(BufferParam.vbuffer0);

	//index buffer
	BufferParam.ibuffer = g_Engine->getHardwareBufferServices()->getStaticIndexBufferQuadList();
}

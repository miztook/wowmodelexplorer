#include "stdafx.h"
#include "CParticleSystemServices.h"
#include "mywow.h"

CParticleSystemServices::CParticleSystemServices( u32 poolQuota, u32 bufferQuota, float density)
	: PoolQuota(poolQuota), BufferQuota(bufferQuota)
{
	ParticlePool.allocateAll(PoolQuota);

	setParticleDensity(density);

	createBuffer();
}

CParticleSystemServices::~CParticleSystemServices()
{
	g_Engine->getHardwareBufferServices()->destroyHardwareBuffers(BufferParam);
	BufferParam.destroy();
}

void CParticleSystemServices::createBuffer()
{
	u32 vsize = BufferQuota * 4;
	u32 isize = BufferQuota * 6;

	//vertex buffer
	GVertices = new SGVertex_PC[vsize];
	TVertices = new STVertex_1T[vsize];
	BufferParam.vbuffer0 = new IVertexBuffer;
	BufferParam.vbuffer1 = new IVertexBuffer;

	BufferParam.vbuffer0->set(GVertices, EST_G_PC, vsize, EMM_DYNAMIC);
	BufferParam.vbuffer1->set(TVertices, EST_T_1T, vsize, EMM_DYNAMIC);

	g_Engine->getHardwareBufferServices()->createHardwareBuffer(BufferParam.vbuffer0);
	g_Engine->getHardwareBufferServices()->createHardwareBuffer(BufferParam.vbuffer1);

	//index buffer
	u16* indices = (u16*)Hunk_AllocateTempMemory(sizeof(u16) * BufferQuota * 6);
	u32 firstVert = 0;
	u32 firstIndex = 0;
	for (u32 i = 0; i < BufferQuota; ++i )
	{
		indices[firstIndex + 0] = (u16)firstVert + 0;
		indices[firstIndex + 1] = (u16)firstVert + 1;
		indices[firstIndex + 2] = (u16)firstVert + 2;
		indices[firstIndex + 3] = (u16)firstVert + 0;
		indices[firstIndex + 4] = (u16)firstVert + 2;
		indices[firstIndex + 5] = (u16)firstVert + 3;

		firstVert += 4; 
		firstIndex += 6;
	}

	BufferParam.ibuffer = new IIndexBuffer(false);
	BufferParam.ibuffer->set(indices, EIT_16BIT, isize, EMM_STATIC);

	g_Engine->getHardwareBufferServices()->createHardwareBuffer(BufferParam.ibuffer);

	g_Engine->getHardwareBufferServices()->updateHardwareBuffer(BufferParam.ibuffer, 0, isize);
	Hunk_FreeTempMemory(indices);
}

void CParticleSystemServices::updateVertices(u32 numVertices, s32& baseVertIndex)
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

Particle* CParticleSystemServices::getParticle()
{
	return ParticlePool.get();
}

void CParticleSystemServices::putParticle( Particle* p )
{
	ParticlePool.put(p);
}
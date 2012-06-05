#include "stdafx.h"
#include "CParticleSystemServices.h"
#include "mywow.h"

CParticleSystemServices::CParticleSystemServices( u32 poolQuota, u32 bufferQuota, float density)
	: PoolQuota(poolQuota), BufferQuota(bufferQuota)
{
	ParticlePool.allocateAll(PoolQuota, true);

	setParticleDensity(density);

	createBuffer();
}

CParticleSystemServices::~CParticleSystemServices()
{
	g_Engine->getHardwareBufferServices()->destroyHardwareBuffer(IndexBuffer);
	delete IndexBuffer;

	g_Engine->getHardwareBufferServices()->destroyHardwareBuffer(VertexBuffer);
	delete VertexBuffer;
}

void CParticleSystemServices::createBuffer()
{
	u32 vsize = BufferQuota * 4;
	u32 isize = BufferQuota * 6;

	//vertex buffer
	Vertices = new S3DVertexBasicTex[vsize];
	VertexBuffer = new IVertexBuffer;
	VertexBuffer->set(Vertices, EVT_BASICTEX, vsize, EMM_DYNAMIC);
	g_Engine->getHardwareBufferServices()->createHardwareBuffer(VertexBuffer);

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

	IndexBuffer = new IIndexBuffer(false);
	IndexBuffer->set(indices, EIT_16BIT, isize, EMM_STATIC);
	g_Engine->getHardwareBufferServices()->createHardwareBuffer(IndexBuffer);
	g_Engine->getHardwareBufferServices()->updateHardwareBuffer(IndexBuffer, 0, isize);
	Hunk_FreeTempMemory(indices);
}

void CParticleSystemServices::updateVertices(u32 numVertices, s32& baseVertIndex)
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

Particle* CParticleSystemServices::getParticle()
{
	return ParticlePool.get();
}

void CParticleSystemServices::putParticle( Particle* p )
{
	ParticlePool.put(p);
}
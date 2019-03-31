#include "stdafx.h"
#include "CParticleSystemServices.h"
#include "mywow.h"

CParticleSystemServices::CParticleSystemServices( uint32_t poolQuota, uint32_t bufferQuota, float density)
	: PoolQuota(poolQuota), LackParticle(false)
{
	BufferQuota = min_(bufferQuota, IHardwareBufferServices::MAX_QUADS());

	ParticlePool.allocateAll(PoolQuota);

	setParticleDensity(density);

	ParticleFactor = 1.0f;

	createBuffer();
}

CParticleSystemServices::~CParticleSystemServices()
{
	BufferParam.ibuffer = nullptr;		//don't destroy common index buffer

	g_Engine->getHardwareBufferServices()->destroyHardwareBuffer(BufferParam.vbuffer0);
	BufferParam.destroy();
}

void CParticleSystemServices::createBuffer()
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

void CParticleSystemServices::updateVertices(uint32_t numVertices)
{
	if (!numVertices)
		return;

	g_Engine->getHardwareBufferServices()->updateHardwareBuffer(BufferParam.vbuffer0, numVertices);
}

Particle* CParticleSystemServices::getParticle()
{
	Particle* p = ParticlePool.get();
	if (!p)
		LackParticle = true;

	return p;
}

void CParticleSystemServices::putParticle( Particle* p )
{
	ParticlePool.put(p);
}

void CParticleSystemServices::adjustParticles()
{
	float portion = getActiveParticlesCount() / (float)PoolQuota;

	if (portion > 0.99f)
	{
		if (LackParticle)
		{
			ParticleFactor *= 0.95f;
			LackParticle = false;
		}
	}
	else
	{
		if (ParticleFactor < 1.0f)
		{
			ParticleFactor *= 1.05f;
			if (ParticleFactor > 1.0f)
				ParticleFactor =1.0f;
		}
	}
}
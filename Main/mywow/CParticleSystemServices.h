#pragma once

#include "IParticleSystemServices.h"
#include "IResourcePool.h"
#include "S3DVertex.h"

class CParticleSystemServices : public IParticleSystemServices
{
public:
	CParticleSystemServices(u32 poolQuota, u32 bufferQuota, float density);
	~CParticleSystemServices();

public:

	virtual void setParticleDensity(float density) { ParticleDensity = min_(1.0f, density); }
	virtual f32 getParticleDensity() const { return ParticleDensity; }
	
	virtual u32 getActiveParticlesCount() const { return ParticlePool.getUsedSize(); }
	virtual void adjustParticles();

	void updateVertices(u32 numVertices);

	Particle* getParticle();
	void putParticle(Particle* p);

	f32 getParticleDynamicDensity() const { return ParticleDensity * ParticleFactor; }
	u32 getMaxVertexCount() const { return BufferQuota * 4; }
	u32 getBufferQuota() const { return BufferQuota; }

private:
	void createBuffer();

private:
	IResourcePool<Particle>			ParticlePool;

	u32		PoolQuota;
	u32		BufferQuota;

	f32	ParticleDensity;
	f32	ParticleFactor;		//在density基础上动态调整

	bool		LackParticle;
};
#pragma once

#include "IParticleSystemServices.h"
#include "IResourcePool.h"
#include "S3DVertex.h"

class CParticleSystemServices : public IParticleSystemServices
{
public:
	CParticleSystemServices(u32 poolQuota, u32 bufferQuota, float density);
	~CParticleSystemServices();

	virtual void updateVertices(u32 numVertices, s32& baseVertIndex);

	virtual Particle* getParticle();
	virtual void putParticle(Particle* p);

	virtual void setParticleDensity(float density) { ParticleDensity = min_(1.0f, density); }
	virtual float getParticleDensity() const { return ParticleDensity; }

	virtual u32 getActiveParticlesCount() const { return ParticlePool.getUsedSize(); }

private:
	void createBuffer();

private:
	IResourcePool<Particle>			ParticlePool;

	u32		PoolQuota;
	u32		BufferQuota;

	float	ParticleDensity;
};
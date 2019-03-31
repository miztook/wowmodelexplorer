#pragma once

#include "IParticleSystemServices.h"
#include "IResourcePool.h"
#include "S3DVertex.h"

class CParticleSystemServices : public IParticleSystemServices
{
public:
	CParticleSystemServices(uint32_t poolQuota, uint32_t bufferQuota, float density);
	~CParticleSystemServices();

public:

	virtual void setParticleDensity(float density) { ParticleDensity = min_(1.0f, density); }
	virtual float getParticleDensity() const { return ParticleDensity; }
	
	virtual uint32_t getActiveParticlesCount() const { return ParticlePool.getUsedSize(); }
	virtual void adjustParticles();

	void updateVertices(uint32_t numVertices);

	Particle* getParticle();
	void putParticle(Particle* p);

	float getParticleDynamicDensity() const { return ParticleDensity * ParticleFactor; }
	uint32_t getMaxVertexCount() const { return BufferQuota * 4; }
	uint32_t getBufferQuota() const { return BufferQuota; }

private:
	void createBuffer();

private:
	IResourcePool<Particle>			ParticlePool;

	uint32_t		PoolQuota;
	uint32_t		BufferQuota;

	float	ParticleDensity;
	float	ParticleFactor;		//在density基础上动态调整

	bool		LackParticle;
};
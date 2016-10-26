#pragma once

#include "base.h"
#include "wow_particle.h"
#include "S3DVertex.h"
#include "VertexIndexBuffer.h"

//给粒子系统提供粒子池和硬件缓存
class IParticleSystemServices
{
public:
	IParticleSystemServices() { Vertices=NULL_PTR; BufferParam.clear(); BufferParam.vType = EVT_PCT; }
	virtual ~IParticleSystemServices() {}

public:
	virtual void adjustParticles() = 0;		//每帧调整粒子数量
	virtual void setParticleDensity(float density) = 0;		//粒子发射密度
	virtual float getParticleDensity() const = 0;

	virtual u32 getActiveParticlesCount() const = 0;

#ifdef FULL_INTERFACE

	virtual f32 getParticleDynamicDensity() const = 0;
	virtual void updateVertices(u32 numVertices) = 0;
	virtual Particle* getParticle()= 0;
	virtual void putParticle(Particle* p) = 0;
	virtual u32 getMaxVertexCount() const = 0;
	virtual u32 getBufferQuota() const = 0;

#endif

public:
	SBufferParam		BufferParam;
	SVertex_PCT*		Vertices;
};
#pragma once

#include "base.h"
#include "wow_particle.h"
#include "S3DVertex.h"

class IVertexBuffer;
class IIndexBuffer;

//给粒子系统提供粒子池和硬件缓存
class IParticleSystemServices
{
public:
	IParticleSystemServices() : Vertices(NULL), CurrentOffset(0) {}
	virtual ~IParticleSystemServices() {}

	virtual void updateVertices(u32 numVertices, s32& baseVertIndex) = 0;
	virtual Particle* getParticle()= 0;
	virtual void putParticle(Particle* p) = 0;

	virtual void setParticleDensity(float density) = 0;		//粒子发射密度
	virtual float getParticleDensity() const = 0;

	virtual u32 getActiveParticlesCount() const = 0;

public:
	IVertexBuffer*		VertexBuffer;
	IIndexBuffer*			IndexBuffer;

	S3DVertexBasicTex*		Vertices;
	u32		CurrentOffset;
	
};
#pragma once

#include "base.h"
#include "wow_particle.h"
#include "S3DVertex.h"
#include "VertexIndexBuffer.h"
#include <vector>

//给粒子系统提供粒子池和硬件缓存
class IParticleSystemServices
{
public:
	IParticleSystemServices() { BufferParam.clear(); BufferParam.vType = EVT_PCT; }
	virtual ~IParticleSystemServices() {}

public:
	virtual void adjustParticles() = 0;		//每帧调整粒子数量
	virtual void setParticleDensity(float density) = 0;		//粒子发射密度
	virtual float getParticleDensity() const = 0;

	virtual u32 getActiveParticlesCount() const = 0;

public:
	SBufferParam		BufferParam;
	std::vector<SVertex_PCT>		Vertices;
};
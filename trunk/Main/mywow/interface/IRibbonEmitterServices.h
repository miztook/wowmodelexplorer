#pragma once

#include "base.h"
#include "wow_particle.h"
#include "S3DVertex.h"
#include "VertexIndexBuffer.h"
#include <vector>

//给ribbon提供segment池和硬件缓存
class IRibbonEmitterServices
{
public:
	IRibbonEmitterServices() { BufferParam.clear(); BufferParam.vType = EVT_PCT; }
	virtual ~IRibbonEmitterServices() {}

public:
	virtual u32 getActiveSegmentsCount() const = 0;
	
public:
	SBufferParam	BufferParam;
	std::vector<SVertex_PCT>		Vertices;
};
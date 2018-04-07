#pragma once

#include "base.h"
#include "S3DVertex.h"
#include "VertexIndexBuffer.h"

class IMeshDecalServices
{
public:
	IMeshDecalServices() { BufferParam.clear(); BufferParam.vType = EVT_PCT; }
	virtual ~IMeshDecalServices() {}

public:

public:
	SBufferParam		BufferParam;
	std::vector<SVertex_PCT>		Vertices;
};
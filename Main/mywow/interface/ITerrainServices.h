#pragma once

#include "base.h"

class IIndexBuffer;

class ITerrainServices
{
public:
	virtual ~ITerrainServices() {}

public:

#ifdef FULL_INTERFACE

	virtual IIndexBuffer* getLowResIndexBuffer() const = 0;
	virtual IIndexBuffer* getHighResIndexBuffer() const = 0;

	virtual u32 getVertexCount() const = 0;
	virtual u32 getLowResPrimCount() const = 0;
	virtual u32 getHighResPrimCount() const = 0;

#endif
};

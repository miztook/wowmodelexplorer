#pragma once

#include "base.h"

class IVideoResource
{
public:
	virtual ~IVideoResource() {}

protected:
	virtual bool buildVideoResources() = 0;
	virtual void releaseVideoResources() = 0;
	virtual bool hasVideoBuilt() const = 0;

public:
	static bool buildVideoResources(IVideoResource* t)
	{
		ASSERT(t);
		return t->buildVideoResources();
	}

	static void releaseVideoResources(IVideoResource* t)
	{
		ASSERT(t);
		t->releaseVideoResources();
	}

	static bool hasVideoBuilt(IVideoResource* t)
	{
		ASSERT(t);
		return t->hasVideoBuilt();
	}
};

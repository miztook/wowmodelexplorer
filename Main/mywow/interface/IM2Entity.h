#pragma once

#include "base.h"

class IM2Entity
{
public:
	~IM2Entity() {}

public:
	virtual const c8* getName() const = 0;
	virtual void tick(u32 timeSinceStart, u32 timeSinceLastFrame) = 0;

public: 
	LENTRY			Link;
};
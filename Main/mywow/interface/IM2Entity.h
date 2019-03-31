#pragma once

#include "base.h"

class IM2Entity
{
public:
	~IM2Entity() {}

public:
	virtual const char* getName() const = 0;
	virtual void tick(uint32_t timeSinceStart, uint32_t timeSinceLastFrame) = 0;

public: 
	LENTRY			Link;
};
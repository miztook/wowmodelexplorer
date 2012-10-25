#pragma once

#include "core.h"

class IRenderTarget : public ILostResetCallback
{
public:
	IRenderTarget() : VideoBuilt(false) { }
	virtual ~IRenderTarget() { }

public:
	virtual void* lock(bool readOnly = false) = 0;
	virtual void unlock() = 0;

	virtual dimension2du getSize() const = 0;
	virtual ECOLOR_FORMAT getColorFormat() const = 0;

	virtual bool isValid() const = 0;

protected:
	bool VideoBuilt;
};
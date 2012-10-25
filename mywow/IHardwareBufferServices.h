
#pragma once

#include "base.h"

class IVertexBuffer;
class IIndexBuffer;
struct SBufferParam;

class IHardwareBufferServices
{
public:
	virtual ~IHardwareBufferServices() { }

public:
	virtual bool createHardwareBuffers(const SBufferParam& bufferParam) = 0;
	virtual bool createHardwareBuffer(IVertexBuffer* vbuffer) = 0;
	virtual bool createHardwareBuffer(IIndexBuffer* ibuffer) = 0;
	virtual void destroyHardwareBuffers(const SBufferParam& bufferParam) = 0;
	virtual void destroyHardwareBuffer(IVertexBuffer* vbuffer) = 0;
	virtual void destroyHardwareBuffer(IIndexBuffer* ibuffer) = 0;
	virtual bool updateHardwareBuffer(IVertexBuffer* vbuffer, u32 offset, u32 size) = 0;
	virtual bool updateHardwareBuffer(IIndexBuffer* ibuffer, u32 offset, u32 size) = 0;

};
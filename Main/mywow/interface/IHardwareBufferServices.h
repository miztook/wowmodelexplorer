
#pragma once

#include "core.h"
#include "ILostResetCallback.h"

class IVertexBuffer;
class IIndexBuffer;
struct SBufferParam;

class IHardwareBufferServices : public ILostResetCallback
{
public:
	IHardwareBufferServices()
	{
		StaticIndexBufferQuadList = NULL_PTR;
	}
	virtual ~IHardwareBufferServices() { }

public:
	virtual bool createHardwareBuffers(const SBufferParam& bufferParam) = 0;
	virtual bool createHardwareBuffer(IVertexBuffer* vbuffer) = 0;
	virtual bool createHardwareBuffer(IIndexBuffer* ibuffer) = 0;
	virtual void destroyHardwareBuffers(const SBufferParam& bufferParam) = 0;
	virtual void destroyHardwareBuffer(IVertexBuffer* vbuffer) = 0;
	virtual void destroyHardwareBuffer(IIndexBuffer* ibuffer) = 0;
	virtual bool updateHardwareBuffer(IVertexBuffer* vbuffer, u32 size) = 0;
	virtual bool updateHardwareBuffer(IIndexBuffer* ibuffer, u32 size) = 0;

	IIndexBuffer* getStaticIndexBufferQuadList() const { return StaticIndexBufferQuadList; }

    static u32 MAX_QUADS() { return MAX_TEXT_LENGTH; }

protected:
	IIndexBuffer*			StaticIndexBufferQuadList;
};
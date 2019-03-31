
#pragma once

#include "core.h"

class CVertexBuffer;
class CIndexBuffer;
struct SBufferParam;

class IHardwareBufferServices
{
public:
	IHardwareBufferServices()
	{
		StaticIndexBufferQuadList = nullptr;
	}
	virtual ~IHardwareBufferServices() { }

public:
	virtual bool createHardwareBuffers(const SBufferParam& bufferParam) = 0;
	virtual bool createHardwareBuffer(CVertexBuffer* vbuffer) = 0;
	virtual bool createHardwareBuffer(CIndexBuffer* ibuffer) = 0;
	virtual void destroyHardwareBuffers(const SBufferParam& bufferParam) = 0;
	virtual void destroyHardwareBuffer(CVertexBuffer* vbuffer) = 0;
	virtual void destroyHardwareBuffer(CIndexBuffer* ibuffer) = 0;
	virtual bool updateHardwareBuffer(CVertexBuffer* vbuffer, uint32_t size) = 0;
	virtual bool updateHardwareBuffer(CIndexBuffer* ibuffer, uint32_t size) = 0;

	CIndexBuffer* getStaticIndexBufferQuadList() const { return StaticIndexBufferQuadList; }

    static uint32_t MAX_QUADS() { return MAX_TEXT_LENGTH; }

protected:
	CIndexBuffer*			StaticIndexBufferQuadList;
};
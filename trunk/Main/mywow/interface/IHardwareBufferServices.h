
#pragma once

#include "core.h"
#include "ILostResetCallback.h"

class CVertexBuffer;
class CIndexBuffer;
struct SBufferParam;

class IHardwareBufferServices : public ILostResetCallback
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
	virtual bool updateHardwareBuffer(CVertexBuffer* vbuffer, u32 size) = 0;
	virtual bool updateHardwareBuffer(CIndexBuffer* ibuffer, u32 size) = 0;

	CIndexBuffer* getStaticIndexBufferQuadList() const { return StaticIndexBufferQuadList; }

    static u32 MAX_QUADS() { return MAX_TEXT_LENGTH; }

protected:
	CIndexBuffer*			StaticIndexBufferQuadList;
};
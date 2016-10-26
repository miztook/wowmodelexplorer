#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_GLES2

#include "IHardwareBufferServices.h"
#include "core.h"

class CGLES2Driver;

class CGLES2HardwareBufferServices : public IHardwareBufferServices
{
private:
	DISALLOW_COPY_AND_ASSIGN(CGLES2HardwareBufferServices);

public:
	CGLES2HardwareBufferServices();
	~CGLES2HardwareBufferServices();

public:
	virtual bool createHardwareBuffers(const SBufferParam& bufferParam);
	virtual bool createHardwareBuffer(IVertexBuffer* vbuffer);
	virtual bool createHardwareBuffer(IIndexBuffer* ibuffer);
	virtual void destroyHardwareBuffers(const SBufferParam& bufferParam);
	virtual void destroyHardwareBuffer(IVertexBuffer* vbuffer);
	virtual void destroyHardwareBuffer(IIndexBuffer* ibuffer);
	virtual bool updateHardwareBuffer(IVertexBuffer* vbuffer, u32 size);
	virtual bool updateHardwareBuffer(IIndexBuffer* ibuffer, u32 size);

public:
	virtual void onLost();
	virtual void onReset();

private:
	bool internalCreateVertexBuffer( IVertexBuffer* vbuffer );
	bool internalCreateIndexBuffer( IIndexBuffer* ibuffer );

	void createStaticIndexBufferQuadList();
	void destroyStaticIndexBufferQuadList();

	CGLES2Driver*		Driver;

	LENTRY	VertexBufferList;
	LENTRY	IndexBufferList;
};


#endif
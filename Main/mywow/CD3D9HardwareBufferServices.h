#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D9

#include "IHardwareBufferServices.h"
#include "core.h"

class CD3D9Driver;

class CD3D9HardwareBufferServices : public IHardwareBufferServices
{
private:
	DISALLOW_COPY_AND_ASSIGN(CD3D9HardwareBufferServices);

public:
	CD3D9HardwareBufferServices();
	~CD3D9HardwareBufferServices();

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
	bool fillVertexBuffer(IVertexBuffer* vbuffer);
	bool fillIndexBuffer(IIndexBuffer* ibuffer);

	void createStaticIndexBufferQuadList();
	void destroyStaticIndexBufferQuadList();

	CD3D9Driver*			Driver;

	LENTRY	VertexBufferList;
	LENTRY	IndexBufferList;
};

#endif
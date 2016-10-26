#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D11

#include "IHardwareBufferServices.h"

class CD3D11Driver;

class CD3D11HardwareBufferServices : public IHardwareBufferServices
{
private:
	DISALLOW_COPY_AND_ASSIGN(CD3D11HardwareBufferServices);

public:
	CD3D11HardwareBufferServices();
	~CD3D11HardwareBufferServices();

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

	CD3D11Driver*	Driver;

	LENTRY	VertexBufferList;
	LENTRY	IndexBufferList;
};

#endif
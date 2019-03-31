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
	virtual bool createHardwareBuffer(CVertexBuffer* vbuffer);
	virtual bool createHardwareBuffer(CIndexBuffer* ibuffer);
	virtual void destroyHardwareBuffers(const SBufferParam& bufferParam);
	virtual void destroyHardwareBuffer(CVertexBuffer* vbuffer);
	virtual void destroyHardwareBuffer(CIndexBuffer* ibuffer);
	virtual bool updateHardwareBuffer(CVertexBuffer* vbuffer, uint32_t size);
	virtual bool updateHardwareBuffer(CIndexBuffer* ibuffer, uint32_t size);

private:
	bool internalCreateVertexBuffer( CVertexBuffer* vbuffer );
	bool internalCreateIndexBuffer( CIndexBuffer* ibuffer );

	void createStaticIndexBufferQuadList();
	void destroyStaticIndexBufferQuadList();

	CD3D11Driver*	Driver;

	LENTRY	VertexBufferList;
	LENTRY	IndexBufferList;
};

#endif
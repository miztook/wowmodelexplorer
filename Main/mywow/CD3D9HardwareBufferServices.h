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
	virtual bool createHardwareBuffer(CVertexBuffer* vbuffer);
	virtual bool createHardwareBuffer(CIndexBuffer* ibuffer);
	virtual void destroyHardwareBuffers(const SBufferParam& bufferParam);
	virtual void destroyHardwareBuffer(CVertexBuffer* vbuffer);
	virtual void destroyHardwareBuffer(CIndexBuffer* ibuffer);
	virtual bool updateHardwareBuffer(CVertexBuffer* vbuffer, u32 size);
	virtual bool updateHardwareBuffer(CIndexBuffer* ibuffer, u32 size);

private:
	bool internalCreateVertexBuffer( CVertexBuffer* vbuffer );
	bool internalCreateIndexBuffer( CIndexBuffer* ibuffer );
	bool fillVertexBuffer(CVertexBuffer* vbuffer);
	bool fillIndexBuffer(CIndexBuffer* ibuffer);

	void createStaticIndexBufferQuadList();
	void destroyStaticIndexBufferQuadList();

	CD3D9Driver*			Driver;

	LENTRY	VertexBufferList;
	LENTRY	IndexBufferList;
};

#endif
#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_OPENGL

#include "IHardwareBufferServices.h"
#include "core.h"

class COpenGLDriver;

class COpenGLHardwareBufferServices : public IHardwareBufferServices
{
private:
	DISALLOW_COPY_AND_ASSIGN(COpenGLHardwareBufferServices);

public:
	COpenGLHardwareBufferServices();
	~COpenGLHardwareBufferServices();

public:
	virtual bool createHardwareBuffers(const SBufferParam& bufferParam);
	virtual bool createHardwareBuffer(CVertexBuffer* vbuffer);
	virtual bool createHardwareBuffer(CIndexBuffer* ibuffer);
	virtual void destroyHardwareBuffers(const SBufferParam& bufferParam);
	virtual void destroyHardwareBuffer(CVertexBuffer* vbuffer);
	virtual void destroyHardwareBuffer(CIndexBuffer* ibuffer);
	virtual bool updateHardwareBuffer(CVertexBuffer* vbuffer, u32 size);
	virtual bool updateHardwareBuffer(CIndexBuffer* ibuffer, u32 size);

public:
	virtual void onLost();
	virtual void onReset();

private:
	bool internalCreateVertexBuffer( CVertexBuffer* vbuffer );
	bool internalCreateIndexBuffer( CIndexBuffer* ibuffer );

	void createStaticIndexBufferQuadList();
	void destroyStaticIndexBufferQuadList();

	COpenGLDriver*		Driver;

	LENTRY	VertexBufferList;
	LENTRY	IndexBufferList;
};

#endif
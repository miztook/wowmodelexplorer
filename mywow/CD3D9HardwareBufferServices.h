#pragma once

#include "IHardwareBufferServices.h"
#include <d3d9.h>
#include <list>

class CD3D9HardwareBufferServices : public IHardwareBufferServices, public ILostResetCallback
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
	virtual bool updateHardwareBuffer(IVertexBuffer* vbuffer, u32 offset, u32 size);
	virtual bool updateHardwareBuffer(IIndexBuffer* ibuffer, u32 offset, u32 size);

public:
	virtual void onLost();
	virtual void onReset();

private:
	bool internalCreateVertexBuffer( IVertexBuffer* vbuffer );
	bool internalCreateIndexBuffer( IIndexBuffer* ibuffer );

	IDirect3DDevice9*		pID3DDevice;

	typedef std::list<IVertexBuffer*, qzone_allocator<IVertexBuffer*>> T_VertexBuffers;
	typedef std::list<IIndexBuffer*, qzone_allocator<IIndexBuffer*>> T_IndexBuffers;

	T_VertexBuffers		VertexBuffers;
	T_IndexBuffers		IndexBuffers;
};
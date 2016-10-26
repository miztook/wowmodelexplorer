#include "stdafx.h"
#include "CD3D9HardwareBufferServices.h"
#include "mywow.h"

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D9

#include "CD3D9Driver.h"
#include "CD3D9Helper.h"

CD3D9HardwareBufferServices::CD3D9HardwareBufferServices()
{
	InitializeListHead(&VertexBufferList);
	InitializeListHead(&IndexBufferList);

	Driver = static_cast<CD3D9Driver*>(g_Engine->getDriver());

	Driver->registerLostReset(this);

	createStaticIndexBufferQuadList();
}

CD3D9HardwareBufferServices::~CD3D9HardwareBufferServices()
{
	destroyStaticIndexBufferQuadList();

	Driver->removeLostReset(this);

	ASSERT(IsListEmpty(&VertexBufferList));
	ASSERT(IsListEmpty(&IndexBufferList));
}

bool CD3D9HardwareBufferServices::createHardwareBuffer( IVertexBuffer* vbuffer )
{
	//CLock lock(&g_Globals.hwbufferCS);

	bool success = internalCreateVertexBuffer(vbuffer);

	if (success)
	{
		InsertTailList(&VertexBufferList, &vbuffer->Link);
	}
	
	return success;
}

bool CD3D9HardwareBufferServices::createHardwareBuffer( IIndexBuffer* ibuffer )
{
	//CLock lock(&g_Globals.hwbufferCS);

	bool success = internalCreateIndexBuffer(ibuffer);

	if (success)
	{
		InsertTailList(&IndexBufferList, &ibuffer->Link);
	}
	
	return success;
}

bool CD3D9HardwareBufferServices::createHardwareBuffers( const SBufferParam& bufferParam )
{
	if (!createHardwareBuffer(bufferParam.vbuffer0))
	{
		ASSERT(false);
		g_Engine->getFileSystem()->writeLog(ELOG_GX, " CD3D9HardwareBufferServices::createHardwareBuffers Failed: vbuffer0");
		return false;
	}
	if (bufferParam.vbuffer1 && !createHardwareBuffer(bufferParam.vbuffer1))
	{
		ASSERT(false);
		g_Engine->getFileSystem()->writeLog(ELOG_GX, " CD3D9HardwareBufferServices::createHardwareBuffers Failed: vbuffer1");
		return false;
	}

	//index buffer
	if (bufferParam.ibuffer && !createHardwareBuffer(bufferParam.ibuffer))
	{
		ASSERT(false);
		g_Engine->getFileSystem()->writeLog(ELOG_GX, " CD3D9HardwareBufferServices::createHardwareBuffers Failed: ibuffer");
		return false;
	}

	return true;
}

void CD3D9HardwareBufferServices::destroyHardwareBuffer( IVertexBuffer* vbuffer )
{
	//CLock lock(&g_Globals.hwbufferCS);

	if(vbuffer->HWLink)
	{
		RemoveEntryList(&vbuffer->Link);
		((IDirect3DVertexBuffer9*)vbuffer->HWLink)->Release();
		vbuffer->HWLink = NULL_PTR;
	}
}

void CD3D9HardwareBufferServices::destroyHardwareBuffer( IIndexBuffer* ibuffer )
{
	//CLock lock(&g_Globals.hwbufferCS);

	if(ibuffer->HWLink)
	{
		RemoveEntryList(&ibuffer->Link);
		((IDirect3DIndexBuffer9*)ibuffer->HWLink)->Release();
		ibuffer->HWLink = NULL_PTR;
	}
}

void CD3D9HardwareBufferServices::destroyHardwareBuffers( const SBufferParam& bufferParam )
{
	if (bufferParam.ibuffer)
		destroyHardwareBuffer(bufferParam.ibuffer);
	if(bufferParam.vbuffer0)
		destroyHardwareBuffer(bufferParam.vbuffer0);
	if (bufferParam.vbuffer1)
		destroyHardwareBuffer(bufferParam.vbuffer1);
}

bool CD3D9HardwareBufferServices::updateHardwareBuffer( IVertexBuffer* vbuffer, u32 size )
{
	if (vbuffer->Size >= 65536 || size > vbuffer->Size || vbuffer->Mapping == EMM_STATIC || !size)
	{
		ASSERT(false);
		return false;
	}

	u32 vertexSize = getStreamPitchFromType(vbuffer->Type);
	u32 sizeToLock = size * vertexSize;
	IDirect3DVertexBuffer9* vertexBuffer = (IDirect3DVertexBuffer9*)vbuffer->HWLink;

	bool dynamic = vbuffer->Mapping == EMM_DYNAMIC;
	void* pLockedBuffer = 0;
	{
		//CLock lock(&g_Globals.hwbufferCS);
		HRESULT hr = vertexBuffer->Lock(0, 0, (void**)&pLockedBuffer,  dynamic ? D3DLOCK_DISCARD : 0);
		if (SUCCEEDED(hr))
		{
			Q_memcpy(pLockedBuffer, sizeToLock, vbuffer->Vertices, sizeToLock);
		}
		else
		{
			g_Engine->getFileSystem()->writeLog(ELOG_GX, " CD3D9HardwareBufferServices::updateHardwareBuffer Failed: vertex buffer");
			ASSERT(false);
		}
		vertexBuffer->Unlock();
	}

	return true;
}

bool CD3D9HardwareBufferServices::updateHardwareBuffer( IIndexBuffer* ibuffer, u32 size )
{
	if (ibuffer->Size >= 65536 || size > ibuffer->Size || ibuffer->Mapping == EMM_STATIC || !size)
	{
		ASSERT(false);
		return false;
	}

	u32 indexSize = ibuffer->Type == EIT_16BIT ? 2 : 4;
	u32 sizeToLock = size * indexSize;
	IDirect3DIndexBuffer9* indexBuffer = (IDirect3DIndexBuffer9*)ibuffer->HWLink;
	bool dynamic = ibuffer->Mapping == EMM_DYNAMIC;

	void* pIndices = 0;
	{
		//CLock lock(&g_Globals.hwbufferCS);
		HRESULT hr = indexBuffer->Lock( 0, 0, (void**)&pIndices, dynamic ? D3DLOCK_DISCARD : 0);
		if (SUCCEEDED(hr))
		{
			Q_memcpy(pIndices, sizeToLock, ibuffer->Indices, sizeToLock);
		}
		else
		{
			g_Engine->getFileSystem()->writeLog(ELOG_GX, " CD3D9HardwareBufferServices::updateHardwareBuffer Failed: index buffer");
			ASSERT(false);
		}
		indexBuffer->Unlock();
	}

	return true;
}

void CD3D9HardwareBufferServices::onLost()
{
	if (g_Engine->getOSInfo()->IsAeroSupport())
		return;

	for (PLENTRY e = VertexBufferList.Flink; e != &VertexBufferList; )
	{
		IVertexBuffer* vbuffer = reinterpret_cast<IVertexBuffer*>CONTAINING_RECORD(e, IVertexBuffer, Link);
		e = e->Flink;

		if (vbuffer->Mapping != EMM_STATIC && vbuffer->HWLink)
		{
			((IDirect3DVertexBuffer9*)vbuffer->HWLink)->Release();
			vbuffer->HWLink = NULL_PTR;
		}
	}

	for (PLENTRY e = IndexBufferList.Flink; e != &IndexBufferList; )
	{
		IIndexBuffer* ibuffer = reinterpret_cast<IIndexBuffer*>CONTAINING_RECORD(e, IIndexBuffer, Link);
		e = e->Flink;

		if (ibuffer->Mapping != EMM_STATIC && ibuffer->HWLink)
		{
			((IDirect3DIndexBuffer9*)ibuffer->HWLink)->Release();
			ibuffer->HWLink = NULL_PTR;
		}
	}
}

void CD3D9HardwareBufferServices::onReset()
{
	if (g_Engine->getOSInfo()->IsAeroSupport())
		return;

	for (PLENTRY e = VertexBufferList.Flink; e != &VertexBufferList; )
	{
		IVertexBuffer* vbuffer = reinterpret_cast<IVertexBuffer*>CONTAINING_RECORD(e, IVertexBuffer, Link);
		e = e->Flink;

		if (vbuffer->Mapping == EMM_STATIC)
			continue;
		internalCreateVertexBuffer(vbuffer);
		updateHardwareBuffer(vbuffer, vbuffer->Size);
	}

	for (PLENTRY e = IndexBufferList.Flink; e != &IndexBufferList; )
	{
		IIndexBuffer* ibuffer = reinterpret_cast<IIndexBuffer*>CONTAINING_RECORD(e, IIndexBuffer, Link);
		e = e->Flink;

		if (ibuffer->Mapping == EMM_STATIC)
			continue;

		internalCreateIndexBuffer(ibuffer);
		updateHardwareBuffer(ibuffer, ibuffer->Size);
	}
}

bool CD3D9HardwareBufferServices::internalCreateIndexBuffer( IIndexBuffer* ibuffer )
{
	ASSERT(NULL_PTR == ibuffer->HWLink);

	DWORD flags = 0;
	D3DPOOL pool = D3DPOOL_DEFAULT;

	switch(ibuffer->Mapping)
	{
	case EMM_STATIC:
		{
			flags = D3DUSAGE_WRITEONLY;
			pool =  g_Engine->getOSInfo()->IsAeroSupport() ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED;;
		}
		break;
	case EMM_DYNAMIC:
		{
			flags = D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC;
			pool = D3DPOOL_DEFAULT;
		}
		break;
	case EMM_SOFTWARE:
		{
			flags = D3DUSAGE_WRITEONLY;
			pool =  D3DPOOL_SYSTEMMEM;
		}
		break;
	default:
		ASSERT(false);
		break;
	}

	ASSERT (ibuffer->Size);
	u32 indexSize = 2;
	D3DFORMAT indexType=D3DFMT_UNKNOWN;
	switch (ibuffer->Type)
	{
	case EIT_16BIT:
		{
			indexType=D3DFMT_INDEX16;
			indexSize = 2;
			break;
		}
	case EIT_32BIT:
		{
			indexType=D3DFMT_INDEX32;
			indexSize = 4;
			break;
		}
	default:
		ASSERT(false);
		break;
	}

	if(FAILED(Driver->pID3DDevice->CreateIndexBuffer( ibuffer->Size * indexSize, flags, indexType, pool, (IDirect3DIndexBuffer9**)&ibuffer->HWLink, NULL_PTR)))
	{
		ASSERT(false);
		return false;
	}	

	//if static, update just this once
	if (ibuffer->Mapping == EMM_STATIC)
	{
		bool ret = fillIndexBuffer(ibuffer);
		ASSERT(ret);
	}

	return true;
}

bool CD3D9HardwareBufferServices::internalCreateVertexBuffer( IVertexBuffer* vbuffer )
{
	ASSERT(NULL_PTR == vbuffer->HWLink);

	DWORD flags = 0;
	D3DPOOL pool = D3DPOOL_DEFAULT;

	switch(vbuffer->Mapping)
	{
	case EMM_STATIC:
		{
			flags = D3DUSAGE_WRITEONLY;
			pool =  g_Engine->getOSInfo()->IsAeroSupport() ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED;
		}
		break;
	case EMM_DYNAMIC:
		{
			flags = D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC;
			pool = D3DPOOL_DEFAULT;
		}
		break;
	case EMM_SOFTWARE:
		{
			flags = D3DUSAGE_WRITEONLY;
			pool =  D3DPOOL_SYSTEMMEM;
		}
		break;
	default:
		ASSERT(false);
		break;
	}

	ASSERT(vbuffer->Size);

	DWORD FVF = 0;
	u32 vertexSize = getStreamPitchFromType(vbuffer->Type);
	if( FAILED(Driver->pID3DDevice->CreateVertexBuffer( vbuffer->Size * vertexSize, flags, FVF, pool, (IDirect3DVertexBuffer9**)&vbuffer->HWLink, NULL_PTR) ) )
	{
		ASSERT(false);
		return false;
	}

	//if static, update just this once
	if (vbuffer->Mapping == EMM_STATIC)
	{
		bool ret = fillVertexBuffer(vbuffer);
		ASSERT(ret);
	}

	return true;
}

bool CD3D9HardwareBufferServices::fillVertexBuffer( IVertexBuffer* vbuffer )
{
	u32 vertexSize = getStreamPitchFromType(vbuffer->Type);
	u32 sizeToLock = vbuffer->Size * vertexSize;

	IDirect3DVertexBuffer9* vertexBuffer = (IDirect3DVertexBuffer9*)vbuffer->HWLink;
	void* pLockedBuffer = 0;

	HRESULT hr = vertexBuffer->Lock(0, 0, (void**)&pLockedBuffer, 0);
	if (SUCCEEDED(hr))
	{
		Q_memcpy(pLockedBuffer, sizeToLock, vbuffer->Vertices, sizeToLock);
	}
	vertexBuffer->Unlock();

	return SUCCEEDED(hr);
}

bool CD3D9HardwareBufferServices::fillIndexBuffer( IIndexBuffer* ibuffer )
{
	u32 indexSize = ibuffer->Type == EIT_16BIT ? 2 : 4;
	u32 sizeToLock = ibuffer->Size * indexSize;

	IDirect3DIndexBuffer9* indexBuffer = (IDirect3DIndexBuffer9*)ibuffer->HWLink;
	void* pIndices = 0;

	HRESULT hr = indexBuffer->Lock( 0, 0, (void**)&pIndices, 0);
	if (SUCCEEDED(hr))
	{
		Q_memcpy(pIndices, sizeToLock, ibuffer->Indices, sizeToLock);
	}
	indexBuffer->Unlock();

	return SUCCEEDED(hr);
}

void CD3D9HardwareBufferServices::createStaticIndexBufferQuadList()
{
	//index buffer
	u16* indices = (u16*)Z_AllocateTempMemory(sizeof(u16) * MAX_QUADS() * 6);
	u32 firstVert = 0;
	u32 firstIndex = 0;
	for (u32 i = 0; i < MAX_QUADS(); ++i )
	{
		indices[firstIndex + 0] = (u16)firstVert + 0;
		indices[firstIndex + 1] = (u16)firstVert + 1;
		indices[firstIndex + 2] = (u16)firstVert + 2;
		indices[firstIndex + 3] = (u16)firstVert + 3;
		indices[firstIndex + 4] = (u16)firstVert + 2;
		indices[firstIndex + 5] = (u16)firstVert + 1;

		firstVert += 4; 
		firstIndex += 6;
	}

	StaticIndexBufferQuadList = new IIndexBuffer(false);
	StaticIndexBufferQuadList->set(indices, EIT_16BIT, MAX_QUADS() * 6, EMM_STATIC);

	createHardwareBuffer(StaticIndexBufferQuadList);

	Z_FreeTempMemory(indices);
}

void CD3D9HardwareBufferServices::destroyStaticIndexBufferQuadList()
{
	destroyHardwareBuffer(StaticIndexBufferQuadList);

	delete StaticIndexBufferQuadList;
}

#endif
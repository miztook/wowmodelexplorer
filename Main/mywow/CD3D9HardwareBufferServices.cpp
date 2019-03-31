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

	createStaticIndexBufferQuadList();
}

CD3D9HardwareBufferServices::~CD3D9HardwareBufferServices()
{
	destroyStaticIndexBufferQuadList();

	ASSERT(IsListEmpty(&VertexBufferList));
	ASSERT(IsListEmpty(&IndexBufferList));
}

bool CD3D9HardwareBufferServices::createHardwareBuffer( CVertexBuffer* vbuffer )
{
	//CLock lock(&g_Globals.hwbufferCS);

	bool success = internalCreateVertexBuffer(vbuffer);

	if (success)
	{
		InsertTailList(&VertexBufferList, &vbuffer->Link);
	}
	
	return success;
}

bool CD3D9HardwareBufferServices::createHardwareBuffer( CIndexBuffer* ibuffer )
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

void CD3D9HardwareBufferServices::destroyHardwareBuffer( CVertexBuffer* vbuffer )
{
	//CLock lock(&g_Globals.hwbufferCS);

	if(vbuffer->HWLink)
	{
		RemoveEntryList(&vbuffer->Link);
		((IDirect3DVertexBuffer9*)vbuffer->HWLink)->Release();
		vbuffer->HWLink = nullptr;
	}
}

void CD3D9HardwareBufferServices::destroyHardwareBuffer( CIndexBuffer* ibuffer )
{
	//CLock lock(&g_Globals.hwbufferCS);

	if(ibuffer->HWLink)
	{
		RemoveEntryList(&ibuffer->Link);
		((IDirect3DIndexBuffer9*)ibuffer->HWLink)->Release();
		ibuffer->HWLink = nullptr;
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

bool CD3D9HardwareBufferServices::updateHardwareBuffer( CVertexBuffer* vbuffer, uint32_t size )
{
	if (vbuffer->Size >= 65536 || size > vbuffer->Size || vbuffer->Mapping == EMM_STATIC || !size)
	{
		ASSERT(false);
		return false;
	}

	uint32_t vertexSize = getStreamPitchFromType(vbuffer->Type);
	uint32_t sizeToLock = size * vertexSize;
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

bool CD3D9HardwareBufferServices::updateHardwareBuffer( CIndexBuffer* ibuffer, uint32_t size )
{
	if (ibuffer->Size >= 65536 || size > ibuffer->Size || ibuffer->Mapping == EMM_STATIC || !size)
	{
		ASSERT(false);
		return false;
	}

	uint32_t indexSize = ibuffer->Type == EIT_16BIT ? 2 : 4;
	uint32_t sizeToLock = size * indexSize;
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

bool CD3D9HardwareBufferServices::internalCreateIndexBuffer( CIndexBuffer* ibuffer )
{
	ASSERT(nullptr == ibuffer->HWLink);

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
	uint32_t indexSize = 2;
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

	if(FAILED(Driver->pID3DDevice->CreateIndexBuffer( ibuffer->Size * indexSize, flags, indexType, pool, (IDirect3DIndexBuffer9**)&ibuffer->HWLink, nullptr)))
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

bool CD3D9HardwareBufferServices::internalCreateVertexBuffer( CVertexBuffer* vbuffer )
{
	ASSERT(nullptr == vbuffer->HWLink);

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
	uint32_t vertexSize = getStreamPitchFromType(vbuffer->Type);
	if( FAILED(Driver->pID3DDevice->CreateVertexBuffer( vbuffer->Size * vertexSize, flags, FVF, pool, (IDirect3DVertexBuffer9**)&vbuffer->HWLink, nullptr) ) )
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

bool CD3D9HardwareBufferServices::fillVertexBuffer( CVertexBuffer* vbuffer )
{
	uint32_t vertexSize = getStreamPitchFromType(vbuffer->Type);
	uint32_t sizeToLock = vbuffer->Size * vertexSize;

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

bool CD3D9HardwareBufferServices::fillIndexBuffer( CIndexBuffer* ibuffer )
{
	uint32_t indexSize = ibuffer->Type == EIT_16BIT ? 2 : 4;
	uint32_t sizeToLock = ibuffer->Size * indexSize;

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
	uint16_t* indices = (uint16_t*)Z_AllocateTempMemory(sizeof(uint16_t) * MAX_QUADS() * 6);
	uint32_t firstVert = 0;
	uint32_t firstIndex = 0;
	for (uint32_t i = 0; i < MAX_QUADS(); ++i )
	{
		indices[firstIndex + 0] = (uint16_t)firstVert + 0;
		indices[firstIndex + 1] = (uint16_t)firstVert + 1;
		indices[firstIndex + 2] = (uint16_t)firstVert + 2;
		indices[firstIndex + 3] = (uint16_t)firstVert + 3;
		indices[firstIndex + 4] = (uint16_t)firstVert + 2;
		indices[firstIndex + 5] = (uint16_t)firstVert + 1;

		firstVert += 4; 
		firstIndex += 6;
	}

	StaticIndexBufferQuadList = new CIndexBuffer(false);
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
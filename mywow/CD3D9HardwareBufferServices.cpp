#include "stdafx.h"
#include "CD3D9HardwareBufferServices.h"
#include "mywow.h"
#include "CD3D9Helper.h"

CD3D9HardwareBufferServices::CD3D9HardwareBufferServices()
{
	pID3DDevice = (IDirect3DDevice9*)g_Engine->getDriver()->getD3DDevice();

	g_Engine->getDriver()->registerLostReset(this);
}

CD3D9HardwareBufferServices::~CD3D9HardwareBufferServices()
{
	_ASSERT(VertexBuffers.empty());
	_ASSERT(IndexBuffers.empty());
}

bool CD3D9HardwareBufferServices::createHardwareBuffer( IVertexBuffer* vbuffer )
{
	_ASSERT( g_Engine->getCurrentThreadId() == ::GetCurrentThreadId() );

	bool success = internalCreateVertexBuffer(vbuffer);

	if (success)
	{
		VertexBuffers.push_back(vbuffer);
	}
	
	return success;
}

bool CD3D9HardwareBufferServices::createHardwareBuffer( IIndexBuffer* ibuffer )
{
	_ASSERT( g_Engine->getCurrentThreadId() == ::GetCurrentThreadId() );

	bool success = internalCreateIndexBuffer(ibuffer);

	if (success)
	{
		IndexBuffers.push_back(ibuffer);
	}
	
	return success;
}

void CD3D9HardwareBufferServices::destroyHardwareBuffer( IVertexBuffer* vbuffer )
{
	_ASSERT( g_Engine->getCurrentThreadId() == ::GetCurrentThreadId() );

	if(!vbuffer->HWLink)
		return;

	for (T_VertexBuffers::iterator itr = VertexBuffers.begin(); itr != VertexBuffers.end();)
	{
		if ((*itr) == vbuffer)
		{
			((IDirect3DVertexBuffer9*)vbuffer->HWLink)->Release();
			vbuffer->HWLink = NULL;
			itr = VertexBuffers.erase(itr);
			break;
		}
		else
			++itr;
	}
}

void CD3D9HardwareBufferServices::destroyHardwareBuffer( IIndexBuffer* ibuffer )
{
	_ASSERT( g_Engine->getCurrentThreadId() == ::GetCurrentThreadId() );

	if(!ibuffer->HWLink)
		return;

	for (T_IndexBuffers::iterator itr = IndexBuffers.begin(); itr != IndexBuffers.end();)
	{
		if ((*itr) == ibuffer)
		{
			((IDirect3DIndexBuffer9*)ibuffer->HWLink)->Release();
			ibuffer->HWLink = NULL;
			itr = IndexBuffers.erase(itr);
			break;
		}
		else
			++itr;
	}		
}

bool CD3D9HardwareBufferServices::updateHardwareBuffer( IVertexBuffer* vbuffer, u32 offset, u32 size )
{
	_ASSERT(offset + size <= vbuffer->Size);

	if (!size)
		return true;

	u32 vertexSize = getVertexPitchFromType(vbuffer->Type);
	void* pLockedBuffer = 0;

	u32 sizeToLock = size * vertexSize;
	u32 offsetSize = offset * vertexSize;

	IDirect3DVertexBuffer9* vertexBuffer = (IDirect3DVertexBuffer9*)vbuffer->HWLink;

	bool dynamic = vbuffer->Mapping == EMM_DYNAMIC;
	HRESULT hr;
	if (offset == 0 && size == vbuffer->Size)
	{
		hr = vertexBuffer->Lock(0, 0, (void**)&pLockedBuffer,  dynamic ? D3DLOCK_DISCARD : 0);
		if (SUCCEEDED(hr))
		{
			memcpy_s(pLockedBuffer, sizeToLock, vbuffer->Vertices, sizeToLock);
		}
	}
	else
	{
		_ASSERT(dynamic);
		DWORD flags = offset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD;
		hr = vertexBuffer->Lock(offsetSize, sizeToLock, (void**)&pLockedBuffer, flags);
		if (SUCCEEDED(hr))
		{
			memcpy_s(pLockedBuffer, sizeToLock, (const void*)((u8*)vbuffer->Vertices + offsetSize), sizeToLock);
		}
	}
	_ASSERT(SUCCEEDED(hr));

	vertexBuffer->Unlock();
	
	return SUCCEEDED(hr);
}

bool CD3D9HardwareBufferServices::updateHardwareBuffer( IIndexBuffer* ibuffer, u32 offset, u32 size )
{
	u32 indexSize = ibuffer->Type == EIT_16BIT ? 2 : 4;
	
	_ASSERT(offset + size <= ibuffer->Size);			

	if (!size)
		return true;

	u32 sizeToLock = size * indexSize;
	u32 offsetSize = offset * indexSize;

	IDirect3DIndexBuffer9* indexBuffer = (IDirect3DIndexBuffer9*)ibuffer->HWLink;
	bool dynamic = ibuffer->Mapping == EMM_DYNAMIC;

	void* pIndices = 0;

	HRESULT hr;
	if (offset == 0 && size == ibuffer->Size)
	{
		hr = indexBuffer->Lock( 0, 0, (void**)&pIndices, dynamic ? D3DLOCK_DISCARD : 0);
		if (SUCCEEDED(hr))
		{
			memcpy_s(pIndices, sizeToLock, ibuffer->Indices, sizeToLock);
		}
	}
	else
	{
		_ASSERT(dynamic);
		DWORD	flags = offset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD;

		hr = indexBuffer->Lock(offsetSize, sizeToLock, (void**)&pIndices, flags);
		if (SUCCEEDED(hr))
		{
			memcpy_s(pIndices, sizeToLock, (const void*)((u8*)ibuffer->Indices + offsetSize), sizeToLock);
		}

	}
	_ASSERT(SUCCEEDED(hr));

	indexBuffer->Unlock();

	return SUCCEEDED(hr);
}

void CD3D9HardwareBufferServices::onLost()
{
	for (T_VertexBuffers::iterator itr = VertexBuffers.begin(); itr != VertexBuffers.end(); ++itr)
	{
		IVertexBuffer* vbuffer = (*itr);
		if (vbuffer->Mapping != EMM_STATIC && vbuffer->HWLink)
		{
			((IDirect3DVertexBuffer9*)vbuffer->HWLink)->Release();
			vbuffer->HWLink = NULL;
		}
	}

	for (T_IndexBuffers::iterator itr = IndexBuffers.begin(); itr != IndexBuffers.end(); ++itr)
	{
		IIndexBuffer* ibuffer = (*itr);
		if (ibuffer->Mapping != EMM_STATIC && ibuffer->HWLink)
		{
			((IDirect3DIndexBuffer9*)ibuffer->HWLink)->Release();
			ibuffer->HWLink = NULL;
		}
	}
}

void CD3D9HardwareBufferServices::onReset()
{
	for (T_VertexBuffers::iterator itr = VertexBuffers.begin(); itr != VertexBuffers.end(); ++itr)
	{
		IVertexBuffer* vbuffer = (*itr);
		if (vbuffer->Mapping == EMM_STATIC)
			continue;
		internalCreateVertexBuffer(vbuffer);
		updateHardwareBuffer(vbuffer, 0, vbuffer->Size);
	}

	for (T_IndexBuffers::iterator itr = IndexBuffers.begin(); itr != IndexBuffers.end(); ++itr)
	{
		IIndexBuffer* ibuffer = (*itr);
		if (ibuffer->Mapping == EMM_STATIC)
			continue;

		internalCreateIndexBuffer(ibuffer);
		updateHardwareBuffer(ibuffer, 0, ibuffer->Size);
	}
}

bool CD3D9HardwareBufferServices::internalCreateIndexBuffer( IIndexBuffer* ibuffer )
{
	_ASSERT(NULL == ibuffer->HWLink);

	DWORD flags = 0;
	D3DPOOL pool;

	switch(ibuffer->Mapping)
	{
	case EMM_STATIC:
		{
			flags = D3DUSAGE_WRITEONLY;
			pool =  g_Engine->IsVista() ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED;
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
		_ASSERT(false);
		break;
	}

	if (ibuffer->Size)
	{
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
			_ASSERT(false);
			break;
		}

		if(FAILED(pID3DDevice->CreateIndexBuffer( ibuffer->Size * indexSize, flags, indexType, pool, (IDirect3DIndexBuffer9**)&ibuffer->HWLink, NULL)))
		{
			_ASSERT(false);
			return false;
		}	
	}

	return true;
}

bool CD3D9HardwareBufferServices::internalCreateVertexBuffer( IVertexBuffer* vbuffer )
{
	_ASSERT(NULL == vbuffer->HWLink);

	DWORD flags = 0;
	D3DPOOL pool;

	switch(vbuffer->Mapping)
	{
	case EMM_STATIC:
		{
			flags = D3DUSAGE_WRITEONLY;
			pool =  g_Engine->IsVista() ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED;
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
		_ASSERT(false);
		break;
	}

	if (vbuffer->Size)
	{
		DWORD FVF = 0;
		u32 vertexSize = getVertexPitchFromType(vbuffer->Type);
		if( FAILED(pID3DDevice->CreateVertexBuffer( vbuffer->Size * vertexSize, flags, FVF, pool, (IDirect3DVertexBuffer9**)&vbuffer->HWLink, NULL) ) )
		{
			_ASSERT(false);
			return false;
		}
	}

	return true;
}
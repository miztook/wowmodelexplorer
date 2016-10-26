#include "stdafx.h"
#include "CD3D11HardwareBufferServices.h"
#include "mywow.h"

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D11

#include "CD3D11Driver.h"

CD3D11HardwareBufferServices::CD3D11HardwareBufferServices()
{
	InitializeListHead(&VertexBufferList);
	InitializeListHead(&IndexBufferList);

	Driver = static_cast<CD3D11Driver*>(g_Engine->getDriver());

	Driver->registerLostReset(this);

	createStaticIndexBufferQuadList();
}

CD3D11HardwareBufferServices::~CD3D11HardwareBufferServices()
{
	destroyStaticIndexBufferQuadList();

	Driver->removeLostReset(this);

	ASSERT(IsListEmpty(&VertexBufferList));
	ASSERT(IsListEmpty(&IndexBufferList));
}

bool CD3D11HardwareBufferServices::createHardwareBuffer( IVertexBuffer* vbuffer )
{
	//CLock lock(&g_Globals.hwbufferCS);

	bool success = internalCreateVertexBuffer(vbuffer);

	if (success)
	{
		InsertTailList(&VertexBufferList, &vbuffer->Link);
	}

	return success;
}

bool CD3D11HardwareBufferServices::createHardwareBuffer( IIndexBuffer* ibuffer )
{
	//CLock lock(&g_Globals.hwbufferCS);

	bool success = internalCreateIndexBuffer(ibuffer);

	if (success)
	{
		InsertTailList(&IndexBufferList, &ibuffer->Link);
	}

	return success;
}

bool CD3D11HardwareBufferServices::createHardwareBuffers( const SBufferParam& bufferParam )
{
	if (!createHardwareBuffer(bufferParam.vbuffer0))
	{
		ASSERT(false);
		g_Engine->getFileSystem()->writeLog(ELOG_GX, " CD3D11HardwareBufferServices::createHardwareBuffers Failed: vbuffer0");
		return false;
	}
	if (bufferParam.vbuffer1 && !createHardwareBuffer(bufferParam.vbuffer1))
	{
		ASSERT(false);
		g_Engine->getFileSystem()->writeLog(ELOG_GX, " CD3D11HardwareBufferServices::createHardwareBuffers Failed: vbuffer1");
		return false;
	}

	//index buffer
	if (bufferParam.ibuffer && !createHardwareBuffer(bufferParam.ibuffer))
	{
		ASSERT(false);
		g_Engine->getFileSystem()->writeLog(ELOG_GX, " CD3D11HardwareBufferServices::createHardwareBuffers Failed: ibuffer");
		return false;
	}

	return true;
}

void CD3D11HardwareBufferServices::destroyHardwareBuffer( IVertexBuffer* vbuffer )
{
	//CLock lock(&g_Globals.hwbufferCS);

	if(vbuffer->HWLink)
	{
		RemoveEntryList(&vbuffer->Link);
		((ID3D11Buffer*)vbuffer->HWLink)->Release();
		vbuffer->HWLink = NULL_PTR;
	}
}

void CD3D11HardwareBufferServices::destroyHardwareBuffer( IIndexBuffer* ibuffer )
{
	//CLock lock(&g_Globals.hwbufferCS);

	if(ibuffer->HWLink)
	{
		RemoveEntryList(&ibuffer->Link);
		((ID3D11Buffer*)ibuffer->HWLink)->Release();
		ibuffer->HWLink = NULL_PTR;
	}
}

void CD3D11HardwareBufferServices::destroyHardwareBuffers( const SBufferParam& bufferParam )
{
	if (bufferParam.ibuffer)
		destroyHardwareBuffer(bufferParam.ibuffer);
	if (bufferParam.vbuffer0)
		destroyHardwareBuffer(bufferParam.vbuffer0);
	if (bufferParam.vbuffer1)
		destroyHardwareBuffer(bufferParam.vbuffer1);
}

bool CD3D11HardwareBufferServices::updateHardwareBuffer( IVertexBuffer* vbuffer, u32 size )
{
	if (vbuffer->Size >= 65536 || size > vbuffer->Size || vbuffer->Mapping == EMM_STATIC || !size)
	{
		ASSERT(false);
		return false;
	}

	u32 vertexSize = getStreamPitchFromType(vbuffer->Type);

	u32 sizeToLock = size * vertexSize;
	ID3D11Buffer* vertexBuffer = (ID3D11Buffer*)vbuffer->HWLink;

	D3D11_MAP maptype = D3D11_MAP_WRITE_DISCARD;
	D3D11_MAPPED_SUBRESOURCE mapOut;

	{
		//CLock lock(&g_Globals.hwbufferCS);

		//ImmediateContext
		HRESULT hr = Driver->ImmediateContext->Map(vertexBuffer, 0, maptype, 0, &mapOut);
		if (FAILED(hr))
		{
			ASSERT(false);
			return false;
		}

		void* destData = (u8*)mapOut.pData;

		const void* srcData = (u8*)vbuffer->Vertices; 
		Q_memcpy(destData, sizeToLock, (const void*)srcData, sizeToLock);

		//ImmediateContext
		Driver->ImmediateContext->Unmap(vertexBuffer, 0);
	}
	
	return true;
}

bool CD3D11HardwareBufferServices::updateHardwareBuffer( IIndexBuffer* ibuffer, u32 size )
{
	if (ibuffer->Size >= 65536 || size > ibuffer->Size || ibuffer->Mapping == EMM_STATIC || !size)
	{
		ASSERT(false);
		return false;
	}

	u32 indexSize = ibuffer->Type == EIT_16BIT ? 2 : 4;

	u32 sizeToLock = size * indexSize;

	ID3D11Buffer* indexBuffer = (ID3D11Buffer*)ibuffer->HWLink;

	D3D11_MAP maptype = D3D11_MAP_WRITE_DISCARD;
	D3D11_MAPPED_SUBRESOURCE mapOut;

	{
		//CLock lock(&g_Globals.hwbufferCS);

		//ImmediateContext	
		HRESULT hr = Driver->ImmediateContext->Map(indexBuffer, 0, maptype, 0, &mapOut);
		if(FAILED(hr))
		{
			ASSERT(false);
			return false;
		}

		void* destData = (u8*)mapOut.pData;
	
		const void* srcData = (u8*)ibuffer->Indices; 
		Q_memcpy(destData, sizeToLock, (const void*)srcData, sizeToLock);

		//ImmediateContext
		Driver->ImmediateContext->Unmap(indexBuffer, 0);
	}

	return true;
}

void CD3D11HardwareBufferServices::onLost()
{

}

void CD3D11HardwareBufferServices::onReset()
{

}

bool CD3D11HardwareBufferServices::internalCreateVertexBuffer( IVertexBuffer* vbuffer )
{
	ASSERT(NULL_PTR == vbuffer->HWLink);

	D3D11_USAGE	usage = D3D11_USAGE_DEFAULT;
	UINT cpuAccess = 0;
	UINT bindFlags = D3D11_BIND_VERTEX_BUFFER;

	switch(vbuffer->Mapping)
	{
	case EMM_STATIC:
		{
			usage = D3D11_USAGE_DEFAULT;
			cpuAccess = 0;
			bindFlags |= D3D11_BIND_STREAM_OUTPUT;
		}
		break;
	case EMM_DYNAMIC:
		{
			usage = D3D11_USAGE_DYNAMIC;
			cpuAccess = D3D11_CPU_ACCESS_WRITE;
		}
		break;
	case EMM_SOFTWARE:
		{
			usage = D3D11_USAGE_STAGING;
			cpuAccess = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
		}
		break;
	default:
		ASSERT(false);
		break;
	}

	D3D11_BUFFER_DESC desc = {0};
	desc.Usage = usage;
	desc.BindFlags = bindFlags;
	desc.ByteWidth = getStreamPitchFromType(vbuffer->Type) * vbuffer->Size;
	desc.MiscFlags = 0;
	desc.CPUAccessFlags = cpuAccess;
	desc.StructureByteStride = 0;

	if (vbuffer->Mapping == EMM_STATIC)
	{
		D3D11_SUBRESOURCE_DATA initData = {0};
		initData.pSysMem = vbuffer->Vertices;
		if(FAILED(Driver->pID3DDevice11->CreateBuffer(&desc, &initData, (ID3D11Buffer**)&vbuffer->HWLink)))
		{
			ASSERT(false);
			return false;
		}
	}
	else
	{
		if(FAILED(Driver->pID3DDevice11->CreateBuffer(&desc, NULL_PTR, (ID3D11Buffer**)&vbuffer->HWLink)))
		{
			ASSERT(false);
			return false;
		}
	}

	return true;
}

bool CD3D11HardwareBufferServices::internalCreateIndexBuffer( IIndexBuffer* ibuffer )
{
	ASSERT(NULL_PTR == ibuffer->HWLink);

	D3D11_USAGE	usage = D3D11_USAGE_DEFAULT;
	UINT cpuAccess = 0;
	UINT bindFlags = D3D11_BIND_INDEX_BUFFER;

	switch(ibuffer->Mapping)
	{
	case EMM_STATIC:
		{
			usage = D3D11_USAGE_DEFAULT;
			cpuAccess = 0;
			bindFlags |= D3D11_BIND_STREAM_OUTPUT;
		}
		break;
	case EMM_DYNAMIC:
		{
			usage = D3D11_USAGE_DYNAMIC;
			cpuAccess = D3D11_CPU_ACCESS_WRITE;
		}
		break;
	case EMM_SOFTWARE:
		{
			usage = D3D11_USAGE_STAGING;
			cpuAccess = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
		}
		break;
	default:
		ASSERT(false);
		break;
	}

	u32 indexSize = ibuffer->Type == EIT_16BIT ? 2 : 4;

	D3D11_BUFFER_DESC desc = {0};
	desc.Usage = usage;
	desc.BindFlags = bindFlags;
	desc.ByteWidth = indexSize * ibuffer->Size;
	desc.MiscFlags = 0;
	desc.CPUAccessFlags = cpuAccess;
	desc.StructureByteStride = 0;

	if (ibuffer->Mapping == EMM_STATIC)
	{
		D3D11_SUBRESOURCE_DATA initData = {0};
		initData.pSysMem = ibuffer->Indices;
		if(FAILED(Driver->pID3DDevice11->CreateBuffer(&desc, &initData, (ID3D11Buffer**)&ibuffer->HWLink)))
		{
			ASSERT(false);
			return false;
		}
	}
	else
	{
		if(FAILED(Driver->pID3DDevice11->CreateBuffer(&desc, NULL_PTR, (ID3D11Buffer**)&ibuffer->HWLink)))
		{
			ASSERT(false);
			return false;
		}
	}

	return true;
}

void CD3D11HardwareBufferServices::createStaticIndexBufferQuadList()
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

void CD3D11HardwareBufferServices::destroyStaticIndexBufferQuadList()
{
	destroyHardwareBuffer(StaticIndexBufferQuadList);

	delete StaticIndexBufferQuadList;
}

#endif
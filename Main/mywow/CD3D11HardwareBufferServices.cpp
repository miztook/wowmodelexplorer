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

	createStaticIndexBufferQuadList();
}

CD3D11HardwareBufferServices::~CD3D11HardwareBufferServices()
{
	destroyStaticIndexBufferQuadList();

	ASSERT(IsListEmpty(&VertexBufferList));
	ASSERT(IsListEmpty(&IndexBufferList));
}

bool CD3D11HardwareBufferServices::createHardwareBuffer( CVertexBuffer* vbuffer )
{
	//CLock lock(&g_Globals.hwbufferCS);

	bool success = internalCreateVertexBuffer(vbuffer);

	if (success)
	{
		InsertTailList(&VertexBufferList, &vbuffer->Link);
	}

	return success;
}

bool CD3D11HardwareBufferServices::createHardwareBuffer( CIndexBuffer* ibuffer )
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

void CD3D11HardwareBufferServices::destroyHardwareBuffer( CVertexBuffer* vbuffer )
{
	//CLock lock(&g_Globals.hwbufferCS);

	if(vbuffer->HWLink)
	{
		RemoveEntryList(&vbuffer->Link);
		((ID3D11Buffer*)vbuffer->HWLink)->Release();
		vbuffer->HWLink = nullptr;
	}
}

void CD3D11HardwareBufferServices::destroyHardwareBuffer( CIndexBuffer* ibuffer )
{
	//CLock lock(&g_Globals.hwbufferCS);

	if(ibuffer->HWLink)
	{
		RemoveEntryList(&ibuffer->Link);
		((ID3D11Buffer*)ibuffer->HWLink)->Release();
		ibuffer->HWLink = nullptr;
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

bool CD3D11HardwareBufferServices::updateHardwareBuffer( CVertexBuffer* vbuffer, uint32_t size )
{
	if (vbuffer->Size >= 65536 || size > vbuffer->Size || vbuffer->Mapping == EMM_STATIC || !size)
	{
		ASSERT(false);
		return false;
	}

	uint32_t vertexSize = getStreamPitchFromType(vbuffer->Type);

	uint32_t sizeToLock = size * vertexSize;
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

		void* destData = (uint8_t*)mapOut.pData;

		const void* srcData = (uint8_t*)vbuffer->Vertices; 
		Q_memcpy(destData, sizeToLock, (const void*)srcData, sizeToLock);

		//ImmediateContext
		Driver->ImmediateContext->Unmap(vertexBuffer, 0);
	}
	
	return true;
}

bool CD3D11HardwareBufferServices::updateHardwareBuffer( CIndexBuffer* ibuffer, uint32_t size )
{
	if (ibuffer->Size >= 65536 || size > ibuffer->Size || ibuffer->Mapping == EMM_STATIC || !size)
	{
		ASSERT(false);
		return false;
	}

	uint32_t indexSize = ibuffer->Type == EIT_16BIT ? 2 : 4;

	uint32_t sizeToLock = size * indexSize;

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

		void* destData = (uint8_t*)mapOut.pData;
	
		const void* srcData = (uint8_t*)ibuffer->Indices; 
		Q_memcpy(destData, sizeToLock, (const void*)srcData, sizeToLock);

		//ImmediateContext
		Driver->ImmediateContext->Unmap(indexBuffer, 0);
	}

	return true;
}

bool CD3D11HardwareBufferServices::internalCreateVertexBuffer( CVertexBuffer* vbuffer )
{
	ASSERT(nullptr == vbuffer->HWLink);

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
		if(FAILED(Driver->pID3DDevice11->CreateBuffer(&desc, nullptr, (ID3D11Buffer**)&vbuffer->HWLink)))
		{
			ASSERT(false);
			return false;
		}
	}

	return true;
}

bool CD3D11HardwareBufferServices::internalCreateIndexBuffer( CIndexBuffer* ibuffer )
{
	ASSERT(nullptr == ibuffer->HWLink);

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

	uint32_t indexSize = ibuffer->Type == EIT_16BIT ? 2 : 4;

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
		if(FAILED(Driver->pID3DDevice11->CreateBuffer(&desc, nullptr, (ID3D11Buffer**)&ibuffer->HWLink)))
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

void CD3D11HardwareBufferServices::destroyStaticIndexBufferQuadList()
{
	destroyHardwareBuffer(StaticIndexBufferQuadList);

	delete StaticIndexBufferQuadList;
}

#endif
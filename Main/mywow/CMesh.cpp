#include "stdafx.h"
#include "CMesh.h"
#include "mywow.h"

CMesh::CMesh(const SBufferParam& bufferParam, E_PRIMITIVE_TYPE primType, u32 primCount, const aabbox3df& box ) 
	: Box(box)
{
	BufferParam = bufferParam;
	PrimType = primType;
	PrimCount = primCount,

	g_Engine->getHardwareBufferServices()->createHardwareBuffers(BufferParam);

	updateVertexBuffer(0);
	updateVertexBuffer(1);
	updateIndexBuffer();
}

CMesh::~CMesh()
{
	g_Engine->getHardwareBufferServices()->destroyHardwareBuffers(BufferParam);

	BufferParam.destroy();
}

void CMesh::updateVertexBuffer(u32 index)
{
	IVertexBuffer* vbuffer = BufferParam.getVBuffer(index);
	if(vbuffer && vbuffer->Mapping != EMM_STATIC)
		g_Engine->getHardwareBufferServices()->updateHardwareBuffer(vbuffer, vbuffer->Size);
}

void CMesh::updateIndexBuffer()
{
	if (BufferParam.ibuffer && BufferParam.ibuffer->Mapping != EMM_STATIC)
		g_Engine->getHardwareBufferServices()->updateHardwareBuffer(BufferParam.ibuffer, BufferParam.ibuffer->Size);
}
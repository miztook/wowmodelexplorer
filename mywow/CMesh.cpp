#include "stdafx.h"
#include "CMesh.h"
#include "mywow.h"

CMesh::CMesh(const SBufferParam& bufferParam, E_PRIMITIVE_TYPE primType, u32 primCount, const aabbox3df& box )
{
	BufferParam = bufferParam;
	PrimType = primType;
	PrimCount = primCount,
	Box = box;

	g_Engine->getHardwareBufferServices()->createHardwareBuffers(BufferParam);

	updateVertexBuffer(0);
	updateVertexBuffer(1);
	updateVertexBuffer(2);
	updateVertexBuffer(3);
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
	if(vbuffer)
		g_Engine->getHardwareBufferServices()->updateHardwareBuffer(vbuffer, 0, vbuffer->Size);
}

void CMesh::updateIndexBuffer()
{
	if (BufferParam.ibuffer)
		g_Engine->getHardwareBufferServices()->updateHardwareBuffer(BufferParam.ibuffer, 0, BufferParam.ibuffer->Size);
}
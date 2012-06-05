#include "stdafx.h"
#include "CMesh.h"
#include "mywow.h"

CMesh::CMesh( IVertexBuffer* vbuffer, IIndexBuffer* ibuffer, E_PRIMITIVE_TYPE primType, u32 primCount, const aabbox3df& box )
{
	VertexBuffer = vbuffer;
	IndexBuffer = ibuffer;
	PrimType = primType;
	PrimCount = primCount,
	Box = box;

	_ASSERT(VertexBuffer);
	g_Engine->getHardwareBufferServices()->createHardwareBuffer(VertexBuffer);

	if (IndexBuffer)
		g_Engine->getHardwareBufferServices()->createHardwareBuffer(IndexBuffer);

	update();
}

CMesh::~CMesh()
{
	if (IndexBuffer)
		g_Engine->getHardwareBufferServices()->destroyHardwareBuffer(IndexBuffer);

	g_Engine->getHardwareBufferServices()->destroyHardwareBuffer(VertexBuffer);

	delete IndexBuffer;
	delete VertexBuffer;
}

void CMesh::update()
{
	g_Engine->getHardwareBufferServices()->updateHardwareBuffer(VertexBuffer, 0, VertexBuffer->Size);

	if (IndexBuffer)
		g_Engine->getHardwareBufferServices()->updateHardwareBuffer(IndexBuffer, 0, IndexBuffer->Size);
}
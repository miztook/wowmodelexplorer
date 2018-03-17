#include "stdafx.h"
#include "VertexIndexBuffer.h"
#include "mywow.h"

bool IVertexBuffer::buildVideoResources()
{
	if (!Vertices || Size == 0)
		return true;

	if (HWLink)
		return true;

	if (!g_Engine->getHardwareBufferServices()->createHardwareBuffer(this))
	{
		ASSERT(false);
		return false;
	}

	return true;
}

void IVertexBuffer::releaseVideoResources()
{
	if (!HWLink)
		return;

	g_Engine->getHardwareBufferServices()->destroyHardwareBuffer(this);
}

bool IVertexBuffer::updateHWBuffer(u32 size)
{
	return g_Engine->getHardwareBufferServices()->updateHardwareBuffer(this, size);
}

bool IIndexBuffer::buildVideoResources()
{
	if (!Indices || Size == 0)
		return true;

	if (HWLink)
		return true;

	if (!g_Engine->getHardwareBufferServices()->createHardwareBuffer(this))
	{
		ASSERT(false);
		return false;
	}

	return true;
}

void IIndexBuffer::releaseVideoResources()
{
	if (!HWLink)
		return;

	g_Engine->getHardwareBufferServices()->destroyHardwareBuffer(this);
}

bool IIndexBuffer::updateHWBuffer(u32 size)
{
	return g_Engine->getHardwareBufferServices()->updateHardwareBuffer(this, size);
}

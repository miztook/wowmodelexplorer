#include "stdafx.h"
#include "VertexIndexBuffer.h"
#include "mywow.h"

bool CVertexBuffer::buildVideoResources()
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

void CVertexBuffer::releaseVideoResources()
{
	if (!HWLink)
		return;

	g_Engine->getHardwareBufferServices()->destroyHardwareBuffer(this);
}

bool CVertexBuffer::updateHWBuffer(uint32_t size)
{
	return g_Engine->getHardwareBufferServices()->updateHardwareBuffer(this, size);
}

bool CIndexBuffer::buildVideoResources()
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

void CIndexBuffer::releaseVideoResources()
{
	if (!HWLink)
		return;

	g_Engine->getHardwareBufferServices()->destroyHardwareBuffer(this);
}

bool CIndexBuffer::updateHWBuffer(uint32_t size)
{
	return g_Engine->getHardwareBufferServices()->updateHardwareBuffer(this, size);
}

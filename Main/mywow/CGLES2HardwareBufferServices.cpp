#include "stdafx.h"
#include "CGLES2HardwareBufferServices.h"
#include "mywow.h"
#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_GLES2

#include "CGLES2Driver.h"
#include "CGLES2Extension.h"
#include "CGLES2Helper.h"
#include "CLock.h"

CGLES2HardwareBufferServices::CGLES2HardwareBufferServices()
{
	InitializeListHead(&VertexBufferList);
	InitializeListHead(&IndexBufferList);

	Driver = static_cast<CGLES2Driver*>(g_Engine->getDriver());

	createStaticIndexBufferQuadList();
}

CGLES2HardwareBufferServices::~CGLES2HardwareBufferServices()
{
	destroyStaticIndexBufferQuadList();

	ASSERT(IsListEmpty(&VertexBufferList));
	ASSERT(IsListEmpty(&IndexBufferList));
}

bool CGLES2HardwareBufferServices::createHardwareBuffers( const SBufferParam& bufferParam )
{
	if (!createHardwareBuffer(bufferParam.vbuffer0))
	{
		ASSERT(false);
		g_Engine->getFileSystem()->writeLog(ELOG_GX, " CGLES2HardwareBufferServices::createHardwareBuffers Failed: vbuffer0");
		return false;
	}
	if (bufferParam.vbuffer1 && !createHardwareBuffer(bufferParam.vbuffer1))
	{
		ASSERT(false);
		g_Engine->getFileSystem()->writeLog(ELOG_GX, " CGLES2HardwareBufferServices::createHardwareBuffers Failed: vbuffer1");
		return false;
	}

	//index buffer
	if (bufferParam.ibuffer && !createHardwareBuffer(bufferParam.ibuffer))
	{
		ASSERT(false);
		g_Engine->getFileSystem()->writeLog(ELOG_GX, " CGLES2HardwareBufferServices::createHardwareBuffers Failed: ibuffer");
		return false;
	}

	return true;
}

bool CGLES2HardwareBufferServices::createHardwareBuffer( IVertexBuffer* vbuffer )
{
	CLock lock(&g_Globals.hwbufferCS);

	bool success = internalCreateVertexBuffer(vbuffer);

	if (success)
	{
		InsertTailList(&VertexBufferList, &vbuffer->Link);
	}

	return success;
}

bool CGLES2HardwareBufferServices::createHardwareBuffer( IIndexBuffer* ibuffer )
{
	CLock lock(&g_Globals.hwbufferCS);

	bool success = internalCreateIndexBuffer(ibuffer);

	if (success)
	{
		InsertTailList(&IndexBufferList, &ibuffer->Link);
	}

	return success;
}

void CGLES2HardwareBufferServices::destroyHardwareBuffers( const SBufferParam& bufferParam )
{
	if (bufferParam.ibuffer)
		destroyHardwareBuffer(bufferParam.ibuffer);
	if(bufferParam.vbuffer0)
		destroyHardwareBuffer(bufferParam.vbuffer0);
	if (bufferParam.vbuffer1)
		destroyHardwareBuffer(bufferParam.vbuffer1);
}

void CGLES2HardwareBufferServices::destroyHardwareBuffer( IVertexBuffer* vbuffer )
{
	CLock lock(&g_Globals.hwbufferCS);

	if(vbuffer->HWLink)
	{
		RemoveEntryList(&vbuffer->Link);

		GLuint buffers[] = { (GLuint)PTR_TO_UINT32(vbuffer->HWLink) };
		Driver->getGLExtension()->extGlDeleteBuffers(1, buffers);
		vbuffer->HWLink = NULL;

		Driver->deleteVao(vbuffer);
	}
}

void CGLES2HardwareBufferServices::destroyHardwareBuffer( IIndexBuffer* ibuffer )
{
	CLock lock(&g_Globals.hwbufferCS);

	if(ibuffer->HWLink)
	{
		RemoveEntryList(&ibuffer->Link);

		GLuint buffers[] = { (GLuint)PTR_TO_UINT32(ibuffer->HWLink) };
		Driver->getGLExtension()->extGlDeleteBuffers(1, buffers);
		ibuffer->HWLink = NULL;
	}
}

bool CGLES2HardwareBufferServices::updateHardwareBuffer( IVertexBuffer* vbuffer, u32 size )
{
	if (vbuffer->Size >= 65536 || size > vbuffer->Size || vbuffer->Mapping == EMM_STATIC || !size)
	{
		ASSERT(false);
		return false;
	}

	GLenum usage = vbuffer->Mapping == EMM_DYNAMIC ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;

	u32 vertexSize = getStreamPitchFromType(vbuffer->Type);
	u32 sizeToLock = size * vertexSize;

	GLuint vertexBuffer = (GLuint)PTR_TO_UINT32(vbuffer->HWLink);

	{
		CLock lock(&g_Globals.hwbufferCS);

		Driver->getGLExtension()->extGlBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

		//glBufferSubData真机上效率低下
		Driver->getGLExtension()->extGlBufferData(GL_ARRAY_BUFFER, sizeToLock, (const GLvoid*)vbuffer->Vertices, usage);

		Driver->getGLExtension()->extGlBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	return true;
}

bool CGLES2HardwareBufferServices::updateHardwareBuffer( IIndexBuffer* ibuffer, u32 size )
{
	if (ibuffer->Size >= 65536 || size > ibuffer->Size || ibuffer->Mapping == EMM_STATIC || !size)
	{
		ASSERT(false);
		return false;
	}

	GLenum usage = ibuffer->Mapping == EMM_DYNAMIC ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;
	u32 indexSize = ibuffer->Type == EIT_16BIT ? 2 : 4;

	u32 sizeToLock = size * indexSize;

	GLuint indexBuffer = (GLuint)PTR_TO_UINT32(ibuffer->HWLink);

	{
		CLock lock(&g_Globals.hwbufferCS);
		Driver->getGLExtension()->extGlBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		
		//glBufferSubData真机上效率低下
		Driver->getGLExtension()->extGlBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeToLock, (const GLvoid*)ibuffer->Indices, usage);

		Driver->getGLExtension()->extGlBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	return true;
}

void CGLES2HardwareBufferServices::onLost()
{

}

void CGLES2HardwareBufferServices::onReset()
{

}

void CGLES2HardwareBufferServices::createStaticIndexBufferQuadList()
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

void CGLES2HardwareBufferServices::destroyStaticIndexBufferQuadList()
{
	destroyHardwareBuffer(StaticIndexBufferQuadList);

	delete StaticIndexBufferQuadList;
}

bool CGLES2HardwareBufferServices::internalCreateVertexBuffer( IVertexBuffer* vbuffer )
{
	ASSERT(NULL == vbuffer->HWLink);

	GLenum usage = GL_DYNAMIC_DRAW;
	switch(vbuffer->Mapping)
	{
	case EMM_STATIC:
		{
			usage = GL_STATIC_DRAW;
		}
		break;
	case EMM_DYNAMIC:
		{
			usage = GL_DYNAMIC_DRAW;
		}
		break;
	case EMM_SOFTWARE:
		{
			usage = GL_STREAM_DRAW;
		}
		break;
	default:
		ASSERT(false);
		break;
	}

	u32 stride = getStreamPitchFromType(vbuffer->Type);
	u32 byteWidth = stride * vbuffer->Size;

	GLuint hwLink;
	Driver->getGLExtension()->extGlGenBuffers(1, &hwLink);

	//create
	Driver->getGLExtension()->extGlBindBuffer(GL_ARRAY_BUFFER, hwLink);
	Driver->getGLExtension()->extGlBufferData(GL_ARRAY_BUFFER, byteWidth, vbuffer->Vertices, usage);
	Driver->getGLExtension()->extGlBindBuffer(GL_ARRAY_BUFFER, 0);

	vbuffer->HWLink = reinterpret_cast<void*>(hwLink);

	return true;
}

bool CGLES2HardwareBufferServices::internalCreateIndexBuffer( IIndexBuffer* ibuffer )
{
	ASSERT(NULL == ibuffer->HWLink);

	GLenum usage = GL_DYNAMIC_DRAW;
	switch(ibuffer->Mapping)
	{
	case EMM_STATIC:
		{
			usage = GL_STATIC_DRAW;
		}
		break;
	case EMM_DYNAMIC:
		{
			usage = GL_DYNAMIC_DRAW;
		}
		break;
	case EMM_SOFTWARE:
		{
			usage = GL_STREAM_DRAW;
		}
		break;
	default:
		ASSERT(false);
		break;
	}

	u32 byteWidth = (ibuffer->Type == EIT_16BIT ? 2 : 4) * ibuffer->Size;

	GLuint hwLink;
	Driver->getGLExtension()->extGlGenBuffers(1, &hwLink);

	//create

	Driver->getGLExtension()->extGlBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hwLink);
	Driver->getGLExtension()->extGlBufferData(GL_ELEMENT_ARRAY_BUFFER, byteWidth, ibuffer->Indices, usage);
	Driver->getGLExtension()->extGlBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	ibuffer->HWLink = reinterpret_cast<void*>(hwLink);

	return true;
}

#endif
#include "stdafx.h"
#include "COpenGLHardwareBufferServices.h"
#include "mywow.h"
#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_OPENGL

#include "gl/glext.h"

#include "COpenGLDriver.h"
#include "COpenGLExtension.h"
#include "COpenGLHelper.h"

COpenGLHardwareBufferServices::COpenGLHardwareBufferServices()
{
	InitializeListHead(&VertexBufferList);
	InitializeListHead(&IndexBufferList);

	Driver = static_cast<COpenGLDriver*>(g_Engine->getDriver());

	createStaticIndexBufferQuadList();
}

COpenGLHardwareBufferServices::~COpenGLHardwareBufferServices()
{
	destroyStaticIndexBufferQuadList();

	ASSERT(IsListEmpty(&VertexBufferList));
	ASSERT(IsListEmpty(&IndexBufferList));
}

bool COpenGLHardwareBufferServices::createHardwareBuffers( const SBufferParam& bufferParam )
{
	if (!createHardwareBuffer(bufferParam.vbuffer0))
	{
		ASSERT(false);
		g_Engine->getFileSystem()->writeLog(ELOG_GX, " COpenGLHardwareBufferServices::createHardwareBuffers Failed: vbuffer0");
		return false;
	}
	if (bufferParam.vbuffer1 && !createHardwareBuffer(bufferParam.vbuffer1))
	{
		ASSERT(false);
		g_Engine->getFileSystem()->writeLog(ELOG_GX, " COpenGLHardwareBufferServices::createHardwareBuffers Failed: vbuffer1");
		return false;
	}

	//index buffer
	if (bufferParam.ibuffer && !createHardwareBuffer(bufferParam.ibuffer))
	{
		ASSERT(false);
		g_Engine->getFileSystem()->writeLog(ELOG_GX, " COpenGLHardwareBufferServices::createHardwareBuffers Failed: ibuffer");
		return false;
	}

	return true;
}

bool COpenGLHardwareBufferServices::createHardwareBuffer( CVertexBuffer* vbuffer )
{
	//CLock lock(&g_Globals.hwbufferCS);

	bool success = internalCreateVertexBuffer(vbuffer);

	if (success)
	{
		InsertTailList(&VertexBufferList, &vbuffer->Link);
	}

	return success;
}

bool COpenGLHardwareBufferServices::createHardwareBuffer( CIndexBuffer* ibuffer )
{
	//CLock lock(&g_Globals.hwbufferCS);

	bool success = internalCreateIndexBuffer(ibuffer);

	if (success)
	{
		InsertTailList(&IndexBufferList, &ibuffer->Link);
	}

	return success;
}

void COpenGLHardwareBufferServices::destroyHardwareBuffers( const SBufferParam& bufferParam )
{
	if (bufferParam.ibuffer)
		destroyHardwareBuffer(bufferParam.ibuffer);
	if(bufferParam.vbuffer0)
		destroyHardwareBuffer(bufferParam.vbuffer0);
	if (bufferParam.vbuffer1)
		destroyHardwareBuffer(bufferParam.vbuffer1);
}

void COpenGLHardwareBufferServices::destroyHardwareBuffer( CVertexBuffer* vbuffer )
{
	//CLock lock(&g_Globals.hwbufferCS);

	if(vbuffer->HWLink)
	{
		RemoveEntryList(&vbuffer->Link);

		GLuint buffers[] = { (GLuint)PTR_TO_UINT32(vbuffer->HWLink) };
		Driver->getGLExtension()->extGlDeleteBuffers(1, buffers);
		vbuffer->HWLink = nullptr;

		Driver->deleteVao(vbuffer);
	}
}

void COpenGLHardwareBufferServices::destroyHardwareBuffer( CIndexBuffer* ibuffer )
{
	//CLock lock(&g_Globals.hwbufferCS);

	if(ibuffer->HWLink)
	{
		RemoveEntryList(&ibuffer->Link);
		GLuint buffers[] = { (GLuint)PTR_TO_UINT32(ibuffer->HWLink) };
		Driver->getGLExtension()->extGlDeleteBuffers(1, buffers);
		ibuffer->HWLink = nullptr;
	}
}

bool COpenGLHardwareBufferServices::updateHardwareBuffer( CVertexBuffer* vbuffer, uint32_t size )
{
	if (vbuffer->Size >= 65536 || size > vbuffer->Size || vbuffer->Mapping == EMM_STATIC || !size)
	{
		ASSERT(false);
		return false;
	}

	GLenum usage = vbuffer->Mapping == EMM_DYNAMIC ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;

	uint32_t vertexSize = getStreamPitchFromType(vbuffer->Type);
	uint32_t sizeToLock = size * vertexSize;

	GLuint vertexBuffer = (GLuint)PTR_TO_UINT32(vbuffer->HWLink);

	{
		//CLock lock(&g_Globals.hwbufferCS);

		Driver->getGLExtension()->extGlBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

		//glBufferSubData真机上效率低下
		Driver->getGLExtension()->extGlBufferData(GL_ARRAY_BUFFER, sizeToLock, (const GLvoid*)vbuffer->Vertices, usage);

		Driver->getGLExtension()->extGlBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	
	return true;
}

bool COpenGLHardwareBufferServices::updateHardwareBuffer( CIndexBuffer* ibuffer, uint32_t size )
{
	if (ibuffer->Size >= 65536 || size > ibuffer->Size || ibuffer->Mapping == EMM_STATIC || !size)
	{
		ASSERT(false);
		return false;
	}

	GLenum usage = ibuffer->Mapping == EMM_DYNAMIC ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;
	uint32_t indexSize = ibuffer->Type == EIT_16BIT ? 2 : 4;

	uint32_t sizeToLock = size * indexSize;

	GLuint indexBuffer = (GLuint)PTR_TO_UINT32(ibuffer->HWLink);

	{
		//CLock lock(&g_Globals.hwbufferCS);

		Driver->getGLExtension()->extGlBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

		//glBufferSubData真机上效率低下
		Driver->getGLExtension()->extGlBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeToLock, (const GLvoid*)ibuffer->Indices, usage);

		Driver->getGLExtension()->extGlBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	
	return true;
}

void COpenGLHardwareBufferServices::createStaticIndexBufferQuadList()
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

void COpenGLHardwareBufferServices::destroyStaticIndexBufferQuadList()
{
	destroyHardwareBuffer(StaticIndexBufferQuadList);

	delete StaticIndexBufferQuadList;
}

bool COpenGLHardwareBufferServices::internalCreateVertexBuffer( CVertexBuffer* vbuffer )
{
	ASSERT(nullptr == vbuffer->HWLink);

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
			usage = GL_DYNAMIC_READ;
		}
		break;
	default:
		ASSERT(false);
		break;
	}

	uint32_t stride = getStreamPitchFromType(vbuffer->Type);
	uint32_t byteWidth = stride * vbuffer->Size;

	GLuint hwLink;
	Driver->getGLExtension()->extGlGenBuffers(1, &hwLink);

	//create
	Driver->getGLExtension()->extGlBindBuffer(GL_ARRAY_BUFFER, hwLink);
	Driver->getGLExtension()->extGlBufferData(GL_ARRAY_BUFFER, byteWidth, vbuffer->Vertices, usage);
	Driver->getGLExtension()->extGlBindBuffer(GL_ARRAY_BUFFER, 0);

	vbuffer->HWLink = reinterpret_cast<void*>(hwLink);

	return true;
}

bool COpenGLHardwareBufferServices::internalCreateIndexBuffer( CIndexBuffer* ibuffer )
{
	ASSERT(nullptr == ibuffer->HWLink);

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
			usage = GL_DYNAMIC_READ;
		}
		break;
	default:
		ASSERT(false);
		break;
	}

	uint32_t byteWidth = (ibuffer->Type == EIT_16BIT ? 2 : 4) * ibuffer->Size;

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


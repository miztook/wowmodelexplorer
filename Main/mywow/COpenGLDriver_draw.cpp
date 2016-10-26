#include "stdafx.h"
#include "COpenGLDriver.h"
#include "mywow.h"

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_OPENGL

#include "COpenGLHelper.h"
#include "COpenGLShader.h"
#include "COpenGLShaderServices.h"
#include "COpenGLMaterialRenderer.h"
#include "COpenGLMaterialRenderServices.h"
#include "COpenGLExtension.h"

void COpenGLDriver::setRenderState3DMode( E_VERTEX_TYPE vType )
{
	CurrentRenderMode = ERM_3D;

	//default vertex shader
#ifndef FIXPIPELINE
	if (!Material.VertexShader)
		Material.VertexShader = OpenGLShaderServices->getDefaultVertexShader(vType);
#endif

	if (ResetRenderStates || LastMaterial != Material )
	{
		OpenGLMaterialRenderServices->setBasicRenderStates( Material, LastMaterial, ResetRenderStates );

		LastMaterial = Material;

		MaterialRenderer = OpenGLMaterialRenderServices->getMaterialRenderer((E_MATERIAL_TYPE)Material.MaterialType);

		// set new material.
		MaterialRenderer->OnSetMaterial( vType, Material, ResetRenderStates);	
	}

	OpenGLMaterialRenderServices->setOverrideRenderStates( OverrideMaterial, ResetRenderStates );

	ResetRenderStates = false;
}

void COpenGLDriver::setRenderState2DMode( E_VERTEX_TYPE vType, const S2DBlendParam& blendParam )
{
	if ( CurrentRenderMode != ERM_2D )
	{
		matrix4 matView(true);
		matView.setTranslation(vector3df(-0.5f,-0.5f,0));

		matrix4 matProject(true);
		const dimension2du& renderTargetSize = ScreenSize;
		matProject.buildProjectionMatrixOrthoLH(f32(renderTargetSize.Width), f32(-(s32)(renderTargetSize.Height)), -1.0, 1.0);
		matProject.setTranslation(vector3df(-1,1,0));

		Matrices[ETS_WORLD] = matrix4::Identity();
		Matrices[ETS_VIEW] = matView;
		Matrices[ETS_PROJECTION] = matProject;

#ifdef FIXPIPELINE
		glMatrixMode(GL_MODELVIEW);
		ASSERT_OPENGL_SUCCESS();

		glLoadMatrixf((Matrices[ETS_VIEW] * Matrices[ETS_WORLD]).pointer());
		ASSERT_OPENGL_SUCCESS();

		GLfloat glmat[16];
		COpenGLHelper::getGLMatrixFromD3D(glmat, matProjection);

		glMatrixMode(GL_PROJECTION);
		ASSERT_OPENGL_SUCCESS();

		glLoadMatrixf(glmat);
		ASSERT_OPENGL_SUCCESS();
#endif

		WV = Matrices[ETS_WORLD] * Matrices[ETS_VIEW];
		WVP = WV * Matrices[ETS_PROJECTION];

		CurrentRenderMode = ERM_2D;
	}

	//default vertex shader
#ifndef FIXPIPELINE
	InitMaterial2D.VertexShader = OpenGLShaderServices->getDefaultVertexShader(vType);
#endif

	if (ResetRenderStates || LastMaterial != InitMaterial2D )
	{
		OpenGLMaterialRenderServices->setBasicRenderStates(InitMaterial2D, LastMaterial, ResetRenderStates);

		LastMaterial = InitMaterial2D;
	}

	OpenGLMaterialRenderServices->set2DRenderStates(blendParam, ResetRenderStates);

	OpenGLMaterialRenderServices->setOverrideRenderStates(InitOverrideMaterial2D, ResetRenderStates);

	ResetRenderStates = false;
}

void COpenGLDriver::draw3DMode( const SBufferParam& bufferParam, E_PRIMITIVE_TYPE primType, u32 primCount, const SDrawParam& drawParam )
{
	setRenderState3DMode(bufferParam.vType);

	//draw
	u32 cPasses = MaterialRenderer->getNumPasses();

	for ( u32 iPass = 0; iPass < cPasses; ++iPass )
	{	
		MaterialRenderer->OnRender(Material, iPass);

		OpenGLShaderServices->applyShaders();
		
		OpenGLShaderServices->setShaderConstants(Material.VertexShader, Material, iPass);
		OpenGLShaderServices->setShaderConstants(OpenGLMaterialRenderServices->getCurrentPixelShader(), Material, iPass);

		OpenGLMaterialRenderServices->applyMaterialChanges();

		drawIndexedPrimitive(bufferParam, primType, primCount, drawParam);

	}
}

void COpenGLDriver::draw2DMode( const SBufferParam& bufferParam, E_PRIMITIVE_TYPE primType, u32 primCount, const SDrawParam& drawParam, const S2DBlendParam& blendParam, bool zTest )
{
	SMaterial& material = zTest ? InitMaterial2DZTest : InitMaterial2D;

	setRenderState2DMode(bufferParam.vType, blendParam);
	
	OpenGLShaderServices->applyShaders();
	OpenGLShaderServices->setShaderConstants(material.VertexShader, material, 0);
	OpenGLShaderServices->setShaderConstants(OpenGLMaterialRenderServices->getCurrentPixelShader(), material, 0);

	OpenGLMaterialRenderServices->applyMaterialChanges();

	drawIndexedPrimitive(bufferParam, primType, primCount, drawParam);
}

void COpenGLDriver::drawDebugInfo( const c8* strMsg )
{
	vector2di pos = vector2di(5,5);

	f32 fps = g_Engine->getSceneManager()->getFPS();

	Q_sprintf(DebugMsg, 512, "Dev: %s\nGraphics: %s\nRes: %d X %d\nFPS: %0.1f\nTriangles: %d\nDraw Call: %d\n", 
		AdapterInfo.description, 
		AdapterInfo.name,
		Viewport.getWidth(), Viewport.getHeight(), 
		fps, 
		(s32)PrimitivesDrawn, 
		(s32)DrawCall);

	Q_strcat(DebugMsg, 512, strMsg);

	g_Engine->getDefaultFont()->drawA(DebugMsg, SColor(0, 255, 0), pos);
}

#ifdef USE_WITH_GLES2

void COpenGLDriver::drawIndexedPrimitive( const SBufferParam& bufferParam, E_PRIMITIVE_TYPE primType, u32 primCount, const SDrawParam& drawParam )
{
	if (!drawParam.numVertices || drawParam.numVertices >= 65536 || !primCount || drawParam.baseVertIndex)
	{	
		IFileSystem* fs = g_Engine->getFileSystem();
		fs->writeLog(ELOG_GX, "COpenGLDriver::drawIndexedPrimitive Failed: param invalid!");
		ASSERT(false);
		return;
	}

	GLenum mode = COpenGLHelper::getGLTopology(primType);
	GLenum type = 0;
	u32 indexSize = 0;
	if (bufferParam.ibuffer)
	{
		switch (bufferParam.ibuffer->Type)
		{
		case EIT_16BIT:
			{
				type = GL_UNSIGNED_SHORT;
				indexSize = 2;
				break;
			}
		case EIT_32BIT:
			{
				type =GL_UNSIGNED_INT;
				indexSize = 4;
				break;
			}
		}

		setVertexDeclarationAndBuffers(bufferParam.vType, OpenGLShaderServices->getCurrentGlProgram(),
			bufferParam.vbuffer0, drawParam.voffset0, bufferParam.vbuffer1, drawParam.voffset1, 
			bufferParam.ibuffer);

		GLExtension->extGlDrawElements(mode, 
			getIndexCount(primType, primCount), 
			type, 
			COpenGLHelper::buffer_offset(indexSize * drawParam.startIndex));
	}
	else
	{
		setVertexDeclarationAndBuffers(bufferParam.vType, OpenGLShaderServices->getCurrentGlProgram(),
			bufferParam.vbuffer0, drawParam.voffset0, bufferParam.vbuffer1, drawParam.voffset1, 
			bufferParam.ibuffer);

		glDrawArrays(mode, drawParam.startIndex, getIndexCount(primType, primCount));
		ASSERT_OPENGL_SUCCESS();
	}

	if (primType == EPT_TRIANGLES || primType == EPT_TRIANGLE_STRIP)
	{
		PrimitivesDrawn += primCount;
	}

	++DrawCall;
}

#else

void COpenGLDriver::drawIndexedPrimitive( const SBufferParam& bufferParam, E_PRIMITIVE_TYPE primType, u32 primCount, const SDrawParam& drawParam )
{
	if (!drawParam.numVertices || drawParam.numVertices >= 65536 || !primCount)
	{
		IFileSystem* fs = g_Engine->getFileSystem();
		fs->writeLog(ELOG_GX, "COpenGLDriver::drawIndexedPrimitive Failed: param invalid!");
		ASSERT(false);
		return;
	}

	GLenum mode = COpenGLHelper::getGLTopology(primType);
	GLenum type = 0;
	u32 indexSize = 0;
	if (bufferParam.ibuffer)
	{
		switch (bufferParam.ibuffer->Type)
		{
		case EIT_16BIT:
			{
				type = GL_UNSIGNED_SHORT;
				indexSize = 2;
				break;
			}
		case EIT_32BIT:
			{
				type =GL_UNSIGNED_INT;
				indexSize = 4;
				break;
			}
		}

		setVertexDeclarationAndBuffers(bufferParam.vType, OpenGLShaderServices->getCurrentGlProgram(),
			bufferParam.vbuffer0, drawParam.voffset0, bufferParam.vbuffer1, drawParam.voffset1, 
			bufferParam.ibuffer);

		GLExtension->extGlDrawRangeElementsBaseVertex(mode, 
			drawParam.minVertIndex, 
			drawParam.minVertIndex + drawParam.numVertices - 1, 
			getIndexCount(primType, primCount), 
			type, 
			COpenGLHelper::buffer_offset(indexSize * drawParam.startIndex), 
			drawParam.baseVertIndex);
	}
	else
	{
		setVertexDeclarationAndBuffers(bufferParam.vType, OpenGLShaderServices->getCurrentGlProgram(),
			bufferParam.vbuffer0, drawParam.voffset0, bufferParam.vbuffer1, drawParam.voffset1, 
			bufferParam.ibuffer);

		glDrawArrays(mode, drawParam.startIndex, getIndexCount(primType, primCount));
		ASSERT_OPENGL_SUCCESS();
	}

	if (primType == EPT_TRIANGLES || primType == EPT_TRIANGLE_STRIP)
	{
		PrimitivesDrawn += primCount;
	}

	++DrawCall;
}

#endif


#endif
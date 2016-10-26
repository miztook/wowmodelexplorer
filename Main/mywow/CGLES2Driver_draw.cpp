#include "stdafx.h"
#include "CGLES2Driver.h"
#include "mywow.h"

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_GLES2

#include "CGLES2Helper.h"
#include "CGLES2Shader.h"
#include "CGLES2ShaderServices.h"
#include "CGLES2MaterialRenderer.h"
#include "CGLES2MaterialRenderServices.h"
#include "CGLES2Extension.h"

void CGLES2Driver::setRenderState3DMode( E_VERTEX_TYPE vType )
{
	CurrentRenderMode = ERM_3D;

	//default vertex shader
	if (!Material.VertexShader)
		Material.VertexShader = GLES2ShaderServices->getDefaultVertexShader(vType);

	if (ResetRenderStates || LastMaterial != Material )
	{
		GLES2MaterialRenderServices->setBasicRenderStates( Material, LastMaterial, ResetRenderStates );

		LastMaterial = Material;

		MaterialRenderer = GLES2MaterialRenderServices->getMaterialRenderer((E_MATERIAL_TYPE)Material.MaterialType);

		// set new material.
		MaterialRenderer->OnSetMaterial( vType, Material, ResetRenderStates);	
	}

	GLES2MaterialRenderServices->setOverrideRenderStates( OverrideMaterial, ResetRenderStates );

	ResetRenderStates = false;
}

void CGLES2Driver::setRenderState2DMode( E_VERTEX_TYPE vType, const S2DBlendParam& blendParam )
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

		WV = Matrices[ETS_WORLD] * Matrices[ETS_VIEW];
		WVP = WV * Matrices[ETS_PROJECTION];

		CurrentRenderMode = ERM_2D;
	}

	//default vertex shader
	InitMaterial2D.VertexShader = GLES2ShaderServices->getDefaultVertexShader(vType);

	if (ResetRenderStates || LastMaterial != InitMaterial2D )
	{
		GLES2MaterialRenderServices->setBasicRenderStates(InitMaterial2D, LastMaterial, ResetRenderStates);

		LastMaterial = InitMaterial2D;
	}

	GLES2MaterialRenderServices->set2DRenderStates(blendParam, ResetRenderStates);

	GLES2MaterialRenderServices->setOverrideRenderStates(InitOverrideMaterial2D, ResetRenderStates);

	ResetRenderStates = false;
}

void CGLES2Driver::draw3DMode( const SBufferParam& bufferParam, E_PRIMITIVE_TYPE primType, u32 primCount, const SDrawParam& drawParam )
{
	setRenderState3DMode(bufferParam.vType);

	//draw
	u32 cPasses = MaterialRenderer->getNumPasses();

	for ( u32 iPass = 0; iPass < cPasses; ++iPass )
	{	
		MaterialRenderer->OnRender(Material, iPass);

		GLES2ShaderServices->applyShaders();

		GLES2ShaderServices->setShaderConstants(Material.VertexShader, Material, iPass);
		GLES2ShaderServices->setShaderConstants(GLES2MaterialRenderServices->getCurrentPixelShader(), Material, iPass);

		GLES2MaterialRenderServices->applyMaterialChanges();

		drawIndexedPrimitive(bufferParam, primType, primCount, drawParam);
	}
	
}

void CGLES2Driver::draw2DMode( const SBufferParam& bufferParam, E_PRIMITIVE_TYPE primType, u32 primCount, const SDrawParam& drawParam, const S2DBlendParam& blendParam, bool zTest )
{
	SMaterial& material = zTest ? InitMaterial2DZTest : InitMaterial2D;

	setRenderState2DMode(bufferParam.vType, blendParam);

	GLES2ShaderServices->applyShaders();
	GLES2ShaderServices->setShaderConstants(material.VertexShader, material, 0);
	GLES2ShaderServices->setShaderConstants(GLES2MaterialRenderServices->getCurrentPixelShader(), material, 0);

	GLES2MaterialRenderServices->applyMaterialChanges();

	drawIndexedPrimitive(bufferParam, primType, primCount, drawParam);
}

void CGLES2Driver::drawDebugInfo( const c8* strMsg )
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

	g_Engine->getFont(EFT_DEFAULT)->drawA(DebugMsg, SColor(0,255, 0), pos);
}

void CGLES2Driver::drawIndexedPrimitive( const SBufferParam& bufferParam, E_PRIMITIVE_TYPE primType, u32 primCount, const SDrawParam& drawParam )
{
	if (!drawParam.numVertices || drawParam.numVertices >= 65536 || !primCount || drawParam.baseVertIndex)
	{	
		IFileSystem* fs = g_Engine->getFileSystem();
		fs->writeLog(ELOG_GX, "CGLES2Driver::drawIndexedPrimitive Failed: param invalid!");
		ASSERT(false);
		return;
	}

	GLenum mode = CGLES2Helper::getGLTopology(primType);
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
        default:
            ASSERT(false);
            break;
		}

		setVertexDeclarationAndBuffers(bufferParam.vType, GLES2ShaderServices->getCurrentGlProgram(),
			bufferParam.vbuffer0, drawParam.voffset0, bufferParam.vbuffer1, drawParam.voffset1, 
			bufferParam.ibuffer);

		GLExtension->extGlDrawElements(mode, 
			getIndexCount(primType, primCount), 
			type, 
			CGLES2Helper::buffer_offset(indexSize * drawParam.startIndex));
	}
	else
	{
		setVertexDeclarationAndBuffers(bufferParam.vType, GLES2ShaderServices->getCurrentGlProgram(),
			bufferParam.vbuffer0, drawParam.voffset0, bufferParam.vbuffer1, drawParam.voffset1, 
			bufferParam.ibuffer);

		glDrawArrays(mode, drawParam.startIndex, getIndexCount(primType, primCount));
		ASSERT_GLES2_SUCCESS();
	}

	if (primType == EPT_TRIANGLES || primType == EPT_TRIANGLE_STRIP)
	{
		PrimitivesDrawn += primCount;
	}

	++DrawCall;
}



#endif

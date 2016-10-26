#include "stdafx.h"
#include "CD3D9Driver.h"
#include "mywow.h"

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D9

#include "CD3D9Helper.h"
#include "CD3D9Shader.h"
#include "CD3D9ShaderServices.h"
#include "CD3D9MaterialRenderer.h"
#include "CD3D9MaterialRenderServices.h"

void CD3D9Driver::setRenderState3DMode(E_VERTEX_TYPE vType)
{
	CurrentRenderMode = ERM_3D;

	//default vertex shader
#ifndef FIXPIPELINE
	if (!Material.VertexShader)
		Material.VertexShader = D3D9ShaderServices->getDefaultVertexShader(vType);
#endif

	if (ResetRenderStates || LastMaterial != Material )
	{
		D3D9MaterialRenderServices->setBasicRenderStates( Material, LastMaterial, ResetRenderStates );

		LastMaterial = Material;

		MaterialRenderer = D3D9MaterialRenderServices->getMaterialRenderer((E_MATERIAL_TYPE)Material.MaterialType);

		// set new material.
		MaterialRenderer->OnSetMaterial( vType, Material, ResetRenderStates);	
	}

	D3D9MaterialRenderServices->setOverrideRenderStates( OverrideMaterial, ResetRenderStates );

	ResetRenderStates = false;
}

void CD3D9Driver::setRenderState2DMode( E_VERTEX_TYPE vType, const S2DBlendParam& blendParam )
{
	if ( CurrentRenderMode != ERM_2D )
	{
		matrix4 matView(true);
		matView.setTranslation(vector3df(-0.5f,-0.5f,0));

		matrix4 matProject(true);
		const dimension2du& renderTargetSize = ScreenSize;
		matProject.buildProjectionMatrixOrthoLH(f32(renderTargetSize.Width), f32(-(s32)(renderTargetSize.Height)), -1.0, 1.0);
		matProject.setTranslation(vector3df(-1,1,0));

#ifdef FIXPIPELINE
		pID3DDevice->SetTransform( D3DTS_WORLD, (D3DMATRIX*)(void*)matrix4::Identity().pointer());
		pID3DDevice->SetTransform( D3DTS_VIEW, (D3DMATRIX*)(void*)matView.pointer());
		pID3DDevice->SetTransform( D3DTS_PROJECTION, (D3DMATRIX*)(void*)matProject.pointer());
#endif
		Matrices[ETS_WORLD] = matrix4::Identity();
		Matrices[ETS_VIEW] = matView;
		Matrices[ETS_PROJECTION] = matProject;

		WV = Matrices[ETS_WORLD] * Matrices[ETS_VIEW];
		WVP = WV * Matrices[ETS_PROJECTION];

		CurrentRenderMode = ERM_2D;
	}

	//default vertex shader
#ifndef FIXPIPELINE
		InitMaterial2D.VertexShader = D3D9ShaderServices->getDefaultVertexShader(vType);
#endif

	if (ResetRenderStates || LastMaterial != InitMaterial2D )
	{
		D3D9MaterialRenderServices->setBasicRenderStates(InitMaterial2D, LastMaterial, ResetRenderStates);

		LastMaterial = InitMaterial2D;
	}

	D3D9MaterialRenderServices->set2DRenderStates(blendParam, ResetRenderStates);

	D3D9MaterialRenderServices->setOverrideRenderStates(InitOverrideMaterial2D, ResetRenderStates);

	ResetRenderStates = false;
}

void CD3D9Driver::draw3DMode( const SBufferParam& bufferParam, E_PRIMITIVE_TYPE primType,
	u32 primCount, const SDrawParam& drawParam)
{
	setRenderState3DMode(bufferParam.vType);

	//draw
	u32 cPasses = MaterialRenderer->getNumPasses();
	
	for ( u32 iPass = 0; iPass < cPasses; ++iPass )
	{	
		MaterialRenderer->OnRender(Material, iPass);

		D3D9ShaderServices->applyShaders();
		D3D9ShaderServices->setShaderConstants(Material.VertexShader, Material, iPass);
		D3D9ShaderServices->setShaderConstants(D3D9MaterialRenderServices->getCurrentPixelShader(), Material, iPass);

		D3D9MaterialRenderServices->applyMaterialChanges();

		if(bufferParam.ibuffer)
			drawIndexedPrimitive(bufferParam, primType, primCount, drawParam);
		else
			drawPrimitive(bufferParam, primType, primCount, drawParam);
	}

}

void CD3D9Driver::draw2DMode( const SBufferParam& bufferParam, E_PRIMITIVE_TYPE primType,
	u32 primCount, const SDrawParam& drawParam,
	const S2DBlendParam& blendParam, bool zTest )
{
	SMaterial& material = zTest ? InitMaterial2DZTest : InitMaterial2D;

	setRenderState2DMode(bufferParam.vType, blendParam);

	D3D9ShaderServices->applyShaders();
	D3D9ShaderServices->setShaderConstants(material.VertexShader, material, 0);
	D3D9ShaderServices->setShaderConstants(D3D9MaterialRenderServices->getCurrentPixelShader(), material, 0);

	D3D9MaterialRenderServices->applyMaterialChanges();

	if(bufferParam.ibuffer)
		drawIndexedPrimitive(bufferParam, primType, primCount, drawParam);
	else
		drawPrimitive(bufferParam, primType, primCount, drawParam);
}

void CD3D9Driver::drawIndexedPrimitive( const SBufferParam& bufferParam, E_PRIMITIVE_TYPE primType, 
	u32 primCount, const SDrawParam& drawParam)
{
	HRESULT hr;
	IFileSystem* fs = g_Engine->getFileSystem();

	if (!drawParam.numVertices || drawParam.numVertices >= 65536 || !primCount)
	{
		fs->writeLog(ELOG_GX, "CD3D9Driver::drawIndexedPrimitive Failed: param invalid!");
		ASSERT(false);
		return;
	}

	setVertexDeclaration(bufferParam.vType);

	//stream 0
	if (CurrentDeviceState.vBuffer0 != bufferParam.vbuffer0 ||
 		CurrentDeviceState.vOffset0 != drawParam.voffset0)
	{
		ASSERT(bufferParam.vbuffer0->HWLink);

		u32 stride = getStreamPitchFromType(bufferParam.vbuffer0->Type);
		hr = pID3DDevice->SetStreamSource(0, (IDirect3DVertexBuffer9*)bufferParam.vbuffer0->HWLink, drawParam.voffset0 * stride, stride);
		if(FAILED(hr))
		{
			fs->writeLog(ELOG_GX, "CD3D9Driver::drawIndexedPrimitive Failed: SetStreamSource0");
		}

		CurrentDeviceState.vBuffer0 = bufferParam.vbuffer0;
		CurrentDeviceState.vOffset0 = drawParam.voffset0;
	}

	//stream 1
	if (bufferParam.vbuffer1)
	{
		if (CurrentDeviceState.vBuffer1 != bufferParam.vbuffer1 ||
			CurrentDeviceState.vOffset1 != drawParam.voffset1)
		{
			ASSERT(bufferParam.vbuffer1->HWLink);

			u32 stride = getStreamPitchFromType(bufferParam.vbuffer1->Type);

			hr = pID3DDevice->SetStreamSource(1, (IDirect3DVertexBuffer9*)bufferParam.vbuffer1->HWLink, drawParam.voffset1 * stride, stride);
			if(FAILED(hr))
			{
				fs->writeLog(ELOG_GX, "CD3D9Driver::drawIndexedPrimitive Failed: SetStreamSource1");
			}

			CurrentDeviceState.vBuffer1 = bufferParam.vbuffer1;
			CurrentDeviceState.vOffset1 = drawParam.voffset1;
		}
	}

	if (CurrentDeviceState.iBuffer != bufferParam.ibuffer)
	{
		ASSERT(bufferParam.ibuffer->HWLink);

		hr = pID3DDevice->SetIndices((IDirect3DIndexBuffer9*)bufferParam.ibuffer->HWLink);
		if(FAILED(hr))
		{
			fs->writeLog(ELOG_GX, "CD3D9Driver::drawIndexedPrimitive Failed: SetIndices");
		}
		CurrentDeviceState.iBuffer = bufferParam.ibuffer;
	}

	D3DPRIMITIVETYPE type = CD3D9Helper::getD3DTopology(primType);
	hr = pID3DDevice->DrawIndexedPrimitive(type, drawParam.baseVertIndex, drawParam.minVertIndex,
		drawParam.numVertices, drawParam.startIndex, primCount);
	
	if (FAILED(hr))
	{
		ASSERT(false);
		fs->writeLog(ELOG_GX, "CD3D9Driver::drawIndexedPrimitive Failed: DrawIndexedPrimitive");
	}

	if (primType == EPT_TRIANGLES || primType == EPT_TRIANGLE_STRIP)
	{
		PrimitivesDrawn += primCount;
	}

	++DrawCall;
}

void CD3D9Driver::drawPrimitive( const SBufferParam& bufferParam,
	E_PRIMITIVE_TYPE primType,
	u32 primCount,
	const SDrawParam& drawParam )
{
	HRESULT hr;
	IFileSystem* fs = g_Engine->getFileSystem();

	if (!drawParam.numVertices || drawParam.numVertices >= 65536 || !primCount)
	{
		fs->writeLog(ELOG_GX, "CD3D9Driver::drawPrimitive Failed: param invalid!");
		ASSERT(false);
		return;
	}

	setVertexDeclaration(bufferParam.vType);

	//stream 0
	if (CurrentDeviceState.vBuffer0 != bufferParam.vbuffer0 ||
		CurrentDeviceState.vOffset0 != drawParam.voffset0)
	{
		ASSERT(bufferParam.vbuffer0->HWLink);

		u32 stride = getStreamPitchFromType(bufferParam.vbuffer0->Type);
		hr = pID3DDevice->SetStreamSource(0, (IDirect3DVertexBuffer9*)bufferParam.vbuffer0->HWLink, drawParam.voffset0 * stride, stride);
		if(FAILED(hr))
		{
			fs->writeLog(ELOG_GX, "CD3D9Driver::drawPrimitive Failed: SetStreamSource0");
		}

		CurrentDeviceState.vBuffer0 = bufferParam.vbuffer0;
		CurrentDeviceState.vOffset0 = drawParam.voffset0;
	}

	//stream 1
	if (bufferParam.vbuffer1)
	{
		u32 stride = getStreamPitchFromType(bufferParam.vbuffer1->Type);

		if (CurrentDeviceState.vBuffer1 != bufferParam.vbuffer1 ||
			CurrentDeviceState.vOffset1 != drawParam.voffset1)
		{
			ASSERT(bufferParam.vbuffer1->HWLink);

			hr = pID3DDevice->SetStreamSource(1, (IDirect3DVertexBuffer9*)bufferParam.vbuffer1->HWLink, drawParam.voffset1 * stride, stride);
			if(FAILED(hr))
			{
				fs->writeLog(ELOG_GX, "CD3D9Driver::drawPrimitive Failed: SetStreamSource1");
			}
			CurrentDeviceState.vBuffer1 = bufferParam.vbuffer1;
			CurrentDeviceState.vOffset1 = drawParam.voffset1;
		}
	}

	D3DPRIMITIVETYPE type = CD3D9Helper::getD3DTopology(primType);
	hr = pID3DDevice->DrawPrimitive(type,  drawParam.startIndex, primCount);
	if (FAILED(hr))
	{
		ASSERT(false);
		fs->writeLog(ELOG_GX, "CD3D9Driver::drawPrimitive Failed: DrawPrimitive");
	}

	if (primType == EPT_TRIANGLES || primType == EPT_TRIANGLE_STRIP)
	{
		PrimitivesDrawn += primCount;
	}

	++DrawCall;
}

void CD3D9Driver::drawDebugInfo( const c8* strMsg )
{
	if (DeviceLost)
		return;

	vector2di pos = vector2di(5,5);

	f32 fps = g_Engine->getSceneManager()->getFPS();
	// 	sprintf_s(DebugMsg, 512, "显卡: %s\n分辨率: %d X %d\nFPS: %0.1f\n三角形数: %d\n三角形Draw次数: %d\n小区域内存剩余(%%): %0.1f\n区域内存剩余(%%): %0.1f\n", 
	// 		DeviceDescription, present.BackBufferWidth, present.BackBufferHeight, getFPS(), PrimitivesDrawn, DrawCall,
	// 		Z_AvailableSmallMemoryPercent() * 100,
	// 		Z_AvailableMemoryPercent() * 100);

	Q_sprintf(DebugMsg, 512, "Dev: %s\nGraphics: %s\nRes: %d X %d\nFPS: %0.1f\nTriangles: %d\nDraw Call: %d\n", 
		AdapterInfo.description, 
		getEnumString(EDT_DIRECT3D9),
		(s32)Present.BackBufferWidth, (s32)Present.BackBufferHeight, 
		fps, 
		(s32)PrimitivesDrawn, 
		(s32)DrawCall);

	Q_strcat(DebugMsg, 512, strMsg);

	g_Engine->getDefaultFont()->drawA(DebugMsg, SColor(0,255,0), pos);
}

#endif
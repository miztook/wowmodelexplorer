#include "stdafx.h"
#include "CD3D9Driver.h"
#include "mywow.h"
#include "CD3D9Helper.h"
#include "CD3D9Shader.h"
#include "CD3D9ShaderServices.h"
#include "CD3D9MaterialRenderer.h"
#include "CD3D9MaterialRenderServices.h"

void CD3D9Driver::setRenderState3DMode()
{
	CurrentRenderMode = ERM_3D;

	if (ResetRenderStates || LastMaterial != Material )
	{
		if (LastMaterial.MaterialType != Material.MaterialType)
		{
			IMaterialRenderer* renderer = MaterialRenderServices->getMaterialRenderer(LastMaterial.MaterialType);
			if (renderer)
				renderer->OnUnsetMaterial();
		}

		MaterialRenderServices->setBasicRenderStates( Material, LastMaterial, ResetRenderStates );

		MaterialRenderer = MaterialRenderServices->getMaterialRenderer(Material.MaterialType);

		// set new material.
		MaterialRenderer->OnSetMaterial( Material, LastMaterial, ResetRenderStates);	
	}

	MaterialRenderServices->setOverrideRenderStates( OverrideMaterial, ResetRenderStates );

	LastMaterial = Material;
	ResetRenderStates = false;
}

void CD3D9Driver::setRenderState2DMode( bool alpha, bool alphaChannel, E_BLEND_FACTOR srcBlend, E_BLEND_FACTOR destBlend )
{
	if ( CurrentRenderMode != ERM_2D )
	{
		setTransform(ETS_WORLD, matrix4::Identity());

		matrix4 matView(true);
		matView.setTranslation(vector3df(-0.5f,-0.5f,0));
		setTransform(ETS_VIEW, matView);

		matrix4 matProject(true);
		const dimension2du& renderTargetSize = ScreenSize;
		matProject.buildProjectionMatrixOrthoLH(f32(renderTargetSize.Width), f32(-(s32)(renderTargetSize.Height)), -1.0, 1.0);
		matProject.setTranslation(vector3df(-1,1,0));
		setTransform(ETS_PROJECTION, matProject);

		CurrentRenderMode = ERM_2D;
	}

	if (ResetRenderStates || LastMaterial != InitMaterial2D )
	{
		MaterialRenderServices->setBasicRenderStates(InitMaterial2D, LastMaterial, ResetRenderStates);
	}

	MaterialRenderServices->set2DRenderStates(alpha, alphaChannel, srcBlend, destBlend, ResetRenderStates);

	MaterialRenderServices->setOverrideRenderStates(InitOverrideMaterial2D, ResetRenderStates);

	LastMaterial = InitMaterial2D;
	ResetRenderStates = false;
}

void CD3D9Driver::draw3DMode( const SBufferParam& bufferParam, E_PRIMITIVE_TYPE primType,
	u32 primCount, const SDrawParam& drawParam)
{
	setRenderState3DMode();

	//draw
	CD3D9Effect* effect = static_cast<CD3D9Effect*>(Material.Effect);

	ID3DXEffect* pEffect = NULL; 
	u32 cPasses = 0; 

	if (effect)
	{
		pEffect = effect->getEffect();
		cPasses = effect->NumPasses;

		ShaderServices->setShaderConstants(effect, Material);
	}
	else		
	{
		cPasses = MaterialRenderer->getNumPasses();

		ShaderServices->setShaderConstants(Material.VertexShader, Material);
	}

	if (pEffect)
	{
		for ( u32 iPass = 0; iPass < cPasses; ++iPass )
		{
			MaterialRenderer->OnRender(Material, iPass);

			pEffect->BeginPass(iPass);

			pEffect->CommitChanges();

			if(bufferParam.ibuffer)
				drawIndexedPrimitive(bufferParam, primType, primCount, drawParam);
			else
				drawPrimitive(bufferParam, primType, primCount, drawParam);

			pEffect->EndPass();
		}
	}
	else
	{
		for ( u32 iPass = 0; iPass < cPasses; ++iPass )
		{
			MaterialRenderer->OnRender(Material, iPass);

			if(bufferParam.ibuffer)
				drawIndexedPrimitive(bufferParam, primType, primCount, drawParam);
			else
				drawPrimitive(bufferParam, primType, primCount, drawParam);
		}
	}

}

void CD3D9Driver::draw2DMode( const SBufferParam& bufferParam, E_PRIMITIVE_TYPE primType,
	u32 primCount, const SDrawParam& drawParam,
	bool alpha, bool alphaChannel, E_BLEND_FACTOR srcBlend, E_BLEND_FACTOR destBlend )
{
	setRenderState2DMode(alpha, alphaChannel, srcBlend, destBlend);

	if(bufferParam.ibuffer)
		drawIndexedPrimitive(bufferParam, primType, primCount, drawParam);
	else
		drawPrimitive(bufferParam, primType, primCount, drawParam);
}

void CD3D9Driver::drawIndexedPrimitive( const SBufferParam& bufferParam, E_PRIMITIVE_TYPE primType, 
	u32 primCount, const SDrawParam& drawParam)
{
	HRESULT hr;

	setVertexDeclaration(bufferParam.vType);

	D3DFORMAT indexType = CD3D9Helper::getIndexType(bufferParam.ibuffer->Type);

	_ASSERT( primCount < 20000 );
	if (!drawParam.numVertices || !primCount)
	{
		_ASSERT(false);
		return;
	}

	//stream 0
	if (CurrentDeviceState.vBuffer0 != bufferParam.vbuffer0 ||
 		CurrentDeviceState.vOffset0 != drawParam.voffset0)
	{
		u32 stride = getStreamPitchFromType(bufferParam.vbuffer0->Type);
		pID3DDevice->SetStreamSource(0, (IDirect3DVertexBuffer9*)bufferParam.vbuffer0->HWLink, drawParam.voffset0 * stride, stride);

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
			pID3DDevice->SetStreamSource(1, (IDirect3DVertexBuffer9*)bufferParam.vbuffer1->HWLink, drawParam.voffset1 * stride, stride);

			CurrentDeviceState.vBuffer1 = bufferParam.vbuffer1;
			CurrentDeviceState.vOffset1 = drawParam.voffset1;
		}
	}

	//stream 2
	if (bufferParam.vbuffer2)
	{
		u32 stride = getStreamPitchFromType(bufferParam.vbuffer2->Type);

		if (CurrentDeviceState.vBuffer2 != bufferParam.vbuffer2 ||
			CurrentDeviceState.vOffset2 != drawParam.voffset2)
		{
			pID3DDevice->SetStreamSource(2, (IDirect3DVertexBuffer9*)bufferParam.vbuffer2->HWLink, drawParam.voffset2 * stride, stride);

			CurrentDeviceState.vBuffer2 = bufferParam.vbuffer2;
			CurrentDeviceState.vOffset2 = drawParam.voffset2;
		}
	}

	//stream 3
	if (bufferParam.vbuffer3)
	{
		u32 stride = getStreamPitchFromType(bufferParam.vbuffer3->Type);

		if (CurrentDeviceState.vBuffer3 != bufferParam.vbuffer3 ||
			CurrentDeviceState.vOffset3 != drawParam.voffset3)
		{
			pID3DDevice->SetStreamSource(3, (IDirect3DVertexBuffer9*)bufferParam.vbuffer3->HWLink, drawParam.voffset3 * stride, stride);

			CurrentDeviceState.vBuffer3 = bufferParam.vbuffer3;
			CurrentDeviceState.vOffset3 = drawParam.voffset3;
		}
	}
	
	if (CurrentDeviceState.iBuffer != bufferParam.ibuffer)
	{
		pID3DDevice->SetIndices((IDirect3DIndexBuffer9*)bufferParam.ibuffer->HWLink);
		CurrentDeviceState.iBuffer = bufferParam.ibuffer;
	}

	switch(primType)
	{
	case EPT_POINTS:
		hr = pID3DDevice->DrawIndexedPrimitive(D3DPT_POINTLIST, drawParam.baseVertIndex, drawParam.minVertIndex,
			drawParam.numVertices, drawParam.startIndex, primCount);
		break;
	case EPT_LINE_STRIP:
		hr = pID3DDevice->DrawIndexedPrimitive(D3DPT_LINESTRIP, drawParam.baseVertIndex, drawParam.minVertIndex,
			drawParam.numVertices, drawParam.startIndex, primCount);
		break;
	case EPT_LINES:
		hr = pID3DDevice->DrawIndexedPrimitive(D3DPT_LINELIST, drawParam.baseVertIndex, drawParam.minVertIndex,
			drawParam.numVertices, drawParam.startIndex, primCount);
		break;
	case EPT_TRIANGLE_STRIP:
		hr = pID3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, drawParam.baseVertIndex, drawParam.minVertIndex,
			drawParam.numVertices, drawParam.startIndex, primCount);
		PrimitivesDrawn += primCount;
		break;
	case EPT_TRIANGLES:
		hr = pID3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, drawParam.baseVertIndex, drawParam.minVertIndex,
			drawParam.numVertices, drawParam.startIndex, primCount);
		PrimitivesDrawn += primCount;
		break;
	}

	if (FAILED(hr))
		_ASSERT(false);

	++DrawCall;
}

void CD3D9Driver::drawPrimitive( const SBufferParam& bufferParam,
	E_PRIMITIVE_TYPE primType,
	u32 primCount,
	const SDrawParam& drawParam )
{
	HRESULT hr;

	setVertexDeclaration(bufferParam.vType);

	_ASSERT( primCount < 20000 );

	//stream 0
	if (CurrentDeviceState.vBuffer0 != bufferParam.vbuffer0 ||
		CurrentDeviceState.vOffset0 != drawParam.voffset0)
	{
		u32 stride = getStreamPitchFromType(bufferParam.vbuffer0->Type);
		pID3DDevice->SetStreamSource(0, (IDirect3DVertexBuffer9*)bufferParam.vbuffer0->HWLink, drawParam.voffset0 * stride, stride);

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
			pID3DDevice->SetStreamSource(1, (IDirect3DVertexBuffer9*)bufferParam.vbuffer1->HWLink, drawParam.voffset1 * stride, stride);

			CurrentDeviceState.vBuffer1 = bufferParam.vbuffer1;
			CurrentDeviceState.vOffset1 = drawParam.voffset1;
		}
	}

	//stream 2
	if (bufferParam.vbuffer2)
	{
		u32 stride = getStreamPitchFromType(bufferParam.vbuffer2->Type);

		if (CurrentDeviceState.vBuffer2 != bufferParam.vbuffer2 ||
			CurrentDeviceState.vOffset2 != drawParam.voffset2)
		{
			pID3DDevice->SetStreamSource(2, (IDirect3DVertexBuffer9*)bufferParam.vbuffer2->HWLink, drawParam.voffset2 * stride, stride);

			CurrentDeviceState.vBuffer2 = bufferParam.vbuffer2;
			CurrentDeviceState.vOffset2 = drawParam.voffset2;
		}
	}
	
	//stream 3
	if (bufferParam.vbuffer3)
	{
		u32 stride = getStreamPitchFromType(bufferParam.vbuffer3->Type);

		if (CurrentDeviceState.vBuffer3 != bufferParam.vbuffer3 ||
			CurrentDeviceState.vOffset3 != drawParam.voffset3)
		{
			pID3DDevice->SetStreamSource(3, (IDirect3DVertexBuffer9*)bufferParam.vbuffer3->HWLink, drawParam.voffset3 * stride, stride);

			CurrentDeviceState.vBuffer3 = bufferParam.vbuffer3;
			CurrentDeviceState.vOffset3 = drawParam.voffset3;
		}
	}
	
	u32 vStart = drawParam.startIndex;

	switch(primType)
	{
	case EPT_POINTS:
		hr = pID3DDevice->DrawPrimitive(D3DPT_POINTLIST, vStart, primCount);
		break;
	case EPT_LINE_STRIP:
		hr = pID3DDevice->DrawPrimitive(D3DPT_LINESTRIP, vStart, primCount);
		break;
	case EPT_LINES:
		hr = pID3DDevice->DrawPrimitive(D3DPT_LINELIST, vStart, primCount);
		break;
	case EPT_TRIANGLE_STRIP:
		hr = pID3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, vStart, primCount);
		PrimitivesDrawn += primCount;
		break;
	case EPT_TRIANGLES:
		hr = pID3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, vStart, primCount);
		PrimitivesDrawn += primCount;
		break;
	}

	if (FAILED(hr))
		_ASSERT(false);

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

	sprintf_s(DebugMsg, 512, "Dev: %s\nRes: %d X %d\nFPS: %0.1f\nTriangles: %d\nDraw Call: %d\n", 
		AdapterInfo.description, present.BackBufferWidth, present.BackBufferHeight, fps, PrimitivesDrawn, DrawCall);

	strcat_s(DebugMsg, 512, strMsg);

	g_Engine->getFont()->drawA(DebugMsg, SColor(0,255,0), pos);
}
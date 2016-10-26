#include "stdafx.h"
#include "CD3D11Driver.h"
#include "mywow.h"

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D11

#include "CD3D11MaterialRenderServices.h"
#include "CD3D11ShaderServices.h"
#include "CD3D11Helper.h"

void CD3D11Driver::setRenderState3DMode( E_VERTEX_TYPE vType )
{
	CurrentRenderMode = ERM_3D;

	//default vertex shader
	if (!Material.VertexShader)
		Material.VertexShader = D3D11ShaderServices->getDefaultVertexShader(vType);

	if (ResetRenderStates || LastMaterial != Material )
	{
		D3D11MaterialRenderServices->setBasicRenderStates( Material, LastMaterial, ResetRenderStates );

		LastMaterial = Material;

		MaterialRenderer = D3D11MaterialRenderServices->getMaterialRenderer((E_MATERIAL_TYPE)Material.MaterialType);

		// set new material.
		MaterialRenderer->OnSetMaterial( vType, Material, ResetRenderStates);	
	}

	D3D11MaterialRenderServices->setOverrideRenderStates( OverrideMaterial, ResetRenderStates );

	ResetRenderStates = false;
}

void CD3D11Driver::setRenderState2DMode( E_VERTEX_TYPE vType, const S2DBlendParam& blendParam )
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
	InitMaterial2D.VertexShader = D3D11ShaderServices->getDefaultVertexShader(vType);

	if (ResetRenderStates || LastMaterial != InitMaterial2D )
	{
		D3D11MaterialRenderServices->setBasicRenderStates(InitMaterial2D, LastMaterial, ResetRenderStates);

		LastMaterial = InitMaterial2D;
	}

	D3D11MaterialRenderServices->set2DRenderStates(blendParam, ResetRenderStates);

	D3D11MaterialRenderServices->setOverrideRenderStates(InitOverrideMaterial2D, ResetRenderStates);

	ResetRenderStates = false;
}

void CD3D11Driver::draw3DMode( const SBufferParam& bufferParam, E_PRIMITIVE_TYPE primType, u32 primCount, const SDrawParam& drawParam )
{
	setRenderState3DMode(bufferParam.vType);

	//draw
	u32 cPasses = MaterialRenderer->getNumPasses();

	for ( u32 iPass = 0; iPass < cPasses; ++iPass )
	{
		MaterialRenderer->OnRender(Material, iPass);	
		
		D3D11ShaderServices->applyShaders();
		D3D11ShaderServices->setShaderConstants(Material.VertexShader, Material, iPass);
		D3D11ShaderServices->setShaderConstants(D3D11MaterialRenderServices->getCurrentPixelShader(), Material, iPass);

		D3D11MaterialRenderServices->applyMaterialChanges();

		if(bufferParam.ibuffer)
			drawIndexedPrimitive(bufferParam, Material.VertexShader, primType, primCount, drawParam);
		else
			drawPrimitive(bufferParam, Material.VertexShader, primType, primCount, drawParam);	
	}
}

void CD3D11Driver::draw2DMode( const SBufferParam& bufferParam, E_PRIMITIVE_TYPE primType, u32 primCount, const SDrawParam& drawParam, const S2DBlendParam& blendParam, bool zTest )
{
	SMaterial& material = zTest ? InitMaterial2DZTest : InitMaterial2D;

	setRenderState2DMode(bufferParam.vType, blendParam);

	D3D11ShaderServices->applyShaders();
	D3D11ShaderServices->setShaderConstants(material.VertexShader, material, 0);
	D3D11ShaderServices->setShaderConstants(D3D11MaterialRenderServices->getCurrentPixelShader(), material, 0);

	D3D11MaterialRenderServices->applyMaterialChanges();

	if(bufferParam.ibuffer)
		drawIndexedPrimitive(bufferParam, material.VertexShader, primType, primCount, drawParam);
	else
		drawPrimitive(bufferParam, material.VertexShader, primType, primCount, drawParam);
}

void CD3D11Driver::drawIndexedPrimitive( const SBufferParam& bufferParam, IVertexShader* vs, E_PRIMITIVE_TYPE primType, u32 primCount, const SDrawParam& drawParam )
{
	if (!drawParam.numVertices || drawParam.numVertices >= 65536 || !primCount)
	{	
		IFileSystem* fs = g_Engine->getFileSystem();
		fs->writeLog(ELOG_GX, "CD3D111Driver::drawIndexedPrimitive Failed: param invalid!");
		ASSERT(false);
		return;
	}

	setVertexDeclaration(bufferParam.vType, vs);

	//stream 0
	if (CurrentDeviceState.vBuffer0 != bufferParam.vbuffer0 ||
		CurrentDeviceState.vOffset0 != drawParam.voffset0)
	{
		u32 stride = getStreamPitchFromType(bufferParam.vbuffer0->Type);
		ID3D11Buffer* buffers[1] = { static_cast<ID3D11Buffer*>(bufferParam.vbuffer0->HWLink) };
		u32 offset = drawParam.voffset0 * stride;

		ImmediateContext->IASetVertexBuffers(0, 1, buffers, &stride, &offset);

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
			//ImmediateContext
			ID3D11Buffer* buffers[1] = { static_cast<ID3D11Buffer*>(bufferParam.vbuffer1->HWLink) };
			u32 offset = drawParam.voffset1 * stride;

			ImmediateContext->IASetVertexBuffers(1, 1, buffers, &stride, &offset);

			CurrentDeviceState.vBuffer1 = bufferParam.vbuffer1;
			CurrentDeviceState.vOffset1 = drawParam.voffset1;
		}
	}

	if (CurrentDeviceState.iBuffer != bufferParam.ibuffer)
	{
		DXGI_FORMAT iType = CD3D11Helper::getD3DIndexType(bufferParam.ibuffer->Type);

		ImmediateContext->IASetIndexBuffer((ID3D11Buffer*)bufferParam.ibuffer->HWLink, iType, 0);

		CurrentDeviceState.iBuffer = bufferParam.ibuffer;
	}

	ImmediateContext->IASetPrimitiveTopology(CD3D11Helper::getD3DTopology(primType));

	ImmediateContext->DrawIndexed(getIndexCount(primType, primCount), drawParam.startIndex, drawParam.baseVertIndex);

	if (primType == EPT_TRIANGLES || primType == EPT_TRIANGLE_STRIP)
	{
		PrimitivesDrawn += primCount;
	}

	++DrawCall;
}

void CD3D11Driver::drawPrimitive( const SBufferParam& bufferParam, IVertexShader* vs, E_PRIMITIVE_TYPE primType, u32 primCount, const SDrawParam& drawParam )
{
	IFileSystem* fs = g_Engine->getFileSystem();

	if (!drawParam.numVertices || drawParam.numVertices >= 65536 || !primCount)
	{
		fs->writeLog(ELOG_GX, "CD3D11Driver::drawPrimitive Failed: param invalid!");
		ASSERT(false);
		return;
	}

	setVertexDeclaration(bufferParam.vType, vs);

	//stream 0
	if (CurrentDeviceState.vBuffer0 != bufferParam.vbuffer0 ||
		CurrentDeviceState.vOffset0 != drawParam.voffset0)
	{
		u32 stride = getStreamPitchFromType(bufferParam.vbuffer0->Type);

		ID3D11Buffer* buffers[1] = { static_cast<ID3D11Buffer*>(bufferParam.vbuffer0->HWLink) };
		u32 offset = drawParam.voffset0 * stride;

		ImmediateContext->IASetVertexBuffers(0, 1, buffers, &stride, &offset);

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
			ID3D11Buffer* buffers[1] = { static_cast<ID3D11Buffer*>(bufferParam.vbuffer1->HWLink) };
			u32 offset = drawParam.voffset1 * stride;

			ImmediateContext->IASetVertexBuffers(1, 1, buffers, &stride, &offset);

			CurrentDeviceState.vBuffer1 = bufferParam.vbuffer1;
			CurrentDeviceState.vOffset1 = drawParam.voffset1;
		}
	}

	ImmediateContext->IASetPrimitiveTopology(CD3D11Helper::getD3DTopology(primType));

	ImmediateContext->Draw(drawParam.numVertices, 0);

	if (primType == EPT_TRIANGLES || primType == EPT_TRIANGLE_STRIP)
	{
		PrimitivesDrawn += primCount;
	}

	++DrawCall;
}

void CD3D11Driver::drawDebugInfo( const c8* strMsg )
{
	vector2di pos = vector2di(5,5);

	f32 fps = g_Engine->getSceneManager()->getFPS();
	// 	sprintf_s(DebugMsg, 512, "显卡: %s\n分辨率: %d X %d\nFPS: %0.1f\n三角形数: %d\n三角形Draw次数: %d\n小区域内存剩余(%%): %0.1f\n区域内存剩余(%%): %0.1f\n", 
	// 		DeviceDescription, present.BackBufferWidth, present.BackBufferHeight, getFPS(), PrimitivesDrawn, DrawCall,
	// 		Z_AvailableSmallMemoryPercent() * 100,
	// 		Z_AvailableMemoryPercent() * 100);

	Q_sprintf(DebugMsg, 512, "Dev: %s\nGraphics: %s\nRes: %d X %d\nFPS: %0.1f\nTriangles: %d\nDraw Call: %d\n", 
		AdapterInfo.description, 
		getEnumString(EDT_DIRECT3D11),
		(s32)Present.BufferDesc.Width, (s32)Present.BufferDesc.Height, 
		fps, 
		(s32)PrimitivesDrawn, 
		(s32)DrawCall);

	Q_strcat(DebugMsg, 512, strMsg);

	g_Engine->getDefaultFont()->drawA(DebugMsg, SColor(0,255,0), pos);

}

#endif
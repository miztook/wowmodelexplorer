#include "stdafx.h"
#include "CD3D11MaterialRenderServices.h"
#include "mywow.h"

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D11

#include "CD3D11Driver.h"
#include "CD3D11Helper.h"
#include "CD3D11Texture.h"
#include "CD3D11MaterialRenderer.h"
#include "CD3D11Shader.h"
#include "CD3D11ShaderServices.h"

CD3D11MaterialRenderer_Solid			g_MaterialRenderer_Solid;
CD3D11MaterialRenderer_Transparent_Alpha_Blend		g_MaterialRenderer_Transparent_Alpha_Blend;
CD3D11MaterialRenderer_Transparent_Alpha_Test		g_MaterialRenderer_Transparent_Alpha_Test;
CD3D11MaterialRenderer_Terrain_MultiPass		g_MaterialRenderer_Terrain_MultiPass;

CD3D11MaterialRenderServices::CD3D11MaterialRenderServices()
{
	Driver = static_cast<CD3D11Driver*>(g_Engine->getDriver());
	ShaderServices = static_cast<CD3D11ShaderServices*>(Driver->getShaderServices());

	resetRSCache();

	::memset(&Last2DBlendParam, 0, sizeof(S2DBlendParam));
	::memset(MaterialRenderers, 0, sizeof(MaterialRenderers));

	MaterialRenderers[EMT_LINE] =
		MaterialRenderers[EMT_SOLID] = &g_MaterialRenderer_Solid;
	MaterialRenderers[EMT_TERRAIN_MULTIPASS] = &g_MaterialRenderer_Terrain_MultiPass;

	MaterialRenderers[EMT_TRANSPARENT_ALPHA_BLEND] =
		MaterialRenderers[EMT_TRANSAPRENT_ALPHA_BLEND_TEST] =
		MaterialRenderers[EMT_TRANSPARENT_ONE_ALPHA] =
		MaterialRenderers[EMT_TRANSPARENT_ADD_ALPHA] =
		MaterialRenderers[EMT_TRANSPARENT_ADD_COLOR] =
		MaterialRenderers[EMT_TRANSPARENT_MODULATE] =
		MaterialRenderers[EMT_TRANSPARENT_MODULATE_X2] = &g_MaterialRenderer_Transparent_Alpha_Blend;
	MaterialRenderers[EMT_ALPHA_TEST] = &g_MaterialRenderer_Transparent_Alpha_Test;
}

CD3D11MaterialRenderServices::~CD3D11MaterialRenderServices()
{
	clearStateMap();
}

ID3D11BlendState* CD3D11MaterialRenderServices::getBlendState()
{
	ID3D11BlendState* state = NULL_PTR;
	const SD3D11_BLEND_DESC& blendDesc = CurrentRenderState.BlendDesc;

	T_BlendStateMap::const_iterator itr = BlendStateMap.find(blendDesc);
	if (itr != BlendStateMap.end())
	{
		state = itr->second;
	}
	else
	{
		if(FAILED(Driver->pID3DDevice11->CreateBlendState(&blendDesc, &state)))
		{
			ASSERT(false);
		}
		BlendStateMap[blendDesc] = state;
	}
	return state;
}

ID3D11RasterizerState* CD3D11MaterialRenderServices::getRasterizerState()
{
	ID3D11RasterizerState* state = NULL_PTR;
	const SD3D11_RASTERIZER_DESC& rasterizerDesc = CurrentRenderState.RasterizerDesc;

	T_RasterizerStateMap::const_iterator itr = RasterizerStateMap.find(rasterizerDesc);
	if (itr != RasterizerStateMap.end())
	{
		state = itr->second;
	}
	else
	{
		if(FAILED(Driver->pID3DDevice11->CreateRasterizerState(&rasterizerDesc, &state)))
		{
			ASSERT(false);
		}
		RasterizerStateMap[rasterizerDesc] = state;
	}
	return state;
}

ID3D11DepthStencilState* CD3D11MaterialRenderServices::getDepthStencilState()
{
	ID3D11DepthStencilState* state = NULL_PTR;
	const SD3D11_DEPTH_STENCIL_DESC& depthStencilDesc = CurrentRenderState.DepthStencilDesc;

	T_DepthStencilStateMap::const_iterator itr = DepthStencilStateMap.find(depthStencilDesc);
	if (itr != DepthStencilStateMap.end())
	{
		state = itr->second;
	}
	else
	{
		if(FAILED(Driver->pID3DDevice11->CreateDepthStencilState(&depthStencilDesc, &state)))
		{
			ASSERT(false);
		}
		DepthStencilStateMap[depthStencilDesc] = state;
	}
	return state;
}

ID3D11SamplerState* CD3D11MaterialRenderServices::getSamplerState( u32 index )
{
	if(index >= MATERIAL_MAX_TEXTURES)
		return NULL_PTR;

	ID3D11SamplerState* state = NULL_PTR;
	const SD3D11_SAMPLER_DESC& samplerDesc = CurrentRenderState.TextureUnits[index].SamplerDesc;

	T_SamplerStateMap::const_iterator itr = SamplerStateMap.find(samplerDesc);
	if (itr != SamplerStateMap.end())
	{
		state = itr->second;
	}
	else
	{
		if(FAILED(Driver->pID3DDevice11->CreateSamplerState(&samplerDesc, &state)))
		{
			ASSERT(false);
		}
		SamplerStateMap[samplerDesc] = state;
	}
	return state;
}

void CD3D11MaterialRenderServices::setBasicRenderStates( const SMaterial& material, const SMaterial& lastMaterial, bool resetAllRenderstates )
{
 	if (resetAllRenderstates)
	{
 		resetRSCache();
		clearStateMap();
	}

	// basic d3d material, in shader

	// shiness, specular highlights, in shader

	// fillmode
	if (resetAllRenderstates || lastMaterial.Wireframe != material.Wireframe)
	{
		CurrentRenderState.RasterizerDesc.FillMode = material.Wireframe ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;
	}
	// shade mode, in shader

	//lighting, in shader

	//zbuffer
	if (resetAllRenderstates || lastMaterial.ZBuffer != material.ZBuffer)
	{
		CurrentRenderState.DepthStencilDesc.DepthEnable = material.ZBuffer == ECFN_NEVER ? FALSE : TRUE;
		CurrentRenderState.DepthStencilDesc.DepthFunc = CD3D11Helper::getD3DCompare((E_COMPARISON_FUNC)material.ZBuffer);
	}

	// zwrite
	if (resetAllRenderstates || lastMaterial.ZWriteEnable != material.ZWriteEnable)
	{
		CurrentRenderState.DepthStencilDesc.DepthWriteMask = material.ZWriteEnable ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
	}

	// stencil
	if (resetAllRenderstates || (lastMaterial.StencilEnable != material.StencilEnable))
	{
		CurrentRenderState.DepthStencilDesc.StencilEnable = material.StencilEnable ? TRUE : FALSE;
	}

	// backface culling
	if (resetAllRenderstates || (lastMaterial.Cull != material.Cull))
	{
		D3D11_CULL_MODE cullmode;
		switch(material.Cull)
		{
		case ECM_FRONT:
			cullmode = D3D11_CULL_FRONT;
			break;
		case ECM_BACK:
			cullmode = D3D11_CULL_BACK;
			break;
		case ECM_NONE:
		default:
			cullmode = D3D11_CULL_NONE;
		}
		CurrentRenderState.RasterizerDesc.CullMode = cullmode;
	}

	// fog, in shader

	//anti alias
	if (Driver->DriverSetting.antialias)
	{
		if (resetAllRenderstates || material.AntiAliasing != lastMaterial.AntiAliasing)
		{
			BOOL multisample;
			BOOL antialiasline;
			switch(material.AntiAliasing)
			{
			case EAAM_SIMPLE:
				multisample = TRUE;
				antialiasline = FALSE;
				break;
			case EAAM_LINE_SMOOTH:
				multisample = TRUE;
				antialiasline = TRUE;
				break;
			case EAAM_OFF:
			default:
				multisample = FALSE;
				antialiasline = FALSE;
				break;
			}
			CurrentRenderState.RasterizerDesc.MultisampleEnable = multisample;
			CurrentRenderState.RasterizerDesc.AntialiasedLineEnable = antialiasline;
		}
	}

	//texture address mode
	for (u32 st=0; st<MATERIAL_MAX_TEXTURES; ++st)
	{	
		if (resetAllRenderstates || 
			material.TextureLayer[st].TextureWrapU != lastMaterial.TextureLayer[st].TextureWrapU ||
			material.TextureLayer[st].TextureWrapV != lastMaterial.TextureLayer[st].TextureWrapV)
		{
			CurrentRenderState.TextureUnits[st].SamplerDesc.AddressU = CD3D11Helper::getD3DTextureAddress((E_TEXTURE_CLAMP)material.TextureLayer[st].TextureWrapU);
			CurrentRenderState.TextureUnits[st].SamplerDesc.AddressV = CD3D11Helper::getD3DTextureAddress((E_TEXTURE_CLAMP)material.TextureLayer[st].TextureWrapV);

			//transform, in shader
		}
	}

	// shader
	if (resetAllRenderstates || material.VertexShader != lastMaterial.VertexShader)
	{
		ShaderServices->useVertexShader(material.VertexShader);
	}
}

void CD3D11MaterialRenderServices::setOverrideRenderStates( const SOverrideMaterial& overrideMaterial, bool resetAllRenderStates )
{
	for (u32 st=0; st<MATERIAL_MAX_TEXTURES; ++st)
	{
		if (resetAllRenderStates || overrideMaterial.TextureFilters[st] != LastOverrideMaterial.TextureFilters[st])
		{
			switch (overrideMaterial.TextureFilters[st])
			{
			case ETF_NONE:
				CurrentRenderState.TextureUnits[st].SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
				break;
			case ETF_BILINEAR:
				CurrentRenderState.TextureUnits[st].SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
				break;
			case ETF_TRILINEAR:
				CurrentRenderState.TextureUnits[st].SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
				break;
			default:		//anisotropic
				CurrentRenderState.TextureUnits[st].SamplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
				break;
			}
			
			CurrentRenderState.TextureUnits[st].SamplerDesc.MaxAnisotropy = (u32)getAnisotropic(overrideMaterial.TextureFilters[st]);
		}

		if (resetAllRenderStates || overrideMaterial.MipMapLodBias[st] != LastOverrideMaterial.MipMapLodBias[st])
		{
			f32 v = overrideMaterial.MipMapLodBias[st] * 0.125f;
			CurrentRenderState.TextureUnits[st].SamplerDesc.MipLODBias = v;
		}
	}
	LastOverrideMaterial = overrideMaterial;
}

void CD3D11MaterialRenderServices::set2DRenderStates( const S2DBlendParam& blendParam, bool resetAllRenderStates )
{
	SRenderStateBlock block;

	set2DPixelShaderMaterialBlock(block, blendParam.alpha, blendParam.alphaChannel);

	//blend
	if (blendParam.alpha || blendParam.alphaChannel)
	{
		block.alphaBlendEnabled = true;
		block.srcBlend = blendParam.srcBlend;
		block.destBlend = blendParam.destBlend;
	}
	else
	{
		block.alphaBlendEnabled = false;
	}

	applyMaterialBlock(block, resetAllRenderStates);

	Last2DBlendParam = blendParam;
}

void CD3D11MaterialRenderServices::set2DPixelShaderMaterialBlock( SRenderStateBlock& block, bool alpha, bool alphaChannel )
{
	ASSERT(!block.alphaTestEnabled);
	if (alphaChannel)
	{
		if (alpha)
			block.pixelShader = ShaderServices->getPixelShader(EPST_UI_ALPHA_ALPHACHANNEL);
		else
			block.pixelShader = ShaderServices->getPixelShader(EPST_UI_ALPHACHANNEL);
	}
	else
	{
		if (alpha)
			block.pixelShader = ShaderServices->getPixelShader(EPST_UI_ALPHA);
		else
			block.pixelShader = ShaderServices->getPixelShader(EPST_UI);
	}
}

void CD3D11MaterialRenderServices::applyMaterialBlock( const SRenderStateBlock& block, bool resetAllRenderStates )
{
	//color op, alpha op, tex index, in shader

	//alpha blend
	if (resetAllRenderStates || block.alphaBlendEnabled != LastMaterialBlock.alphaBlendEnabled)
	{
		BOOL v = block.alphaBlendEnabled ? TRUE : FALSE;
		CurrentRenderState.BlendDesc.RenderTarget[0].BlendEnable = v;
	}

	if (resetAllRenderStates || block.blendOp != LastMaterialBlock.blendOp)
	{
		D3D11_BLEND_OP v = CD3D11Helper::getD3DBlendOp(block.blendOp);
		CurrentRenderState.BlendDesc.RenderTarget[0].BlendOp = v;
	}

	if (resetAllRenderStates || block.srcBlend != LastMaterialBlock.srcBlend)
	{
		D3D11_BLEND v= CD3D11Helper::getD3DBlend(block.srcBlend);
		CurrentRenderState.BlendDesc.RenderTarget[0].SrcBlend = v;
	}

	if (resetAllRenderStates || block.destBlend != LastMaterialBlock.destBlend)
	{
		D3D11_BLEND v= CD3D11Helper::getD3DBlend(block.destBlend);
		CurrentRenderState.BlendDesc.RenderTarget[0].DestBlend = v;
	}

	//alpha test, in shader

	//alpha coverage
	if (resetAllRenderStates || block.alphaToCoverage != LastMaterialBlock.alphaToCoverage)
	{
		BOOL v = block.alphaToCoverage ? TRUE : FALSE;
		CurrentRenderState.BlendDesc.AlphaToCoverageEnable = v;
	}

	//pixel shader
	if (resetAllRenderStates|| block.pixelShader != LastMaterialBlock.pixelShader)
	{
		ShaderServices->usePixelShader(block.pixelShader);
	}

	LastMaterialBlock = block;
}

void CD3D11MaterialRenderServices::setPixelShaderMaterialBlock( SRenderStateBlock& block, E_PS_TYPE psType, E_VERTEX_TYPE vType )
{
	ASSERT(psType < EPST_COUNT);
	E_PS_MACRO macro = getPSMacro(block);
	if (psType >= 0 && psType < EPST_COUNT)
	{
		block.pixelShader = ShaderServices->getPixelShader(psType, macro);
	}
	else
	{
		block.pixelShader = ShaderServices->getDefaultPixelShader(vType, macro);
	}
	ASSERT(block.pixelShader);
}

void CD3D11MaterialRenderServices::setTextureWrap( u32 st, E_TEXTURE_ADDRESS address, E_TEXTURE_CLAMP wrap )
{
	D3D11_TEXTURE_ADDRESS_MODE v = CD3D11Helper::getD3DTextureAddress(wrap);
	switch (address)
	{
	case ETA_U:
		CurrentRenderState.TextureUnits[st].SamplerDesc.AddressU = v;
		Driver->LastMaterial.TextureLayer[st].TextureWrapU = wrap;
		break;
	case ETA_V:
		CurrentRenderState.TextureUnits[st].SamplerDesc.AddressV = v;
		Driver->LastMaterial.TextureLayer[st].TextureWrapU = wrap;
		break;
	case ETA_W:
		CurrentRenderState.TextureUnits[st].SamplerDesc.AddressW = v;
		Driver->LastMaterial.TextureLayer[st].TextureWrapU = wrap;
		break;
	default:
		ASSERT(false);
	}
}

E_TEXTURE_CLAMP CD3D11MaterialRenderServices::getTextureWrap( u32 st, E_TEXTURE_ADDRESS address ) const
{
	D3D11_TEXTURE_ADDRESS_MODE v = D3D11_TEXTURE_ADDRESS_WRAP;
	switch (address)
	{
	case ETA_U:
		v = CurrentRenderState.TextureUnits[st].SamplerDesc.AddressU;
		break;
	case ETA_V:
		v = CurrentRenderState.TextureUnits[st].SamplerDesc.AddressV;
		break;
	case ETA_W:
		v = CurrentRenderState.TextureUnits[st].SamplerDesc.AddressW;
		break;
	default:
		ASSERT(false);
	}

	return CD3D11Helper::getTextureAddressMode(v);
}

void CD3D11MaterialRenderServices::resetRSCache()
{
	//blend state
	RsCache.BlendState = NULL_PTR;

	//rasterizer state
	RsCache.RasterizerState = NULL_PTR;

	//depth stencil state
	RsCache.DepthStencilState = NULL_PTR;
}

void CD3D11MaterialRenderServices::clearStateMap()
{
	for (T_BlendStateMap::iterator itr = BlendStateMap.begin(); itr != BlendStateMap.end(); ++itr)
	{
		SAFE_RELEASE(itr->second);
	}
	BlendStateMap.clear();

	for (T_RasterizerStateMap::iterator itr = RasterizerStateMap.begin(); itr != RasterizerStateMap.end(); ++itr)
	{
		SAFE_RELEASE(itr->second);
	}
	RasterizerStateMap.clear();

	for (T_DepthStencilStateMap::iterator itr = DepthStencilStateMap.begin(); itr != DepthStencilStateMap.end(); ++itr)
	{
		SAFE_RELEASE(itr->second);
	}
	DepthStencilStateMap.clear();

	for (T_SamplerStateMap::iterator itr = SamplerStateMap.begin(); itr != SamplerStateMap.end(); ++itr)
	{
		SAFE_RELEASE(itr->second);
	}
	SamplerStateMap.clear();
}

void CD3D11MaterialRenderServices::applyMaterialChanges()
{
	ID3D11SamplerState* samplers[MATERIAL_MAX_TEXTURES];
	ID3D11ShaderResourceView* views[MATERIAL_MAX_TEXTURES];

	CD3D11PixelShader* ps = static_cast<CD3D11PixelShader*>(getCurrentPixelShader());

	//在shader中， sampler和texture的设置不能是不连续的，比如：不可以只用texture1,3而不用2
	//暂不支持数组

	//texture
	u32 textureCount = ps->getTextureCount();
	for (u32 i=0; i<textureCount; ++i)
	{
		const CD3D11Texture* tex = static_cast<const CD3D11Texture*>(CurrentRenderState.TextureUnits[i].texture);
		views[i] = tex ? tex->getShaderResourceView() : NULL_PTR;
		ASSERT(views[i]);
	}

	if (textureCount)
	{
		//ImmediateContext
		Driver->ImmediateContext->PSSetShaderResources( 0, textureCount, views );
	}

	//sampler
	u32 samplerCount = ps->getSamplerCount();
	for (u32 i=0; i<samplerCount; ++i)
	{
		samplers[i] = getSamplerState(i);
	}

	if (samplerCount)
	{
		//ImmediateContext
		Driver->ImmediateContext->PSSetSamplers( 0, samplerCount, samplers );
	}

	// Bind depth-stencil view
	ID3D11DepthStencilState* depthStencilState = getDepthStencilState();
	if (RsCache.DepthStencilState != depthStencilState)
	{
		//ImmediateContext
		Driver->ImmediateContext->OMSetDepthStencilState( depthStencilState, 1 );

		RsCache.DepthStencilState = depthStencilState;
	}

	// Bind blend state
	ID3D11BlendState* blendState = getBlendState();
	if (RsCache.BlendState != blendState)
	{
		//ImmediateContext
		Driver->ImmediateContext->OMSetBlendState( blendState, 0, 0xffffffff );

		RsCache.BlendState = blendState;
	}

	// Bind rasterizer state
	ID3D11RasterizerState* rasterizerState = getRasterizerState();
	if (RsCache.RasterizerState != rasterizerState)
	{
		//ImmediateContext
		Driver->ImmediateContext->RSSetState( rasterizerState );

		RsCache.RasterizerState = rasterizerState;
	}
}

#endif
#include "stdafx.h"
#include "CD3D9MaterialRenderServices.h"
#include "mywow.h"

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D9

#include "CD3D9Driver.h"
#include "CD3D9MaterialRenderer.h"
#include "CD3D9Helper.h"
#include "CD3D9ShaderServices.h"
#include "CD3D9Texture.h"

#define  DEVICE_SET_RENDER_STATE(prop, d3drs, v)	if (RsCache.prop != (v))		\
		{	pID3DDevice->SetRenderState(d3drs, (v));				\
		RsCache.prop = (v);	}						
													
#define  DEVICE_SET_SAMPLER_STATE(st, prop, d3dss, v)	if(RsCache.TextureUnits[st].prop != (v))		\
			{	pID3DDevice->SetSamplerState (st, d3dss, (v));		\
			RsCache.TextureUnits[st].prop = (v);	}

#define  DEVICE_SET_TEXTURE_STAGE_STATE(st, prop, d3dtss, v)		if(RsCache.TextureUnits[st].prop != (v))		\
			{	pID3DDevice->SetTextureStageState (st, d3dtss, (v));			\
			RsCache.TextureUnits[st].prop = (v);	}


CD3D9MaterialRenderer_Solid			g_MaterialRenderer_Solid;
CD3D9MaterialRenderer_Transparent_Alpha_Blend		g_MaterialRenderer_Transparent_Alpha_Blend;
CD3D9MaterialRenderer_Transparent_Alpha_Test		g_MaterialRenderer_Transparent_Alpha_Test;
CD3D9MaterialRenderer_Terrain_MultiPass		g_MaterialRenderer_Terrain_MultiPass;

CD3D9MaterialRenderServices::CD3D9MaterialRenderServices()
{
	Driver = static_cast<CD3D9Driver*>(g_Engine->getDriver());
	pID3DDevice = Driver->pID3DDevice;
	ShaderServices = static_cast<CD3D9ShaderServices*>(Driver->getShaderServices());

	resetRSCache();

	CurrentRenderState = RsCache;

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

void CD3D9MaterialRenderServices::setBasicRenderStates( const SMaterial& material, const SMaterial& lastMaterial, bool resetAllRenderstates )
{
	if (resetAllRenderstates)
		resetRSCache();

	// basic d3d material, FFP only
#ifdef FIXPIPELINE
		if ( resetAllRenderstates ||
			lastMaterial.AmbientColor != material.AmbientColor ||
			lastMaterial.DiffuseColor != material.DiffuseColor ||
			lastMaterial.SpecularColor != material.SpecularColor ||
			lastMaterial.EmissiveColor != material.EmissiveColor ||
			lastMaterial.Shininess != material.Shininess)
		{
			D3DMATERIAL9 mat;
			mat.Diffuse = CD3D9Helper::colorfToD3D(material.DiffuseColor);
			mat.Ambient = CD3D9Helper::colorfToD3D(material.AmbientColor);
			mat.Specular = CD3D9Helper::colorfToD3D(material.SpecularColor);
			mat.Emissive = CD3D9Helper::colorfToD3D(material.EmissiveColor);
			mat.Power = material.Shininess;
			pID3DDevice->SetMaterial(&mat);	
		}
#endif
	
	// shiness, specular highlights
#ifdef FIXPIPELINE
	{
		BOOL enable = material.Shininess == 0.0f ? FALSE : TRUE;

		CurrentRenderState.SpecularEnable = enable;
	}
#endif

	// fillmode
	if (resetAllRenderstates || lastMaterial.Wireframe != material.Wireframe)
	{
		DWORD fillMode = material.Wireframe ? D3DFILL_WIREFRAME : D3DFILL_SOLID;

		CurrentRenderState.FillMode = fillMode;
	}

	//shade mode
#ifdef FIXPIPELINE
		if (resetAllRenderstates || lastMaterial.GouraudShading != material.GouraudShading)
		{
			CurrentRenderState.ShadeMode = material.GouraudShading ? D3DSHADE_GOURAUD : D3DSHADE_FLAT;
		}
#endif

	// lighting
#ifdef FIXPIPELINE
		if (resetAllRenderstates || lastMaterial.Lighting != material.Lighting)
		{
			CurrentRenderState.Lighting = material.Lighting ? TRUE : FALSE;
		}
#endif

	// zbuffer
	if (resetAllRenderstates || lastMaterial.ZBuffer != material.ZBuffer)
	{
		CurrentRenderState.ZEnable = material.ZBuffer == ECFN_NEVER ? FALSE : TRUE;
		CurrentRenderState.ZFunc = CD3D9Helper::getD3DCompare((E_COMPARISON_FUNC)material.ZBuffer);
	}

	// zwrite
	if (resetAllRenderstates || lastMaterial.ZWriteEnable != material.ZWriteEnable)
	{
		CurrentRenderState.ZWriteEnable  = material.ZWriteEnable ? TRUE : FALSE;
	}

	// stencil
	if (resetAllRenderstates || (lastMaterial.StencilEnable != material.StencilEnable))
	{
		CurrentRenderState.StencilEnable = material.StencilEnable ? TRUE : FALSE;
	}

	// backface culling
	if (resetAllRenderstates || (lastMaterial.Cull != material.Cull))
	{
		DWORD cullmode;
		switch(material.Cull)
		{
		case ECM_FRONT:
			cullmode = D3DCULL_CW;
			break;
		case ECM_BACK:
			cullmode = D3DCULL_CCW;
			break;
		case ECM_NONE:
		default:
			cullmode = D3DCULL_NONE;
		}

		CurrentRenderState.CullMode = cullmode;
	}

	// fog
#ifdef FIXPIPELINE
		if ( resetAllRenderstates || lastMaterial.FogEnable != material.FogEnable)
		{
			CurrentRenderState.FogEnable = material.FogEnable ? TRUE : FALSE;
		}
#endif
	
	// anti aliasing
	if (Driver->DriverSetting.antialias)
	{
		if (resetAllRenderstates || material.AntiAliasing != lastMaterial.AntiAliasing)
		{
			bool multisample = false;
			bool antialiasline = false;
			switch(material.AntiAliasing)
			{
			case EAAM_SIMPLE:
				multisample = true;
				break;
			case EAAM_LINE_SMOOTH:
				multisample = true;
				antialiasline = true;
				break;
			case EAAM_OFF:
			default:
				break;
			}
			
			CurrentRenderState.MultiSampleAntiAlias = multisample ? TRUE : FALSE;
			CurrentRenderState.AntiAliasedLineEnable = antialiasline ? TRUE : FALSE;
		}
	}

	//texture address mode
	for (u32 st=0; st<MATERIAL_MAX_TEXTURES; ++st)
	{	
		if (resetAllRenderstates || 
			material.TextureLayer[st].TextureWrapU != lastMaterial.TextureLayer[st].TextureWrapU ||
			material.TextureLayer[st].TextureWrapV != lastMaterial.TextureLayer[st].TextureWrapV)
		{
			CurrentRenderState.TextureUnits[st].addressU = CD3D9Helper::getD3DTextureAddress((E_TEXTURE_CLAMP)material.TextureLayer[st].TextureWrapU);
			CurrentRenderState.TextureUnits[st].addressV = CD3D9Helper::getD3DTextureAddress((E_TEXTURE_CLAMP)material.TextureLayer[st].TextureWrapV);
		}

		//transform
#ifdef FIXPIPELINE
		if ( resetAllRenderstates || lastMaterial.TextureLayer[st].UseTextureMatrix != material.TextureLayer[st].UseTextureMatrix)
		{
			if(material.TextureLayer[st].UseTextureMatrix)
			{
				if(!material.VertexShader)
					CurrentRenderState.TextureUnits[st].textureTransformFlags = D3DTTFF_COUNT2;

				Driver->setTransform((E_TRANSFORMATION_STATE)(ETS_TEXTURE_0+st), *material.TextureLayer[st].TextureMatrix);
			}
			else
			{
				CurrentRenderState.TextureUnits[st].textureTransformFlags = D3DTTFF_DISABLE;
			}
		}
#endif
	}

	//shader
	if (resetAllRenderstates || material.VertexShader != lastMaterial.VertexShader)
	{
		ShaderServices->useVertexShader(material.VertexShader);
	}
}

void CD3D9MaterialRenderServices::setOverrideRenderStates( const SOverrideMaterial& overrideMaterial,
	bool resetAllRenderStates )
{
	for (u32 st=0; st<MATERIAL_MAX_TEXTURES; ++st)
	{
		if (resetAllRenderStates || overrideMaterial.TextureFilters[st] != LastOverrideMaterial.TextureFilters[st])
		{
			D3DTEXTUREFILTERTYPE tftMag, tftMin, tftMip;

			// Bilinear, trilinear, and anisotropic filter	
			u32 maxAnisotropy = 1;
			if (overrideMaterial.TextureFilters[st] != ETF_NONE)
			{
				u8 anisotropic = getAnisotropic(overrideMaterial.TextureFilters[st]);
				tftMag = ((Driver->Caps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFANISOTROPIC) &&
					anisotropic) ? D3DTEXF_ANISOTROPIC : D3DTEXF_LINEAR;
				tftMin = ((Driver->Caps.TextureFilterCaps & D3DPTFILTERCAPS_MINFANISOTROPIC) &&
					anisotropic) ? D3DTEXF_ANISOTROPIC : D3DTEXF_LINEAR;
				tftMip = overrideMaterial.TextureFilters[st] > ETF_BILINEAR ? D3DTEXF_LINEAR : D3DTEXF_POINT;

				maxAnisotropy = min_((DWORD)anisotropic, Driver->Caps.MaxAnisotropy);
			}
			else
			{
				tftMag = D3DTEXF_POINT;
				tftMin = D3DTEXF_POINT;
				tftMip = D3DTEXF_NONE;
			}

			CurrentRenderState.TextureUnits[st].magFilter = tftMag;
			CurrentRenderState.TextureUnits[st].minFilter = tftMin;
			CurrentRenderState.TextureUnits[st].mipFilter = tftMip;
			CurrentRenderState.TextureUnits[st].maxAniso = maxAnisotropy;
		}

		if (resetAllRenderStates || overrideMaterial.MipMapLodBias[st] != LastOverrideMaterial.MipMapLodBias[st])
		{
			CurrentRenderState.TextureUnits[st].mipLodBias = (DWORD)overrideMaterial.MipMapLodBias[st];
		}
	}
	LastOverrideMaterial = overrideMaterial;
}

void CD3D9MaterialRenderServices::set2DRenderStates( const S2DBlendParam& blendParam, bool resetAllRenderStates)
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

void CD3D9MaterialRenderServices::set2DPixelShaderMaterialBlock( SRenderStateBlock& block, bool alpha, bool alphaChannel )
{
#ifdef FIXPIPELINE
		block.textureUnits[0].colorOp = ETO_MODULATE;
		block.textureUnits[0].colorArg1 = ETA_TEXTURE;
		block.textureUnits[0].colorArg2 = ETA_DIFFUSE;
		block.textureUnits[0].alphaArg1 = ETA_TEXTURE;
		block.textureUnits[0].alphaArg2 = ETA_DIFFUSE;
		if (alphaChannel)
		{
			if (alpha)
				block.textureUnits[0].alphaOp = ETO_MODULATE;	
			else
				block.textureUnits[0].alphaOp = ETO_ARG1;
		}
		else
		{
			if (alpha)
			{
				block.textureUnits[0].alphaOp = ETO_ARG1;
				block.textureUnits[0].alphaArg1 = ETA_DIFFUSE;
			}
			else
				block.textureUnits[0].alphaOp = ETO_DISABLE;
		}
#else
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
#endif
}

void CD3D9MaterialRenderServices::applyMaterialBlock( const SRenderStateBlock& block, bool resetAllRenderStates )
{
	for (u32 st=0; st<MATERIAL_MAX_TEXTURES; ++st)
	{	
		//color op, alpha op, FFP only
#ifdef FIXPIPELINE
		//color op
		if (resetAllRenderStates || block.textureUnits[st].colorOp != LastMaterialBlock.textureUnits[st].colorOp)
		{
			CurrentRenderState.TextureUnits[st].colorOp = CD3D9Helper::getD3DTextureOp(block.textureUnits[st].colorOp);
		}

		if (resetAllRenderStates || block.textureUnits[st].colorArg1 != LastMaterialBlock.textureUnits[st].colorArg1)
		{
			CurrentRenderState.TextureUnits[st].colorArg1 = CD3D9Helper::getD3DTextureArg(block.textureUnits[st].colorArg1);
		}

		if (resetAllRenderStates || block.textureUnits[st].colorArg2 != LastMaterialBlock.textureUnits[st].colorArg2)
		{
			CurrentRenderState.TextureUnits[st].colorArg2 = CD3D9Helper::getD3DTextureArg(block.textureUnits[st].colorArg2);
		}

		//alpha op
		if (resetAllRenderStates || block.textureUnits[st].alphaOp != LastMaterialBlock.textureUnits[st].alphaOp)
		{
			CurrentRenderState.TextureUnits[st].alphaOp = CD3D9Helper::getD3DTextureOp(block.textureUnits[st].alphaOp);
		}

		if (resetAllRenderStates || block.textureUnits[st].alphaArg1 != LastMaterialBlock.textureUnits[st].alphaArg1)
		{
			CurrentRenderState.TextureUnits[st].alphaArg1 = CD3D9Helper::getD3DTextureArg(block.textureUnits[st].alphaArg1);
		}

		if (resetAllRenderStates || block.textureUnits[st].alphaArg2 != LastMaterialBlock.textureUnits[st].alphaArg2)
		{
			CurrentRenderState.TextureUnits[st].alphaArg2 = CD3D9Helper::getD3DTextureArg(block.textureUnits[st].alphaArg2);
		}
#endif
		
		//tex coord index
		if (resetAllRenderStates || block.textureUnits[st].texcoordIndex != LastMaterialBlock.textureUnits[st].texcoordIndex)
		{
			CurrentRenderState.TextureUnits[st].texcoordIndex = (u32)block.textureUnits[st].texcoordIndex;
		}
	}

	//alpha blend
	if (resetAllRenderStates || block.alphaBlendEnabled != LastMaterialBlock.alphaBlendEnabled)
	{
		CurrentRenderState.AlphaBlendEnable = block.alphaBlendEnabled ? TRUE : FALSE;
	}

	if (resetAllRenderStates || block.blendOp != LastMaterialBlock.blendOp)
	{
		CurrentRenderState.BlendOp= CD3D9Helper::getD3DBlendOp(block.blendOp);
	}

	if (resetAllRenderStates || block.srcBlend != LastMaterialBlock.srcBlend)
	{
		CurrentRenderState.SrcBlend = CD3D9Helper::getD3DBlend(block.srcBlend);
	}

	if (resetAllRenderStates || block.destBlend != LastMaterialBlock.destBlend)
	{
		CurrentRenderState.DestBlend = CD3D9Helper::getD3DBlend(block.destBlend);
	}

	//alpha test
	if (resetAllRenderStates || block.alphaTestEnabled != LastMaterialBlock.alphaTestEnabled)
	{
		CurrentRenderState.AlphaTestEnable = block.alphaTestEnabled ? TRUE : FALSE;
	}

	if (resetAllRenderStates || block.alphaTestFunc != LastMaterialBlock.alphaTestFunc)
	{
		CurrentRenderState.AlphaFunc = CD3D9Helper::getD3DCompare(block.alphaTestFunc);
	}

	if (resetAllRenderStates || block.alphaTestRef != LastMaterialBlock.alphaTestRef)
	{
		CurrentRenderState.AlphaRef = (DWORD)min_(block.alphaTestRef, (u8)255);
	}

	/*
	if (resetAllRenderStates || block.alphaToCoverage != LastMaterialBlock.alphaToCoverage)
	{
		if (Driver->AlphaToCoverageSupport)
		{
			if (Driver->AdapterInfo.vendorID == 0x10DE)			//nvidia
			{
				CurrentRenderState.AdaptivetessY = block.alphaToCoverage ? MAKEFOURCC('A','T','O','C') : D3DFMT_UNKNOWN;
			}
			else if(Driver->AdapterInfo.vendorID == 0x1002)		//ati
			{
				CurrentRenderState.PointSize = block.alphaToCoverage ? MAKEFOURCC('A','2','M','1') : MAKEFOURCC('A','2','M','0');
			}
		}
	}
	*/

	if (resetAllRenderStates|| block.pixelShader != LastMaterialBlock.pixelShader)
	{
		ShaderServices->usePixelShader(block.pixelShader);
	}

	LastMaterialBlock = block;
}

void CD3D9MaterialRenderServices::resetRSCache()
{
#ifdef FIXPIPELINE
	pID3DDevice->GetRenderState(D3DRS_LIGHTING, &RsCache.Lighting);
	pID3DDevice->GetRenderState(D3DRS_SPECULARENABLE, &RsCache.SpecularEnable);
	pID3DDevice->GetRenderState(D3DRS_SHADEMODE, &RsCache.ShadeMode);
	pID3DDevice->GetRenderState(D3DRS_FOGENABLE, &RsCache.FogEnable);
#endif
	pID3DDevice->GetRenderState(D3DRS_ZENABLE, &RsCache.ZEnable);
	pID3DDevice->GetRenderState(D3DRS_ZFUNC, &RsCache.ZFunc);
	pID3DDevice->GetRenderState(D3DRS_FILLMODE, &RsCache.FillMode);
	pID3DDevice->GetRenderState(D3DRS_ZWRITEENABLE, &RsCache.ZWriteEnable);
	pID3DDevice->GetRenderState(D3DRS_STENCILENABLE, &RsCache.StencilEnable);
	pID3DDevice->GetRenderState(D3DRS_CULLMODE, &RsCache.CullMode);
	pID3DDevice->GetRenderState(D3DRS_MULTISAMPLEANTIALIAS, &RsCache.MultiSampleAntiAlias);
	pID3DDevice->GetRenderState(D3DRS_ANTIALIASEDLINEENABLE, &RsCache.AntiAliasedLineEnable);
	pID3DDevice->GetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, &RsCache.SlopeScaleDepthBias);
	pID3DDevice->GetRenderState(D3DRS_DEPTHBIAS, &RsCache.DepthBias);
	pID3DDevice->GetRenderState(D3DRS_ALPHABLENDENABLE, &RsCache.AlphaBlendEnable);
	pID3DDevice->GetRenderState(D3DRS_BLENDOP, &RsCache.BlendOp);
	pID3DDevice->GetRenderState(D3DRS_SRCBLEND, &RsCache.SrcBlend);
	pID3DDevice->GetRenderState(D3DRS_DESTBLEND, &RsCache.DestBlend);
	pID3DDevice->GetRenderState(D3DRS_ALPHATESTENABLE, &RsCache.AlphaTestEnable);
	pID3DDevice->GetRenderState(D3DRS_ALPHAFUNC, &RsCache.AlphaFunc);
	pID3DDevice->GetRenderState(D3DRS_ALPHAREF, &RsCache.AlphaRef);
	pID3DDevice->GetRenderState(D3DRS_ADAPTIVETESS_Y, &RsCache.AdaptivetessY);
	pID3DDevice->GetRenderState(D3DRS_POINTSIZE, &RsCache.PointSize);

	for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
	{
		RsCache.TextureUnits[i].texture = NULL_PTR;

#ifdef FIXPIPELINE		
		pID3DDevice->GetTextureStageState(i, D3DTSS_TEXTURETRANSFORMFLAGS, &RsCache.TextureUnits[i].textureTransformFlags);
		pID3DDevice->GetTextureStageState(i, D3DTSS_COLOROP, &RsCache.TextureUnits[i].colorOp);
		pID3DDevice->GetTextureStageState(i, D3DTSS_COLORARG1, &RsCache.TextureUnits[i].colorArg1);
		pID3DDevice->GetTextureStageState(i, D3DTSS_COLORARG2, &RsCache.TextureUnits[i].colorArg2);
		pID3DDevice->GetTextureStageState(i, D3DTSS_ALPHAOP, &RsCache.TextureUnits[i].alphaOp);
		pID3DDevice->GetTextureStageState(i, D3DTSS_ALPHAARG1, &RsCache.TextureUnits[i].alphaArg1);
		pID3DDevice->GetTextureStageState(i, D3DTSS_ALPHAARG2, &RsCache.TextureUnits[i].alphaArg2);
#endif
		pID3DDevice->GetTextureStageState(i, D3DTSS_TEXCOORDINDEX, &RsCache.TextureUnits[i].texcoordIndex);

		pID3DDevice->GetSamplerState(i, D3DSAMP_ADDRESSU, &RsCache.TextureUnits[i].addressU);
		pID3DDevice->GetSamplerState(i, D3DSAMP_ADDRESSV, &RsCache.TextureUnits[i].addressV);
		pID3DDevice->GetSamplerState(i, D3DSAMP_ADDRESSW, &RsCache.TextureUnits[i].addressW);
		pID3DDevice->GetSamplerState(i, D3DSAMP_MAXANISOTROPY, &RsCache.TextureUnits[i].maxAniso);
		pID3DDevice->GetSamplerState(i, D3DSAMP_MAGFILTER, &RsCache.TextureUnits[i].magFilter);
		pID3DDevice->GetSamplerState(i, D3DSAMP_MINFILTER, &RsCache.TextureUnits[i].minFilter);
		pID3DDevice->GetSamplerState(i, D3DSAMP_MIPFILTER, &RsCache.TextureUnits[i].mipFilter);
		pID3DDevice->GetSamplerState(i, D3DSAMP_MIPMAPLODBIAS, &RsCache.TextureUnits[i].mipLodBias);
	}

}

void CD3D9MaterialRenderServices::setTextureWrap( u32 st, E_TEXTURE_ADDRESS address, E_TEXTURE_CLAMP wrap )
{
	D3DTEXTUREADDRESS v = CD3D9Helper::getD3DTextureAddress(wrap);
	switch (address)
	{
	case ETA_U:
		CurrentRenderState.TextureUnits[st].addressU = v;
		Driver->LastMaterial.TextureLayer[st].TextureWrapU = wrap;
		break;
	case ETA_V:
		CurrentRenderState.TextureUnits[st].addressV = v;
		Driver->LastMaterial.TextureLayer[st].TextureWrapV = wrap;
		break;
	case ETA_W:
		CurrentRenderState.TextureUnits[st].addressW = v;
		Driver->LastMaterial.TextureLayer[st].TextureWrapW = wrap;
		break;
	default:
		ASSERT(false);
	}
}

E_TEXTURE_CLAMP CD3D9MaterialRenderServices::getTextureWrap( u32 st, E_TEXTURE_ADDRESS address ) const
{
	DWORD v = 0;
	switch (address)
	{
	case ETA_U:
		v = CurrentRenderState.TextureUnits[st].addressU;
		break;
	case ETA_V:
		v = CurrentRenderState.TextureUnits[st].addressV;
		break;
	case ETA_W:
		v = CurrentRenderState.TextureUnits[st].addressW;
		break;
	default:
		ASSERT(false);
	}

	return CD3D9Helper::getTextureAddressMode((D3DTEXTUREADDRESS)v);
}

void CD3D9MaterialRenderServices::setDepthBias( f32 depthbias )
{
	bool enable =  depthbias != 0.0f;
	static f32 fSlope = 0.1f;
	CurrentRenderState.SlopeScaleDepthBias = enable ? F32_AS_DWORD(fSlope) : 0;
	CurrentRenderState.DepthBias = enable ? F32_AS_DWORD(depthbias) : 0;
}

f32 CD3D9MaterialRenderServices::getDepthBias() const
{
	return DWORD_AS_F32(CurrentRenderState.DepthBias);
}

void CD3D9MaterialRenderServices::setPixelShaderMaterialBlock( SRenderStateBlock& block, E_PS_TYPE psType, E_VERTEX_TYPE vType )
{
#ifdef FIXPIPELINE
switch(psType)
		{
		case EPST_COMBINERS_MOD:
			{
				//texture stage
				block.textureUnits[0].colorOp = ETO_MODULATE;
				block.textureUnits[0].colorArg1 = ETA_TEXTURE;
				block.textureUnits[0].colorArg2 = ETA_DIFFUSE;
				block.textureUnits[0].alphaOp = ETO_MODULATE;
				block.textureUnits[0].alphaArg1 = ETA_TEXTURE;
				block.textureUnits[0].alphaArg2 = ETA_DIFFUSE;

				block.textureUnits[1].alphaOp = ETO_DISABLE;
				block.textureUnits[1].colorOp = ETO_DISABLE;
			}
			break;
		case EPST_COMBINERS_OPAQUE:
			{
				//texture stage
				block.textureUnits[0].colorOp = ETO_MODULATE;
				block.textureUnits[0].colorArg1 = ETA_TEXTURE;
				block.textureUnits[0].colorArg2 = ETA_DIFFUSE;
				block.textureUnits[0].alphaOp = ETO_ARG1;
				block.textureUnits[0].alphaArg1 = ETA_DIFFUSE;

				block.textureUnits[1].alphaOp = ETO_DISABLE;
				block.textureUnits[1].colorOp = ETO_DISABLE;
			}
			break;
		case EPST_DEFAULT_PC:
			{
				//texture stage
				block.textureUnits[0].colorOp = ETO_ARG1;
				block.textureUnits[0].colorArg1 = ETA_DIFFUSE;
				block.textureUnits[0].alphaOp = ETO_ARG1;
				block.textureUnits[0].alphaArg1 = ETA_DIFFUSE;

				block.textureUnits[1].alphaOp = ETO_DISABLE;
				block.textureUnits[1].colorOp = ETO_DISABLE;
			}
			break;
		case EPST_MAPOBJ_DIFFUSE:
			{
				//texture stage
				block.textureUnits[0].colorOp = ETO_MODULATE;
				block.textureUnits[0].colorArg1 = ETA_TEXTURE;
				block.textureUnits[0].colorArg2 = ETA_DIFFUSE;
				block.textureUnits[0].alphaOp = ETO_MODULATE;
				block.textureUnits[0].alphaArg1 = ETA_TEXTURE;
				block.textureUnits[0].alphaArg2 = ETA_DIFFUSE;

				block.textureUnits[1].alphaOp = ETO_DISABLE;
				block.textureUnits[1].colorOp = ETO_DISABLE;
			}
			break;
		case EPST_MAPOBJ_DIFFUSEEMISSIVE:
			{
				//texture stage
				block.textureUnits[0].colorOp = ETO_MODULATE_X2;
				block.textureUnits[0].colorArg1 = ETA_TEXTURE;
				block.textureUnits[0].colorArg2 = ETA_DIFFUSE;
				block.textureUnits[0].alphaOp = ETO_MODULATE;
				block.textureUnits[0].alphaArg1 = ETA_TEXTURE;
				block.textureUnits[0].alphaArg2 = ETA_DIFFUSE;

				block.textureUnits[1].alphaOp = ETO_DISABLE;
				block.textureUnits[1].colorOp = ETO_DISABLE;
			}
			break;
		case EPST_MAPOBJ_OPAQUE:
			{
				//texture stage
				block.textureUnits[0].colorOp = ETO_ARG1;
				block.textureUnits[0].colorArg1 = ETA_TEXTURE;
				block.textureUnits[0].alphaOp = ETO_ARG1;
				block.textureUnits[0].alphaArg1 = ETA_DIFFUSE;

				block.textureUnits[1].alphaOp = ETO_DISABLE;
				block.textureUnits[1].colorOp = ETO_DISABLE;
			}
			break;
		case EPST_MAPOBJ_SPECULAR:
			{
				//texture stage
				block.textureUnits[0].colorOp = ETO_MODULATE;
				block.textureUnits[0].colorArg1 = ETA_TEXTURE;
				block.textureUnits[0].colorArg2 = ETA_DIFFUSE;
				block.textureUnits[0].alphaOp = ETO_MODULATE;
				block.textureUnits[0].alphaArg1 = ETA_TEXTURE;
				block.textureUnits[0].alphaArg2 = ETA_DIFFUSE;

				block.textureUnits[1].alphaOp = ETO_DISABLE;
				block.textureUnits[1].colorOp = ETO_DISABLE;
			}
			break;
		case EPST_MAPOBJ_METAL:
		case EPST_MAPOBJ_ENVMETAL:
			{
				//texture stage
				block.textureUnits[0].colorOp = ETO_MODULATE_X2;
				block.textureUnits[0].colorArg1 = ETA_TEXTURE;
				block.textureUnits[0].colorArg2 = ETA_DIFFUSE;
				block.textureUnits[0].alphaOp = ETO_MODULATE_X2;
				block.textureUnits[0].alphaArg1 = ETA_TEXTURE;
				block.textureUnits[0].alphaArg2 = ETA_DIFFUSE;

				block.textureUnits[1].alphaOp = ETO_DISABLE;
				block.textureUnits[1].colorOp = ETO_DISABLE;
			}
			break;
		case EPST_MAPOBJ_TWOLAYERDIFFUSE:
			{
				//texture stage
				block.textureUnits[0].colorOp = ETO_MODULATE;
				block.textureUnits[0].colorArg1 = ETA_TEXTURE;
				block.textureUnits[0].colorArg2 = ETA_DIFFUSE;
				block.textureUnits[0].alphaOp = ETO_MODULATE;
				block.textureUnits[0].alphaArg1 = ETA_TEXTURE;
				block.textureUnits[0].alphaArg2 = ETA_DIFFUSE;

				block.textureUnits[0].colorOp = ETO_MODULATE;
				block.textureUnits[0].colorArg1 = ETA_TEXTURE;
				block.textureUnits[0].colorArg2 = ETA_CURRENT;
				block.textureUnits[0].alphaOp = ETO_MODULATE;
				block.textureUnits[0].alphaArg1 = ETA_TEXTURE;
				block.textureUnits[0].alphaArg2 = ETA_CURRENT;
			}
			break;
		default:
			{
				//texture stage
				if (hasTexture(vType))
				{
					block.textureUnits[0].colorOp = ETO_MODULATE;
					block.textureUnits[0].colorArg1 = ETA_TEXTURE;
					block.textureUnits[0].colorArg2 = ETA_DIFFUSE;
					block.textureUnits[0].alphaOp = ETO_MODULATE;
					block.textureUnits[0].alphaArg1 = ETA_TEXTURE;
					block.textureUnits[0].alphaArg2 = ETA_DIFFUSE;
				}
				else
				{
					block.textureUnits[0].colorOp = ETO_ARG1;
					block.textureUnits[0].colorArg1 = ETA_DIFFUSE;
					block.textureUnits[0].alphaOp = ETO_ARG1;
					block.textureUnits[0].alphaArg1 = ETA_DIFFUSE;
				}
			}
			break;
		}
#else
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
#endif

}

void CD3D9MaterialRenderServices::applyMaterialChanges()
{
#ifdef FIXPIPELINE
	DEVICE_SET_RENDER_STATE(SpecularEnable, D3DRS_SPECULARENABLE, CurrentRenderState.SpecularEnable);
	DEVICE_SET_RENDER_STATE(ShadeMode, D3DRS_SHADEMODE, CurrentRenderState.ShadeMode);
	DEVICE_SET_RENDER_STATE(Lighting, D3DRS_LIGHTING, CurrentRenderState.Lighting);
	DEVICE_SET_RENDER_STATE(FogEnable, D3DRS_FOGENABLE, CurrentRenderState.FogEnable);
#endif

	DEVICE_SET_RENDER_STATE(FillMode, D3DRS_FILLMODE, CurrentRenderState.FillMode);
	DEVICE_SET_RENDER_STATE(ZEnable, D3DRS_ZENABLE, CurrentRenderState.ZEnable);
	DEVICE_SET_RENDER_STATE(ZFunc, D3DRS_ZFUNC, CurrentRenderState.ZFunc);
	DEVICE_SET_RENDER_STATE(ZWriteEnable, D3DRS_ZWRITEENABLE, CurrentRenderState.ZWriteEnable);
	DEVICE_SET_RENDER_STATE(StencilEnable, D3DRS_STENCILENABLE, CurrentRenderState.StencilEnable);
	DEVICE_SET_RENDER_STATE(CullMode, D3DRS_CULLMODE, CurrentRenderState.CullMode);

	DEVICE_SET_RENDER_STATE(MultiSampleAntiAlias, D3DRS_MULTISAMPLEANTIALIAS, CurrentRenderState.MultiSampleAntiAlias);
	DEVICE_SET_RENDER_STATE(AntiAliasedLineEnable, D3DRS_ANTIALIASEDLINEENABLE, CurrentRenderState.AntiAliasedLineEnable);

	for (u32 st = 0; st < MATERIAL_MAX_TEXTURES; ++st)
	{
		const SRenderStateCache::STextureUnit& texunit = CurrentRenderState.TextureUnits[st];

		if (RsCache.TextureUnits[st].texture != texunit.texture)
		{
			if (!texunit.texture)
			{
				pID3DDevice->SetTexture(st, NULL_PTR);
				RsCache.TextureUnits[st].texture = NULL_PTR;
				continue;
			}

			const CD3D9Texture* tex = static_cast<const CD3D9Texture*>(texunit.texture);
			pID3DDevice->SetTexture(st, tex->getDXTexture());
			RsCache.TextureUnits[st].texture = texunit.texture;

			ASSERT(RsCache.TextureUnits[st].texture);

			DEVICE_SET_SAMPLER_STATE(st, addressU, D3DSAMP_ADDRESSU, texunit.addressU);
			DEVICE_SET_SAMPLER_STATE(st, addressV, D3DSAMP_ADDRESSV, texunit.addressV);
			DEVICE_SET_SAMPLER_STATE(st, addressW, D3DSAMP_ADDRESSW, texunit.addressW);

	#ifdef FIXPIPELINE
				DEVICE_SET_TEXTURE_STAGE_STATE(st, textureTransformFlags, D3DTSS_TEXTURETRANSFORMFLAGS, texunit.textureTransformFlags);
	#endif

			DEVICE_SET_SAMPLER_STATE(st, magFilter, D3DSAMP_MAGFILTER, texunit.magFilter);
			DEVICE_SET_SAMPLER_STATE(st, minFilter, D3DSAMP_MINFILTER, texunit.minFilter);
			DEVICE_SET_SAMPLER_STATE(st, mipFilter, D3DSAMP_MIPFILTER, texunit.mipFilter);
			DEVICE_SET_SAMPLER_STATE(st, maxAniso, D3DSAMP_MAXANISOTROPY, texunit.maxAniso);
	
			DEVICE_SET_SAMPLER_STATE(st, mipLodBias, D3DSAMP_MIPMAPLODBIAS, texunit.mipLodBias);

			//color op, alpha op, FFP only
	#ifdef FIXPIPELINE
			DEVICE_SET_TEXTURE_STAGE_STATE(st, colorOp, D3DTSS_COLOROP, texunit.colorOp);
			DEVICE_SET_TEXTURE_STAGE_STATE(st, colorArg1, D3DTSS_COLORARG1, texunit.colorArg1);
			DEVICE_SET_TEXTURE_STAGE_STATE(st, colorArg2, D3DTSS_COLORARG2, texunit.colorArg2);
			DEVICE_SET_TEXTURE_STAGE_STATE(st, alphaOp, D3DTSS_ALPHAOP, texunit.alphaOp);
			DEVICE_SET_TEXTURE_STAGE_STATE(st, alphaArg1, D3DTSS_ALPHAARG1, texunit.alphaArg1);
			DEVICE_SET_TEXTURE_STAGE_STATE(st, alphaArg2, D3DTSS_ALPHAARG2, texunit.alphaArg2);
	#endif

			DEVICE_SET_TEXTURE_STAGE_STATE(st, texcoordIndex, D3DTSS_TEXCOORDINDEX, texunit.texcoordIndex);
		}
	}

	DEVICE_SET_RENDER_STATE(AlphaBlendEnable, D3DRS_ALPHABLENDENABLE, CurrentRenderState.AlphaBlendEnable);

	DEVICE_SET_RENDER_STATE(BlendOp, D3DRS_BLENDOP, CurrentRenderState.BlendOp);
	DEVICE_SET_RENDER_STATE(SrcBlend, D3DRS_SRCBLEND, CurrentRenderState.SrcBlend);
	DEVICE_SET_RENDER_STATE(DestBlend, D3DRS_DESTBLEND, CurrentRenderState.DestBlend);

	DEVICE_SET_RENDER_STATE(AlphaTestEnable, D3DRS_ALPHATESTENABLE, CurrentRenderState.AlphaTestEnable);
	DEVICE_SET_RENDER_STATE(AlphaFunc, D3DRS_ALPHAFUNC, CurrentRenderState.AlphaFunc);
	DEVICE_SET_RENDER_STATE(AlphaRef, D3DRS_ALPHAREF, CurrentRenderState.AlphaRef);
	DEVICE_SET_RENDER_STATE(AdaptivetessY, D3DRS_ADAPTIVETESS_Y, CurrentRenderState.AdaptivetessY );
	DEVICE_SET_RENDER_STATE(PointSize, D3DRS_POINTSIZE, CurrentRenderState.PointSize);

	DEVICE_SET_RENDER_STATE(SlopeScaleDepthBias, D3DRS_SLOPESCALEDEPTHBIAS, CurrentRenderState.SlopeScaleDepthBias);
	DEVICE_SET_RENDER_STATE(DepthBias, D3DRS_DEPTHBIAS, CurrentRenderState.DepthBias);

}

#endif

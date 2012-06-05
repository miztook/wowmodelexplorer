#include "stdafx.h"
#include "CD3D9MaterialRenderServices.h"
#include "mywow.h"
#include "CD3D9Driver.h"
#include "CD3D9MaterialRenderer.h"
#include "CD3D9Helper.h"
#include "CD3D9ShaderServices.h"
#include "CD3D9Texture.h"

#define  DEVICE_SET_RENDER_STATE(prop, d3drs, v)	if (RsCache.##prop != v)		\
		{	pID3DDevice->SetRenderState(d3drs, v);				\
		RsCache.##prop = v;	}													
														
#define  DEVICE_SET_SAMPLER_STATE(st, prop, d3dss, v)	if(RsCache.TextureUnits[st].##prop != v)		\
			{	pID3DDevice->SetSamplerState (st, d3dss, v);		\
			RsCache.TextureUnits[st].##prop = v;	}

#define  DEVICE_SET_TEXTURE_STAGE_STATE(st, prop, d3dtss, v)		if(RsCache.TextureUnits[st].##prop != v)		\
			{	pID3DDevice->SetTextureStageState (st, d3dtss, v);			\
			RsCache.TextureUnits[st].##prop = v;	}


CD3D9MaterialRenderer_OneColor		g_MaterialRenderer_OneColor;
CD3D9MaterialRenderer_Solid			g_MaterialRenderer_Solid;
CD3D9MaterialRenderer_LightMap		g_MaterialRenderer_LightMap;
CD3D9MaterialRenderer_DetailMap		g_MaterialRenderer_DetailMap;
CD3D9MaterialRenderer_Transparent_Alpha_Blend		g_MaterialRenderer_Transparent_Alpha_Blend;
CD3D9MaterialRenderer_Transparent_Alpha_Test		g_MaterialRenderer_Transparent_Alpha_Test;

CD3D9MaterialRenderServices::CD3D9MaterialRenderServices(bool ppipeline)
	: PPipeline(ppipeline)
{
	Driver = static_cast<CD3D9Driver*>(g_Engine->getDriver());
	pID3DDevice = Driver->pID3DDevice;

	ResetRSCache();

	MaterialRenderersMap[EMT_ONECOLOR] = &g_MaterialRenderer_OneColor;
	MaterialRenderersMap[EMT_SOLID] = &g_MaterialRenderer_Solid;
	MaterialRenderersMap[EMT_LIGHTMAP] =
		MaterialRenderersMap[EMT_LIGHTMAP_ADD] =
		MaterialRenderersMap[EMT_LIGHTMAP_M2] =
		MaterialRenderersMap[EMT_LIGHTMAP_M4] =
		MaterialRenderersMap[EMT_LIGHTMAP_LIGHTING] =
		MaterialRenderersMap[EMT_LIGHTMAP_LIGHTING_M2] =
		MaterialRenderersMap[EMT_LIGHTMAP_LIGHTING_M4] = &g_MaterialRenderer_LightMap;
	MaterialRenderersMap[EMT_DETAIL_MAP] = &g_MaterialRenderer_DetailMap;

	MaterialRenderersMap[EMT_TRANSPARENT_ADD_ALPHA] =
		MaterialRenderersMap[EMT_TRANSPARENT_ALPHA_BLEND] =
		MaterialRenderersMap[EMT_TRANSPARENT_ADD_COLOR] = &g_MaterialRenderer_Transparent_Alpha_Blend;
	MaterialRenderersMap[EMT_ALPHA_TEST] = &g_MaterialRenderer_Transparent_Alpha_Test;

}

void CD3D9MaterialRenderServices::setBasicRenderStates( const SMaterial& material, const SMaterial& lastMaterial, bool resetAllRenderstates )
{
	if (resetAllRenderstates)
		ResetRSCache();

	//basic d3d material
	if (resetAllRenderstates ||
		lastMaterial.AmbientColor != material.AmbientColor ||
		lastMaterial.DiffuseColor != material.DiffuseColor ||
		lastMaterial.SpecularColor != material.SpecularColor ||
		lastMaterial.EmissiveColor != material.EmissiveColor ||
		!equals_(lastMaterial.Shininess, material.Shininess))
	{
		D3DMATERIAL9 mat;
		mat.Diffuse = CD3D9Helper::colorToD3D(material.DiffuseColor);
		mat.Ambient = CD3D9Helper::colorToD3D(material.AmbientColor);
		mat.Specular = CD3D9Helper::colorToD3D(material.SpecularColor);
		mat.Emissive = CD3D9Helper::colorToD3D(material.EmissiveColor);
		mat.Power = material.Shininess;
		pID3DDevice->SetMaterial(&mat);	
	}

	// shiness, specular highlights
	if (resetAllRenderstates || !equals_(lastMaterial.Shininess, material.Shininess))
	{
		BOOL enable = equals_(material.Shininess, 0.0f) ? FALSE : TRUE;

		DEVICE_SET_RENDER_STATE(SpecularEnable, D3DRS_SPECULARENABLE, enable);
	}

	// fillmode
	if (resetAllRenderstates || lastMaterial.Wireframe != material.Wireframe)
	{
		DWORD fillMode = material.Wireframe ? D3DFILL_WIREFRAME : D3DFILL_SOLID;

		DEVICE_SET_RENDER_STATE(FillMode, D3DRS_FILLMODE, fillMode);
	}

	//shade mode
	if (resetAllRenderstates || lastMaterial.GouraudShading != material.GouraudShading)
	{
		DWORD shademode = material.GouraudShading ? D3DSHADE_GOURAUD : D3DSHADE_FLAT;

		DEVICE_SET_RENDER_STATE(ShadeMode, D3DRS_SHADEMODE, shademode);
	}

	// lighting
	if (resetAllRenderstates || lastMaterial.Lighting != material.Lighting)
	{
		DWORD lighting = material.Lighting ? TRUE : FALSE;

		DEVICE_SET_RENDER_STATE(Lighting, D3DRS_LIGHTING, lighting);
	}

	// zbuffer
	if (resetAllRenderstates || lastMaterial.ZBuffer != material.ZBuffer)
	{
		u32 zEnable = RsCache.ZEnable;
		u32 zFunc = RsCache.ZFunc;

		switch (material.ZBuffer)
		{
		case ECFN_NEVER:
			zEnable = D3DZB_FALSE;
			break;
		case ECFN_LESSEQUAL:
			zEnable = D3DZB_TRUE;
			zFunc = D3DCMP_LESSEQUAL;
			break;
		case ECFN_EQUAL:
			zEnable = D3DZB_TRUE;
			zFunc = D3DCMP_EQUAL;
			break;
		case ECFN_LESS:
			zEnable = D3DZB_TRUE;
			zFunc = D3DCMP_LESS;
			break;
		case ECFN_NOTEQUAL:
			zEnable = D3DZB_TRUE;
			zFunc = D3DCMP_NOTEQUAL;
			break;
		case ECFN_GREATEREQUAL:
			zEnable = D3DZB_TRUE;
			zFunc = D3DCMP_GREATEREQUAL;
			break;
		case ECFN_GREATER:
			zEnable = D3DZB_TRUE;
			zFunc = D3DCMP_GREATER;
			break;
		case ECFN_ALWAYS:
			zEnable = D3DZB_TRUE;
			zFunc = D3DCMP_ALWAYS;
			break;
		}

		DEVICE_SET_RENDER_STATE(ZEnable, D3DRS_ZENABLE, zEnable);
		DEVICE_SET_RENDER_STATE(ZFunc, D3DRS_ZFUNC, zFunc);
	}

	// zwrite
	if (resetAllRenderstates || lastMaterial.ZWriteEnable != material.ZWriteEnable)
	{
		DWORD zwrite = material.ZWriteEnable ? TRUE : FALSE;

		DEVICE_SET_RENDER_STATE(ZWriteEnable, D3DRS_ZWRITEENABLE, zwrite);
	}

	// stencil
	if (resetAllRenderstates || (lastMaterial.StencilEnable != material.StencilEnable))
	{
		DWORD stencil = material.StencilEnable ? TRUE : FALSE;

		DEVICE_SET_RENDER_STATE(StencilEnable, D3DRS_STENCILENABLE, stencil);
	}

	// backface culling
	if (resetAllRenderstates || (lastMaterial.FrontfaceCulling != material.FrontfaceCulling) || (lastMaterial.BackfaceCulling != material.BackfaceCulling))
	{
		DWORD cullmode = material.FrontfaceCulling ? D3DCULL_CW : ( material.BackfaceCulling ? D3DCULL_CCW : D3DCULL_NONE );
		
		DEVICE_SET_RENDER_STATE(CullMode, D3DRS_CULLMODE, cullmode);
	}

	// fog
	if (resetAllRenderstates || lastMaterial.FogEnable != material.FogEnable)
	{
		DWORD fog = material.FogEnable ? TRUE : FALSE;

		DEVICE_SET_RENDER_STATE(FogEnable, D3DRS_FOGENABLE, fog);
	}

	/*
	//alpha to coverage
	if (resetAllRenderstates || material.AlphaToCoverage != lastMaterial.AlphaToCoverage)
	{
		if (Driver->AlphaToCoverageSupport)
		{
			if (Driver->VendorID == 0x10DE)			//nvidia
			{
				DWORD v = material.AlphaToCoverage ? MAKEFOURCC('A','T','O','C') : D3DFMT_UNKNOWN;

				DEVICE_SET_RENDER_STATE(AdaptivetessY, D3DRS_ADAPTIVETESS_Y, v);
			}
			else if(Driver->VendorID == 0x1002)		//ati
			{
				DWORD v = material.AlphaToCoverage ? MAKEFOURCC('A','2','M','1') : MAKEFOURCC('A','2','M','0');

				DEVICE_SET_RENDER_STATE(PointSize, D3DRS_POINTSIZE, v);
			}
		}
	}
	*/

	// anti aliasing
	if (Driver->AntiAliasing)
	{
		if (resetAllRenderstates || material.AntiAliasing != lastMaterial.AntiAliasing)
		{
			DWORD v = material.AntiAliasing ? TRUE : FALSE;

			DEVICE_SET_RENDER_STATE(MultiSampleAntiAlias, D3DRS_MULTISAMPLEANTIALIAS, v);
		}
	}

	//texture address mode
	for (u32 st=0; st<MATERIAL_MAX_TEXTURES; ++st)
	{	
		if (resetAllRenderstates || 
			material.TextureLayer[st].TextureWrapU != lastMaterial.TextureLayer[st].TextureWrapU ||
			material.TextureLayer[st].TextureWrapV != lastMaterial.TextureLayer[st].TextureWrapV)
		{
			DWORD addressU = 0;
			DWORD addressV = 0;

			addressU = CD3D9Helper::getD3DTextureAddress(material.TextureLayer[st].TextureWrapU);
			addressV = CD3D9Helper::getD3DTextureAddress(material.TextureLayer[st].TextureWrapV);

			DEVICE_SET_SAMPLER_STATE(st, addressU, D3DSAMP_ADDRESSU, addressU);
			DEVICE_SET_SAMPLER_STATE(st, addressV, D3DSAMP_ADDRESSV, addressV);
		}

		if (resetAllRenderstates || material.TextureLayer[st].TextureFilter != lastMaterial.TextureLayer[st].TextureFilter)
		{
			D3DTEXTUREFILTERTYPE tftMag, tftMin, tftMip;

			// Bilinear, trilinear, and anisotropic filter	
			if (material.TextureLayer[st].TextureFilter != ETF_NONE)
			{
				u8 anisotropic = getAnisotropic(material.TextureLayer[st].TextureFilter);
				tftMag = ((Driver->Caps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFANISOTROPIC) &&
					anisotropic) ? D3DTEXF_ANISOTROPIC : D3DTEXF_LINEAR;
				tftMin = ((Driver->Caps.TextureFilterCaps & D3DPTFILTERCAPS_MINFANISOTROPIC) &&
					anisotropic) ? D3DTEXF_ANISOTROPIC : D3DTEXF_LINEAR;
				tftMip = material.TextureLayer[st].TextureFilter > ETF_BILINEAR ? D3DTEXF_LINEAR : D3DTEXF_POINT;

				if (tftMag==D3DTEXF_ANISOTROPIC || tftMin == D3DTEXF_ANISOTROPIC)
				{
					u32 v = min_((DWORD)anisotropic, Driver->Caps.MaxAnisotropy);

					for (u32 st=0; st<MATERIAL_MAX_TEXTURES; ++st)
					{
						DEVICE_SET_SAMPLER_STATE(st, maxAniso, D3DSAMP_MAXANISOTROPY, v);
					}
				}
			}
			else
			{
				tftMag = D3DTEXF_POINT;
				tftMin = D3DTEXF_POINT;
				tftMip = D3DTEXF_NONE;
			}

			DEVICE_SET_SAMPLER_STATE(st, magFilter, D3DSAMP_MAGFILTER, tftMag);
			DEVICE_SET_SAMPLER_STATE(st, minFilter, D3DSAMP_MINFILTER, tftMin);
			DEVICE_SET_SAMPLER_STATE(st, mipFilter, D3DSAMP_MIPFILTER, tftMip);

		}

		//transform
		if (resetAllRenderstates || lastMaterial.TextureLayer[st].UseTextureMatrix != material.TextureLayer[st].UseTextureMatrix)
		{
			if(material.TextureLayer[st].UseTextureMatrix)
			{
				if (!material.VertexShader)
				{
					DEVICE_SET_TEXTURE_STAGE_STATE(st, textureTransformFlags, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
					
					Driver->setTransform((E_TRANSFORMATION_STATE)(ETS_TEXTURE_0+st), *material.TextureLayer[st].TextureMatrix);
				}	
			}
			else
			{
				DEVICE_SET_TEXTURE_STAGE_STATE(st, textureTransformFlags, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
			}
		}
	}

	//shader
	if (!material.Effect)
	{
		if (resetAllRenderstates || material.VertexShader != lastMaterial.VertexShader)
		{
			Driver->getShaderServices()->useVertexShader(material.VertexShader);
		}
	}

	if (resetAllRenderstates || material.Effect != lastMaterial.Effect)
	{
		Driver->getShaderServices()->useEffect(material.Effect);
	}
}

void CD3D9MaterialRenderServices::set2DRenderStates( bool alpha, bool texture, bool alphaChannel, bool resetAllRenderStates)
{
	if (!texture)
		return;

	SRenderStateBlock block;

	if (alphaChannel)
	{
		block.textureUnits[0].colorOp = ETO_MODULATE;
		block.textureUnits[0].colorArg1 = ETA_TEXTURE;
		block.textureUnits[0].colorArg2 = ETA_DIFFUSE;
		block.textureUnits[0].alphaArg1 = ETA_TEXTURE;

		if (alpha)
		{
			block.textureUnits[0].alphaOp = ETO_MODULATE;
			block.textureUnits[0].alphaArg2 = ETA_DIFFUSE;
		}
		else
		{
			block.textureUnits[0].alphaOp = ETO_ARG1;
		}

		block.alphaBlendEnabled = true;
		block.srcBlend = EBF_ONE;
		block.destBlend = EBF_ONE_MINUS_SRC_ALPHA;
	} 
	else	//no alpha channel
	{
		block.textureUnits[0].colorOp = ETO_MODULATE;
		block.textureUnits[0].colorArg1 = ETA_TEXTURE;
		block.textureUnits[0].colorArg2 = ETA_DIFFUSE;

		block.textureUnits[0].alphaArg2 = ETA_DIFFUSE;

		if (alpha)
		{
			block.textureUnits[0].alphaOp = ETO_ARG2;
			block.alphaBlendEnabled = true;
			block.srcBlend = EBF_ONE;
			block.destBlend = EBF_ONE_MINUS_SRC_ALPHA;
		}
		else
		{
			block.textureUnits[0].alphaOp = ETO_DISABLE;
			block.alphaBlendEnabled = false;
		}
	}	//alpha channel

	applyMaterialBlock(block, resetAllRenderStates);
}

void CD3D9MaterialRenderServices::applyMaterialBlock( const SRenderStateBlock& block, bool resetAllRenderStates )
{
	for (u32 st=0; st<MATERIAL_MAX_TEXTURES; ++st)
	{	
		//color op
		if (resetAllRenderStates || block.textureUnits[st].colorOp != LastMaterialBlock.textureUnits[st].colorOp)
		{
			DWORD v = CD3D9Helper::getD3DTextureOp(block.textureUnits[st].colorOp);

			DEVICE_SET_TEXTURE_STAGE_STATE(st, colorOp, D3DTSS_COLOROP, v);
		}

		if (resetAllRenderStates || block.textureUnits[st].colorArg1 != LastMaterialBlock.textureUnits[st].colorArg1)
		{
			DWORD v = CD3D9Helper::getD3DTextureArg(block.textureUnits[st].colorArg1);
	
			DEVICE_SET_TEXTURE_STAGE_STATE(st, colorArg1, D3DTSS_COLORARG1, v);
		}

		if (resetAllRenderStates || block.textureUnits[st].colorArg2 != LastMaterialBlock.textureUnits[st].colorArg2)
		{
			DWORD v = CD3D9Helper::getD3DTextureArg(block.textureUnits[st].colorArg2);

			DEVICE_SET_TEXTURE_STAGE_STATE(st, colorArg2, D3DTSS_COLORARG2, v);
		}

		//alpha op
		if (resetAllRenderStates || block.textureUnits[st].alphaOp != LastMaterialBlock.textureUnits[st].alphaOp)
		{
			DWORD v = CD3D9Helper::getD3DTextureOp(block.textureUnits[st].alphaOp);

			DEVICE_SET_TEXTURE_STAGE_STATE(st, alphaOp, D3DTSS_ALPHAOP, v);
		}

		if (resetAllRenderStates || block.textureUnits[st].alphaArg1 != LastMaterialBlock.textureUnits[st].alphaArg1)
		{
			DWORD v = CD3D9Helper::getD3DTextureArg(block.textureUnits[st].alphaArg1);

			DEVICE_SET_TEXTURE_STAGE_STATE(st, alphaArg1, D3DTSS_ALPHAARG1, v);
		}

		if (resetAllRenderStates || block.textureUnits[st].alphaArg2 != LastMaterialBlock.textureUnits[st].alphaArg2)
		{
			DWORD v = CD3D9Helper::getD3DTextureArg(block.textureUnits[st].alphaArg2);

			DEVICE_SET_TEXTURE_STAGE_STATE(st, alphaArg2, D3DTSS_ALPHAARG2, v);
		}

		//tex coord index
		if (resetAllRenderStates || block.textureUnits[st].texcoordIndex != LastMaterialBlock.textureUnits[st].texcoordIndex)
		{
			DWORD v = (u32)block.textureUnits[st].texcoordIndex;

			DEVICE_SET_TEXTURE_STAGE_STATE(st, texcoordIndex, D3DTSS_TEXCOORDINDEX, v);
		}
	}

	//alpha blend
	if (resetAllRenderStates || block.alphaBlendEnabled != LastMaterialBlock.alphaBlendEnabled)
	{
		DWORD v = block.alphaBlendEnabled ? TRUE : FALSE;

		DEVICE_SET_RENDER_STATE(AlphaBlendEnable, D3DRS_ALPHABLENDENABLE, v);
	}

	if (resetAllRenderStates || block.srcBlend != LastMaterialBlock.srcBlend)
	{
		DWORD v= CD3D9Helper::getD3DBlend(block.srcBlend);

		DEVICE_SET_RENDER_STATE(SrcBlend, D3DRS_SRCBLEND, v);
	}

	if (resetAllRenderStates || block.destBlend != LastMaterialBlock.destBlend)
	{
		DWORD v = CD3D9Helper::getD3DBlend(block.destBlend);

		DEVICE_SET_RENDER_STATE(DestBlend, D3DRS_DESTBLEND, v);
	}

	//alpha test
	if (resetAllRenderStates || block.alphaTestEnabled != LastMaterialBlock.alphaTestEnabled)
	{
		DWORD v = block.alphaTestEnabled ? TRUE : FALSE;

		DEVICE_SET_RENDER_STATE(AlphaTestEnable, D3DRS_ALPHATESTENABLE, v);
	}

	if (resetAllRenderStates || block.alphaTestFunc != LastMaterialBlock.alphaTestFunc)
	{
		DWORD v = CD3D9Helper::getD3DCompare(block.alphaTestFunc);

		DEVICE_SET_RENDER_STATE(AlphaFunc, D3DRS_ALPHAFUNC, v);
	}

	if (resetAllRenderStates || block.alphaTestRef != LastMaterialBlock.alphaTestRef)
	{
		DWORD v = (DWORD)min_(block.alphaTestRef, (u8)255);

		DEVICE_SET_RENDER_STATE(AlphaRef, D3DRS_ALPHAREF, v);
	}

	if (resetAllRenderStates|| block.pixelShader != LastMaterialBlock.pixelShader)
	{
		Driver->getShaderServices()->usePixelShader(block.pixelShader);
	}

	LastMaterialBlock = block;
}


void CD3D9MaterialRenderServices::setSampler_Texture( u32 st, ITexture* tex )
{
	if (RsCache.TextureUnits[st].texture != tex)
	{
		pID3DDevice->SetTexture(st, tex ? static_cast<CD3D9Texture*>(tex)->getDXTexture() : NULL);

		RsCache.TextureUnits[st].texture = tex;
	}
}

void CD3D9MaterialRenderServices::ResetRSCache()
{
	pID3DDevice->GetRenderState(D3DRS_LIGHTING, &RsCache.Lighting);
	pID3DDevice->GetRenderState(D3DRS_SPECULARENABLE, &RsCache.SpecularEnable);
	pID3DDevice->GetRenderState(D3DRS_ZENABLE, &RsCache.ZEnable);
	pID3DDevice->GetRenderState(D3DRS_ZFUNC, &RsCache.ZFunc);
	pID3DDevice->GetRenderState(D3DRS_FILLMODE, &RsCache.FillMode);
	pID3DDevice->GetRenderState(D3DRS_SHADEMODE, &RsCache.ShadeMode);
	pID3DDevice->GetRenderState(D3DRS_ZWRITEENABLE, &RsCache.ZWriteEnable);
	pID3DDevice->GetRenderState(D3DRS_STENCILENABLE, &RsCache.StencilEnable);
	pID3DDevice->GetRenderState(D3DRS_CULLMODE, &RsCache.CullMode);
	pID3DDevice->GetRenderState(D3DRS_FOGENABLE, &RsCache.FogEnable);
	pID3DDevice->GetRenderState(D3DRS_MULTISAMPLEANTIALIAS, &RsCache.MultiSampleAntiAlias);
//	pID3DDevice->GetRenderState(D3DRS_ANTIALIASEDLINEENABLE, &RsCache.AntiAliasedLineEnable);
	pID3DDevice->GetRenderState(D3DRS_ALPHABLENDENABLE, &RsCache.AlphaBlendEnable);
	pID3DDevice->GetRenderState(D3DRS_SRCBLEND, &RsCache.SrcBlend);
	pID3DDevice->GetRenderState(D3DRS_DESTBLEND, &RsCache.DestBlend);
	pID3DDevice->GetRenderState(D3DRS_ALPHATESTENABLE, &RsCache.AlphaTestEnable);
	pID3DDevice->GetRenderState(D3DRS_ALPHAFUNC, &RsCache.AlphaFunc);
	pID3DDevice->GetRenderState(D3DRS_ALPHAREF, &RsCache.AlphaRef);
//	pID3DDevice->GetRenderState(D3DRS_ADAPTIVETESS_Y, &RsCache.AdaptivetessY);
//	pID3DDevice->GetRenderState(D3DRS_POINTSIZE, &RsCache.PointSize);

	for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
	{
		RsCache.TextureUnits[i].texture = NULL;

		pID3DDevice->GetTextureStageState(i, D3DTSS_TEXTURETRANSFORMFLAGS, &RsCache.TextureUnits[i].textureTransformFlags);
		pID3DDevice->GetTextureStageState(i, D3DTSS_COLOROP, &RsCache.TextureUnits[i].colorOp);
		pID3DDevice->GetTextureStageState(i, D3DTSS_COLORARG1, &RsCache.TextureUnits[i].colorArg1);
		pID3DDevice->GetTextureStageState(i, D3DTSS_COLORARG2, &RsCache.TextureUnits[i].colorArg2);
		pID3DDevice->GetTextureStageState(i, D3DTSS_ALPHAOP, &RsCache.TextureUnits[i].alphaOp);
		pID3DDevice->GetTextureStageState(i, D3DTSS_ALPHAARG1, &RsCache.TextureUnits[i].alphaArg1);
		pID3DDevice->GetTextureStageState(i, D3DTSS_ALPHAARG2, &RsCache.TextureUnits[i].alphaArg2);
		pID3DDevice->GetTextureStageState(i, D3DTSS_TEXCOORDINDEX, &RsCache.TextureUnits[i].texcoordIndex);

		pID3DDevice->GetSamplerState(i, D3DSAMP_ADDRESSU, &RsCache.TextureUnits[i].addressU);
		pID3DDevice->GetSamplerState(i, D3DSAMP_ADDRESSV, &RsCache.TextureUnits[i].addressV);
		pID3DDevice->GetSamplerState(i, D3DSAMP_ADDRESSW, &RsCache.TextureUnits[i].addressW);
		pID3DDevice->GetSamplerState(i, D3DSAMP_MAXANISOTROPY, &RsCache.TextureUnits[i].maxAniso);
		pID3DDevice->GetSamplerState(i, D3DSAMP_MAGFILTER, &RsCache.TextureUnits[i].magFilter);
		pID3DDevice->GetSamplerState(i, D3DSAMP_MINFILTER, &RsCache.TextureUnits[i].minFilter);
		pID3DDevice->GetSamplerState(i, D3DSAMP_MIPFILTER, &RsCache.TextureUnits[i].mipFilter);
	}

}



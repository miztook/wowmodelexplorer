#include "stdafx.h"
#include "CGLES2MaterialRenderServices.h"
#include "mywow.h"
#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_GLES2

#include "CGLES2Driver.h"
#include "CGLES2Extension.h"
#include "CGLES2MaterialRenderer.h"
#include "CGLES2ShaderServices.h"
#include "CGLES2Helper.h"
#include "CGLES2Texture.h"

#define  DEVICE_SET_LIGHTMODEL_STATE(prop, d3drs, v)	if (RsCache.prop != (v))		\
		{	glLightModeli(d3drs, (v));				\
		ASSERT_GLES2_SUCCESS();			\
		RsCache.prop = (v);	}

#define  DEVICE_SET_POLYGONMODE_STATE(prop, v)	if (RsCache.prop != (v))		\
		{	glPolygonMode(GL_FRONT_AND_BACK, (v));				\
		ASSERT_GLES2_SUCCESS();			\
		RsCache.prop = (v);	}

#define  DEVICE_SET_SHADEMODEL_STATE(prop, v)	if (RsCache.prop != (v))		\
		{	glShadeModel(v);				\
		ASSERT_GLES2_SUCCESS();			\
		RsCache.prop = (v);	}

#define  DEVICE_SET_BOOL_STATE(prop, d3drs, v)	if (RsCache.prop != (v))		\
		{	v != GL_FALSE ? glEnable(d3drs) : glDisable(d3drs);				\
		ASSERT_GLES2_SUCCESS();				\
		RsCache.prop = (v);	}

#define  DEVICE_SET_DEPTHFUNC_STATE(prop, v)	if (RsCache.prop != (v))		\
		{	glDepthFunc(v);				\
		ASSERT_GLES2_SUCCESS();			\
		RsCache.prop = (v);	}

#define  DEVICE_SET_DEPTHMASK_STATE(prop, v)	if (RsCache.prop != (v))		\
		{	glDepthMask(v);				\
		ASSERT_GLES2_SUCCESS();			\
		RsCache.prop = (v);	}

#define  DEVICE_SET_CULLFACE_STATE(prop, v)	if (RsCache.prop != (v))		\
		{	glCullFace(v);				\
		ASSERT_GLES2_SUCCESS();			\
		RsCache.prop = (v);	}

#define  DEVICE_SET_FRONTFACE_STATE(prop, v)	if (RsCache.prop != (v))		\
		{	glFrontFace(v);				\
		ASSERT_GLES2_SUCCESS();			\
		RsCache.prop = (v);	}

#define DEVICE_SET_BLEND_STATE(prop1, prop2, v1, v2) if (RsCache.prop1 != (v1) || RsCache.prop2 != (v2))		\
	{	glBlendFunc(v1, v2); 					\
	ASSERT_GLES2_SUCCESS();		\
	RsCache.prop1 = v1; RsCache.prop2 = v2; }

#define DEVICE_SET_ALPHAFUNC_STATE(prop1, prop2, v1, v2) if (RsCache.prop1 != (v1) || RsCache.prop2 != (v2))		\
	{	glAlphaFunc(v1, v2); 					\
	ASSERT_GLES2_SUCCESS();			\
	RsCache.prop1 = v1; RsCache.prop2 = v2; }

#define  DEVICE_SET_TEXTURE2D_BOOL_STATE(st, prop, d3drs, v)	if (RsCache.TextureUnits[st].prop != (v))		\
		{	v != GL_FALSE ? glEnable(d3drs) : glDisable(d3drs);				\
		ASSERT_GLES2_SUCCESS();				\
		RsCache.TextureUnits[st].prop = (v);	}

#define DEVICE_SET_TEXTURE2D_PARAMETER_I(st, prop, d3drs, v)		\
	{	glTexParameteri(GL_TEXTURE_2D, d3drs, v);				\
	ASSERT_GLES2_SUCCESS();			\
	RsCache.TextureUnits[st].prop = (v);	}

#define DEVICE_SET_TEXTURE_ENV_F(st, prop, d3drs, v)	if (RsCache.TextureUnits[st].prop != (v))		\
	{	glTexEnvf(GL_TEXTURE_ENV, d3drs, v);				\
	ASSERT_GLES2_SUCCESS();			\
	RsCache.TextureUnits[st].prop = (v);	}

#define DEVICE_SET_TEXTURE_ENV_I(st, prop, d3drs, v)	if (RsCache.TextureUnits[st].prop != (v))		\
	{	glTexEnvi(GL_TEXTURE_ENV, d3drs, v);				\
	ASSERT_GLES2_SUCCESS();			\
	RsCache.TextureUnits[st].prop = (v);	}

#define DEVICE_SET_TEXTURE_FILTER_CONTROL_F(st, prop, d3drs, v)	if (RsCache.TextureUnits[st].prop != (v))		\
	{	glTexEnvf(GL_TEXTURE_FILTER_CONTROL_EXT, d3drs, v);				\
	ASSERT_GLES2_SUCCESS();			\
	RsCache.TextureUnits[st].prop = (v);	}

CGLES2MaterialRenderer_Solid			g_MaterialRenderer_Solid;
CGLES2MaterialRenderer_Transparent_Alpha_Blend		g_MaterialRenderer_Transparent_Alpha_Blend;
CGLES2MaterialRenderer_Transparent_Alpha_Test		g_MaterialRenderer_Transparent_Alpha_Test;
CGLES2MaterialRenderer_Terrain_MultiPass		g_MaterialRenderer_Terrain_MultiPass;

CGLES2MaterialRenderServices::CGLES2MaterialRenderServices()
{
	Driver = static_cast<CGLES2Driver*>(g_Engine->getDriver());
	Extension = Driver->getGLExtension();
	ShaderServices = static_cast<CGLES2ShaderServices*>(Driver->getShaderServices());
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

CGLES2MaterialRenderServices::~CGLES2MaterialRenderServices()
{
}

void CGLES2MaterialRenderServices::setBasicRenderStates( const SMaterial& material, const SMaterial& lastMaterial, bool resetAllRenderstates )
{
	if (resetAllRenderstates)
		resetRSCache();

	// basic gl material, FFP only

	// shiness, specular highlights

	// fillmode

	//shade mode

	// lighting

	// zbuffer
	if (resetAllRenderstates || lastMaterial.ZBuffer != material.ZBuffer)
	{
		CurrentRenderState.ZEnable = material.ZBuffer == ECFN_NEVER ? false : true;
		CurrentRenderState.ZFunc = CGLES2Helper::getGLCompare((E_COMPARISON_FUNC)material.ZBuffer);
	}

	// zwrite
	if (resetAllRenderstates || lastMaterial.ZWriteEnable != material.ZWriteEnable)
	{
		CurrentRenderState.ZWriteEnable  = material.ZWriteEnable ? GL_TRUE : GL_FALSE;
	}

	// stencil
	if (resetAllRenderstates || (lastMaterial.StencilEnable != material.StencilEnable))
	{
		CurrentRenderState.StencilEnable = material.StencilEnable ? GL_TRUE : GL_FALSE;
	}

	// backface culling
	if (resetAllRenderstates || (lastMaterial.Cull != material.Cull))
	{
		GLenum cullmode;
		GLenum frontface = GL_CW;
		switch(material.Cull)
		{
		case ECM_FRONT:
			cullmode = GL_FRONT;
			break;
		case ECM_BACK:
			cullmode = GL_BACK;
			break;
		case ECM_NONE:
		default:
			cullmode = GL_FRONT_AND_BACK;
			break;
		}

		CurrentRenderState.FrontFace = frontface;
		CurrentRenderState.CullMode = cullmode;
		CurrentRenderState.CullEnable = material.Cull != ECM_NONE;
	}

	// fog

	//texture address mode
	for (u32 st=0; st<MATERIAL_MAX_TEXTURES; ++st)
	{	

		if (resetAllRenderstates || 
			material.TextureLayer[st].TextureWrapU != lastMaterial.TextureLayer[st].TextureWrapU ||
			material.TextureLayer[st].TextureWrapV != lastMaterial.TextureLayer[st].TextureWrapV)
		{
			CurrentRenderState.TextureUnits[st].addressU = CGLES2Helper::getGLTextureAddress((E_TEXTURE_CLAMP)material.TextureLayer[st].TextureWrapU);
			CurrentRenderState.TextureUnits[st].addressV = CGLES2Helper::getGLTextureAddress((E_TEXTURE_CLAMP)material.TextureLayer[st].TextureWrapV);
		}

		//transform
	}

	//shader
	if (resetAllRenderstates || material.VertexShader != lastMaterial.VertexShader)
	{
		ShaderServices->useVertexShader(material.VertexShader);
	}
}

void CGLES2MaterialRenderServices::setOverrideRenderStates( const SOverrideMaterial& overrideMaterial, bool resetAllRenderStates )
{
	for (u32 st=0; st<MATERIAL_MAX_TEXTURES; ++st)
	{
		if (resetAllRenderStates || overrideMaterial.TextureFilters[st] != LastOverrideMaterial.TextureFilters[st])
		{
			GLint tftMag, tftMin, tftMip;

			// Bilinear, trilinear, and anisotropic filter	
			if (overrideMaterial.TextureFilters[st] != ETF_NONE)
			{
				tftMag = GL_LINEAR;
				tftMin = GL_LINEAR;
				tftMip = overrideMaterial.TextureFilters[st] > ETF_BILINEAR ? GL_LINEAR_MIPMAP_NEAREST : GL_NEAREST_MIPMAP_NEAREST;
			}
			else
			{
				tftMag = GL_NEAREST;
				tftMin = GL_NEAREST;
				tftMip = GL_NEAREST_MIPMAP_NEAREST;
			}

			CurrentRenderState.TextureUnits[st].magFilter = tftMag;
			CurrentRenderState.TextureUnits[st].minFilter = tftMin;
			CurrentRenderState.TextureUnits[st].mipFilter = tftMip;
		}
	}
	LastOverrideMaterial = overrideMaterial;
}

void CGLES2MaterialRenderServices::set2DRenderStates( const S2DBlendParam& blendParam, bool resetAllRenderStates )
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

void CGLES2MaterialRenderServices::applyMaterialBlock( const SRenderStateBlock& block, bool resetAllRenderStates )
{
	//alpha blend
	if (resetAllRenderStates || block.alphaBlendEnabled != LastMaterialBlock.alphaBlendEnabled)
	{
		CurrentRenderState.AlphaBlendEnable = block.alphaBlendEnabled ? GL_TRUE : GL_FALSE;
	}

	if (resetAllRenderStates || block.srcBlend != LastMaterialBlock.srcBlend)
	{
		CurrentRenderState.SrcBlend = CGLES2Helper::getGLBlend(block.srcBlend);
	}

	if (resetAllRenderStates || block.destBlend != LastMaterialBlock.destBlend)
	{
		CurrentRenderState.DestBlend = CGLES2Helper::getGLBlend(block.destBlend);
	}

	if (resetAllRenderStates || block.alphaToCoverage != LastMaterialBlock.alphaToCoverage)
	{
		CurrentRenderState.AlphaToCoverage = block.alphaToCoverage;
	}

	if (resetAllRenderStates|| block.pixelShader != LastMaterialBlock.pixelShader)
	{
		ShaderServices->usePixelShader(block.pixelShader);
	}

	LastMaterialBlock = block;
}

void CGLES2MaterialRenderServices::setPixelShaderMaterialBlock( SRenderStateBlock& block, E_PS_TYPE psType, E_VERTEX_TYPE vType )
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

void CGLES2MaterialRenderServices::applyMaterialChanges()
{
	DEVICE_SET_BOOL_STATE(ZEnable, GL_DEPTH_TEST, CurrentRenderState.ZEnable);
	if (CurrentRenderState.ZEnable)
	{
		DEVICE_SET_DEPTHFUNC_STATE(ZFunc, CurrentRenderState.ZFunc);
	}
	DEVICE_SET_DEPTHMASK_STATE(ZWriteEnable, CurrentRenderState.ZWriteEnable);
	
	DEVICE_SET_BOOL_STATE(StencilEnable, GL_STENCIL_TEST, CurrentRenderState.StencilEnable);
	DEVICE_SET_BOOL_STATE(CullEnable, GL_CULL_FACE, CurrentRenderState.CullEnable);

	if (CurrentRenderState.CullEnable)
	{
		DEVICE_SET_CULLFACE_STATE(CullMode, CurrentRenderState.CullMode);
	}

	DEVICE_SET_FRONTFACE_STATE(FrontFace, CurrentRenderState.FrontFace);
	DEVICE_SET_BOOL_STATE(AlphaBlendEnable, GL_BLEND, CurrentRenderState.AlphaBlendEnable);

	if (CurrentRenderState.AlphaBlendEnable)
	{
		DEVICE_SET_BLEND_STATE(SrcBlend, DestBlend, CurrentRenderState.SrcBlend, CurrentRenderState.DestBlend);
	}

	DEVICE_SET_BOOL_STATE(AlphaToCoverage, GL_SAMPLE_ALPHA_TO_COVERAGE, CurrentRenderState.AlphaToCoverage);

	//textures
	u32 samplerCount = ShaderServices->getSamplerCount();

	for (u32 st = 0; st < samplerCount; ++st)
	{
		const SRenderStateCache::STextureUnit& texunit = CurrentRenderState.TextureUnits[st];

		if (RsCache.TextureUnits[st].texture != texunit.texture)
		{		
			setActiveTexture(st);

			if (!texunit.texture)
			{
				glBindTexture(GL_TEXTURE_2D, 0);
				ASSERT_GLES2_SUCCESS();
				RsCache.TextureUnits[st].texture = NULL;
				continue;
			}
			
			CGLES2Texture* gltex = static_cast<CGLES2Texture*>(texunit.texture);
			glBindTexture(GL_TEXTURE_2D, gltex->getGLTexture());
			ASSERT_GLES2_SUCCESS();
			RsCache.TextureUnits[st].texture = texunit.texture;

			setTextureEnable(st, true);

			DEVICE_SET_TEXTURE2D_PARAMETER_I(st, addressU, GL_TEXTURE_WRAP_S, texunit.addressU);

			DEVICE_SET_TEXTURE2D_PARAMETER_I(st, addressV, GL_TEXTURE_WRAP_T, texunit.addressV);

			DEVICE_SET_TEXTURE2D_PARAMETER_I(st, magFilter, GL_TEXTURE_MAG_FILTER, texunit.magFilter);

			if (texunit.texture->hasMipMaps())
			{
				DEVICE_SET_TEXTURE2D_PARAMETER_I(st, mipFilter, GL_TEXTURE_MIN_FILTER, texunit.mipFilter);
			}
			else
			{
				DEVICE_SET_TEXTURE2D_PARAMETER_I(st, minFilter, GL_TEXTURE_MIN_FILTER, texunit.minFilter);
			}
		}
	}

}

void CGLES2MaterialRenderServices::resetRSCache()
{
	RsCache.ZEnable = glIsEnabled(GL_DEPTH_TEST);
	glGetIntegerv(GL_DEPTH_FUNC, &RsCache.ZFunc);
	GLboolean bv;
	glGetBooleanv(GL_DEPTH_WRITEMASK, &bv);
	RsCache.ZWriteEnable = bv;
	RsCache.StencilEnable = glIsEnabled(GL_STENCIL_TEST);
	RsCache.CullEnable = glIsEnabled(GL_CULL_FACE);
	glGetIntegerv(GL_CULL_FACE_MODE, &RsCache.CullMode);
	glGetIntegerv(GL_FRONT_FACE, &RsCache.FrontFace);
	RsCache.AlphaBlendEnable = glIsEnabled(GL_BLEND);
	glGetIntegerv(GL_BLEND_SRC_RGB, &RsCache.SrcBlend);
	glGetIntegerv(GL_BLEND_DST_RGB, &RsCache.DestBlend);

	RsCache.AlphaToCoverage = glIsEnabled(GL_SAMPLE_ALPHA_TO_COVERAGE);

	//texture op
	for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
	{
		RsCache.TextureUnits[i].texture = NULL;
	}

	glGetIntegerv(GL_ACTIVE_TEXTURE, &RsCache.ActiveTextureIndex);

	for (u32 idx=0; idx<MATERIAL_MAX_TEXTURES; ++idx)
	{
		setActiveTexture(idx);

		glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &RsCache.TextureUnits[idx].minFilter);
		glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &RsCache.TextureUnits[idx].magFilter);
		glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &RsCache.TextureUnits[idx].addressU);
		glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, &RsCache.TextureUnits[idx].addressV);
	}

}

void CGLES2MaterialRenderServices::set2DPixelShaderMaterialBlock( SRenderStateBlock& block, bool alpha, bool alphaChannel )
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

void CGLES2MaterialRenderServices::setTextureWrap( u32 st, E_TEXTURE_ADDRESS address, E_TEXTURE_CLAMP wrap )
{
	GLint v = CGLES2Helper::getGLTextureAddress(wrap);
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
		ASSERT(false);
		Driver->LastMaterial.TextureLayer[st].TextureWrapW = wrap;
		break;
	default:
		ASSERT(false);
	}
}

void CGLES2MaterialRenderServices::applyTextureFilter( u32 st, E_TEXTURE_FILTER filter )
{
	GLint tftMag, tftMin, tftMip;

	// Bilinear, trilinear, and anisotropic filter	
	if (filter != ETF_NONE)
	{
		tftMag = GL_LINEAR;
		tftMin = GL_LINEAR;
		tftMip = filter > ETF_BILINEAR ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR_MIPMAP_NEAREST;
	}
	else
	{
		tftMag = GL_NEAREST;
		tftMin = GL_NEAREST;
		tftMip = GL_NEAREST_MIPMAP_NEAREST;
	}

	CurrentRenderState.TextureUnits[st].magFilter = tftMag;
	CurrentRenderState.TextureUnits[st].minFilter = tftMin;
	CurrentRenderState.TextureUnits[st].mipFilter = tftMip;

	DEVICE_SET_TEXTURE2D_PARAMETER_I(st, magFilter, GL_TEXTURE_MAG_FILTER, tftMag);

	if (RsCache.TextureUnits[st].texture && RsCache.TextureUnits[st].texture->hasMipMaps())
	{
		DEVICE_SET_TEXTURE2D_PARAMETER_I(st, mipFilter, GL_TEXTURE_MIN_FILTER, tftMip);
	}
	else
	{
		DEVICE_SET_TEXTURE2D_PARAMETER_I(st, minFilter, GL_TEXTURE_MIN_FILTER, tftMin);
	}

	LastOverrideMaterial.TextureFilters[st] = filter;
}

E_TEXTURE_CLAMP CGLES2MaterialRenderServices::getTextureWrap( u32 st, E_TEXTURE_ADDRESS address ) const
{
	GLint v = 0;
	switch (address)
	{
	case ETA_U:
		v = CurrentRenderState.TextureUnits[st].addressU;
		break;
	case ETA_V:
		v = CurrentRenderState.TextureUnits[st].addressV;
		break;
	case ETA_W:
		ASSERT(false);
		break;
	default:
		ASSERT(false);
	}

	return CGLES2Helper::getTextureAddressMode(v);
}

void CGLES2MaterialRenderServices::setZWriteEnable( bool enable )
{
	CurrentRenderState.ZWriteEnable = enable ? GL_TRUE : GL_FALSE;
	DEVICE_SET_DEPTHMASK_STATE(ZWriteEnable, CurrentRenderState.ZWriteEnable);
	Driver->LastMaterial.ZWriteEnable = enable;
}

bool CGLES2MaterialRenderServices::getZWriteEnable() const
{
	return CurrentRenderState.ZWriteEnable != GL_FALSE;
}

void CGLES2MaterialRenderServices::setActiveTexture( u32 st )
{
	if (Extension->MaxTextureUnits > 1)
	{
		GLenum v = GL_TEXTURE0 + st;

		if (RsCache.ActiveTextureIndex != (v))		
		{	
			Extension->extGlActiveTexture(v);						
			RsCache.ActiveTextureIndex = (v);
		}
	}
}

void CGLES2MaterialRenderServices::setTextureEnable( u32 st, bool enable )
{
}

void CGLES2MaterialRenderServices::applyTextureMipMap( u32 st, bool mipmap )
{
}

void CGLES2MaterialRenderServices::applySamplerTexture( u32 st, ITexture* tex )
{
	setActiveTexture(st);

	CGLES2Texture* gltex = static_cast<CGLES2Texture*>(tex);
	glBindTexture(GL_TEXTURE_2D, gltex ? gltex->getGLTexture() : 0);
	ASSERT_GLES2_SUCCESS();
	RsCache.TextureUnits[st].texture = tex;
}

void CGLES2MaterialRenderServices::applyTextureWrap( u32 st, E_TEXTURE_ADDRESS address, E_TEXTURE_CLAMP wrap )
{
	ASSERT(RsCache.TextureUnits[st].texture);

	GLint v = CGLES2Helper::getGLTextureAddress(wrap);
	switch (address)
	{
	case ETA_U:
		DEVICE_SET_TEXTURE2D_PARAMETER_I(st, addressU, GL_TEXTURE_WRAP_S,v);
		break;
	case ETA_V:
		DEVICE_SET_TEXTURE2D_PARAMETER_I(st, addressV, GL_TEXTURE_WRAP_T, v);
		break;
	case ETA_W:
		ASSERT(false);
		break;
	default:
		ASSERT(false);
	}
}

#endif
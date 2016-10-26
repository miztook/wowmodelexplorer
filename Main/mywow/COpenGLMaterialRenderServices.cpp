#include "stdafx.h"
#include "COpenGLMaterialRenderServices.h"
#include "mywow.h"

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_OPENGL

#include "COpenGLDriver.h"
#include "COpenGLExtension.h"
#include "COpenGLMaterialRenderer.h"
#include "COpenGLShaderServices.h"
#include "COpenGLHelper.h"
#include "COpenGLTexture.h"

#define  DEVICE_SET_LIGHTMODEL_STATE(prop, d3drs, v)	if (RsCache.prop != (v))		\
		{	glLightModeli(d3drs, (v));				\
		ASSERT_OPENGL_SUCCESS();			\
		RsCache.prop = (v);	}	

#define  DEVICE_SET_POLYGONMODE_STATE(prop, v)	if (RsCache.prop != (v))		\
		{	glPolygonMode(GL_FRONT_AND_BACK, (v));				\
		ASSERT_OPENGL_SUCCESS();			\
		RsCache.prop = (v);	}

#define  DEVICE_SET_SHADEMODEL_STATE(prop, v)	if (RsCache.prop != (v))		\
		{	glShadeModel(v);				\
		ASSERT_OPENGL_SUCCESS();			\
		RsCache.prop = (v);	}

#define  DEVICE_SET_BOOL_STATE(prop, d3drs, v)	if (RsCache.prop != (v))		\
		{	v != GL_FALSE ? glEnable(d3drs) : glDisable(d3drs);				\
		ASSERT_OPENGL_SUCCESS();				\
		RsCache.prop = (v);	}	

#define  DEVICE_SET_DEPTHFUNC_STATE(prop, v)	if (RsCache.prop != (v))		\
		{	glDepthFunc(v);				\
		ASSERT_OPENGL_SUCCESS();			\
		RsCache.prop = (v);	}

#define  DEVICE_SET_DEPTHMASK_STATE(prop, v)	if (RsCache.prop != (v))		\
		{	glDepthMask(v);				\
		ASSERT_OPENGL_SUCCESS();			\
		RsCache.prop = (v);	}	

#define  DEVICE_SET_CULLFACE_STATE(prop, v)	if (RsCache.prop != (v))		\
		{	glCullFace(v);				\
		ASSERT_OPENGL_SUCCESS();			\
		RsCache.prop = (v);	}

#define  DEVICE_SET_FRONTFACE_STATE(prop, v)	if (RsCache.prop != (v))		\
		{	glFrontFace(v);				\
		ASSERT_OPENGL_SUCCESS();			\
		RsCache.prop = (v);	}

#define DEVICE_SET_BLEND_STATE(prop1, prop2, v1, v2) if (RsCache.prop1 != (v1) || RsCache.prop2 != (v2))		\
	{	glBlendFunc(v1, v2); 					\
	ASSERT_OPENGL_SUCCESS();		\
	RsCache.prop1 = v1; RsCache.prop2 = v2; }

#define DEVICE_SET_ALPHAFUNC_STATE(prop1, prop2, v1, v2) if (RsCache.prop1 != (v1) || RsCache.prop2 != (v2))		\
	{	glAlphaFunc(v1, v2); 					\
	ASSERT_OPENGL_SUCCESS();			\
	RsCache.prop1 = v1; RsCache.prop2 = v2; }

#define  DEVICE_SET_TEXTURE2D_BOOL_STATE(st, prop, d3drs, v)	if (RsCache.TextureUnits[st].prop != (v))		\
		{	v != GL_FALSE ? glEnable(d3drs) : glDisable(d3drs);				\
		ASSERT_OPENGL_SUCCESS();				\
		RsCache.TextureUnits[st].prop = (v);	}	

#define DEVICE_SET_TEXTURE2D_PARAMETER_I(st, prop, d3drs, v)		\
	{	glTexParameteri(GL_TEXTURE_2D, d3drs, v);				\
	ASSERT_OPENGL_SUCCESS();			\
	RsCache.TextureUnits[st].prop = (v);	}

#define DEVICE_SET_TEXTURE_ENV_F(st, prop, d3drs, v)	if (RsCache.TextureUnits[st].prop != (v))		\
	{	glTexEnvf(GL_TEXTURE_ENV, d3drs, v);				\
	ASSERT_OPENGL_SUCCESS();			\
	RsCache.TextureUnits[st].prop = (v);	}

#define DEVICE_SET_TEXTURE_ENV_I(st, prop, d3drs, v)	if (RsCache.TextureUnits[st].prop != (v))		\
	{	glTexEnvi(GL_TEXTURE_ENV, d3drs, v);				\
	ASSERT_OPENGL_SUCCESS();			\
	RsCache.TextureUnits[st].prop = (v);	}

#define DEVICE_SET_TEXTURE_FILTER_CONTROL_F(st, prop, d3drs, v)	if (RsCache.TextureUnits[st].prop != (v))		\
	{	glTexEnvf(GL_TEXTURE_FILTER_CONTROL_EXT, d3drs, v);				\
	ASSERT_OPENGL_SUCCESS();			\
	RsCache.TextureUnits[st].prop = (v);	}

COpenGLMaterialRenderer_Solid			g_MaterialRenderer_Solid;
COpenGLMaterialRenderer_Transparent_Alpha_Blend		g_MaterialRenderer_Transparent_Alpha_Blend;
COpenGLMaterialRenderer_Transparent_Alpha_Test		g_MaterialRenderer_Transparent_Alpha_Test;
COpenGLMaterialRenderer_Terrain_MultiPass		g_MaterialRenderer_Terrain_MultiPass;

COpenGLMaterialRenderServices::COpenGLMaterialRenderServices()
{
	Driver = static_cast<COpenGLDriver*>(g_Engine->getDriver());
	Extension = Driver->getGLExtension();
	ShaderServices = static_cast<COpenGLShaderServices*>(Driver->getShaderServices());
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

COpenGLMaterialRenderServices::~COpenGLMaterialRenderServices()
{
}

void COpenGLMaterialRenderServices::setBasicRenderStates( const SMaterial& material, const SMaterial& lastMaterial, bool resetAllRenderstates )
{
	if (resetAllRenderstates)
		resetRSCache();

	// basic gl material, FFP only
#ifdef FIXPIPELINE
	if ( resetAllRenderstates || lastMaterial.AmbientColor != material.AmbientColor )
	{
		GLfloat color[4];
		COpenGLHelper::colorfToGLfloat4(material.AmbientColor, color);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, color);
		ASSERT_OPENGL_SUCCESS();
	}	
	if ( resetAllRenderstates || lastMaterial.DiffuseColor != material.DiffuseColor ) 
	{
		GLfloat color[4];
		COpenGLHelper::colorfToGLfloat4(material.DiffuseColor, color);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color);
		ASSERT_OPENGL_SUCCESS();
	}
	if ( resetAllRenderstates || lastMaterial.SpecularColor != material.SpecularColor )
	{
		GLfloat color[4];
		COpenGLHelper::colorfToGLfloat4(material.SpecularColor, color);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, color);
		ASSERT_OPENGL_SUCCESS();
	}
	if ( resetAllRenderstates || lastMaterial.EmissiveColor != material.EmissiveColor )
	{
		GLfloat color[4];
		COpenGLHelper::colorfToGLfloat4(material.EmissiveColor, color);
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, color);
		ASSERT_OPENGL_SUCCESS();
	}
	if ( resetAllRenderstates || lastMaterial.Shininess != material.Shininess)
	{
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material.Shininess);
		ASSERT_OPENGL_SUCCESS();
	}
#endif

	// shiness, specular highlights
#ifdef FIXPIPELINE
	{
		GLint colorControl = GL_SINGLE_COLOR;

#ifdef GL_EXT_separate_specular_color
		if (Extension->queryOpenGLFeature(IRR_EXT_separate_specular_color))
		{
			if (material.Shininess != 0.0f)
				colorControl = GL_SEPARATE_SPECULAR_COLOR;
		}
#endif

		CurrentRenderState.ModelColorControl = colorControl;
	}

#endif

	// fillmode
#ifdef FIXPIPELINE
	if (resetAllRenderstates || lastMaterial.Wireframe != material.Wireframe)
	{
		GLenum fillMode = material.Wireframe ? GL_LINE : GL_FILL;
		CurrentRenderState.FillMode = fillMode;
	}
#endif

	//shade mode
#ifdef FIXPIPELINE
	if (resetAllRenderstates || lastMaterial.GouraudShading != material.GouraudShading)
	{
		CurrentRenderState.ShadeMode = material.GouraudShading ? GL_SMOOTH : GL_FLAT;
	}
#endif

	// lighting
#ifdef FIXPIPELINE
	if (resetAllRenderstates || lastMaterial.Lighting != material.Lighting)
	{
		CurrentRenderState.Lighting = material.Lighting ? GL_TRUE : GL_FALSE;
	}
#endif

	// zbuffer
	if (resetAllRenderstates || lastMaterial.ZBuffer != material.ZBuffer)
	{
		CurrentRenderState.ZEnable = material.ZBuffer == ECFN_NEVER ? GL_FALSE : GL_TRUE;
		CurrentRenderState.ZFunc = COpenGLHelper::getGLCompare((E_COMPARISON_FUNC)material.ZBuffer);
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
#ifdef FIXPIPELINE
	if ( resetAllRenderstates || lastMaterial.FogEnable != material.FogEnable)
	{
		CurrentRenderState.FogEnable = material.FogEnable ? GL_TRUE : GL_FALSE;
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

			CurrentRenderState.MultiSampleAntiAlias = multisample ? GL_TRUE : GL_FALSE;
			CurrentRenderState.AntiAliasedLineEnable = antialiasline ? GL_TRUE : GL_FALSE;
		}
	}

	//texture address mode
	for (u32 st=0; st<MATERIAL_MAX_TEXTURES; ++st)
	{	

		if (resetAllRenderstates || 
			material.TextureLayer[st].TextureWrapU != lastMaterial.TextureLayer[st].TextureWrapU ||
			material.TextureLayer[st].TextureWrapV != lastMaterial.TextureLayer[st].TextureWrapV)
		{
			CurrentRenderState.TextureUnits[st].addressU = COpenGLHelper::getGLTextureAddress((E_TEXTURE_CLAMP)material.TextureLayer[st].TextureWrapU);
			CurrentRenderState.TextureUnits[st].addressV = COpenGLHelper::getGLTextureAddress((E_TEXTURE_CLAMP)material.TextureLayer[st].TextureWrapV);
		}

		//transform
#ifdef FIXPIPELINE
		if ( resetAllRenderstates || lastMaterial.TextureLayer[st].UseTextureMatrix != material.TextureLayer[st].UseTextureMatrix)
		{
			if(material.TextureLayer[st].UseTextureMatrix)
			{
				Driver->setTransform((E_TRANSFORMATION_STATE)(ETS_TEXTURE_0+st), *material.TextureLayer[st].TextureMatrix);
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

void COpenGLMaterialRenderServices::setOverrideRenderStates( const SOverrideMaterial& overrideMaterial, bool resetAllRenderStates )
{
	for (u32 st=0; st<MATERIAL_MAX_TEXTURES; ++st)
	{
		if (resetAllRenderStates || overrideMaterial.TextureFilters[st] != LastOverrideMaterial.TextureFilters[st])
		{
			GLint tftMag, tftMin, tftMip;

			// Bilinear, trilinear, and anisotropic filter	
			GLint maxAnisotropy = 1;
			if (overrideMaterial.TextureFilters[st] != ETF_NONE)
			{
				u8 anisotropic = getAnisotropic(overrideMaterial.TextureFilters[st]);
				tftMag = GL_LINEAR;
				tftMin = GL_LINEAR;
				tftMip = overrideMaterial.TextureFilters[st] > ETF_BILINEAR ? GL_LINEAR_MIPMAP_NEAREST : GL_NEAREST_MIPMAP_NEAREST;

				if (Extension->queryOpenGLFeature(IRR_EXT_texture_filter_anisotropic))
					maxAnisotropy = min_(anisotropic, Extension->MaxAnisotropy);
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
			CurrentRenderState.TextureUnits[st].maxAniso = maxAnisotropy;
		}

#ifdef FIXPIPELINE
		if (resetAllRenderStates || overrideMaterial.MipMapLodBias[st] != LastOverrideMaterial.MipMapLodBias[st])
		{
			if (Extension->queryOpenGLFeature(IRR_EXT_texture_lod_bias))
			{
				if (overrideMaterial.MipMapLodBias[st])
					CurrentRenderState.TextureUnits[st].mipLodBias  = clamp_(overrideMaterial.MipMapLodBias[st] * 0.125f, - Extension->MaxTextureLODBias, Extension->MaxTextureLODBias);
				else
					CurrentRenderState.TextureUnits[st].mipLodBias = 0.0f;
			}
		}
#endif
	}
	LastOverrideMaterial = overrideMaterial;
}

void COpenGLMaterialRenderServices::set2DRenderStates( const S2DBlendParam& blendParam, bool resetAllRenderStates )
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

void COpenGLMaterialRenderServices::applyMaterialBlock( const SRenderStateBlock& block, bool resetAllRenderStates )
{

	for (u32 st=0; st<MATERIAL_MAX_TEXTURES; ++st)
	{	
		//color op, alpha op, FFP only
#ifdef FIXPIPELINE

		CurrentRenderState.TextureUnits[st].envMode = COpenGLHelper::getGLTextureEnvMode(block.textureUnits[st].colorOp, block.textureUnits[st].alphaOp);

		//color op
		if (resetAllRenderStates || block.textureUnits[st].colorOp != LastMaterialBlock.textureUnits[st].colorOp)
		{
			CurrentRenderState.TextureUnits[st].colorOp = (GLfloat)COpenGLHelper::getGLTextureOp(block.textureUnits[st].colorOp, CurrentRenderState.TextureUnits[st].colorScale);
		}

		if (resetAllRenderStates || block.textureUnits[st].colorArg1 != LastMaterialBlock.textureUnits[st].colorArg1)
		{
			CurrentRenderState.TextureUnits[st].colorArg1 = (GLfloat)COpenGLHelper::getGLTextureArg(block.textureUnits[st].colorArg1);
		}

		if (resetAllRenderStates || block.textureUnits[st].colorArg2 != LastMaterialBlock.textureUnits[st].colorArg2)
		{
			CurrentRenderState.TextureUnits[st].colorArg2 = (GLfloat)COpenGLHelper::getGLTextureArg(block.textureUnits[st].colorArg2);
		}

		//alpha op
		if (resetAllRenderStates || block.textureUnits[st].alphaOp != LastMaterialBlock.textureUnits[st].alphaOp)
		{
			CurrentRenderState.TextureUnits[st].alphaOp = (GLfloat)COpenGLHelper::getGLTextureOp(block.textureUnits[st].alphaOp, CurrentRenderState.TextureUnits[st].alphaScale);
		}

		if (resetAllRenderStates || block.textureUnits[st].alphaArg1 != LastMaterialBlock.textureUnits[st].alphaArg1)
		{
			CurrentRenderState.TextureUnits[st].alphaArg1 = (GLfloat)COpenGLHelper::getGLTextureArg(block.textureUnits[st].alphaArg1);
		}

		if (resetAllRenderStates || block.textureUnits[st].alphaArg2 != LastMaterialBlock.textureUnits[st].alphaArg2)
		{
			CurrentRenderState.TextureUnits[st].alphaArg2 = (GLfloat)COpenGLHelper::getGLTextureArg(block.textureUnits[st].alphaArg2);
		}
#endif
	}

	//alpha blend
	if (resetAllRenderStates || block.alphaBlendEnabled != LastMaterialBlock.alphaBlendEnabled)
	{
		CurrentRenderState.AlphaBlendEnable = block.alphaBlendEnabled ? GL_TRUE : GL_FALSE;
	}

	if (resetAllRenderStates || block.srcBlend != LastMaterialBlock.srcBlend)
	{
		CurrentRenderState.SrcBlend = COpenGLHelper::getGLBlend(block.srcBlend);
	}

	if (resetAllRenderStates || block.destBlend != LastMaterialBlock.destBlend)
	{
		CurrentRenderState.DestBlend = COpenGLHelper::getGLBlend(block.destBlend);
	}

#ifdef FIXPIPELINE
	//alpha test
	if (resetAllRenderStates || block.alphaTestEnabled != LastMaterialBlock.alphaTestEnabled)
	{
		CurrentRenderState.AlphaTestEnable = block.alphaTestEnabled ? GL_TRUE : GL_FALSE;
	}

	if (resetAllRenderStates || block.alphaTestFunc != LastMaterialBlock.alphaTestFunc)
	{
		CurrentRenderState.AlphaFunc = COpenGLHelper::getGLCompare(block.alphaTestFunc);
	}

	if (resetAllRenderStates || block.alphaTestRef != LastMaterialBlock.alphaTestRef)
	{
		CurrentRenderState.AlphaRef = (GLfloat)min_(block.alphaTestRef / 255.0f, 1.0f);
	}

#endif

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

void COpenGLMaterialRenderServices::setPixelShaderMaterialBlock( SRenderStateBlock& block, E_PS_TYPE psType, E_VERTEX_TYPE vType )
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

void COpenGLMaterialRenderServices::applyMaterialChanges()
{
#ifdef FIXPIPELINE
	DEVICE_SET_LIGHTMODEL_STATE(ModelColorControl, GL_LIGHT_MODEL_COLOR_CONTROL, CurrentRenderState.ModelColorControl);
	DEVICE_SET_SHADEMODEL_STATE(ShadeMode, CurrentRenderState.ShadeMode);
	DEVICE_SET_BOOL_STATE(Lighting, GL_LIGHTING, CurrentRenderState.Lighting);
	DEVICE_SET_BOOL_STATE(FogEnable, GL_FOG, CurrentRenderState.FogEnable);
	DEVICE_SET_POLYGONMODE_STATE(FillMode, CurrentRenderState.FillMode);
#endif

	DEVICE_SET_BOOL_STATE(ZEnable, GL_DEPTH_TEST, CurrentRenderState.ZEnable);
	if(CurrentRenderState.ZEnable)
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
	if (Extension->queryOpenGLFeature(IRR_ARB_multisample))
	{
		DEVICE_SET_BOOL_STATE(MultiSampleAntiAlias, GL_MULTISAMPLE_ARB, CurrentRenderState.MultiSampleAntiAlias);
	}
	DEVICE_SET_BOOL_STATE(AntiAliasedLineEnable, GL_LINE_SMOOTH, CurrentRenderState.AntiAliasedLineEnable);

	DEVICE_SET_BOOL_STATE(AlphaBlendEnable, GL_BLEND, CurrentRenderState.AlphaBlendEnable);
	if(CurrentRenderState.AlphaBlendEnable)
	{
		DEVICE_SET_BLEND_STATE(SrcBlend, DestBlend, CurrentRenderState.SrcBlend, CurrentRenderState.DestBlend);
	}	
	DEVICE_SET_BOOL_STATE(AlphaToCoverage, GL_SAMPLE_ALPHA_TO_COVERAGE_ARB, CurrentRenderState.AlphaToCoverage);

#ifdef FIXPIPELINE
	DEVICE_SET_BOOL_STATE(AlphaTestEnable, GL_ALPHA_TEST, CurrentRenderState.AlphaTestEnable);
	DEVICE_SET_ALPHAFUNC_STATE(AlphaFunc, AlphaRef, CurrentRenderState.AlphaFunc, CurrentRenderState.AlphaRef);
#endif

	u32 maxTextrues = min_((u32)MATERIAL_MAX_TEXTURES, (u32)Extension->MaxTextureUnits);

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
				ASSERT_OPENGL_SUCCESS();
				RsCache.TextureUnits[st].texture = NULL_PTR;
				continue;
			}

			const COpenGLTexture* gltex = static_cast<const COpenGLTexture*>(texunit.texture);
			glBindTexture(GL_TEXTURE_2D, gltex->getGLTexture());
			ASSERT_OPENGL_SUCCESS();
			RsCache.TextureUnits[st].texture = texunit.texture;

			setTextureEnable(st, true);

			if (texunit.texture->getSampleCount() <= 1)
			{
				DEVICE_SET_TEXTURE2D_PARAMETER_I(st, addressU, GL_TEXTURE_WRAP_S, texunit.addressU);

				DEVICE_SET_TEXTURE2D_PARAMETER_I(st, addressV, GL_TEXTURE_WRAP_T, texunit.addressV);

				DEVICE_SET_TEXTURE2D_PARAMETER_I(st, addressW, GL_TEXTURE_WRAP_R, texunit.addressW);

				DEVICE_SET_TEXTURE2D_PARAMETER_I(st, magFilter, GL_TEXTURE_MAG_FILTER, texunit.magFilter);

				if (texunit.texture->hasMipMaps())
				{
					DEVICE_SET_TEXTURE2D_PARAMETER_I(st, mipFilter, GL_TEXTURE_MIN_FILTER, texunit.mipFilter);
				}
				else
				{
					DEVICE_SET_TEXTURE2D_PARAMETER_I(st, minFilter, GL_TEXTURE_MIN_FILTER, texunit.minFilter);
				}

// 				if (Extension->queryOpenGLFeature(IRR_EXT_texture_filter_anisotropic))
// 				{
// 					DEVICE_SET_TEXTURE2D_PARAMETER_I(st, maxAniso, GL_TEXTURE_MAX_ANISOTROPY_EXT, texunit.maxAniso);
// 				}
			}

#ifdef FIXPIPELINE
			DEVICE_SET_TEXTURE_FILTER_CONTROL_F(st, mipLodBias, GL_TEXTURE_LOD_BIAS_EXT, texunit.mipLodBias);

			DEVICE_SET_TEXTURE_ENV_I(st, envMode, GL_TEXTURE_ENV_MODE, texunit.envMode);

			DEVICE_SET_TEXTURE_ENV_F(st, alphaScale, GL_ALPHA_SCALE, texunit.alphaScale);

			if (Extension->queryOpenGLFeature(IRR_ARB_texture_env_combine))
			{
				DEVICE_SET_TEXTURE_ENV_F(st, alphaOp, GL_COMBINE_ALPHA_ARB, texunit.alphaOp);

				DEVICE_SET_TEXTURE_ENV_F(st, alphaArg1, GL_SOURCE0_ALPHA_ARB, texunit.alphaArg1);

				DEVICE_SET_TEXTURE_ENV_F(st, alphaArg2, GL_SOURCE1_ALPHA_ARB, texunit.alphaArg2);

				DEVICE_SET_TEXTURE_ENV_F(st, colorOp, GL_COMBINE_RGB_ARB, texunit.colorOp);

				DEVICE_SET_TEXTURE_ENV_F(st, colorArg1, GL_SOURCE0_RGB_ARB, texunit.colorArg1);

				DEVICE_SET_TEXTURE_ENV_F(st, colorArg2, GL_SOURCE1_RGB_ARB, texunit.colorArg2);

				DEVICE_SET_TEXTURE_ENV_F(st, colorScale, GL_RGB_SCALE_ARB, texunit.colorScale);
			}
			else if (Extension->queryOpenGLFeature(IRR_EXT_texture_env_combine))
			{
				DEVICE_SET_TEXTURE_ENV_F(st, alphaOp, GL_COMBINE_ALPHA_EXT, texunit.alphaOp);

				DEVICE_SET_TEXTURE_ENV_F(st, alphaArg1, GL_SOURCE0_ALPHA_EXT, texunit.alphaArg1);

				DEVICE_SET_TEXTURE_ENV_F(st, alphaArg2, GL_SOURCE1_ALPHA_EXT, texunit.alphaArg2);

				DEVICE_SET_TEXTURE_ENV_F(st, colorOp, GL_COMBINE_RGB_EXT, texunit.colorOp);

				DEVICE_SET_TEXTURE_ENV_F(st, colorArg1, GL_SOURCE0_RGB_EXT, texunit.colorArg1);

				DEVICE_SET_TEXTURE_ENV_F(st, colorArg2, GL_SOURCE1_RGB_EXT, texunit.colorArg2);

				DEVICE_SET_TEXTURE_ENV_F(st, colorScale, GL_RGB_SCALE_EXT, texunit.colorScale);
			}
#endif
		}
	}

}

void COpenGLMaterialRenderServices::resetRSCache()
{
#ifdef FIXPIPELINE
	RsCache.Lighting = glIsEnabled(GL_LIGHTING);
	glGetIntegerv(GL_LIGHT_MODEL_COLOR_CONTROL, &RsCache.ModelColorControl);
	glGetIntegerv(GL_SHADE_MODEL, &RsCache.ShadeMode);
	RsCache.FogEnable = glIsEnabled(GL_FOG);
	glGetIntegerv(GL_POLYGON_MODE, &RsCache.FillMode);
#endif
	
	RsCache.ZEnable = glIsEnabled(GL_DEPTH_TEST);
	glGetIntegerv(GL_DEPTH_FUNC, &RsCache.ZFunc);
	GLboolean bv;
	glGetBooleanv(GL_DEPTH_WRITEMASK, &bv);
	RsCache.ZWriteEnable = bv;
	RsCache.StencilEnable = glIsEnabled(GL_STENCIL_TEST);
	RsCache.CullEnable = glIsEnabled(GL_CULL_FACE);
	glGetIntegerv(GL_CULL_FACE_MODE, &RsCache.CullMode);
	glGetIntegerv(GL_FRONT_FACE, &RsCache.FrontFace);
	RsCache.MultiSampleAntiAlias = glIsEnabled(GL_MULTISAMPLE_ARB);
	RsCache.AntiAliasedLineEnable = glIsEnabled(GL_LINE_SMOOTH);
	RsCache.AlphaBlendEnable = glIsEnabled(GL_BLEND);
	glGetIntegerv(GL_BLEND_SRC_RGB, &RsCache.SrcBlend);
	glGetIntegerv(GL_BLEND_DST_RGB, &RsCache.DestBlend);

#ifdef FIXPIPELINE
	RsCache.AlphaTestEnable = glIsEnabled(GL_ALPHA_TEST);
	glGetIntegerv(GL_ALPHA_TEST_FUNC, &RsCache.AlphaFunc);
	glGetFloatv(GL_ALPHA_TEST_REF, &RsCache.AlphaRef);
#endif

	RsCache.AlphaToCoverage = glIsEnabled(GL_SAMPLE_ALPHA_TO_COVERAGE_ARB);

	//texture op
	for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
	{
		RsCache.TextureUnits[i].texture = NULL_PTR;
	}

	glGetIntegerv(GL_ACTIVE_TEXTURE, &RsCache.ActiveTextureIndex);

	for (u32 idx=0; idx<MATERIAL_MAX_TEXTURES; ++idx)
	{
		setActiveTexture(idx);

#ifdef FIXPIPELINE
		RsCache.TextureUnits[idx].texEnable = glIsEnabled(GL_TEXTURE_2D);
#endif

		glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, &RsCache.TextureUnits[idx].baseLevel);
		glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, &RsCache.TextureUnits[idx].maxLevel);
		glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &RsCache.TextureUnits[idx].minFilter);
		glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &RsCache.TextureUnits[idx].magFilter);
		glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &RsCache.TextureUnits[idx].addressU);
		glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, &RsCache.TextureUnits[idx].addressV);
		glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, &RsCache.TextureUnits[idx].addressW);
		glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, &RsCache.TextureUnits[idx].maxAniso);
			
#ifdef FIXPIPELINE	
		glGetTexEnvfv(GL_TEXTURE_FILTER_CONTROL_EXT, GL_TEXTURE_LOD_BIAS_EXT, &RsCache.TextureUnits[idx].mipLodBias);

		glGetTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &RsCache.TextureUnits[idx].envMode);
		glGetTexEnvfv(GL_TEXTURE_ENV, GL_ALPHA_SCALE, &RsCache.TextureUnits[idx].alphaScale);
#ifdef GL_ARB_texture_env_combine
		glGetTexEnvfv(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, &RsCache.TextureUnits[idx].colorOp);
		glGetTexEnvfv(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, &RsCache.TextureUnits[idx].colorArg1);
		glGetTexEnvfv(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, &RsCache.TextureUnits[idx].colorArg2);
		glGetTexEnvfv(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, &RsCache.TextureUnits[idx].alphaOp);
		glGetTexEnvfv(GL_TEXTURE_ENV, GL_SOURCE1_ALPHA_ARB, &RsCache.TextureUnits[idx].alphaArg1);
		glGetTexEnvfv(GL_TEXTURE_ENV, GL_SOURCE2_ALPHA_ARB, &RsCache.TextureUnits[idx].alphaArg2);
		glGetTexEnvfv(GL_TEXTURE_ENV, GL_RGB_SCALE_ARB, &RsCache.TextureUnits[idx].colorScale);
#else
		glGetTexEnvfv(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, &RsCache.TextureUnits[idx].colorOp);
		glGetTexEnvfv(GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT, &RsCache.TextureUnits[idx].colorArg1);
		glGetTexEnvfv(GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, &RsCache.TextureUnits[idx].colorArg2);
		glGetTexEnvfv(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_EXT, &RsCache.TextureUnits[idx].alphaOp);
		glGetTexEnvfv(GL_TEXTURE_ENV, GL_SOURCE1_ALPHA_EXT, &RsCache.TextureUnits[idx].alphaArg1);
		glGetTexEnvfv(GL_TEXTURE_ENV, GL_SOURCE2_ALPHA_EXT, &RsCache.TextureUnits[idx].alphaArg2);
		glGetTexEnvfv(GL_TEXTURE_ENV, GL_RGB_SCALE_EXT, &RsCache.TextureUnits[idx].colorScale);
#endif
#endif
	}

}

void COpenGLMaterialRenderServices::set2DPixelShaderMaterialBlock( SRenderStateBlock& block, bool alpha, bool alphaChannel )
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

void COpenGLMaterialRenderServices::setTextureWrap( u32 st, E_TEXTURE_ADDRESS address, E_TEXTURE_CLAMP wrap )
{
	GLint v = COpenGLHelper::getGLTextureAddress(wrap);
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

void COpenGLMaterialRenderServices::applyTextureFilter( u32 st, E_TEXTURE_FILTER filter )
{
	GLint tftMag, tftMin, tftMip;

	// Bilinear, trilinear, and anisotropic filter	
	GLint maxAnisotropy = 1;
	if (filter != ETF_NONE)
	{
		u8 anisotropic = getAnisotropic(filter);
		tftMag = GL_LINEAR;
		tftMin = GL_LINEAR;
		tftMip = filter > ETF_BILINEAR ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR_MIPMAP_NEAREST;

		if (Extension->queryOpenGLFeature(IRR_EXT_texture_filter_anisotropic))
			maxAnisotropy = min_(anisotropic, Extension->MaxAnisotropy);
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
	CurrentRenderState.TextureUnits[st].maxAniso = maxAnisotropy;

	DEVICE_SET_TEXTURE2D_PARAMETER_I(st, magFilter, GL_TEXTURE_MAG_FILTER, tftMag);

	if (RsCache.TextureUnits[st].texture && RsCache.TextureUnits[st].texture->hasMipMaps())
	{
		DEVICE_SET_TEXTURE2D_PARAMETER_I(st, mipFilter, GL_TEXTURE_MIN_FILTER, tftMip);
	}
	else
	{
		DEVICE_SET_TEXTURE2D_PARAMETER_I(st, minFilter, GL_TEXTURE_MIN_FILTER, tftMin);
	}
	if (Extension->queryOpenGLFeature(IRR_EXT_texture_filter_anisotropic))
	{
		DEVICE_SET_TEXTURE2D_PARAMETER_I(st, maxAniso, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
	}
	
	LastOverrideMaterial.TextureFilters[st] = filter;
}

E_TEXTURE_CLAMP COpenGLMaterialRenderServices::getTextureWrap( u32 st, E_TEXTURE_ADDRESS address ) const
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
		v = CurrentRenderState.TextureUnits[st].addressW;
		break;
	default:
		ASSERT(false);
	}

	return COpenGLHelper::getTextureAddressMode(v);
}

void COpenGLMaterialRenderServices::setZWriteEnable( bool enable )
{
	CurrentRenderState.ZWriteEnable = enable ? GL_TRUE : GL_FALSE;
	DEVICE_SET_DEPTHMASK_STATE(ZWriteEnable, CurrentRenderState.ZWriteEnable);
	Driver->LastMaterial.ZWriteEnable = enable;
}

bool COpenGLMaterialRenderServices::getZWriteEnable() const
{
	return CurrentRenderState.ZWriteEnable != GL_FALSE;
}

void COpenGLMaterialRenderServices::setActiveTexture( u32 st )
{
	if (Extension->MaxTextureUnits > 1)
	{
		GLenum v = GL_TEXTURE0_ARB + st;

		if (RsCache.ActiveTextureIndex != (v))		
		{	
			Extension->extGlActiveTexture(v);						
			RsCache.ActiveTextureIndex = (v);
		}
	}
}

void COpenGLMaterialRenderServices::setTextureEnable( u32 st, bool enable )
{
#ifdef FIXPIPELINE
	if (st >= MATERIAL_MAX_TEXTURES)
		return;

	setActiveTexture(st);

	CurrentRenderState.TextureUnits[st].texEnable = enable ? GL_TRUE : GL_FALSE;
	DEVICE_SET_TEXTURE2D_BOOL_STATE(st, texEnable, GL_TEXTURE_2D, CurrentRenderState.TextureUnits[st].texEnable);
#endif
}

void COpenGLMaterialRenderServices::applyTextureMipMap( u32 st, bool mipmap )
{
	GLint baselevel = 0;
	GLint maxlevel = mipmap ? 1000 : 0;

	DEVICE_SET_TEXTURE2D_PARAMETER_I(st, baseLevel, GL_TEXTURE_BASE_LEVEL, baselevel);
	DEVICE_SET_TEXTURE2D_PARAMETER_I(st, maxLevel, GL_TEXTURE_MAX_LEVEL, maxlevel);
}

void COpenGLMaterialRenderServices::applySamplerTexture( u32 st, ITexture* tex )
{
	setActiveTexture(st);

	COpenGLTexture* gltex = static_cast<COpenGLTexture*>(tex);
	glBindTexture(GL_TEXTURE_2D, gltex ? gltex->getGLTexture() : 0);
	ASSERT_OPENGL_SUCCESS();
	RsCache.TextureUnits[st].texture = tex;
}

void COpenGLMaterialRenderServices::applySamplerTextureMultiSample(u32 st, ITexture* tex)
{
	setActiveTexture(st);

	COpenGLTexture* gltex = static_cast<COpenGLTexture*>(tex);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, gltex ? gltex->getGLTexture() : 0);
	ASSERT_OPENGL_SUCCESS();
	RsCache.TextureUnits[st].texture = tex;
}

void COpenGLMaterialRenderServices::applyTextureWrap( u32 st, E_TEXTURE_ADDRESS address, E_TEXTURE_CLAMP wrap)
{
	ASSERT(RsCache.TextureUnits[st].texture);

	GLint v = COpenGLHelper::getGLTextureAddress(wrap);

	switch (address)
	{
	case ETA_U:
			DEVICE_SET_TEXTURE2D_PARAMETER_I(st, addressU, GL_TEXTURE_WRAP_S,v);
		break;
	case ETA_V:
			DEVICE_SET_TEXTURE2D_PARAMETER_I(st, addressV, GL_TEXTURE_WRAP_T, v);
		break;
	case ETA_W:
			DEVICE_SET_TEXTURE2D_PARAMETER_I(st, addressW, GL_TEXTURE_WRAP_R, v);
		break;
	default:
		ASSERT(false);
	}
}

#endif
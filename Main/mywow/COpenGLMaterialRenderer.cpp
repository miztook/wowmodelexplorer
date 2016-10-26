#include "stdafx.h"
#include "COpenGLMaterialRenderer.h"
#include "mywow.h"

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_OPENGL

#include "COpenGLDriver.h"
#include "COpenGLExtension.h"
#include "COpenGLMaterialRenderServices.h"
#include "COpenGLShaderServices.h"

void COpenGLMaterialRenderer_Solid::OnSetMaterial( E_VERTEX_TYPE vType, const SMaterial& material, bool resetAllRenderStates )
{
	COpenGLMaterialRenderServices* services = static_cast<COpenGLMaterialRenderServices*>(g_Engine->getDriver()->getMaterialRenderServices());

	MaterialBlock.alphaBlendEnabled = false;
	MaterialBlock.alphaTestEnabled = false;
	MaterialBlock.alphaToCoverage = false;

	if (material.MaterialType == EMT_LINE)
	{
		services->setPixelShaderMaterialBlock(MaterialBlock, EPST_DEFAULT_PC, vType);	
	}
	else
	{
		if (hasTexture(vType))
			services->setPixelShaderMaterialBlock(MaterialBlock, (E_PS_TYPE)material.PsType, vType);
		else
			services->setPixelShaderMaterialBlock(MaterialBlock, (E_PS_TYPE)-1, vType);	//default
	}

	services->applyMaterialBlock(MaterialBlock, resetAllRenderStates);
}

void COpenGLMaterialRenderer_Transparent_Alpha_Blend::OnSetMaterial( E_VERTEX_TYPE vType, const SMaterial& material, bool resetAllRenderStates )
{
	COpenGLMaterialRenderServices* services = static_cast<COpenGLMaterialRenderServices*>(g_Engine->getDriver()->getMaterialRenderServices());

	switch (material.MaterialType)
	{
	case EMT_TRANSPARENT_ALPHA_BLEND:
		{
			//blend, test
			MaterialBlock.srcBlend = EBF_SRC_ALPHA;
			MaterialBlock.destBlend = EBF_ONE_MINUS_SRC_ALPHA;
			MaterialBlock.alphaBlendEnabled = true;
			MaterialBlock.alphaTestEnabled = false;
			MaterialBlock.alphaToCoverage = false;
		}
		break;
	case EMT_TRANSAPRENT_ALPHA_BLEND_TEST:
		{
			//blend, test
			MaterialBlock.srcBlend = EBF_SRC_ALPHA;
			MaterialBlock.destBlend = EBF_ONE_MINUS_SRC_ALPHA;
			MaterialBlock.alphaBlendEnabled = true;
			MaterialBlock.alphaTestEnabled = true;
			MaterialBlock.alphaToCoverage = true;
			MaterialBlock.alphaTestFunc = ECFN_GREATEREQUAL;
			MaterialBlock.alphaTestRef = 200;
		}
		break;
	case EMT_TRANSPARENT_ONE_ALPHA:
		{
			//blend
			MaterialBlock.srcBlend = EBF_ONE;
			MaterialBlock.destBlend = EBF_ONE_MINUS_SRC_ALPHA;
			MaterialBlock.alphaBlendEnabled = true;
			MaterialBlock.alphaTestEnabled = false;
			MaterialBlock.alphaToCoverage = false;
		}
		break;
	case EMT_TRANSPARENT_ADD_ALPHA:
		{
			//blend, test
			MaterialBlock.srcBlend = EBF_SRC_ALPHA;
			MaterialBlock.destBlend = EBF_ONE;
			MaterialBlock.alphaBlendEnabled = true;
			MaterialBlock.alphaTestEnabled = false;
			MaterialBlock.alphaToCoverage = false;
		}
		break;
	case EMT_TRANSPARENT_ADD_COLOR:
		{
			//blend, test
			MaterialBlock.srcBlend = EBF_SRC_COLOR;
			MaterialBlock.destBlend = EBF_ONE;
			MaterialBlock.alphaBlendEnabled = true;
			MaterialBlock.alphaTestEnabled = false;
			MaterialBlock.alphaToCoverage = false;
		}
		break;
	case EMT_TRANSPARENT_MODULATE:
		{
			//blend, test
			MaterialBlock.srcBlend = EBF_ZERO;
			MaterialBlock.destBlend = EBF_SRC_COLOR;
			MaterialBlock.alphaBlendEnabled = true;
			MaterialBlock.alphaTestEnabled = false;
			MaterialBlock.alphaToCoverage = false;
		}
		break;
	case EMT_TRANSPARENT_MODULATE_X2:
		{
			//blend, test
			MaterialBlock.srcBlend = EBF_DST_COLOR;
			MaterialBlock.destBlend = EBF_SRC_COLOR;
			MaterialBlock.alphaBlendEnabled = true;
			MaterialBlock.alphaTestEnabled = false;
			MaterialBlock.alphaToCoverage = false;
		}
		break;
	default:
		ASSERT(FALSE);
	}

	//texture stage
	if (hasTexture(vType))
		services->setPixelShaderMaterialBlock(MaterialBlock, (E_PS_TYPE)material.PsType, vType);
	else
		services->setPixelShaderMaterialBlock(MaterialBlock, (E_PS_TYPE)-1, vType);	//default

	ASSERT(!MaterialBlock.alphaTestEnabled || (material.ZWriteEnable && material.ZBuffer != ECFN_NEVER));

	services->applyMaterialBlock(MaterialBlock, resetAllRenderStates);
}

void COpenGLMaterialRenderer_Transparent_Alpha_Blend::OnRender( const SMaterial& material, u32 pass )
{
	if (material.MaterialType == EMT_TRANSAPRENT_ALPHA_BLEND_TEST)
	{
		COpenGLMaterialRenderServices* services = static_cast<COpenGLMaterialRenderServices*>(g_Engine->getDriver()->getMaterialRenderServices());

		MaterialBlock.alphaTestRef = (u8)(200 * material.getMaterialAlpha());
		services->applyMaterialBlock(MaterialBlock, false);
	}
}

void COpenGLMaterialRenderer_Transparent_Alpha_Test::OnSetMaterial( E_VERTEX_TYPE vType, const SMaterial& material, bool resetAllRenderStates )
{
	COpenGLMaterialRenderServices* services = static_cast<COpenGLMaterialRenderServices*>(g_Engine->getDriver()->getMaterialRenderServices());

	MaterialBlock.alphaBlendEnabled = false;
	MaterialBlock.alphaTestEnabled = true;
	MaterialBlock.alphaToCoverage = true;
	MaterialBlock.alphaTestFunc = ECFN_GREATEREQUAL;
	MaterialBlock.alphaTestRef = 200;

	//texture stage
	if (hasTexture(vType))
		services->setPixelShaderMaterialBlock(MaterialBlock, (E_PS_TYPE)material.PsType, vType);
	else
		services->setPixelShaderMaterialBlock(MaterialBlock, (E_PS_TYPE)-1, vType);	//default

	ASSERT(!MaterialBlock.alphaTestEnabled || (material.ZWriteEnable && material.ZBuffer != ECFN_NEVER));

	services->applyMaterialBlock(MaterialBlock, resetAllRenderStates);
}

void COpenGLMaterialRenderer_Transparent_Alpha_Test::OnRender( const SMaterial& material, u32 pass )
{
	COpenGLMaterialRenderServices* services = static_cast<COpenGLMaterialRenderServices*>(g_Engine->getDriver()->getMaterialRenderServices());

	MaterialBlock.alphaTestRef = (u8)(200 * material.getMaterialAlpha());
	services->applyMaterialBlock(MaterialBlock, false);
}

void COpenGLMaterialRenderer_Terrain_MultiPass::OnSetMaterial( E_VERTEX_TYPE vType, const SMaterial& material, bool resetAllRenderStates )
{
	COpenGLMaterialRenderServices* services = static_cast<COpenGLMaterialRenderServices*>(g_Engine->getDriver()->getMaterialRenderServices());

	ASSERT(hasTexture(vType));

	//num pass
	const CMapChunk* chunk = reinterpret_cast<const CMapChunk*>(g_Engine->getSceneRenderServices()->getCurrentUnit()->u.chunk);
	
#ifdef FIXPIPELINE
	MaterialBlock.alphaTestEnabled = false;
#else

	switch(chunk->numTextures)
	{
	case 1:
		MaterialBlock.pixelShader = g_Engine->getDriver()->getShaderServices()->getPixelShader(EPST_TERRAIN_1LAYER);
		break;
	case 2:
		MaterialBlock.pixelShader = g_Engine->getDriver()->getShaderServices()->getPixelShader(EPST_TERRAIN_2LAYER);
		break;
	case 3:
		MaterialBlock.pixelShader = g_Engine->getDriver()->getShaderServices()->getPixelShader(EPST_TERRAIN_3LAYER);
		break;
	case 4:
		MaterialBlock.pixelShader = g_Engine->getDriver()->getShaderServices()->getPixelShader(EPST_TERRAIN_4LAYER);
		break;
	default:
		ASSERT(false);
		break;;
	}

	//alpha blend texture
	services->setTextureWrap(0, ETA_U, ETC_CLAMP_TO_BORDER);
	services->setTextureWrap(0, ETA_V, ETC_CLAMP_TO_BORDER);

	for (u32 i=1; i<5; ++i)				//base textures
	{
		services->setTextureWrap(i, ETA_U, ETC_REPEAT);
		services->setTextureWrap(i, ETA_V, ETC_REPEAT);
	}

#endif
	
	services->applyMaterialBlock(MaterialBlock, resetAllRenderStates);

	if (chunk->numTextures == 0)
		NumPass =  0;
	else
	{
#ifdef FIXPIPELINE
		NumPass = chunk->numTextures + 1;
#else
		NumPass = 1;
#endif
	}
}

void COpenGLMaterialRenderer_Terrain_MultiPass::OnRender( const SMaterial& material, u32 pass )
{
#ifdef FIXPIPELINE

	COpenGLMaterialRenderServices* services = static_cast<COpenGLMaterialRenderServices*>(g_Engine->getDriver()->getMaterialRenderServices());

	const CMapChunk* chunk = reinterpret_cast<CMapChunk*>(g_Engine->getSceneRenderServices()->getCurrentUnit()->u.chunk);
	//solid texture
	if (pass == 0)
	{
		MaterialBlock.textureUnits[0].colorOp = ETO_ARG1;
		MaterialBlock.textureUnits[0].colorArg1 = ETA_TEXTURE;
		MaterialBlock.textureUnits[0].colorArg2 = ETA_DIFFUSE;
		MaterialBlock.textureUnits[0].alphaOp = ETO_DISABLE;

		MaterialBlock.textureUnits[1].alphaOp = ETO_DISABLE;
		MaterialBlock.textureUnits[1].colorOp = ETO_DISABLE;

		MaterialBlock.alphaBlendEnabled = false;

		services->setSampler_Texture(0, chunk->textures[0]);

		services->applyMaterialBlock(MaterialBlock, false);

		services->setTextureWrap(0, ETA_U, ETC_REPEAT);
		services->setTextureWrap(0, ETA_V, ETC_REPEAT);
		services->setTextureWrap(1, ETA_U, ETC_CLAMP_TO_BORDER);
		services->setTextureWrap(1, ETA_V, ETC_CLAMP_TO_BORDER);
	}
	else if( pass < chunk->numTextures)
	{
		MaterialBlock.textureUnits[0].colorOp = ETO_MODULATE;
		MaterialBlock.textureUnits[0].colorArg1 = ETA_TEXTURE;
		MaterialBlock.textureUnits[0].colorArg2 = ETA_CURRENT;
		MaterialBlock.textureUnits[0].alphaOp = ETO_DISABLE;

		MaterialBlock.textureUnits[1].colorOp = ETO_MODULATE;
		MaterialBlock.textureUnits[1].colorArg1 = ETA_TEXTURE;
		MaterialBlock.textureUnits[1].colorArg2 = ETA_CURRENT;
		MaterialBlock.textureUnits[1].alphaOp = ETO_ARG1;
		MaterialBlock.textureUnits[1].alphaArg1 = ETA_TEXTURE;

		MaterialBlock.alphaBlendEnabled = true;
		MaterialBlock.srcBlend = EBF_SRC_ALPHA;
		MaterialBlock.destBlend = EBF_ONE_MINUS_SRC_ALPHA;

		services->applyMaterialBlock(MaterialBlock, false);

		services->setSampler_Texture(0, chunk->textures[pass]);
		services->setSampler_Texture(1, chunk->alphamap[pass-1]);	
	}
	else
	{
		if (chunk->data_shadowmap)
		{
			MaterialBlock.textureUnits[0].colorOp = ETO_ARG1;
			MaterialBlock.textureUnits[0].colorArg1 = ETA_CURRENT;
			MaterialBlock.textureUnits[0].alphaOp = ETO_DISABLE;

			MaterialBlock.textureUnits[1].colorOp = ETO_MODULATE;
			MaterialBlock.textureUnits[1].colorArg1 = ETA_TEXTURE;
			MaterialBlock.textureUnits[1].colorArg2 = ETA_CURRENT;
			MaterialBlock.textureUnits[1].alphaOp = ETO_ARG1;
			MaterialBlock.textureUnits[1].alphaArg1 = ETA_TEXTURE;

			MaterialBlock.alphaBlendEnabled = true;
			MaterialBlock.srcBlend = EBF_SRC_ALPHA;
			MaterialBlock.destBlend = EBF_ONE_MINUS_SRC_ALPHA;

			services->applyMaterialBlock(MaterialBlock, false);

			services->setSampler_Texture(1, chunk->shadowmap);
		}
	}

#endif
}

#endif


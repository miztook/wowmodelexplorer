#include "stdafx.h"
#include "CD3D11MaterialRenderer.h"
#include "mywow.h"

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D11

#include "CD3D11MaterialRenderServices.h"

void CD3D11MaterialRenderer_Solid::OnSetMaterial( E_VERTEX_TYPE vType, const SMaterial& material, bool resetAllRenderStates )
{
	CD3D11MaterialRenderServices* services = static_cast<CD3D11MaterialRenderServices*>(g_Engine->getDriver()->getMaterialRenderServices());

	if (material.MaterialType == EMT_LINE)
	{
		services->setPixelShaderMaterialBlock(MaterialBlock, EPST_DEFAULT_PC, vType);	
	}
	else
	{
		if (hasTexture(vType))
			services->setPixelShaderMaterialBlock(MaterialBlock, material.PsType, vType);
		else
			services->setPixelShaderMaterialBlock(MaterialBlock, (E_PS_TYPE)-1, vType);	//default
	}

	MaterialBlock.alphaBlendEnabled = false;
	MaterialBlock.alphaTestEnabled = false;
	MaterialBlock.alphaToCoverage = false;

	services->applyMaterialBlock(MaterialBlock, resetAllRenderStates);
}

void CD3D11MaterialRenderer_Transparent_Alpha_Blend::OnSetMaterial( E_VERTEX_TYPE vType, const SMaterial& material, bool resetAllRenderStates )
{
	CD3D11MaterialRenderServices* services = static_cast<CD3D11MaterialRenderServices*>(g_Engine->getDriver()->getMaterialRenderServices());

	//texture stage
	if (hasTexture(vType))
		services->setPixelShaderMaterialBlock(MaterialBlock, material.PsType, vType);
	else
		services->setPixelShaderMaterialBlock(MaterialBlock, (E_PS_TYPE)-1, vType);	//default

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
		break;
	}

	ASSERT(!MaterialBlock.alphaTestEnabled || (material.ZWriteEnable && material.ZBuffer != ECFN_NEVER));

	services->applyMaterialBlock(MaterialBlock, resetAllRenderStates);
}

void CD3D11MaterialRenderer_Transparent_Alpha_Blend::OnRender( const SMaterial& material, u32 pass )
{
	if (material.MaterialType == EMT_TRANSAPRENT_ALPHA_BLEND_TEST)
	{
		CD3D11MaterialRenderServices* services = static_cast<CD3D11MaterialRenderServices*>(g_Engine->getDriver()->getMaterialRenderServices());

		MaterialBlock.alphaTestRef = (u8)(200 * material.getMaterialAlpha());
		services->applyMaterialBlock(MaterialBlock, false);
	}
}

void CD3D11MaterialRenderer_Transparent_Alpha_Test::OnSetMaterial( E_VERTEX_TYPE vType, const SMaterial& material, bool resetAllRenderStates )
{
	CD3D11MaterialRenderServices* services = static_cast<CD3D11MaterialRenderServices*>(g_Engine->getDriver()->getMaterialRenderServices());

	//texture stage
	if (hasTexture(vType))
		services->setPixelShaderMaterialBlock(MaterialBlock, material.PsType, vType);
	else
		services->setPixelShaderMaterialBlock(MaterialBlock, (E_PS_TYPE)-1, vType);	//default

	MaterialBlock.alphaBlendEnabled = false;
	MaterialBlock.alphaTestEnabled = true;
	MaterialBlock.alphaToCoverage = true;
	MaterialBlock.alphaTestFunc = ECFN_GREATEREQUAL;
	MaterialBlock.alphaTestRef = 200;

	ASSERT(!MaterialBlock.alphaTestEnabled || (material.ZWriteEnable && material.ZBuffer != ECFN_NEVER));

	services->applyMaterialBlock(MaterialBlock, resetAllRenderStates);
}

void CD3D11MaterialRenderer_Transparent_Alpha_Test::OnRender( const SMaterial& material, u32 pass )
{
	CD3D11MaterialRenderServices* services = static_cast<CD3D11MaterialRenderServices*>(g_Engine->getDriver()->getMaterialRenderServices());

	MaterialBlock.alphaTestRef = (u8)(200 * material.getMaterialAlpha());
	services->applyMaterialBlock(MaterialBlock, false);
}

void CD3D11MaterialRenderer_Terrain_MultiPass::OnSetMaterial( E_VERTEX_TYPE vType, const SMaterial& material, bool resetAllRenderStates )
{
	CD3D11MaterialRenderServices* services = static_cast<CD3D11MaterialRenderServices*>(g_Engine->getDriver()->getMaterialRenderServices());

	ASSERT(hasTexture(vType));

	const CMapChunk* chunk = reinterpret_cast<const CMapChunk*>(g_Engine->getSceneRenderServices()->getCurrentUnit()->u.chunk);

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
	services->setTextureWrap(0, ETA_U, ETC_CLAMP);
	services->setTextureWrap(0, ETA_V, ETC_CLAMP);

	for (u32 i=1; i<5; ++i)				//base textures
	{
		services->setTextureWrap(i, ETA_U, ETC_REPEAT);
		services->setTextureWrap(i, ETA_V, ETC_REPEAT);
	}

	services->applyMaterialBlock(MaterialBlock, resetAllRenderStates);

	//num pass
	if (chunk->numTextures == 0)
		NumPass = 0;
	else
		NumPass = 1;
}


#endif
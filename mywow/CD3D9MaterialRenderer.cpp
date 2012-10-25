#include "stdafx.h"
#include "CD3D9MaterialRenderer.h"
#include "mywow.h"
#include "CD3D9MaterialRenderServices.h"
#include "CD3D9ShaderServices.h"
#include "CD3D9Driver.h"

void CD3D9MaterialRenderer_OneColor::OnSetMaterial( const SMaterial& material, const SMaterial& lastMaterial, bool resetAllRenderStates )
{
	CD3D9MaterialRenderServices* services = (CD3D9MaterialRenderServices*)g_Engine->getDriver()->getMaterialRenderServices();
	if (material.MaterialType != lastMaterial.MaterialType || resetAllRenderStates)
	{
		services->applyMaterialBlock(MaterialBlock, resetAllRenderStates);
	}
}

void CD3D9MaterialRenderer_Solid::OnSetMaterial( const SMaterial& material, const SMaterial& lastMaterial, bool resetAllRenderStates )
{
	CD3D9MaterialRenderServices* services = (CD3D9MaterialRenderServices*)g_Engine->getDriver()->getMaterialRenderServices();
	if (material.MaterialType != lastMaterial.MaterialType || resetAllRenderStates)
	{
		services->applyMaterialBlock(MaterialBlock, resetAllRenderStates);
	}
}

void CD3D9MaterialRenderer_LightMap::OnSetMaterial( const SMaterial& material, const SMaterial& lastMaterial, bool resetAllRenderStates )
{
	CD3D9MaterialRenderServices* services = (CD3D9MaterialRenderServices*)g_Engine->getDriver()->getMaterialRenderServices();
	if ( material.MaterialType != lastMaterial.MaterialType || resetAllRenderStates )
	{
		if (material.MaterialType >= EMT_LIGHTMAP_LIGHTING)
		{
			MaterialBlock.textureUnits[0].colorOp = ETO_MODULATE;
			MaterialBlock.textureUnits[0].colorArg1 = ETA_TEXTURE;
			MaterialBlock.textureUnits[0].colorArg2 = ETA_DIFFUSE;
		}
		else
		{
			MaterialBlock.textureUnits[0].colorOp = ETO_ARG1;
			MaterialBlock.textureUnits[0].colorArg1 = ETA_TEXTURE;
		}
		MaterialBlock.textureUnits[0].alphaOp = ETO_DISABLE;

		//tex1的操作
		if (material.MaterialType == EMT_LIGHTMAP_ADD)
		{
			MaterialBlock.textureUnits[1].colorOp = ETO_ADD;
		}
		else
			if (material.MaterialType == EMT_LIGHTMAP_M4 || material.MaterialType == EMT_LIGHTMAP_LIGHTING_M4)
			{
				MaterialBlock.textureUnits[1].colorOp = ETO_MODULATE_X4;
			}
			else
				if (material.MaterialType == EMT_LIGHTMAP_M2 || material.MaterialType == EMT_LIGHTMAP_LIGHTING_M2)
				{
					MaterialBlock.textureUnits[1].colorOp = ETO_MODULATE_X2;
				}
				else
				{
					MaterialBlock.textureUnits[1].colorOp = ETO_MODULATE;
				}

		MaterialBlock.textureUnits[1].colorArg1 = ETA_TEXTURE;
		MaterialBlock.textureUnits[1].colorArg2 = ETA_CURRENT;
		MaterialBlock.textureUnits[1].alphaOp = ETO_DISABLE;

		services->applyMaterialBlock(MaterialBlock, resetAllRenderStates);
	}
}


void CD3D9MaterialRenderer_DetailMap::OnSetMaterial( const SMaterial& material, const SMaterial& lastMaterial, bool resetAllRenderStates )
{
	CD3D9MaterialRenderServices* services = (CD3D9MaterialRenderServices*)g_Engine->getDriver()->getMaterialRenderServices();
	if ( material.MaterialType != lastMaterial.MaterialType || resetAllRenderStates )
	{
		MaterialBlock.textureUnits[0].colorOp = ETO_MODULATE;
		MaterialBlock.textureUnits[0].colorArg1 = ETA_TEXTURE;
		MaterialBlock.textureUnits[0].colorArg2 = ETA_DIFFUSE;
		MaterialBlock.textureUnits[0].alphaOp = ETO_DISABLE;

		MaterialBlock.textureUnits[1].texcoordIndex = 0;
		MaterialBlock.textureUnits[1].colorOp = ETO_ADDSIGNED;
		MaterialBlock.textureUnits[1].colorArg1 = ETA_TEXTURE;
		MaterialBlock.textureUnits[1].colorArg2 = ETA_CURRENT;
		MaterialBlock.textureUnits[1].alphaOp = ETO_DISABLE;

		MaterialBlock.alphaBlendEnabled = false;

		services->applyMaterialBlock(MaterialBlock, resetAllRenderStates);
	}
}

void CD3D9MaterialRenderer_Transparent_Alpha_Blend::OnSetMaterial( const SMaterial& material, const SMaterial& lastMaterial, bool resetAllRenderStates )
{
	CD3D9MaterialRenderServices* services = (CD3D9MaterialRenderServices*)g_Engine->getDriver()->getMaterialRenderServices();
	if (material.MaterialType != lastMaterial.MaterialType || resetAllRenderStates)
	{
		switch (material.MaterialType)
		{
		case EMT_TRANSPARENT_ALPHA_BLEND:
			{
				MaterialBlock.textureUnits[0].colorOp = ETO_MODULATE;
				MaterialBlock.srcBlend = EBF_SRC_ALPHA;
				MaterialBlock.destBlend = EBF_ONE_MINUS_SRC_ALPHA;

				MaterialBlock.alphaTestEnabled = false;
			}
			break;
		case EMT_TRANSAPRENT_ALPHA_BLEND_TEST:
			{
				MaterialBlock.textureUnits[0].colorOp = ETO_MODULATE;
				MaterialBlock.srcBlend = EBF_SRC_ALPHA;
				MaterialBlock.destBlend = EBF_ONE_MINUS_SRC_ALPHA;

				MaterialBlock.alphaTestEnabled = true;
				MaterialBlock.alphaTestFunc = ECFN_GREATEREQUAL;
				MaterialBlock.alphaTestRef = 204;
			}
			break;
		case EMT_TRANSPARENT_ADD_ALPHA:
			{
				MaterialBlock.textureUnits[0].colorOp = ETO_MODULATE;
				MaterialBlock.srcBlend = EBF_SRC_ALPHA;
				MaterialBlock.destBlend = EBF_ONE;

				MaterialBlock.alphaTestEnabled = false;
			}
			break;
		case EMT_TRANSPARENT_ADD_COLOR:
			{
				MaterialBlock.textureUnits[0].colorOp = ETO_MODULATE;
				MaterialBlock.srcBlend = EBF_SRC_COLOR;
				MaterialBlock.destBlend = EBF_ONE;

				MaterialBlock.alphaTestEnabled = false;
			}
			break;
		default:
			_ASSERT(FALSE);
		}

		services->applyMaterialBlock(MaterialBlock, resetAllRenderStates);
	}
}

void CD3D9MaterialRenderer_Transparent_Alpha_Blend::OnRender( const SMaterial& material, u32 pass )
{
	if (material.MaterialType == EMT_TRANSAPRENT_ALPHA_BLEND_TEST)
	{
		CD3D9MaterialRenderServices* services = (CD3D9MaterialRenderServices*)g_Engine->getDriver()->getMaterialRenderServices();
		MaterialBlock.alphaTestRef = (u8)(204 * material.getMaterialAlpha());
		services->applyMaterialBlock(MaterialBlock, false);
	}
}

void CD3D9MaterialRenderer_Transparent_Alpha_Test::OnSetMaterial( const SMaterial& material, const SMaterial& lastMaterial, bool resetAllRenderStates )
{
	CD3D9MaterialRenderServices* services = (CD3D9MaterialRenderServices*)g_Engine->getDriver()->getMaterialRenderServices();
	if (material.MaterialType != lastMaterial.MaterialType || resetAllRenderStates)
	{
		services->applyMaterialBlock(MaterialBlock, resetAllRenderStates);
	}
}

void CD3D9MaterialRenderer_Transparent_Alpha_Test::OnRender( const SMaterial& material, u32 pass )
{
	//有effect时 ps失效
	CD3D9MaterialRenderServices* services = (CD3D9MaterialRenderServices*)g_Engine->getDriver()->getMaterialRenderServices();
	MaterialBlock.alphaTestRef = (u8)(204 * material.getMaterialAlpha());
	services->applyMaterialBlock(MaterialBlock, false);
}

u32 CD3D9MaterialRenderer_Terrain_MultiPass::getNumPasses() const
{
	const SMapChunk* chunk = (SMapChunk*)g_Engine->getSceneRenderServices()->getCurrentUnit()->u.chunk;
	if (chunk->numTextures == 0)
		return 0;

	CD3D9MaterialRenderServices* services = (CD3D9MaterialRenderServices*)g_Engine->getDriver()->getMaterialRenderServices();

	if (services->isFFPipeline())
		return chunk->numTextures + 1;
	else
		return 1;
}

void CD3D9MaterialRenderer_Terrain_MultiPass::OnSetMaterial( const SMaterial& material, const SMaterial& lastMaterial, bool resetAllRenderStates )
{
	CD3D9MaterialRenderServices* services = (CD3D9MaterialRenderServices*)g_Engine->getDriver()->getMaterialRenderServices();
	CD3D9Driver* driver = (CD3D9Driver*)g_Engine->getDriver();

	//back up
	OldOverrideMaterial = driver->getOverrideMaterial();
	for(u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
	{
		OldTextureWrap[i][0] = services->getTextureWrap(i, ETA_U);
		OldTextureWrap[i][1] = services->getTextureWrap(i, ETA_V);
	}

	if(services->isFFPipeline())
	{
		MaterialBlock.alphaTestEnabled = false;
	}
	else
	{
		MaterialBlock.pixelShader = g_Engine->getDriver()->getShaderServices()->getPixelShader(EPST_TERRAIN);

		//ps 需要的render state 
		SOverrideMaterial mat;
		for (u32 i=0; i<4; ++i)				//base textures
		{
			mat.TextureFilters[i] = ETF_TRILINEAR;

			services->setTextureWrap(i, ETA_U, ETC_REPEAT);
			services->setTextureWrap(i, ETA_V, ETC_REPEAT);
		}
		mat.TextureFilters[4] = ETF_TRILINEAR;		//alpha map
		services->setOverrideRenderStates(mat, false);

  		services->setTextureWrap(4, ETA_U, ETC_CLAMP);
  		services->setTextureWrap(4, ETA_V, ETC_CLAMP);
	}

	if (material.MaterialType != lastMaterial.MaterialType || resetAllRenderStates)
	{
		services->applyMaterialBlock(MaterialBlock, resetAllRenderStates);
	}
}

void CD3D9MaterialRenderer_Terrain_MultiPass::OnUnsetMaterial()
{
	CD3D9MaterialRenderServices* services = (CD3D9MaterialRenderServices*)g_Engine->getDriver()->getMaterialRenderServices();
	CD3D9Driver* driver = (CD3D9Driver*)g_Engine->getDriver();

	//restore
	SOverrideMaterial& mat = driver->getOverrideMaterial();
	mat= OldOverrideMaterial;
	services->setOverrideRenderStates(mat, false);

	for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
	{
		services->setTextureWrap(i, ETA_U, OldTextureWrap[i][0]);
		services->setTextureWrap(i, ETA_V, OldTextureWrap[i][1]);
	}
}

void CD3D9MaterialRenderer_Terrain_MultiPass::OnRender( const SMaterial& material, u32 pass )
{
	//有effect时 ps失效
	if (MaterialBlock.pixelShader || material.Effect)
	{
		CD3D9ShaderServices* shaderServices = (CD3D9ShaderServices*)g_Engine->getDriver()->getShaderServices();
		shaderServices->setShaderConstants(MaterialBlock.pixelShader, material);
		return;
	}

	CD3D9MaterialRenderServices* services = (CD3D9MaterialRenderServices*)g_Engine->getDriver()->getMaterialRenderServices();
	CD3D9Driver* driver = (CD3D9Driver*)g_Engine->getDriver();

	const SMapChunk* chunk = (SMapChunk*)g_Engine->getSceneRenderServices()->getCurrentUnit()->u.chunk;
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

		driver->setTexture(0, chunk->textures[0]);

		services->applyMaterialBlock(MaterialBlock, false);

		SOverrideMaterial mat;
		mat.TextureFilters[0] = ETF_TRILINEAR;
		mat.TextureFilters[1] = ETF_TRILINEAR;
		services->setOverrideRenderStates(mat, false);

		services->setTextureWrap(0, ETA_U, ETC_REPEAT);
		services->setTextureWrap(0, ETA_V, ETC_REPEAT);
		services->setTextureWrap(1, ETA_U, ETC_CLAMP);
		services->setTextureWrap(1, ETA_V, ETC_CLAMP);
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

		driver->setTexture(0, chunk->textures[pass]);
		driver->setTexture(1, chunk->alphamap[pass-1]);	
	}
	else
	{
		if (chunk->data_shadowmap)
		{
			MaterialBlock.textureUnits[0].colorOp = ETO_ARG2;
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

			driver->setTexture(1, chunk->shadowmap);
		}
	}
}


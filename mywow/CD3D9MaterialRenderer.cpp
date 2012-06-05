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
		case EMT_TRANSPARENT_ADD_ALPHA:
			{
				MaterialBlock.textureUnits[0].colorOp = ETO_MODULATE;
				MaterialBlock.srcBlend = EBF_SRC_ALPHA;
				MaterialBlock.destBlend = EBF_ONE;
			}
			break;
		case EMT_TRANSPARENT_ALPHA_BLEND:
			{
				MaterialBlock.textureUnits[0].colorOp = ETO_MODULATE;
				MaterialBlock.srcBlend = EBF_SRC_ALPHA;
				MaterialBlock.destBlend = EBF_ONE_MINUS_SRC_ALPHA;
			}
			break;
		case EMT_TRANSPARENT_ADD_COLOR:
			{
				MaterialBlock.textureUnits[0].colorOp = ETO_MODULATE;
				MaterialBlock.srcBlend = EBF_SRC_COLOR;
				MaterialBlock.destBlend = EBF_ONE;
			}
			break;
		default:
			_ASSERT(FALSE);
		}

		services->applyMaterialBlock(MaterialBlock, resetAllRenderStates);
	}
}

void CD3D9MaterialRenderer_Transparent_Alpha_Test::OnSetMaterial( const SMaterial& material, const SMaterial& lastMaterial, bool resetAllRenderStates )
{
	CD3D9MaterialRenderServices* services = (CD3D9MaterialRenderServices*)g_Engine->getDriver()->getMaterialRenderServices();

	if(services->isFFPipeline())
	{
		MaterialBlock.textureUnits[0].colorOp = ETO_MODULATE;
		MaterialBlock.textureUnits[0].colorArg1 = ETA_TEXTURE;
		MaterialBlock.textureUnits[0].colorArg2 = ETA_DIFFUSE;
		MaterialBlock.textureUnits[0].alphaOp = ETO_MODULATE;
		MaterialBlock.textureUnits[0].alphaArg1 = ETA_TEXTURE;
		MaterialBlock.textureUnits[0].alphaArg2 = ETA_DIFFUSE;

		MaterialBlock.textureUnits[1].alphaOp = ETO_DISABLE;
		MaterialBlock.textureUnits[1].colorOp = ETO_DISABLE;

		MaterialBlock.alphaBlendEnabled = false;
		MaterialBlock.alphaTestEnabled = true;
		MaterialBlock.alphaTestFunc = ECFN_GREATEREQUAL;
		MaterialBlock.alphaTestRef = 204;
 	}	
 	else
 	{
		MaterialBlock.pixelShader = g_Engine->getDriver()->getShaderServices()->getPixelShader(EPST_ALPHATEST);
		
		//ps 需要的render state 

	}

	if (material.MaterialType != lastMaterial.MaterialType || resetAllRenderStates)
	{
		services->applyMaterialBlock(MaterialBlock, resetAllRenderStates);
	}
}

void CD3D9MaterialRenderer_Transparent_Alpha_Test::OnRender( const SMaterial& material, u32 pass )
{
	//有effect时 ps失效
	if (!MaterialBlock.pixelShader && !material.Effect)
	{
		CD3D9MaterialRenderServices* services = (CD3D9MaterialRenderServices*)g_Engine->getDriver()->getMaterialRenderServices();
		MaterialBlock.alphaTestRef = (u8)(204 * material.getMaterialAlpha());
		services->applyMaterialBlock(MaterialBlock, false);
	}
	else
	{
		IShaderServices* shaderServices = g_Engine->getDriver()->getShaderServices();
		shaderServices->setShaderConstants(MaterialBlock.pixelShader, material);
	}
}


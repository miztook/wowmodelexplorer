#include "stdafx.h"
#include "CGLES2_PS_ETC1.h"
#include "mywow.h"
#include "compileconfig.h"
#include "CFileADT.h"

#ifdef MW_COMPILE_WITH_GLES2

#include "CGLES2Shader.h"
#include "CGLES2ShaderServices.h"
#include "CGLES2Driver.h"
#include "CGLES2MaterialRenderServices.h"
#include "CGLES2SceneStateServices.h"

static CGLES2_PS_ETC1::SPixelShaderEntry g_gles2ps_pixelShaderEntries[] =
{
	{EPST_DEFAULT_P, "Default_P", NULL},
	{EPST_DEFAULT_PC, "Default_PC", NULL},
	{EPST_DEFAULT_PCT, "Default_PCT", CGLES2_PS_ETC1::Default_setShaderConst_T1},
	{EPST_DEFAULT_PN, "Default_PN", NULL},
	{EPST_DEFAULT_PNC, "Default_PNC", NULL},
	{EPST_DEFAULT_PNCT, "Default_PNCT", CGLES2_PS_ETC1::Default_setShaderConst_T1},
	{EPST_DEFAULT_PNCT2, "Default_PNCT2", CGLES2_PS_ETC1::Default_setShaderConst_T2},
	{EPST_DEFAULT_PNT, "Default_PNT", CGLES2_PS_ETC1::Default_setShaderConst_T1},
	{EPST_DEFAULT_PT, "Default_PT", CGLES2_PS_ETC1::Default_setShaderConst_T1},

	{EPST_UI, "UI", CGLES2_PS_ETC1::UI_setShaderConst},
	{EPST_UI_ALPHA, "UI_Alpha", CGLES2_PS_ETC1::UI_setShaderConst},	
	{EPST_UI_ALPHACHANNEL, "UI_AlphaChannel", CGLES2_PS_ETC1::UI_setShaderConst},
	{EPST_UI_ALPHA_ALPHACHANNEL, "UI_Alpha_AlphaChannel", CGLES2_PS_ETC1::UI_setShaderConst},
	{EPST_TERRAIN_1LAYER, "Terrain_1Layer", CGLES2_PS_ETC1::Terrain_setShaderConst},
	{EPST_TERRAIN_2LAYER, "Terrain_2Layer", CGLES2_PS_ETC1::Terrain_setShaderConst},
	{EPST_TERRAIN_3LAYER, "Terrain_3Layer", CGLES2_PS_ETC1::Terrain_setShaderConst},
	{EPST_TERRAIN_4LAYER, "Terrain_4Layer", CGLES2_PS_ETC1::Terrain_setShaderConst},

	{EPST_COMBINERS_MOD, "Combiners_Mod", CGLES2_PS_ETC1::DiffuseT1_setShaderConst},
	{EPST_COMBINERS_MOD_ADD, "Combiners_Mod_Add", CGLES2_PS_ETC1::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_MOD_ADD_ALPHA, "Combiners_Mod_Add_Alpha", CGLES2_PS_ETC1::DiffuseT3_setShaderConst},
	{EPST_COMBINERS_MOD_ADDALPHA, "Combiners_Mod_AddAlpha", CGLES2_PS_ETC1::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_MOD_ADDALPHA_ALPHA, "Combiners_Mod_AddAlpha_Alpha", CGLES2_PS_ETC1::DiffuseT3_setShaderConst},
	{EPST_COMBINERS_MOD_ADDALPHA_WGT, "Combiners_Mod_AddAlpha_Wgt", CGLES2_PS_ETC1::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_MOD_ADDNA, "Combiners_Mod_AddNA", CGLES2_PS_ETC1::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_MOD_MOD, "Combiners_Mod_Mod", CGLES2_PS_ETC1::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_MOD_MOD2X, "Combiners_Mod_Mod2x", CGLES2_PS_ETC1::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_MOD_MOD2XNA, "Combiners_Mod_Mod2xNA", CGLES2_PS_ETC1::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_MOD_OPAQUE, "Combiners_Mod_Opaque", CGLES2_PS_ETC1::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_OPAQUE, "Combiners_Opaque", CGLES2_PS_ETC1::DiffuseT1_setShaderConst},
	{EPST_COMBINERS_OPAQUE_ADDALPHA, "Combiners_Opaque_AddAlpha", CGLES2_PS_ETC1::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_OPAQUE_ADDALPHA_ALPHA, "Combiners_Opaque_AddAlpha_Alpha", CGLES2_PS_ETC1::DiffuseT3_setShaderConst},
	{EPST_COMBINERS_OPAQUE_ADDALPHA_WGT, "Combiners_Opaque_AddAlpha_Wgt", CGLES2_PS_ETC1::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_OPAQUE_ALPHA, "Combiners_Opaque_Alpha", CGLES2_PS_ETC1::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_OPAQUE_ALPHA_ALPHA, "Combiners_Opaque_Alpha_Alpha", CGLES2_PS_ETC1::DiffuseT3_setShaderConst},
	{EPST_COMBINERS_OPAQUE_MOD, "Combiners_Opaque_Mod", CGLES2_PS_ETC1::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_OPAQUE_MOD2X, "Combiners_Opaque_Mod2x", CGLES2_PS_ETC1::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_OPAQUE_MOD2XNA, "Combiners_Opaque_Mod2xNA", CGLES2_PS_ETC1::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_OPAQUE_MOD2XNA_ALPHA, "Combiners_Opaque_Mod2xNA_Alpha", CGLES2_PS_ETC1::DiffuseT3_setShaderConst},
	{EPST_COMBINERS_OPAQUE_MOD2XNA_ALPHA_3S, "Combiners_Opaque_Mod2xNA_Alpha_3s", CGLES2_PS_ETC1::DiffuseT3_setShaderConst},
	{EPST_COMBINERS_OPAQUE_MOD2XNA_ALPHA_ADD, "Combiners_Opaque_Mod2xNA_Alpha_Add", CGLES2_PS_ETC1::DiffuseT3_setShaderConst},
	{EPST_COMBINERS_OPAQUE_MOD2XNA_ALPHA_ALPHA, "Combiners_Opaque_Mod2xNA_Alpha_Alpha", CGLES2_PS_ETC1::DiffuseT3_setShaderConst},
	{EPST_COMBINERS_OPAQUE_MOD2XNA_ALPHA_UNSHALPHA, "Combiners_Opaque_Mod2xNA_Alpha_UnshAlpha", CGLES2_PS_ETC1::DiffuseT3_setShaderConst},
	{EPST_COMBINERS_OPAQUE_MOD_ADD_WGT, "Combiners_Opaque_Mod_Add_Wgt", CGLES2_PS_ETC1::DiffuseT3_setShaderConst},
	{EPST_COMBINERS_OPAQUE_MODNA_ALPHA, "Combiners_Opaque_ModNA_Alpha", CGLES2_PS_ETC1::DiffuseT3_setShaderConst},
	{EPST_COMBINERS_OPAQUE_OPAQUE, "Combiners_Opaque_Opaque", CGLES2_PS_ETC1::DiffuseT2_setShaderConst},

	{EPST_MAPOBJ_DIFFUSE, "MapObjDiffuse", CGLES2_PS_ETC1::MapObj_setShaderConst},
	{EPST_MAPOBJ_DIFFUSEEMISSIVE, "MapObjDiffuseEmissive", CGLES2_PS_ETC1::MapObj_setShaderConst},
	{EPST_MAPOBJ_OPAQUE, "MapObjOpaque", CGLES2_PS_ETC1::MapObjOpaque_setShaderConst},
	{EPST_MAPOBJ_SPECULAR, "MapObjSpecular", CGLES2_PS_ETC1::MapObj_setShaderConst},
	{EPST_MAPOBJ_METAL, "MapObjMetal", CGLES2_PS_ETC1::MapObj_setShaderConst},
	{EPST_MAPOBJ_TWOLAYERDIFFUSE, "MapObjTwoLayerDiffuse", CGLES2_PS_ETC1::MapObjTwoLayer_setShaderConst},
	{EPST_MAPOBJ_ENVMETAL, "MapObjEnvMetal", CGLES2_PS_ETC1::MapObjTwoLayer_setShaderConst},
	{EPST_MAPOBJ_ENV, "MapObjEnv", CGLES2_PS_ETC1::MapObjTwoLayer_setShaderConst},
	{EPST_MAPOBJ_TWOLAYERDIFFUSEOPAQUE, "MapObjTwoLayerDiffuseOpaque", CGLES2_PS_ETC1::MapObjTwoLayerOpaque_setShaderConst},
	{EPST_MAPOBJ_TWOLAYERENVMETAL, "MapObjTwoLayerEnvMetal", CGLES2_PS_ETC1::MapObjTwoLayer_setShaderConst},
	{EPST_MAPOBJ_TWOLAYERTERRAIN, "MapObjTwoLayerTerrain", CGLES2_PS_ETC1::MapObjTwoLayer_setShaderConst},

};

void CGLES2_PS_ETC1::loadAll( CGLES2ShaderServices* shaderServices )
{
	u32 num = sizeof(g_gles2ps_pixelShaderEntries) / sizeof(SPixelShaderEntry);
	for (u32 i=0; i<num; ++i)
	{
		loadPShaderHLSL(shaderServices, g_gles2ps_pixelShaderEntries[i].psType, PS_Macro_None);
	}
}

bool CGLES2_PS_ETC1::loadPShaderHLSL(CGLES2ShaderServices* shaderServices, E_PS_TYPE type, E_PS_MACRO macro)
{
	const c8* profile = "gles2_ps_etc1";

	c8 basePath[128];
	Q_sprintf(basePath, 128, "Pixel\\%s\\", profile);

	s32 index = -1;
	u32 num = sizeof(g_gles2ps_pixelShaderEntries) / sizeof(SPixelShaderEntry);
	for (u32 i=0; i<num; ++i)
	{
		if (g_gles2ps_pixelShaderEntries[i].psType == type)
		{
			index = (s32)i;
			break;
		}
	}

	if (index == -1)
		return false;

	string128 path(basePath);
	path.append(g_gles2ps_pixelShaderEntries[index].path.c_str());

	path.append(".fx");

	path.normalize();

	return shaderServices->loadPShaderHLSL(path.c_str(), "main", profile, g_gles2ps_pixelShaderEntries[index].psType, macro, g_gles2ps_pixelShaderEntries[index].callback);
}

void CGLES2_PS_ETC1::Default_setShaderConst_T1( IPixelShader* ps, const SMaterial& material, u32 pass )
{
	CGLES2MaterialRenderServices* services = static_cast<CGLES2MaterialRenderServices*>(g_Engine->getDriver()->getMaterialRenderServices());
	CGLES2ShaderServices* shaderServices = static_cast<CGLES2ShaderServices*>(g_Engine->getDriver()->getShaderServices());
	const SRenderUnit* renderUnit = g_Engine->getSceneRenderServices()->getCurrentUnit();
	const SRenderStateBlock& block = services->getRenderStateBlock();

	Common_CB cbuffer;
	cbuffer.TextureParams[0] = getTextureParams(material, 0, renderUnit->textures[0]);
	if (block.alphaTestEnabled)
	{
		cbuffer.params[0] = block.alphaTestEnabled ? 1.0f : 0.0f;
		cbuffer.params[1] = block.alphaTestRef / 255.0f;
	}

	u32 size = block.alphaTestEnabled ? sizeof(cbuffer) : sizeof(cbuffer) - sizeof(cbuffer.params);
	shaderServices->setShaderUniformF("g_psbuffer", (const f32*)&cbuffer, sizeof(cbuffer));

	services->setSampler_Texture(0, renderUnit->textures[0]);
}

void CGLES2_PS_ETC1::Default_setShaderConst_T2( IPixelShader* ps, const SMaterial& material, u32 pass )
{
	CGLES2MaterialRenderServices* services = static_cast<CGLES2MaterialRenderServices*>(g_Engine->getDriver()->getMaterialRenderServices());
	CGLES2ShaderServices* shaderServices = static_cast<CGLES2ShaderServices*>(g_Engine->getDriver()->getShaderServices());
	const SRenderUnit* renderUnit = g_Engine->getSceneRenderServices()->getCurrentUnit();
	const SRenderStateBlock& block = services->getRenderStateBlock();

	Common_CB cbuffer;
	cbuffer.TextureParams[0] = getTextureParams(material, 0, renderUnit->textures[0]);
	cbuffer.TextureParams[1] = getTextureParams(material, 1, renderUnit->textures[1]);
	if (block.alphaTestEnabled)
	{
		cbuffer.params[0] = block.alphaTestEnabled ? 1.0f : 0.0f;
		cbuffer.params[1] = block.alphaTestRef / 255.0f;
	}

	u32 size = block.alphaTestEnabled ? sizeof(cbuffer) : sizeof(cbuffer) - sizeof(cbuffer.params);
	shaderServices->setShaderUniformF("g_psbuffer", (const f32*)&cbuffer, sizeof(cbuffer));

	services->setSampler_Texture(0, renderUnit->textures[0]);
	services->setSampler_Texture(1, renderUnit->textures[1]);
}

void CGLES2_PS_ETC1::UI_setShaderConst( IPixelShader* ps, const SMaterial& material, u32 pass )
{
	CGLES2MaterialRenderServices* matServices = static_cast<CGLES2MaterialRenderServices*>(g_Engine->getDriver()->getMaterialRenderServices());

	matServices->setSampler_Texture(0,  matServices->getSampler_Texture(0));
}

void CGLES2_PS_ETC1::DiffuseT1_setShaderConst( IPixelShader* ps, const SMaterial& material, u32 pass )
{
	CGLES2MaterialRenderServices* services = static_cast<CGLES2MaterialRenderServices*>(g_Engine->getDriver()->getMaterialRenderServices());
	CGLES2ShaderServices* shaderServices = static_cast<CGLES2ShaderServices*>(g_Engine->getDriver()->getShaderServices());
	CGLES2SceneStateServices* sceneStateServices = static_cast<CGLES2SceneStateServices*>(g_Engine->getDriver()->getSceneStateServices());
	const SRenderUnit* renderUnit = g_Engine->getSceneRenderServices()->getCurrentUnit();
	const SRenderStateBlock& block = services->getRenderStateBlock();

	// param
	Diffuse_CB cbuffer;		
	cbuffer.FogColor = SColorf(sceneStateServices->getFog().FogColor);
	cbuffer.TextureParams[0] = getTextureParams(material, 0, renderUnit->textures[0]);
	if(block.alphaTestEnabled)
	{
		cbuffer.params[0] = block.alphaTestEnabled ? 1.0f : 0.0f;
		cbuffer.params[1] = block.alphaTestRef / 255.0f;
	}

	u32 size = block.alphaTestEnabled ? sizeof(cbuffer) : sizeof(cbuffer) - sizeof(cbuffer.params);
	shaderServices->setShaderUniformF("g_psbuffer", (const f32*)&cbuffer, size);

	services->setSampler_Texture(0,  renderUnit->textures[0]);

}

void CGLES2_PS_ETC1::DiffuseT2_setShaderConst( IPixelShader* ps, const SMaterial& material, u32 pass )
{
	CGLES2MaterialRenderServices* services = static_cast<CGLES2MaterialRenderServices*>(g_Engine->getDriver()->getMaterialRenderServices());
	CGLES2ShaderServices* shaderServices = static_cast<CGLES2ShaderServices*>(g_Engine->getDriver()->getShaderServices());
	CGLES2SceneStateServices* sceneStateServices = static_cast<CGLES2SceneStateServices*>(g_Engine->getDriver()->getSceneStateServices());
	const SRenderUnit* renderUnit = g_Engine->getSceneRenderServices()->getCurrentUnit();
	const SRenderStateBlock& block = services->getRenderStateBlock();

	// param
	Diffuse_CB cbuffer;		
	cbuffer.FogColor = SColorf(sceneStateServices->getFog().FogColor);
	cbuffer.TextureParams[0] = getTextureParams(material, 0, renderUnit->textures[0]);
	cbuffer.TextureParams[1] = getTextureParams(material, 1, renderUnit->textures[1]);
	if(block.alphaTestEnabled)
	{
		cbuffer.params[0] = block.alphaTestEnabled ? 1.0f : 0.0f;
		cbuffer.params[1] = block.alphaTestRef / 255.0f;
	}

	u32 size = block.alphaTestEnabled ? sizeof(cbuffer) : sizeof(cbuffer) - sizeof(cbuffer.params);
	shaderServices->setShaderUniformF("g_psbuffer", (const f32*)&cbuffer, size);

	services->setSampler_Texture(0,  renderUnit->textures[0]);
	services->setSampler_Texture(1,  renderUnit->textures[1]);
}

void CGLES2_PS_ETC1::DiffuseT3_setShaderConst( IPixelShader* ps, const SMaterial& material, u32 pass )
{
	CGLES2MaterialRenderServices* services = static_cast<CGLES2MaterialRenderServices*>(g_Engine->getDriver()->getMaterialRenderServices());
	CGLES2ShaderServices* shaderServices = static_cast<CGLES2ShaderServices*>(g_Engine->getDriver()->getShaderServices());
	CGLES2SceneStateServices* sceneStateServices = static_cast<CGLES2SceneStateServices*>(g_Engine->getDriver()->getSceneStateServices());
	const SRenderUnit* renderUnit = g_Engine->getSceneRenderServices()->getCurrentUnit();
	const SRenderStateBlock& block = services->getRenderStateBlock();

	// param
	Diffuse_CB cbuffer;		
	cbuffer.FogColor = SColorf(sceneStateServices->getFog().FogColor);
	cbuffer.TextureParams[0] = getTextureParams(material, 0, renderUnit->textures[0]);
	cbuffer.TextureParams[1] = getTextureParams(material, 1, renderUnit->textures[1]);
	cbuffer.TextureParams[2] = getTextureParams(material, 2, renderUnit->textures[2]);
	if(block.alphaTestEnabled)
	{
		cbuffer.params[0] = block.alphaTestEnabled ? 1.0f : 0.0f;
		cbuffer.params[1] = block.alphaTestRef / 255.0f;
	}

	u32 size = block.alphaTestEnabled ? sizeof(cbuffer) : sizeof(cbuffer) - sizeof(cbuffer.params);
	shaderServices->setShaderUniformF("g_psbuffer", (const f32*)&cbuffer, size);

	services->setSampler_Texture(0,  renderUnit->textures[0]);
	services->setSampler_Texture(1,  renderUnit->textures[1]);
	services->setSampler_Texture(2,  renderUnit->textures[2]);
}

void CGLES2_PS_ETC1::Terrain_setShaderConst( IPixelShader* ps, const SMaterial& material, u32 pass )
{
	CGLES2MaterialRenderServices* services = static_cast<CGLES2MaterialRenderServices*>(g_Engine->getDriver()->getMaterialRenderServices());
	CGLES2ShaderServices* shaderServices = static_cast<CGLES2ShaderServices*>(g_Engine->getDriver()->getShaderServices());
	const SRenderUnit* renderUnit = g_Engine->getSceneRenderServices()->getCurrentUnit();
	const CMapChunk* chunk = reinterpret_cast<const CMapChunk*>(renderUnit->u.chunk);
	const CFileADT* adt = reinterpret_cast<const CFileADT*>(renderUnit->u.adt);
	CGLES2SceneStateServices* sceneStateServices = static_cast<CGLES2SceneStateServices*>(g_Engine->getDriver()->getSceneStateServices());
	SFogParam fogParam = sceneStateServices->getFog();

	// param
	Terrain_CB cbuffer;
	cbuffer.params[0] =  (float)chunk->numTextures;
	cbuffer.fogColor = SColorf(fogParam.FogColor);
	cbuffer.TextureParams[0] = getTextureParams(material, 1, renderUnit->textures[0]);
	cbuffer.TextureParams[1] = getTextureParams(material, 2, renderUnit->textures[1]);
	cbuffer.TextureParams[2] = getTextureParams(material, 3, renderUnit->textures[2]);
	cbuffer.TextureParams[3] = getTextureParams(material, 4, renderUnit->textures[3]);
	shaderServices->setShaderUniformF("g_psbuffer", (const f32*)&cbuffer, sizeof(cbuffer));

	services->setSampler_Texture(0, adt->getBlendMap());
	services->setSampler_Texture(1, chunk->textures[0]);

	if (chunk->numTextures > 1)
		services->setSampler_Texture(2, chunk->textures[1]);

	if (chunk->numTextures > 2)
		services->setSampler_Texture(3, chunk->textures[2]);

	if (chunk->numTextures > 3)
		services->setSampler_Texture(4, chunk->textures[3]);
}

void CGLES2_PS_ETC1::MapObjOpaque_setShaderConst( IPixelShader* ps, const SMaterial& material, u32 pass )
{
	CGLES2MaterialRenderServices* services = static_cast<CGLES2MaterialRenderServices*>(g_Engine->getDriver()->getMaterialRenderServices());
	CGLES2ShaderServices* shaderServices = static_cast<CGLES2ShaderServices*>(g_Engine->getDriver()->getShaderServices());
	const SRenderUnit* renderUnit = g_Engine->getSceneRenderServices()->getCurrentUnit();
	CGLES2SceneStateServices* sceneStateServices = static_cast<CGLES2SceneStateServices*>(g_Engine->getDriver()->getSceneStateServices());

	// param
	MapObjOpaque_CB cbuffer;
	cbuffer.AmbientColor = material.Lighting ? SColorf(sceneStateServices->getAmbientLight()) * material.AmbientColor : material.EmissiveColor;
	cbuffer.FogColor = SColorf(sceneStateServices->getFog().FogColor);
	cbuffer.TextureParams[0] = getTextureParams(material, 0, renderUnit->textures[0]);
	shaderServices->setShaderUniformF("g_psbuffer", (const f32*)&cbuffer, sizeof(cbuffer));

	services->setSampler_Texture(0, renderUnit->textures[0]);
}

void CGLES2_PS_ETC1::MapObj_setShaderConst( IPixelShader* ps, const SMaterial& material, u32 pass )
{
	//texture
	CGLES2MaterialRenderServices* services = static_cast<CGLES2MaterialRenderServices*>(g_Engine->getDriver()->getMaterialRenderServices());
	CGLES2ShaderServices* shaderServices = static_cast<CGLES2ShaderServices*>(g_Engine->getDriver()->getShaderServices());
	const SRenderUnit* renderUnit = g_Engine->getSceneRenderServices()->getCurrentUnit();
	CGLES2SceneStateServices* sceneStateServices = static_cast<CGLES2SceneStateServices*>(g_Engine->getDriver()->getSceneStateServices());
	const SRenderStateBlock& block = services->getRenderStateBlock();

	// param
	MapObjAlphaTest_CB cbuffer;
	cbuffer.AmbientColor = material.Lighting ? SColorf(sceneStateServices->getAmbientLight()) * material.AmbientColor : material.EmissiveColor;
	cbuffer.FogColor = SColorf(sceneStateServices->getFog().FogColor);
	cbuffer.TextureParams[0] = getTextureParams(material, 0, renderUnit->textures[0]);
	if(block.alphaTestEnabled)
	{	
		cbuffer.params[0] = block.alphaTestEnabled ? 1.0f : 0.0f;
		cbuffer.params[1] = block.alphaTestRef / 255.0f;
	}

	u32 size = block.alphaTestEnabled ? sizeof(cbuffer) : sizeof(cbuffer) - sizeof(cbuffer.params);
	shaderServices->setShaderUniformF("g_psbuffer", (const f32*)&cbuffer, size);

	services->setSampler_Texture(0, renderUnit->textures[0]);
}

void CGLES2_PS_ETC1::MapObjTwoLayer_setShaderConst( IPixelShader* ps, const SMaterial& material, u32 pass )
{
	//texture
	CGLES2MaterialRenderServices* services = static_cast<CGLES2MaterialRenderServices*>(g_Engine->getDriver()->getMaterialRenderServices());
	CGLES2ShaderServices* shaderServices = static_cast<CGLES2ShaderServices*>(g_Engine->getDriver()->getShaderServices());
	const SRenderUnit* renderUnit = g_Engine->getSceneRenderServices()->getCurrentUnit();
	CGLES2SceneStateServices* sceneStateServices = static_cast<CGLES2SceneStateServices*>(g_Engine->getDriver()->getSceneStateServices());
	const SRenderStateBlock& block = services->getRenderStateBlock();

	// param
	MapObjAlphaTest_CB cbuffer;
	cbuffer.AmbientColor = material.Lighting ? SColorf(sceneStateServices->getAmbientLight()) * material.AmbientColor : material.EmissiveColor;
	cbuffer.FogColor = SColorf(sceneStateServices->getFog().FogColor);
	cbuffer.TextureParams[0] = getTextureParams(material, 0, renderUnit->textures[0]);
	cbuffer.TextureParams[1] = getTextureParams(material, 1, renderUnit->textures[1]);
	if(block.alphaTestEnabled)
	{	
		cbuffer.params[0] = block.alphaTestEnabled ? 1.0f : 0.0f;
		cbuffer.params[1] = block.alphaTestRef / 255.0f;
	}

	u32 size = block.alphaTestEnabled ? sizeof(cbuffer) : sizeof(cbuffer) - sizeof(cbuffer.params);
	shaderServices->setShaderUniformF("g_psbuffer", (const f32*)&cbuffer, size);

	services->setSampler_Texture(0, renderUnit->textures[0]);
	services->setSampler_Texture(1, renderUnit->textures[1]);
}

void CGLES2_PS_ETC1::MapObjTwoLayerOpaque_setShaderConst( IPixelShader* ps, const SMaterial& material, u32 pass )
{
	CGLES2MaterialRenderServices* services = static_cast<CGLES2MaterialRenderServices*>(g_Engine->getDriver()->getMaterialRenderServices());
	CGLES2ShaderServices* shaderServices = static_cast<CGLES2ShaderServices*>(g_Engine->getDriver()->getShaderServices());
	CGLES2SceneStateServices* sceneStateServices = static_cast<CGLES2SceneStateServices*>(g_Engine->getDriver()->getSceneStateServices());
	const SRenderUnit* renderUnit = g_Engine->getSceneRenderServices()->getCurrentUnit();

	// param
	MapObjOpaque_CB cbuffer;
	cbuffer.AmbientColor = material.Lighting ? SColorf(sceneStateServices->getAmbientLight()) * material.AmbientColor : material.EmissiveColor;
	cbuffer.FogColor = SColorf(sceneStateServices->getFog().FogColor);
	cbuffer.TextureParams[0] = getTextureParams(material, 0, renderUnit->textures[0]);
	cbuffer.TextureParams[1] = getTextureParams(material, 1, renderUnit->textures[1]);

	shaderServices->setShaderUniformF("g_psbuffer", (const f32*)&cbuffer, sizeof(cbuffer));

	services->setSampler_Texture(0, renderUnit->textures[0]);
	services->setSampler_Texture(1, renderUnit->textures[1]);
}


#endif
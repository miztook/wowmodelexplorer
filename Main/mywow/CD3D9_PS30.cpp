#include "stdafx.h"
#include "CD3D9_PS30.h"
#include "mywow.h"
#include "CFileADT.h"

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D9

#include "CD3D9Shader.h"
#include "CD3D9Texture.h"
#include "CD3D9ShaderServices.h"
#include "CD3D9MaterialRenderServices.h"
#include "CD3D9SceneStateServices.h"

static CD3D9_PS30::SPixelShaderEntry g_d3d9ps30_pixelShaderEntries[] =
{
	{EPST_DEFAULT_P, "Default_P", NULL_PTR},
	{EPST_DEFAULT_PC, "Default_PC", NULL_PTR},
	{EPST_DEFAULT_PCT, "Default_PCT", CD3D9_PS30::Default_setShaderConst_T1},
	{EPST_DEFAULT_PN, "Default_PN", NULL_PTR},
	{EPST_DEFAULT_PNC, "Default_PNC", NULL_PTR},
	{EPST_DEFAULT_PNCT, "Default_PNCT", CD3D9_PS30::Default_setShaderConst_T1},
	{EPST_DEFAULT_PNCT2, "Default_PNCT2", CD3D9_PS30::Default_setShaderConst_T2},
	{EPST_DEFAULT_PNT, "Default_PNT", CD3D9_PS30::Default_setShaderConst_T1},
	{EPST_DEFAULT_PT, "Default_PT", CD3D9_PS30::Default_setShaderConst_T1},

	{EPST_UI, "UI", CD3D9_PS30::UI_setShaderConst},
	{EPST_UI_ALPHA, "UI_Alpha", CD3D9_PS30::UI_setShaderConst},	
	{EPST_UI_ALPHACHANNEL, "UI_AlphaChannel", CD3D9_PS30::UI_setShaderConst},
	{EPST_UI_ALPHA_ALPHACHANNEL, "UI_Alpha_AlphaChannel", CD3D9_PS30::UI_setShaderConst},
	{EPST_TERRAIN_1LAYER, "Terrain_1Layer", CD3D9_PS30::Terrain_setShaderConst},
	{EPST_TERRAIN_2LAYER, "Terrain_2Layer", CD3D9_PS30::Terrain_setShaderConst},
	{EPST_TERRAIN_3LAYER, "Terrain_3Layer", CD3D9_PS30::Terrain_setShaderConst},
	{EPST_TERRAIN_4LAYER, "Terrain_4Layer", CD3D9_PS30::Terrain_setShaderConst},

	{EPST_COMBINERS_MOD, "Combiners_Mod", CD3D9_PS30::DiffuseT1_setShaderConst},
	{EPST_COMBINERS_MOD_ADD, "Combiners_Mod_Add", CD3D9_PS30::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_MOD_ADD_ALPHA, "Combiners_Mod_Add_Alpha", CD3D9_PS30::DiffuseT3_setShaderConst},
	{EPST_COMBINERS_MOD_ADDALPHA, "Combiners_Mod_AddAlpha", CD3D9_PS30::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_MOD_ADDALPHA_ALPHA, "Combiners_Mod_AddAlpha_Alpha", CD3D9_PS30::DiffuseT3_setShaderConst},
	{EPST_COMBINERS_MOD_ADDALPHA_WGT, "Combiners_Mod_AddAlpha_Wgt", CD3D9_PS30::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_MOD_ADDNA, "Combiners_Mod_AddNA", CD3D9_PS30::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_MOD_MOD, "Combiners_Mod_Mod", CD3D9_PS30::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_MOD_MOD2X, "Combiners_Mod_Mod2x", CD3D9_PS30::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_MOD_MOD2XNA, "Combiners_Mod_Mod2xNA", CD3D9_PS30::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_MOD_OPAQUE, "Combiners_Mod_Opaque", CD3D9_PS30::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_OPAQUE, "Combiners_Opaque", CD3D9_PS30::DiffuseT1_setShaderConst},
	{EPST_COMBINERS_OPAQUE_ADDALPHA, "Combiners_Opaque_AddAlpha", CD3D9_PS30::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_OPAQUE_ADDALPHA_ALPHA, "Combiners_Opaque_AddAlpha_Alpha", CD3D9_PS30::DiffuseT3_setShaderConst},
	{EPST_COMBINERS_OPAQUE_ADDALPHA_WGT, "Combiners_Opaque_AddAlpha_Wgt", CD3D9_PS30::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_OPAQUE_ALPHA, "Combiners_Opaque_Alpha", CD3D9_PS30::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_OPAQUE_ALPHA_ALPHA, "Combiners_Opaque_Alpha_Alpha", CD3D9_PS30::DiffuseT3_setShaderConst},
	{EPST_COMBINERS_OPAQUE_MOD, "Combiners_Opaque_Mod", CD3D9_PS30::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_OPAQUE_MOD2X, "Combiners_Opaque_Mod2x", CD3D9_PS30::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_OPAQUE_MOD2XNA, "Combiners_Opaque_Mod2xNA", CD3D9_PS30::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_OPAQUE_MOD2XNA_ALPHA, "Combiners_Opaque_Mod2xNA_Alpha", CD3D9_PS30::DiffuseT3_setShaderConst},
	{EPST_COMBINERS_OPAQUE_MOD2XNA_ALPHA_3S, "Combiners_Opaque_Mod2xNA_Alpha_3s", CD3D9_PS30::DiffuseT3_setShaderConst},
	{EPST_COMBINERS_OPAQUE_MOD2XNA_ALPHA_ADD, "Combiners_Opaque_Mod2xNA_Alpha_Add", CD3D9_PS30::DiffuseT3_setShaderConst},
	{EPST_COMBINERS_OPAQUE_MOD2XNA_ALPHA_ALPHA, "Combiners_Opaque_Mod2xNA_Alpha_Alpha", CD3D9_PS30::DiffuseT3_setShaderConst},
	{EPST_COMBINERS_OPAQUE_MOD2XNA_ALPHA_UNSHALPHA, "Combiners_Opaque_Mod2xNA_Alpha_UnshAlpha", CD3D9_PS30::DiffuseT3_setShaderConst},
	{EPST_COMBINERS_OPAQUE_MOD_ADD_WGT, "Combiners_Opaque_Mod_Add_Wgt", CD3D9_PS30::DiffuseT3_setShaderConst},
	{EPST_COMBINERS_OPAQUE_MODNA_ALPHA, "Combiners_Opaque_ModNA_Alpha", CD3D9_PS30::DiffuseT3_setShaderConst},
	{EPST_COMBINERS_OPAQUE_OPAQUE, "Combiners_Opaque_Opaque", CD3D9_PS30::DiffuseT2_setShaderConst},

	{EPST_MAPOBJ_DIFFUSE, "MapObjDiffuse", CD3D9_PS30::MapObj_setShaderConst},
	{EPST_MAPOBJ_DIFFUSEEMISSIVE, "MapObjDiffuseEmissive", CD3D9_PS30::MapObj_setShaderConst},
	{EPST_MAPOBJ_OPAQUE, "MapObjOpaque", CD3D9_PS30::MapObj_setShaderConst},
	{EPST_MAPOBJ_SPECULAR, "MapObjSpecular", CD3D9_PS30::MapObj_setShaderConst},
	{EPST_MAPOBJ_METAL, "MapObjMetal", CD3D9_PS30::MapObj_setShaderConst},
	{EPST_MAPOBJ_TWOLAYERDIFFUSE, "MapObjTwoLayerDiffuse", CD3D9_PS30::MapObjTwoLayer_setShaderConst},
	{EPST_MAPOBJ_ENVMETAL, "MapObjEnvMetal", CD3D9_PS30::MapObjTwoLayer_setShaderConst},
	{EPST_MAPOBJ_ENV, "MapObjEnv", CD3D9_PS30::MapObjTwoLayer_setShaderConst},
	{EPST_MAPOBJ_TWOLAYERDIFFUSEOPAQUE, "MapObjTwoLayerDiffuseOpaque", CD3D9_PS30::MapObjTwoLayer_setShaderConst},
	{EPST_MAPOBJ_TWOLAYERENVMETAL, "MapObjTwoLayerEnvMetal", CD3D9_PS30::MapObjTwoLayer_setShaderConst},
	{EPST_MAPOBJ_TWOLAYERTERRAIN, "MapObjTwoLayerTerrain", CD3D9_PS30::MapObjTwoLayer_setShaderConst},

};

void CD3D9_PS30::loadAll(CD3D9ShaderServices* shaderServices)
{
	u32 num = sizeof(g_d3d9ps30_pixelShaderEntries) / sizeof(SPixelShaderEntry);
	for (u32 i=0; i<num; ++i)
	{
		for (u32 k=PS_Macro_None; k<PS_Macro_Num; ++k)
			loadPShaderHLSL(shaderServices, g_d3d9ps30_pixelShaderEntries[i].psType, (E_PS_MACRO)k);
	}
}

bool CD3D9_PS30::loadPShaderHLSL( CD3D9ShaderServices* shaderServices, E_PS_TYPE type, E_PS_MACRO macro )
{
	const c8* profile = "ps_3_0";

	c8 basePath[128];
	Q_sprintf(basePath, 128, "Pixel\\%s\\", profile);

	s32 index = -1;
	u32 num = sizeof(g_d3d9ps30_pixelShaderEntries) / sizeof(SPixelShaderEntry);
	for (u32 i=0; i<num; ++i)
	{
		if (g_d3d9ps30_pixelShaderEntries[i].psType == type)
		{
			index = (s32)i;
			break;
		}
	}

	if (index == -1)
		return false;

	string128 path(basePath);
	path.append(g_d3d9ps30_pixelShaderEntries[index].path.c_str());

#ifdef DIRECTX_USE_COMPILED_SHADER
	path.append(".bls");
#else
	path.append(".fx");
#endif

	path.normalize();

	shaderServices->loadPShaderHLSL(path.c_str(), "main", profile, type, macro, g_d3d9ps30_pixelShaderEntries[index].callback);

	return true;
}

void CD3D9_PS30::Terrain_setShaderConst(IPixelShader* ps, const SMaterial& material, u32 pass)
{
	CD3D9PixelShader* d3d9ps = static_cast<CD3D9PixelShader*>(ps);
	ISceneRenderServices* sceneRenderServices = g_Engine->getSceneRenderServices();
	const CMapChunk* chunk = reinterpret_cast<const CMapChunk*>(sceneRenderServices->getCurrentUnit()->u.chunk);
	const CFileADT* adt = reinterpret_cast<const CFileADT*>(sceneRenderServices->getCurrentUnit()->u.adt);
	CD3D9SceneStateServices* sceneStateServices = static_cast<CD3D9SceneStateServices*>(g_Engine->getDriver()->getSceneStateServices());
	const SFogParam& fogParam = sceneStateServices->getFog();

	// param
	Terrain_CB cbuffer;
	cbuffer.params[0] =  (float)chunk->numTextures;
	cbuffer.fogColor = SColorf(fogParam.FogColor);

	//set
	d3d9ps->setShaderConstant("g_cbuffer", &cbuffer, sizeof(Terrain_CB));

	//texture
	d3d9ps->setTextureConstant("g_TexSampler0", adt->getBlendMap());
	d3d9ps->setTextureConstant("g_TexSampler1", chunk->textures[0]);

	if(chunk->numTextures > 1)
	{
		d3d9ps->setTextureConstant("g_TexSampler2", chunk->textures[1]);
	}

	if(chunk->numTextures > 2)
	{
		d3d9ps->setTextureConstant("g_TexSampler3", chunk->textures[2]);
	}

	if(chunk->numTextures > 3)
	{
		d3d9ps->setTextureConstant("g_TexSampler4", chunk->textures[3]);
	}
}

void CD3D9_PS30::UI_setShaderConst( IPixelShader* ps, const SMaterial& material, u32 pass )
{
	CD3D9PixelShader* d3d9ps = static_cast<CD3D9PixelShader*>(ps);
	CD3D9MaterialRenderServices* services = static_cast<CD3D9MaterialRenderServices*>(g_Engine->getDriver()->getMaterialRenderServices());

	d3d9ps->setTextureConstant("g_TexSampler0", services->getSampler_Texture(0));
}

void CD3D9_PS30::Default_setShaderConst_T1( IPixelShader* ps, const SMaterial& material, u32 pass)
{
	//texture
	CD3D9PixelShader* d3d9ps = static_cast<CD3D9PixelShader*>(ps);
	const SRenderUnit* renderUnit = g_Engine->getSceneRenderServices()->getCurrentUnit();

	d3d9ps->setTextureConstant("g_TexSampler0", renderUnit->textures[0]);
}

void CD3D9_PS30::Default_setShaderConst_T2( IPixelShader* ps, const SMaterial& material, u32 pass)
{
	//texture
	CD3D9PixelShader* d3d9ps = static_cast<CD3D9PixelShader*>(ps);
	const SRenderUnit* renderUnit = g_Engine->getSceneRenderServices()->getCurrentUnit();

	d3d9ps->setTextureConstant("g_TexSampler0", renderUnit->textures[0]);

	d3d9ps->setTextureConstant("g_TexSampler1", renderUnit->textures[1]);
}

void CD3D9_PS30::MapObj_setShaderConst( IPixelShader* ps, const SMaterial& material, u32 pass )
{
	CD3D9PixelShader* d3d9ps = static_cast<CD3D9PixelShader*>(ps);
	CD3D9SceneStateServices* sceneStateServices = static_cast<CD3D9SceneStateServices*>(g_Engine->getDriver()->getSceneStateServices());
	const SRenderUnit* renderUnit = g_Engine->getSceneRenderServices()->getCurrentUnit();

	// param
	MapObj_CB cbuffer;
	cbuffer.AmbientColor = material.Lighting ? SColorf(sceneStateServices->getAmbientLight()) * material.AmbientColor : material.EmissiveColor;
	cbuffer.FogColor = SColorf(sceneStateServices->getFog().FogColor);

	//set
	d3d9ps->setShaderConstant("g_cbuffer", &cbuffer, sizeof(MapObj_CB));

	//texture
	d3d9ps->setTextureConstant("g_TexSampler0", renderUnit->textures[0]);
}

void CD3D9_PS30::MapObjTwoLayer_setShaderConst( IPixelShader* ps, const SMaterial& material, u32 pass )
{
	CD3D9PixelShader* d3d9ps = static_cast<CD3D9PixelShader*>(ps);
	CD3D9SceneStateServices* sceneStateServices = static_cast<CD3D9SceneStateServices*>(g_Engine->getDriver()->getSceneStateServices());
	const SRenderUnit* renderUnit = g_Engine->getSceneRenderServices()->getCurrentUnit();

	// param
	MapObj_CB cbuffer;
	cbuffer.AmbientColor = material.Lighting ? SColorf(sceneStateServices->getAmbientLight()) * material.AmbientColor : material.EmissiveColor;
	cbuffer.FogColor = SColorf(sceneStateServices->getFog().FogColor);

	//set
	d3d9ps->setShaderConstant("g_cbuffer", &cbuffer, sizeof(MapObj_CB));

	//texture
	d3d9ps->setTextureConstant("g_TexSampler0", renderUnit->textures[0]);

	d3d9ps->setTextureConstant("g_TexSampler1", renderUnit->textures[1]);
}

void CD3D9_PS30::DiffuseT1_setShaderConst( IPixelShader* ps, const SMaterial& material, u32 pass )
{
	CD3D9PixelShader* d3d9ps = static_cast<CD3D9PixelShader*>(ps);
	CD3D9SceneStateServices* sceneStateServices = static_cast<CD3D9SceneStateServices*>(g_Engine->getDriver()->getSceneStateServices());
	const SRenderUnit* renderUnit = g_Engine->getSceneRenderServices()->getCurrentUnit();

	// param
	Diffuse_CB cbuffer;
	cbuffer.FogColor = SColorf(sceneStateServices->getFog().FogColor);

	//set
	d3d9ps->setShaderConstant("g_cbuffer", &cbuffer, sizeof(Diffuse_CB));

	//texture
	d3d9ps->setTextureConstant("g_TexSampler0", renderUnit->textures[0]);
}

void CD3D9_PS30::DiffuseT2_setShaderConst( IPixelShader* ps, const SMaterial& material, u32 pass )
{
	CD3D9PixelShader* d3d9ps = static_cast<CD3D9PixelShader*>(ps);
	CD3D9SceneStateServices* sceneStateServices = static_cast<CD3D9SceneStateServices*>(g_Engine->getDriver()->getSceneStateServices());
	const SRenderUnit* renderUnit = g_Engine->getSceneRenderServices()->getCurrentUnit();

	// param
	Diffuse_CB cbuffer;
	cbuffer.FogColor = SColorf(sceneStateServices->getFog().FogColor);

	//set
	d3d9ps->setShaderConstant("g_cbuffer", &cbuffer, sizeof(Diffuse_CB));

	//texture
	d3d9ps->setTextureConstant("g_TexSampler0", renderUnit->textures[0]);

	d3d9ps->setTextureConstant("g_TexSampler1", renderUnit->textures[1]);
}

void CD3D9_PS30::DiffuseT3_setShaderConst( IPixelShader* ps, const SMaterial& material, u32 pass )
{
	CD3D9PixelShader* d3d9ps = static_cast<CD3D9PixelShader*>(ps);
	CD3D9SceneStateServices* sceneStateServices = static_cast<CD3D9SceneStateServices*>(g_Engine->getDriver()->getSceneStateServices());
	const SRenderUnit* renderUnit = g_Engine->getSceneRenderServices()->getCurrentUnit();

	// param
	Diffuse_CB cbuffer;
	cbuffer.FogColor = SColorf(sceneStateServices->getFog().FogColor);

	//set
	d3d9ps->setShaderConstant("g_cbuffer", &cbuffer, sizeof(Diffuse_CB));

	//texture
	d3d9ps->setTextureConstant("g_TexSampler0", renderUnit->textures[0]);

	d3d9ps->setTextureConstant("g_TexSampler1", renderUnit->textures[1]);

	d3d9ps->setTextureConstant("g_TexSampler2", renderUnit->textures[2]);
}

#endif
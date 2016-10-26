#include "stdafx.h"
#include "CD3D11_PS40.h"
#include "mywow.h"
#include "CFileADT.h"

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D11

#include "CD3D11Shader.h"
#include "CD3D11Texture.h"
#include "CD3D11SceneStateServices.h"
#include "CD3D11ShaderServices.h"
#include "CD3D11SceneStateServices.h"
#include "CD3D11MaterialRenderServices.h"

static CD3D11_PS40::SPixelShaderEntry g_d3d11ps40_pixelShaderEntries[] =
{
	{EPST_DEFAULT_P, "Default_P", NULL_PTR},
	{EPST_DEFAULT_PC, "Default_PC", NULL_PTR},
	{EPST_DEFAULT_PCT, "Default_PCT", CD3D11_PS40::Default_setShaderConst_T1},
	{EPST_DEFAULT_PN, "Default_PN", NULL_PTR},
	{EPST_DEFAULT_PNC, "Default_PNC", NULL_PTR},
	{EPST_DEFAULT_PNCT, "Default_PNCT", CD3D11_PS40::Default_setShaderConst_T1},
	{EPST_DEFAULT_PNCT2, "Default_PNCT2", CD3D11_PS40::Default_setShaderConst_T2},
	{EPST_DEFAULT_PNT, "Default_PNT", CD3D11_PS40::Default_setShaderConst_T1},
	{EPST_DEFAULT_PT, "Default_PT", CD3D11_PS40::Default_setShaderConst_T1},

	{EPST_UI, "UI", CD3D11_PS40::UI_setShaderConst},
	{EPST_UI_ALPHA, "UI_Alpha", CD3D11_PS40::UI_setShaderConst},	
	{EPST_UI_ALPHACHANNEL, "UI_AlphaChannel", CD3D11_PS40::UI_setShaderConst},
	{EPST_UI_ALPHA_ALPHACHANNEL, "UI_Alpha_AlphaChannel", CD3D11_PS40::UI_setShaderConst},
	{EPST_TERRAIN_1LAYER, "Terrain_1Layer", CD3D11_PS40::Terrain_setShaderConst},
	{EPST_TERRAIN_2LAYER, "Terrain_2Layer", CD3D11_PS40::Terrain_setShaderConst},
	{EPST_TERRAIN_3LAYER, "Terrain_3Layer", CD3D11_PS40::Terrain_setShaderConst},
	{EPST_TERRAIN_4LAYER, "Terrain_4Layer", CD3D11_PS40::Terrain_setShaderConst},

	{EPST_COMBINERS_MOD, "Combiners_Mod", CD3D11_PS40::DiffuseT1_setShaderConst},
	{EPST_COMBINERS_MOD_ADD, "Combiners_Mod_Add", CD3D11_PS40::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_MOD_ADD_ALPHA, "Combiners_Mod_Add_Alpha", CD3D11_PS40::DiffuseT3_setShaderConst},
	{EPST_COMBINERS_MOD_ADDALPHA, "Combiners_Mod_AddAlpha", CD3D11_PS40::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_MOD_ADDALPHA_ALPHA, "Combiners_Mod_AddAlpha_Alpha", CD3D11_PS40::DiffuseT3_setShaderConst},
	{EPST_COMBINERS_MOD_ADDALPHA_WGT, "Combiners_Mod_AddAlpha_Wgt", CD3D11_PS40::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_MOD_ADDNA, "Combiners_Mod_AddNA", CD3D11_PS40::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_MOD_MOD, "Combiners_Mod_Mod", CD3D11_PS40::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_MOD_MOD2X, "Combiners_Mod_Mod2x", CD3D11_PS40::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_MOD_MOD2XNA, "Combiners_Mod_Mod2xNA", CD3D11_PS40::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_MOD_OPAQUE, "Combiners_Mod_Opaque", CD3D11_PS40::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_OPAQUE, "Combiners_Opaque", CD3D11_PS40::DiffuseT1_setShaderConst},
	{EPST_COMBINERS_OPAQUE_ADDALPHA, "Combiners_Opaque_AddAlpha", CD3D11_PS40::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_OPAQUE_ADDALPHA_ALPHA, "Combiners_Opaque_AddAlpha_Alpha", CD3D11_PS40::DiffuseT3_setShaderConst},
	{EPST_COMBINERS_OPAQUE_ADDALPHA_WGT, "Combiners_Opaque_AddAlpha_Wgt", CD3D11_PS40::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_OPAQUE_ALPHA, "Combiners_Opaque_Alpha", CD3D11_PS40::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_OPAQUE_ALPHA_ALPHA, "Combiners_Opaque_Alpha_Alpha", CD3D11_PS40::DiffuseT3_setShaderConst},
	{EPST_COMBINERS_OPAQUE_MOD, "Combiners_Opaque_Mod", CD3D11_PS40::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_OPAQUE_MOD2X, "Combiners_Opaque_Mod2x", CD3D11_PS40::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_OPAQUE_MOD2XNA, "Combiners_Opaque_Mod2xNA", CD3D11_PS40::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_OPAQUE_MOD2XNA_ALPHA, "Combiners_Opaque_Mod2xNA_Alpha", CD3D11_PS40::DiffuseT3_setShaderConst},
	{EPST_COMBINERS_OPAQUE_MOD2XNA_ALPHA_3S, "Combiners_Opaque_Mod2xNA_Alpha_3s", CD3D11_PS40::DiffuseT3_setShaderConst},
	{EPST_COMBINERS_OPAQUE_MOD2XNA_ALPHA_ADD, "Combiners_Opaque_Mod2xNA_Alpha_Add", CD3D11_PS40::DiffuseT3_setShaderConst},
	{EPST_COMBINERS_OPAQUE_MOD2XNA_ALPHA_ALPHA, "Combiners_Opaque_Mod2xNA_Alpha_Alpha", CD3D11_PS40::DiffuseT3_setShaderConst},
	{EPST_COMBINERS_OPAQUE_MOD2XNA_ALPHA_UNSHALPHA, "Combiners_Opaque_Mod2xNA_Alpha_UnshAlpha", CD3D11_PS40::DiffuseT3_setShaderConst},
	{EPST_COMBINERS_OPAQUE_MOD_ADD_WGT, "Combiners_Opaque_Mod_Add_Wgt", CD3D11_PS40::DiffuseT3_setShaderConst},
	{EPST_COMBINERS_OPAQUE_MODNA_ALPHA, "Combiners_Opaque_ModNA_Alpha", CD3D11_PS40::DiffuseT3_setShaderConst},
	{EPST_COMBINERS_OPAQUE_OPAQUE, "Combiners_Opaque_Opaque", CD3D11_PS40::DiffuseT2_setShaderConst},

	{EPST_MAPOBJ_DIFFUSE, "MapObjDiffuse", CD3D11_PS40::MapObj_setShaderConst},
	{EPST_MAPOBJ_DIFFUSEEMISSIVE, "MapObjDiffuseEmissive", CD3D11_PS40::MapObj_setShaderConst},
	{EPST_MAPOBJ_OPAQUE, "MapObjOpaque", CD3D11_PS40::MapObj_setShaderConst},
	{EPST_MAPOBJ_SPECULAR, "MapObjSpecular", CD3D11_PS40::MapObj_setShaderConst},
	{EPST_MAPOBJ_METAL, "MapObjMetal", CD3D11_PS40::MapObj_setShaderConst},
	{EPST_MAPOBJ_TWOLAYERDIFFUSE, "MapObjTwoLayerDiffuse", CD3D11_PS40::MapObjTwoLayer_setShaderConst},
	{EPST_MAPOBJ_ENVMETAL, "MapObjEnvMetal", CD3D11_PS40::MapObjTwoLayer_setShaderConst},
	{EPST_MAPOBJ_ENV, "MapObjEnv", CD3D11_PS40::MapObjTwoLayer_setShaderConst},
	{EPST_MAPOBJ_TWOLAYERDIFFUSEOPAQUE, "MapObjTwoLayerDiffuseOpaque", CD3D11_PS40::MapObjTwoLayer_setShaderConst},
	{EPST_MAPOBJ_TWOLAYERENVMETAL, "MapObjTwoLayerEnvMetal", CD3D11_PS40::MapObjTwoLayer_setShaderConst},
	{EPST_MAPOBJ_TWOLAYERTERRAIN, "MapObjTwoLayerTerrain", CD3D11_PS40::MapObjTwoLayer_setShaderConst},

};

void CD3D11_PS40::loadAll(CD3D11ShaderServices* shaderServices)
{
	u32 num = sizeof(g_d3d11ps40_pixelShaderEntries) / sizeof(SPixelShaderEntry);
	for (u32 i=0; i<num; ++i)
	{
		for (u32 k=PS_Macro_None; k<PS_Macro_Num; ++k)
			loadPShaderHLSL(shaderServices, g_d3d11ps40_pixelShaderEntries[i].psType, (E_PS_MACRO)k);
	}
}

bool CD3D11_PS40::loadPShaderHLSL( CD3D11ShaderServices* shaderServices, E_PS_TYPE type, E_PS_MACRO macro )
{
	const c8* profile = "ps_4_0";

	//load

	c8 basePath[128];
	Q_sprintf(basePath, 128, "Pixel\\%s\\", profile);

	s32 index = -1;
	u32 num = sizeof(g_d3d11ps40_pixelShaderEntries) / sizeof(SPixelShaderEntry);
	for (u32 i=0; i<num; ++i)
	{
		if (g_d3d11ps40_pixelShaderEntries[i].psType == type)
		{
			index = (s32)i;
			break;
		}
	}

	if (index == -1)
		return false;

	string128 path(basePath);
	path.append(g_d3d11ps40_pixelShaderEntries[index].path.c_str());

#ifdef DIRECTX_USE_COMPILED_SHADER
	path.append(".bls");
#else
	path.append(".fx");
#endif

	path.normalize();

	shaderServices->loadPShaderHLSL(path.c_str(), "main", profile, type, macro, g_d3d11ps40_pixelShaderEntries[index].callback);

	return true;
}

void CD3D11_PS40::Terrain_setShaderConst( IPixelShader* ps, const SMaterial& material, u32 pass )
{
	CD3D11PixelShader* d3d11ps = static_cast<CD3D11PixelShader*>(ps);
	ISceneRenderServices* sceneRenderServices = g_Engine->getSceneRenderServices();
	CD3D11SceneStateServices* sceneStateServices = static_cast<CD3D11SceneStateServices*>(g_Engine->getDriver()->getSceneStateServices());
	const CMapChunk* chunk = reinterpret_cast<const CMapChunk*>(sceneRenderServices->getCurrentUnit()->u.chunk);
	const CFileADT* adt = reinterpret_cast<const CFileADT*>(sceneRenderServices->getCurrentUnit()->u.adt);
	const SFogParam& fogParam = sceneStateServices->getFog();

	// param
	Terrain_CB cbuffer;
	cbuffer.params[0] =  (float)chunk->numTextures;
	cbuffer.fogColor = SColorf(fogParam.FogColor);

	//set
	d3d11ps->setShaderConstant("cb0", &cbuffer, sizeof(Terrain_CB));

	//texture
	d3d11ps->setTextureConstant("g_Tex0", adt->getBlendMap());
	d3d11ps->setTextureConstant("g_Tex1", chunk->textures[0]);

	if(chunk->numTextures > 1)
	{
		d3d11ps->setTextureConstant("g_Tex2", chunk->textures[1]);
	}

	if(chunk->numTextures > 2)
	{
		d3d11ps->setTextureConstant("g_Tex3", chunk->textures[2]);
	}

	if(chunk->numTextures > 3)
	{
		d3d11ps->setTextureConstant("g_Tex4", chunk->textures[3]);
	}
}

void CD3D11_PS40::UI_setShaderConst( IPixelShader* ps, const SMaterial& material, u32 pass )
{
	CD3D11PixelShader* d3d11ps = static_cast<CD3D11PixelShader*>(ps);
	CD3D11MaterialRenderServices* materialRenderServices = static_cast<CD3D11MaterialRenderServices*>(g_Engine->getDriver()->getMaterialRenderServices());

	d3d11ps->setTextureConstant("g_Tex0", materialRenderServices->getSampler_Texture(0));
}

void CD3D11_PS40::Default_setShaderConst_T1( IPixelShader* ps, const SMaterial& material, u32 pass )
{
	//texture
	CD3D11PixelShader* d3d11ps = static_cast<CD3D11PixelShader*>(ps);
	CD3D11MaterialRenderServices* materialRenderServices = static_cast<CD3D11MaterialRenderServices*>(g_Engine->getDriver()->getMaterialRenderServices());
	const SRenderUnit* renderUnit = g_Engine->getSceneRenderServices()->getCurrentUnit();
	const SRenderStateBlock& block = materialRenderServices->getRenderStateBlock();
	
	// param
	Common_CB cbuffer;
	cbuffer.params[0] = block.alphaTestEnabled ? 1.0f : 0.0f;
	if(block.alphaTestEnabled)
		cbuffer.params[1] = block.alphaTestRef / 255.0f;

	//set
	d3d11ps->setShaderConstant("cb0", &cbuffer, sizeof(Common_CB));

	d3d11ps->setTextureConstant("g_Tex0", renderUnit->textures[0]);
}

void CD3D11_PS40::Default_setShaderConst_T2( IPixelShader* ps, const SMaterial& material, u32 pass )
{
	//texture
	CD3D11PixelShader* d3d11ps = static_cast<CD3D11PixelShader*>(ps);
	CD3D11MaterialRenderServices* materialRenderServices = static_cast<CD3D11MaterialRenderServices*>(g_Engine->getDriver()->getMaterialRenderServices());
	const SRenderUnit* renderUnit = g_Engine->getSceneRenderServices()->getCurrentUnit();
	const SRenderStateBlock& block = materialRenderServices->getRenderStateBlock();

	// param
	Common_CB cbuffer;
	cbuffer.params[0] = block.alphaTestEnabled ? 1.0f : 0.0f;
	if(block.alphaTestEnabled)
		cbuffer.params[1] = block.alphaTestRef / 255.0f;

	//set
	d3d11ps->setShaderConstant("cb0", &cbuffer, sizeof(Common_CB));

	d3d11ps->setTextureConstant("g_Tex0", renderUnit->textures[0]);

	d3d11ps->setTextureConstant("g_Tex1", renderUnit->textures[1]);
}

void CD3D11_PS40::MapObjOpaque_setShaderConst( IPixelShader* ps, const SMaterial& material, u32 pass )
{
	CD3D11PixelShader* d3d11ps = static_cast<CD3D11PixelShader*>(ps);
	CD3D11SceneStateServices* sceneStateServices = static_cast<CD3D11SceneStateServices*>(g_Engine->getDriver()->getSceneStateServices());
	const SRenderUnit* renderUnit = g_Engine->getSceneRenderServices()->getCurrentUnit();

	// param
	MapObjOpaque_CB cbuffer;
	cbuffer.AmbientColor = material.Lighting ? SColorf(sceneStateServices->getAmbientLight()) * material.AmbientColor : material.EmissiveColor;
	cbuffer.FogColor = SColorf(sceneStateServices->getFog().FogColor);

	//set
	d3d11ps->setShaderConstant("cb0", &cbuffer, sizeof(MapObjOpaque_CB));

	//texture
	d3d11ps->setTextureConstant("g_Tex0", renderUnit->textures[0]);
}

void CD3D11_PS40::MapObj_setShaderConst( IPixelShader* ps, const SMaterial& material, u32 pass )
{
	CD3D11PixelShader* d3d11ps = static_cast<CD3D11PixelShader*>(ps);
	CD3D11MaterialRenderServices* materialRenderServices = static_cast<CD3D11MaterialRenderServices*>(g_Engine->getDriver()->getMaterialRenderServices());
	CD3D11SceneStateServices* sceneStateServices = static_cast<CD3D11SceneStateServices*>(g_Engine->getDriver()->getSceneStateServices());
	const SRenderUnit* renderUnit = g_Engine->getSceneRenderServices()->getCurrentUnit();
	const SRenderStateBlock& block = materialRenderServices->getRenderStateBlock();

	// param
	MapObjAlphaTest_CB cbuffer;
	cbuffer.AmbientColor = material.Lighting ? SColorf(sceneStateServices->getAmbientLight()) * material.AmbientColor : material.EmissiveColor;
	cbuffer.FogColor = SColorf(sceneStateServices->getFog().FogColor);
	cbuffer.params[0] = block.alphaTestEnabled ? 1.0f : 0.0f;
	if(block.alphaTestEnabled)
		cbuffer.params[1] = block.alphaTestRef / 255.0f;

	//set
	d3d11ps->setShaderConstant("cb0", &cbuffer, sizeof(MapObjAlphaTest_CB));

	//texture
	d3d11ps->setTextureConstant("g_Tex0", renderUnit->textures[0]);
}

void CD3D11_PS40::MapObjTwoLayer_setShaderConst( IPixelShader* ps, const SMaterial& material, u32 pass )
{
	CD3D11PixelShader* d3d11ps = static_cast<CD3D11PixelShader*>(ps);
	CD3D11MaterialRenderServices* materialRenderServices = static_cast<CD3D11MaterialRenderServices*>(g_Engine->getDriver()->getMaterialRenderServices());
	CD3D11SceneStateServices* sceneStateServices = static_cast<CD3D11SceneStateServices*>(g_Engine->getDriver()->getSceneStateServices());
	const SRenderUnit* renderUnit = g_Engine->getSceneRenderServices()->getCurrentUnit();
	const SRenderStateBlock& block = materialRenderServices->getRenderStateBlock();

	// param
	MapObjAlphaTest_CB cbuffer;
	cbuffer.AmbientColor = material.Lighting ? SColorf(sceneStateServices->getAmbientLight()) * material.AmbientColor : material.EmissiveColor;
	cbuffer.FogColor = SColorf(sceneStateServices->getFog().FogColor);
	cbuffer.params[0] = block.alphaTestEnabled ? 1.0f : 0.0f;
	if(block.alphaTestEnabled)
		cbuffer.params[1] = block.alphaTestRef / 255.0f;

	//set
	d3d11ps->setShaderConstant("cb0", &cbuffer, sizeof(MapObjAlphaTest_CB));

	//texture
	d3d11ps->setTextureConstant("g_Tex0", renderUnit->textures[0]);

	d3d11ps->setTextureConstant("g_Tex1", renderUnit->textures[1]);
}

void CD3D11_PS40::MapObjTwoLayerOpaque_setShaderConst( IPixelShader* ps, const SMaterial& material, u32 pass )
{
	CD3D11PixelShader* d3d11ps = static_cast<CD3D11PixelShader*>(ps);
	CD3D11SceneStateServices* sceneStateServices = static_cast<CD3D11SceneStateServices*>(g_Engine->getDriver()->getSceneStateServices());
	const SRenderUnit* renderUnit = g_Engine->getSceneRenderServices()->getCurrentUnit();

	// param
	MapObjOpaque_CB cbuffer;
	cbuffer.AmbientColor = material.Lighting ? SColorf(sceneStateServices->getAmbientLight()) * material.AmbientColor : material.EmissiveColor;
	cbuffer.FogColor = SColorf(sceneStateServices->getFog().FogColor);

	//set
	d3d11ps->setShaderConstant("cb0", &cbuffer, sizeof(MapObjOpaque_CB));

	//texture
	d3d11ps->setTextureConstant("g_Tex0", renderUnit->textures[0]);

	d3d11ps->setTextureConstant("g_Tex1", renderUnit->textures[1]);

}

void CD3D11_PS40::DiffuseT1_setShaderConst( IPixelShader* ps, const SMaterial& material, u32 pass )
{
	//texture
	CD3D11PixelShader* d3d11ps = static_cast<CD3D11PixelShader*>(ps);
	CD3D11MaterialRenderServices* materialRenderServices = static_cast<CD3D11MaterialRenderServices*>(g_Engine->getDriver()->getMaterialRenderServices());
	CD3D11SceneStateServices* sceneStateServices = static_cast<CD3D11SceneStateServices*>(g_Engine->getDriver()->getSceneStateServices());
	const SRenderUnit* renderUnit = g_Engine->getSceneRenderServices()->getCurrentUnit();
	const SRenderStateBlock& block = materialRenderServices->getRenderStateBlock();

	// param
	Diffuse_CB cbuffer;
	cbuffer.params[0] = block.alphaTestEnabled ? 1.0f : 0.0f;
	if(block.alphaTestEnabled)
		cbuffer.params[1] = block.alphaTestRef / 255.0f;

	cbuffer.fogColor = SColorf(sceneStateServices->getFog().FogColor);

	//set
	d3d11ps->setShaderConstant("cb0", &cbuffer, sizeof(cbuffer));

	d3d11ps->setTextureConstant("g_Tex0", renderUnit->textures[0]);
}

void CD3D11_PS40::DiffuseT2_setShaderConst( IPixelShader* ps, const SMaterial& material, u32 pass )
{
	//texture
	CD3D11PixelShader* d3d11ps = static_cast<CD3D11PixelShader*>(ps);
	CD3D11MaterialRenderServices* materialRenderServices = static_cast<CD3D11MaterialRenderServices*>(g_Engine->getDriver()->getMaterialRenderServices());
	CD3D11SceneStateServices* sceneStateServices = static_cast<CD3D11SceneStateServices*>(g_Engine->getDriver()->getSceneStateServices());
	const SRenderUnit* renderUnit = g_Engine->getSceneRenderServices()->getCurrentUnit();
	const SRenderStateBlock& block = materialRenderServices->getRenderStateBlock();

	// param
	Diffuse_CB cbuffer;
	cbuffer.params[0] = block.alphaTestEnabled ? 1.0f : 0.0f;
	if(block.alphaTestEnabled)
		cbuffer.params[1] = block.alphaTestRef / 255.0f;

	cbuffer.fogColor = SColorf(sceneStateServices->getFog().FogColor);

	//set
	d3d11ps->setShaderConstant("cb0", &cbuffer, sizeof(cbuffer));

	d3d11ps->setTextureConstant("g_Tex0", renderUnit->textures[0]);

	d3d11ps->setTextureConstant("g_Tex1", renderUnit->textures[1]);
}

void CD3D11_PS40::DiffuseT3_setShaderConst( IPixelShader* ps, const SMaterial& material, u32 pass )
{
	//texture
	CD3D11PixelShader* d3d11ps = static_cast<CD3D11PixelShader*>(ps);
	CD3D11MaterialRenderServices* materialRenderServices = static_cast<CD3D11MaterialRenderServices*>(g_Engine->getDriver()->getMaterialRenderServices());
	CD3D11SceneStateServices* sceneStateServices = static_cast<CD3D11SceneStateServices*>(g_Engine->getDriver()->getSceneStateServices());
	const SRenderUnit* renderUnit = g_Engine->getSceneRenderServices()->getCurrentUnit();
	const SRenderStateBlock& block = materialRenderServices->getRenderStateBlock();

	// param
	Diffuse_CB cbuffer;
	cbuffer.params[0] = block.alphaTestEnabled ? 1.0f : 0.0f;
	if(block.alphaTestEnabled)
		cbuffer.params[1] = block.alphaTestRef / 255.0f;

	cbuffer.fogColor = SColorf(sceneStateServices->getFog().FogColor);

	//set
	d3d11ps->setShaderConstant("cb0", &cbuffer, sizeof(cbuffer));

	d3d11ps->setTextureConstant("g_Tex0", renderUnit->textures[0]);

	d3d11ps->setTextureConstant("g_Tex1", renderUnit->textures[1]);

	d3d11ps->setTextureConstant("g_Tex2", renderUnit->textures[2]);

}

#endif
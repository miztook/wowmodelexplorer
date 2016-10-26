#include "stdafx.h"
#include "COpenGL_PS15.h"
#include "mywow.h"
#include "CFileADT.h"

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_OPENGL

#include "COpenGLShader.h"
#include "COpenGLShaderServices.h"
#include "COpenGLDriver.h"
#include "COpenGLMaterialRenderServices.h"
#include "COpenGLSceneStateServices.h"

static COpenGL_PS15::SPixelShaderEntry g_openglps15_pixelShaderEntries[] =
{
	{EPST_DEFAULT_P, "Default_P", NULL_PTR},
	{EPST_DEFAULT_PC, "Default_PC", NULL_PTR},
	{EPST_DEFAULT_PCT, "Default_PCT", COpenGL_PS15::Default_setShaderConst_T1},
	{EPST_DEFAULT_PN, "Default_PN", NULL_PTR},
	{EPST_DEFAULT_PNC, "Default_PNC", NULL_PTR},
	{EPST_DEFAULT_PNCT, "Default_PNCT", COpenGL_PS15::Default_setShaderConst_T1},
	{EPST_DEFAULT_PNCT2, "Default_PNCT2", COpenGL_PS15::Default_setShaderConst_T2},
	{EPST_DEFAULT_PNT, "Default_PNT", COpenGL_PS15::Default_setShaderConst_T1},
	{EPST_DEFAULT_PT, "Default_PT", COpenGL_PS15::Default_setShaderConst_T1},

	{EPST_UI, "UI", COpenGL_PS15::UI_setShaderConst},
	{EPST_UI_ALPHA, "UI_Alpha", COpenGL_PS15::UI_setShaderConst},	
	{EPST_UI_ALPHACHANNEL, "UI_AlphaChannel", COpenGL_PS15::UI_setShaderConst},
	{EPST_UI_ALPHA_ALPHACHANNEL, "UI_Alpha_AlphaChannel", COpenGL_PS15::UI_setShaderConst},
	{EPST_TERRAIN_1LAYER, "Terrain_1Layer", COpenGL_PS15::Terrain_setShaderConst},
	{EPST_TERRAIN_2LAYER, "Terrain_2Layer", COpenGL_PS15::Terrain_setShaderConst},
	{EPST_TERRAIN_3LAYER, "Terrain_3Layer", COpenGL_PS15::Terrain_setShaderConst},
	{EPST_TERRAIN_4LAYER, "Terrain_4Layer", COpenGL_PS15::Terrain_setShaderConst},

	{EPST_COMBINERS_MOD, "Combiners_Mod", COpenGL_PS15::DiffuseT1_setShaderConst},
	{EPST_COMBINERS_MOD_ADD, "Combiners_Mod_Add", COpenGL_PS15::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_MOD_ADD_ALPHA, "Combiners_Mod_Add_Alpha", COpenGL_PS15::DiffuseT3_setShaderConst},
	{EPST_COMBINERS_MOD_ADDALPHA, "Combiners_Mod_AddAlpha", COpenGL_PS15::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_MOD_ADDALPHA_ALPHA, "Combiners_Mod_AddAlpha_Alpha", COpenGL_PS15::DiffuseT3_setShaderConst},
	{EPST_COMBINERS_MOD_ADDALPHA_WGT, "Combiners_Mod_AddAlpha_Wgt", COpenGL_PS15::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_MOD_ADDNA, "Combiners_Mod_AddNA", COpenGL_PS15::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_MOD_MOD, "Combiners_Mod_Mod", COpenGL_PS15::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_MOD_MOD2X, "Combiners_Mod_Mod2x", COpenGL_PS15::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_MOD_MOD2XNA, "Combiners_Mod_Mod2xNA", COpenGL_PS15::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_MOD_OPAQUE, "Combiners_Mod_Opaque", COpenGL_PS15::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_OPAQUE, "Combiners_Opaque", COpenGL_PS15::DiffuseT1_setShaderConst},
	{EPST_COMBINERS_OPAQUE_ADDALPHA, "Combiners_Opaque_AddAlpha", COpenGL_PS15::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_OPAQUE_ADDALPHA_ALPHA, "Combiners_Opaque_AddAlpha_Alpha", COpenGL_PS15::DiffuseT3_setShaderConst},
	{EPST_COMBINERS_OPAQUE_ADDALPHA_WGT, "Combiners_Opaque_AddAlpha_Wgt", COpenGL_PS15::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_OPAQUE_ALPHA, "Combiners_Opaque_Alpha", COpenGL_PS15::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_OPAQUE_ALPHA_ALPHA, "Combiners_Opaque_Alpha_Alpha", COpenGL_PS15::DiffuseT3_setShaderConst},
	{EPST_COMBINERS_OPAQUE_MOD, "Combiners_Opaque_Mod", COpenGL_PS15::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_OPAQUE_MOD2X, "Combiners_Opaque_Mod2x", COpenGL_PS15::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_OPAQUE_MOD2XNA, "Combiners_Opaque_Mod2xNA", COpenGL_PS15::DiffuseT2_setShaderConst},
	{EPST_COMBINERS_OPAQUE_MOD2XNA_ALPHA, "Combiners_Opaque_Mod2xNA_Alpha", COpenGL_PS15::DiffuseT3_setShaderConst},
	{EPST_COMBINERS_OPAQUE_MOD2XNA_ALPHA_3S, "Combiners_Opaque_Mod2xNA_Alpha_3s", COpenGL_PS15::DiffuseT3_setShaderConst},
	{EPST_COMBINERS_OPAQUE_MOD2XNA_ALPHA_ADD, "Combiners_Opaque_Mod2xNA_Alpha_Add", COpenGL_PS15::DiffuseT3_setShaderConst},
	{EPST_COMBINERS_OPAQUE_MOD2XNA_ALPHA_ALPHA, "Combiners_Opaque_Mod2xNA_Alpha_Alpha", COpenGL_PS15::DiffuseT3_setShaderConst},
	{EPST_COMBINERS_OPAQUE_MOD2XNA_ALPHA_UNSHALPHA, "Combiners_Opaque_Mod2xNA_Alpha_UnshAlpha", COpenGL_PS15::DiffuseT3_setShaderConst},
	{EPST_COMBINERS_OPAQUE_MOD_ADD_WGT, "Combiners_Opaque_Mod_Add_Wgt", COpenGL_PS15::DiffuseT3_setShaderConst},
	{EPST_COMBINERS_OPAQUE_MODNA_ALPHA, "Combiners_Opaque_ModNA_Alpha", COpenGL_PS15::DiffuseT3_setShaderConst},
	{EPST_COMBINERS_OPAQUE_OPAQUE, "Combiners_Opaque_Opaque", COpenGL_PS15::DiffuseT2_setShaderConst},

	{EPST_MAPOBJ_DIFFUSE, "MapObjDiffuse", COpenGL_PS15::MapObj_setShaderConst},
	{EPST_MAPOBJ_DIFFUSEEMISSIVE, "MapObjDiffuseEmissive", COpenGL_PS15::MapObj_setShaderConst},
	{EPST_MAPOBJ_OPAQUE, "MapObjOpaque", COpenGL_PS15::MapObjOpaque_setShaderConst},
	{EPST_MAPOBJ_SPECULAR, "MapObjSpecular", COpenGL_PS15::MapObj_setShaderConst},
	{EPST_MAPOBJ_METAL, "MapObjMetal", COpenGL_PS15::MapObj_setShaderConst},
	{EPST_MAPOBJ_TWOLAYERDIFFUSE, "MapObjTwoLayerDiffuse", COpenGL_PS15::MapObjTwoLayer_setShaderConst},
	{EPST_MAPOBJ_ENVMETAL, "MapObjEnvMetal", COpenGL_PS15::MapObjTwoLayer_setShaderConst},
	{EPST_MAPOBJ_ENV, "MapObjEnv", COpenGL_PS15::MapObjTwoLayer_setShaderConst},
	{EPST_MAPOBJ_TWOLAYERDIFFUSEOPAQUE, "MapObjTwoLayerDiffuseOpaque", COpenGL_PS15::MapObjTwoLayerOpaque_setShaderConst},
	{EPST_MAPOBJ_TWOLAYERENVMETAL, "MapObjTwoLayerEnvMetal", COpenGL_PS15::MapObjTwoLayer_setShaderConst},
	{EPST_MAPOBJ_TWOLAYERTERRAIN, "MapObjTwoLayerTerrain", COpenGL_PS15::MapObjTwoLayer_setShaderConst},
};

void COpenGL_PS15::loadAll( COpenGLShaderServices* shaderServices )
{
	u32 num = sizeof(g_openglps15_pixelShaderEntries) / sizeof(SPixelShaderEntry);
	for (u32 i=0; i<num; ++i)
	{
		loadPShaderHLSL(shaderServices, g_openglps15_pixelShaderEntries[i].psType, PS_Macro_None);
	}
}

bool COpenGL_PS15::loadPShaderHLSL(COpenGLShaderServices* shaderServices, E_PS_TYPE type, E_PS_MACRO macro)
{
	const c8* profile = "glps_1_5";

	c8 basePath[128];
	Q_sprintf(basePath, 128, "Pixel\\%s\\", profile);

	s32 index = -1;
	u32 num = sizeof(g_openglps15_pixelShaderEntries) / sizeof(SPixelShaderEntry);
	for (u32 i=0; i<num; ++i)
	{
		if (g_openglps15_pixelShaderEntries[i].psType == type)
		{
			index = (s32)i;
			break;
		}
	}

	if (index == -1)
		return false;

	string128 path(basePath);
	path.append(g_openglps15_pixelShaderEntries[index].path.c_str());

	path.append(".fx");

	path.normalize();

	return shaderServices->loadPShaderHLSL(path.c_str(), "main", profile, g_openglps15_pixelShaderEntries[index].psType, macro, g_openglps15_pixelShaderEntries[index].callback);
}

void COpenGL_PS15::Default_setShaderConst_T1( IPixelShader* ps, const SMaterial& material, u32 pass )
{
	COpenGLMaterialRenderServices* services = static_cast<COpenGLMaterialRenderServices*>(g_Engine->getDriver()->getMaterialRenderServices());
	COpenGLShaderServices* shaderServices = static_cast<COpenGLShaderServices*>(g_Engine->getDriver()->getShaderServices());
	const SRenderUnit* renderUnit = g_Engine->getSceneRenderServices()->getCurrentUnit();
	const SRenderStateBlock& block = services->getRenderStateBlock();

	if (block.alphaTestEnabled)
	{
		Common_CB cbuffer;
		cbuffer.params[0] = block.alphaTestEnabled ? 1.0f : 0.0f;
		cbuffer.params[1] = block.alphaTestRef / 255.0f;

		shaderServices->setShaderUniformF("g_psbuffer", (const f32*)&cbuffer, sizeof(cbuffer));
	}

	services->setSampler_Texture(0, renderUnit->textures[0]);
}

void COpenGL_PS15::Default_setShaderConst_T2( IPixelShader* ps, const SMaterial& material, u32 pass )
{
	COpenGLMaterialRenderServices* services = static_cast<COpenGLMaterialRenderServices*>(g_Engine->getDriver()->getMaterialRenderServices());
	COpenGLShaderServices* shaderServices = static_cast<COpenGLShaderServices*>(g_Engine->getDriver()->getShaderServices());
	const SRenderUnit* renderUnit = g_Engine->getSceneRenderServices()->getCurrentUnit();
	const SRenderStateBlock& block = services->getRenderStateBlock();

	if(block.alphaTestEnabled)
	{
		Common_CB cbuffer;
		cbuffer.params[0] = block.alphaTestEnabled ? 1.0f : 0.0f;
		cbuffer.params[1] = block.alphaTestRef / 255.0f;

		shaderServices->setShaderUniformF("g_psbuffer", (const f32*)&cbuffer, sizeof(cbuffer));
	}

	services->setSampler_Texture(0, renderUnit->textures[0]);

	services->setSampler_Texture(1, renderUnit->textures[1]);
}

void COpenGL_PS15::UI_setShaderConst( IPixelShader* ps, const SMaterial& material, u32 pass )
{
	COpenGLMaterialRenderServices* matServices = static_cast<COpenGLMaterialRenderServices*>(g_Engine->getDriver()->getMaterialRenderServices());
	ITexture* tex0 = matServices->getSampler_Texture(0);

	matServices->setSampler_Texture(0,  matServices->getSampler_Texture(0));
}

void COpenGL_PS15::DiffuseT1_setShaderConst( IPixelShader* ps, const SMaterial& material, u32 pass )
{
	COpenGLMaterialRenderServices* services = static_cast<COpenGLMaterialRenderServices*>(g_Engine->getDriver()->getMaterialRenderServices());
	COpenGLShaderServices* shaderServices = static_cast<COpenGLShaderServices*>(g_Engine->getDriver()->getShaderServices());
	COpenGLSceneStateServices* sceneStateServices = static_cast<COpenGLSceneStateServices*>(g_Engine->getDriver()->getSceneStateServices());
	const SRenderUnit* renderUnit = g_Engine->getSceneRenderServices()->getCurrentUnit();
	const SRenderStateBlock& block = services->getRenderStateBlock();

	// param
	Diffuse_CB cbuffer;
	cbuffer.FogColor = SColorf(sceneStateServices->getFog().FogColor);
	cbuffer.params[0] = block.alphaTestEnabled ? 1.0f : 0.0f;
	if(block.alphaTestEnabled)
		cbuffer.params[1] = block.alphaTestRef / 255.0f;

	u32 size = block.alphaTestEnabled ? sizeof(cbuffer) : sizeof(cbuffer) - sizeof(cbuffer.params);
	shaderServices->setShaderUniformF("g_psbuffer", (const f32*)&cbuffer, size);

	services->setSampler_Texture(0,  renderUnit->textures[0]);
}

void COpenGL_PS15::DiffuseT2_setShaderConst( IPixelShader* ps, const SMaterial& material, u32 pass )
{
	COpenGLMaterialRenderServices* services = static_cast<COpenGLMaterialRenderServices*>(g_Engine->getDriver()->getMaterialRenderServices());
	COpenGLShaderServices* shaderServices = static_cast<COpenGLShaderServices*>(g_Engine->getDriver()->getShaderServices());
	COpenGLSceneStateServices* sceneStateServices = static_cast<COpenGLSceneStateServices*>(g_Engine->getDriver()->getSceneStateServices());
	const SRenderUnit* renderUnit = g_Engine->getSceneRenderServices()->getCurrentUnit();
	const SRenderStateBlock& block = services->getRenderStateBlock();

	// param
	Diffuse_CB cbuffer;
	cbuffer.FogColor = SColorf(sceneStateServices->getFog().FogColor);
	cbuffer.params[0] = block.alphaTestEnabled ? 1.0f : 0.0f;
	if(block.alphaTestEnabled)
		cbuffer.params[1] = block.alphaTestRef / 255.0f;

	u32 size = block.alphaTestEnabled ? sizeof(cbuffer) : sizeof(cbuffer) - sizeof(cbuffer.params);
	shaderServices->setShaderUniformF("g_psbuffer", (const f32*)&cbuffer, size);

	services->setSampler_Texture(0,  renderUnit->textures[0]);

	services->setSampler_Texture(1,  renderUnit->textures[1]);
}

void COpenGL_PS15::DiffuseT3_setShaderConst( IPixelShader* ps, const SMaterial& material, u32 pass )
{
	COpenGLMaterialRenderServices* services = static_cast<COpenGLMaterialRenderServices*>(g_Engine->getDriver()->getMaterialRenderServices());
	COpenGLShaderServices* shaderServices = static_cast<COpenGLShaderServices*>(g_Engine->getDriver()->getShaderServices());
	COpenGLSceneStateServices* sceneStateServices = static_cast<COpenGLSceneStateServices*>(g_Engine->getDriver()->getSceneStateServices());
	const SRenderUnit* renderUnit = g_Engine->getSceneRenderServices()->getCurrentUnit();;
	const SRenderStateBlock& block = services->getRenderStateBlock();

	// param
	Diffuse_CB cbuffer;
	cbuffer.FogColor = SColorf(sceneStateServices->getFog().FogColor);
	cbuffer.params[0] = block.alphaTestEnabled ? 1.0f : 0.0f;
	if(block.alphaTestEnabled)
		cbuffer.params[1] = block.alphaTestRef / 255.0f;

	u32 size = block.alphaTestEnabled ? sizeof(cbuffer) : sizeof(cbuffer) - sizeof(cbuffer.params);
	shaderServices->setShaderUniformF("g_psbuffer", (const f32*)&cbuffer, size);

	services->setSampler_Texture(0,  renderUnit->textures[0]);

	services->setSampler_Texture(1,  renderUnit->textures[1]);

	services->setSampler_Texture(2,  renderUnit->textures[2]);
}

void COpenGL_PS15::Terrain_setShaderConst( IPixelShader* ps, const SMaterial& material, u32 pass )
{
	COpenGLMaterialRenderServices* services = static_cast<COpenGLMaterialRenderServices*>(g_Engine->getDriver()->getMaterialRenderServices());
	COpenGLShaderServices* shaderServices = static_cast<COpenGLShaderServices*>(g_Engine->getDriver()->getShaderServices());
	const SRenderUnit* renderUnit = g_Engine->getSceneRenderServices()->getCurrentUnit();
	const CMapChunk* chunk = reinterpret_cast<const CMapChunk*>(renderUnit->u.chunk);
	const CFileADT* adt = reinterpret_cast<const CFileADT*>(renderUnit->u.adt);
	COpenGLSceneStateServices* sceneStateServices = static_cast<COpenGLSceneStateServices*>(g_Engine->getDriver()->getSceneStateServices());
	const SFogParam& fogParam = sceneStateServices->getFog();

	// param
	Terrain_CB cbuffer;
	cbuffer.params[0] =  (float)chunk->numTextures;
	cbuffer.fogColor = SColorf(fogParam.FogColor);

	shaderServices->setShaderUniformF("g_psbuffer", (const f32*)&cbuffer, sizeof(cbuffer));

	services->setSampler_Texture(0, adt->getBlendMap());
	services->setSampler_Texture(1, chunk->textures[0]);

	if (chunk->numTextures > 1)
	{
		services->setSampler_Texture(2, chunk->textures[1]);
	}

	if (chunk->numTextures > 2)
	{
		services->setSampler_Texture(3, chunk->textures[2]);
	}

	if (chunk->numTextures > 3)
	{
		services->setSampler_Texture(4, chunk->textures[3]);
	}
}

void COpenGL_PS15::MapObjOpaque_setShaderConst( IPixelShader* ps, const SMaterial& material, u32 pass )
{
	COpenGLMaterialRenderServices* services = static_cast<COpenGLMaterialRenderServices*>(g_Engine->getDriver()->getMaterialRenderServices());
	COpenGLShaderServices* shaderServices = static_cast<COpenGLShaderServices*>(g_Engine->getDriver()->getShaderServices());
	COpenGLSceneStateServices* sceneStateServices = static_cast<COpenGLSceneStateServices*>(g_Engine->getDriver()->getSceneStateServices());
	const SRenderUnit* renderUnit = g_Engine->getSceneRenderServices()->getCurrentUnit();

	// param
	MapObjOpaque_CB cbuffer;
	cbuffer.AmbientColor = material.Lighting ? SColorf(sceneStateServices->getAmbientLight()) * material.AmbientColor : material.EmissiveColor;
	cbuffer.FogColor = SColorf(sceneStateServices->getFog().FogColor);

	shaderServices->setShaderUniformF("g_psbuffer", (const f32*)&cbuffer, sizeof(cbuffer));

	services->setSampler_Texture(0, renderUnit->textures[0]);
}

void COpenGL_PS15::MapObj_setShaderConst( IPixelShader* ps, const SMaterial& material, u32 pass )
{
	//texture
	COpenGLMaterialRenderServices* services = static_cast<COpenGLMaterialRenderServices*>(g_Engine->getDriver()->getMaterialRenderServices());
	COpenGLShaderServices* shaderServices = static_cast<COpenGLShaderServices*>(g_Engine->getDriver()->getShaderServices());
	COpenGLSceneStateServices* sceneStateServices = static_cast<COpenGLSceneStateServices*>(g_Engine->getDriver()->getSceneStateServices());
	const SRenderUnit* renderUnit = g_Engine->getSceneRenderServices()->getCurrentUnit();
	const SRenderStateBlock& block = services->getRenderStateBlock();

	// param
	MapObjAlphaTest_CB cbuffer;
	cbuffer.AmbientColor = material.Lighting ? SColorf(sceneStateServices->getAmbientLight()) * material.AmbientColor : material.EmissiveColor;
	cbuffer.FogColor = SColorf(sceneStateServices->getFog().FogColor);
	cbuffer.params[0] = block.alphaTestEnabled ? 1.0f : 0.0f;
	if(block.alphaTestEnabled)
		cbuffer.params[1] = block.alphaTestRef / 255.0f;

	u32 size = block.alphaTestEnabled ? sizeof(cbuffer) : sizeof(cbuffer) - sizeof(cbuffer.params);
	shaderServices->setShaderUniformF("g_psbuffer", (const f32*)&cbuffer, size);

	services->setSampler_Texture(0, renderUnit->textures[0]);
}

void COpenGL_PS15::MapObjTwoLayer_setShaderConst( IPixelShader* ps, const SMaterial& material, u32 pass )
{
	COpenGLMaterialRenderServices* services = static_cast<COpenGLMaterialRenderServices*>(g_Engine->getDriver()->getMaterialRenderServices());
	COpenGLShaderServices* shaderServices = static_cast<COpenGLShaderServices*>(g_Engine->getDriver()->getShaderServices());
	COpenGLSceneStateServices* sceneStateServices = static_cast<COpenGLSceneStateServices*>(g_Engine->getDriver()->getSceneStateServices());
	const SRenderUnit* renderUnit = g_Engine->getSceneRenderServices()->getCurrentUnit();
	const SRenderStateBlock& block = services->getRenderStateBlock();

	// param
	MapObjAlphaTest_CB cbuffer;
	cbuffer.AmbientColor = material.Lighting ? SColorf(sceneStateServices->getAmbientLight()) * material.AmbientColor : material.EmissiveColor;
	cbuffer.FogColor = SColorf(sceneStateServices->getFog().FogColor);
	cbuffer.params[0] = block.alphaTestEnabled ? 1.0f : 0.0f;
	if(block.alphaTestEnabled)
		cbuffer.params[1] = block.alphaTestRef / 255.0f;

	u32 size = block.alphaTestEnabled ? sizeof(cbuffer) : sizeof(cbuffer) - sizeof(cbuffer.params);
	shaderServices->setShaderUniformF("g_psbuffer", (const f32*)&cbuffer, size);

	services->setSampler_Texture(0, renderUnit->textures[0]);

	services->setSampler_Texture(1, renderUnit->textures[1]);
}

void COpenGL_PS15::MapObjTwoLayerOpaque_setShaderConst( IPixelShader* ps, const SMaterial& material, u32 pass )
{
	COpenGLMaterialRenderServices* services = static_cast<COpenGLMaterialRenderServices*>(g_Engine->getDriver()->getMaterialRenderServices());
	COpenGLShaderServices* shaderServices = static_cast<COpenGLShaderServices*>(g_Engine->getDriver()->getShaderServices());
	COpenGLSceneStateServices* sceneStateServices = static_cast<COpenGLSceneStateServices*>(g_Engine->getDriver()->getSceneStateServices());
	const SRenderUnit* renderUnit = g_Engine->getSceneRenderServices()->getCurrentUnit();

	// param
	MapObjOpaque_CB cbuffer;
	cbuffer.AmbientColor = material.Lighting ? SColorf(sceneStateServices->getAmbientLight()) * material.AmbientColor : material.EmissiveColor;
	cbuffer.FogColor = SColorf(sceneStateServices->getFog().FogColor);

	shaderServices->setShaderUniformF("g_psbuffer", (const f32*)&cbuffer, sizeof(cbuffer));

	services->setSampler_Texture(0, renderUnit->textures[0]);

	services->setSampler_Texture(1, renderUnit->textures[1]);
}

#endif
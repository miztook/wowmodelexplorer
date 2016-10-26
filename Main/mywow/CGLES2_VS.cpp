#include "stdafx.h"
#include "CGLES2_VS.h"
#include "mywow.h"
#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_GLES2

#include "CGLES2Shader.h"
#include "CGLES2ShaderServices.h"
#include "CGLES2Driver.h"
#include "CGLES2SceneStateServices.h"

static CGLES2_VS::SVertexShaderEntry g_gles2vs_vertexShaderEntries[] =
{
	{EVST_TERRAIN, "Terrain", CGLES2_VS::Terrain_setShaderConst},
	{EVST_DEFAULT_P, "Default_P", CGLES2_VS::Default_WCol_setShaderConst},
	{EVST_DEFAULT_PC, "Default_PC", CGLES2_VS::Default_W_setShaderConst},
	{EVST_DEFAULT_PCT, "Default_PCT", CGLES2_VS::Default_WTex_setShaderConst},
	{EVST_DEFAULT_PN, "Default_PN", CGLES2_VS::Default_WCol_setShaderConst},
	{EVST_DEFAULT_PNC, "Default_PNC", CGLES2_VS::Default_W_setShaderConst},
	{EVST_DEFAULT_PNCT, "Default_PNCT", CGLES2_VS::Default_WTex_setShaderConst},
	{EVST_DEFAULT_PNCT2, "Default_PNCT2", CGLES2_VS::Default_WTex_setShaderConst},
	{EVST_DEFAULT_PNT, "Default_PNT", CGLES2_VS::Default_WColTex_setShaderConst},
	{EVST_DEFAULT_PT, "Default_PT", CGLES2_VS::Default_WColTex_setShaderConst},

	{EVST_CDIFFUSE_T1, "CDiffuse_T1", CGLES2_VS::CDiffuseT1_setShaderConst},
	{EVST_DIFFUSE_T1, "Diffuse_T1", CGLES2_VS::DiffuseT1_setShaderConst},
	{EVST_DIFFUSE_ENV, "Diffuse_Env", CGLES2_VS::DiffuseEnv_setShaderConst},
	{EVST_DIFFUSE_T1_T1, "Diffuse_T1_T1", CGLES2_VS::DiffuseT1_setShaderConst},
	{EVST_DIFFUSE_T1_T2, "Diffuse_T1_T2", CGLES2_VS::DiffuseT1_setShaderConst},
	{EVST_DIFFUSE_T1_ENV, "Diffuse_T1_Env", CGLES2_VS::DiffuseT1_setShaderConst},
	{EVST_DIFFUSE_ENV_T1, "Diffuse_Env_T1", CGLES2_VS::DiffuseEnv_setShaderConst},
	{EVST_DIFFUSE_ENV_ENV, "Diffuse_Env_Env", CGLES2_VS::DiffuseEnv_setShaderConst},
	{EVST_DIFFUSE_T1_T1_T1, "Diffuse_T1_T1_T1", CGLES2_VS::DiffuseT1_setShaderConst},
	{EVST_DIFFUSE_T1_T2_T1, "Diffuse_T1_T2_T1", CGLES2_VS::DiffuseT1_setShaderConst},
	{EVST_DIFFUSE_T1_ENV_T1, "Diffuse_T1_Env_T1", CGLES2_VS::DiffuseT1_setShaderConst},
	{EVST_DIFFUSE_T1_ENV_T2, "Diffuse_T1_Env_T2", CGLES2_VS::DiffuseT1_setShaderConst},

	{EVST_MAPOBJ_DIFFUSE_T1, "MapObjDiffuse_T1", CGLES2_VS::MapObjDiffuse_setShaderConst},
	{EVST_MAPOBJ_SPECULAR_T1, "MapObjSpecular_T1", CGLES2_VS::MapObjSpecular_setShaderConst},
	{EVST_MAPOBJ_DIFFUSE_T1_T2, "MapObjDiffuse_T1_T2", CGLES2_VS::MapObjDiffuse_setShaderConst},
	{EVST_MAPOBJ_DIFFUSE_T1_ENV_T2, "MapObjDiffuse_T1_Env_T2", CGLES2_VS::MapObjDiffuse_setShaderConst},
	{EVST_MAPOBJ_DIFFUSE_T1_REFL, "MapObjDiffuse_T1_Refl", CGLES2_VS::MapObjDiffuse_setShaderConst},
};

void CGLES2_VS::loadAll(CGLES2ShaderServices* shaderServices)
{
	u32 num = sizeof(g_gles2vs_vertexShaderEntries) / sizeof(SVertexShaderEntry);
	for (u32 i=0; i<num; ++i)
	{
		loadVShaderHLSL(shaderServices, g_gles2vs_vertexShaderEntries[i].vsType);
	}
}

bool CGLES2_VS::loadVShaderHLSL(CGLES2ShaderServices* shaderServices, E_VS_TYPE type)
{
	const c8* profile = "gles2_vs";

	c8 basePath[128];
	Q_sprintf(basePath, 128, "Vertex\\%s\\", profile);

	s32 index = -1;
	u32 num = sizeof(g_gles2vs_vertexShaderEntries) / sizeof(SVertexShaderEntry);
	for (u32 i=0; i<num; ++i)
	{
		if(g_gles2vs_vertexShaderEntries[i].vsType == type)
		{
			index = (s32)i;
			break;
		}
	}

	if (index == -1)
		return false;

	string128 path(basePath);
	path.append(g_gles2vs_vertexShaderEntries[index].path.c_str());

	path.append(".fx");

	path.normalize();

	return shaderServices->loadVShaderHLSL(path.c_str(), "main", profile, g_gles2vs_vertexShaderEntries[index].vsType, g_gles2vs_vertexShaderEntries[index].callback);
}

void CGLES2_VS::Default_W_setShaderConst( IVertexShader* vs, const SMaterial& material, u32 pass )
{
	CGLES2Driver* driver = static_cast<CGLES2Driver*>(g_Engine->getDriver());
	CGLES2ShaderServices* services = static_cast<CGLES2ShaderServices*>(driver->getShaderServices());

	// para
	SParamW cbuffer;

	//wvp
	driver->getWVPMatrix(cbuffer.mWorldViewProjection);
	cbuffer.mWorldViewProjection.transpose();

	services->setShaderUniformF("g_vsbuffer", (const f32*)&cbuffer, sizeof(cbuffer));
}

void CGLES2_VS::Default_WCol_setShaderConst( IVertexShader* vs, const SMaterial& material, u32 pass )
{
	CGLES2Driver* driver = static_cast<CGLES2Driver*>(g_Engine->getDriver());
	CGLES2ShaderServices* services = static_cast<CGLES2ShaderServices*>(driver->getShaderServices());

	// para
	SParamWCol cbuffer;

	//wvp
	driver->getWVPMatrix(cbuffer.mWorldViewProjection);
	cbuffer.mWorldViewProjection.transpose();

	//color
	cbuffer.vColor =  material.Lighting ? SColorf(1.0f, 1.0f, 1.0f) : material.EmissiveColor;

	services->setShaderUniformF("g_vsbuffer", (const f32*)&cbuffer, sizeof(cbuffer));
}

void CGLES2_VS::Default_WColTex_setShaderConst( IVertexShader* vs, const SMaterial& material, u32 pass )
{
	CGLES2Driver* driver = static_cast<CGLES2Driver*>(g_Engine->getDriver());
	CGLES2ShaderServices* services = static_cast<CGLES2ShaderServices*>(driver->getShaderServices());

	// para
	SParamWColTex cbuffer;

	//wvp
	driver->getWVPMatrix(cbuffer.mWorldViewProjection);
	cbuffer.mWorldViewProjection.transpose();

	//color
	cbuffer.vColor =  material.Lighting ? SColorf(1.0f, 1.0f, 1.0f) : material.EmissiveColor;

	//tex
	bool useAnimTex = material.TextureLayer[0].UseTextureMatrix;
	cbuffer.Params[1] =  useAnimTex ? 1.0f : 0.0f;
	if (useAnimTex)
	{
		cbuffer.mTexture = *material.TextureLayer[0].TextureMatrix;
		cbuffer.mTexture.transpose();
	}

	u32 size = useAnimTex ? sizeof(SParamWColTex) : sizeof(SParamWColTex) - sizeof(matrix4);
	services->setShaderUniformF("g_vsbuffer", (const f32*)&cbuffer, size);
}

void CGLES2_VS::Default_WTex_setShaderConst( IVertexShader* vs, const SMaterial& material, u32 pass )
{
	CGLES2Driver* driver = static_cast<CGLES2Driver*>(g_Engine->getDriver());
	CGLES2ShaderServices* services = static_cast<CGLES2ShaderServices*>(driver->getShaderServices());

	// para
	SParamWTex cbuffer;

	//wvp
	driver->getWVPMatrix(cbuffer.mWorldViewProjection);
	cbuffer.mWorldViewProjection.transpose();

	//tex
	bool useAnimTex = material.TextureLayer[0].UseTextureMatrix;
	cbuffer.Params[1] =  useAnimTex ? 1.0f : 0.0f;
	if (useAnimTex)
	{
		cbuffer.mTexture = *material.TextureLayer[0].TextureMatrix;
		cbuffer.mTexture.transpose();
	}

	u32 size = useAnimTex ? sizeof(SParamWTex) : sizeof(SParamWTex) - sizeof(matrix4);
	services->setShaderUniformF("g_vsbuffer", (const f32*)&cbuffer, size);
}

void CGLES2_VS::CDiffuseT1_setShaderConst( IVertexShader* vs, const SMaterial& material, u32 pass )
{
	CGLES2Driver* driver = static_cast<CGLES2Driver*>(g_Engine->getDriver());
	CGLES2ShaderServices* services = static_cast<CGLES2ShaderServices*>(driver->getShaderServices());
	CGLES2SceneStateServices* sceneStateServices = static_cast<CGLES2SceneStateServices*>(driver->getSceneStateServices());
	SFogParam fogParam = sceneStateServices->getFog();

	// para
	SCDiffuse cbuffer;

	//wvp
	driver->getWVPMatrix(cbuffer.mWorldViewProjection);
	cbuffer.mWorldViewProjection.transpose();
	driver->getWVMatrix(cbuffer.mWorldView);
	cbuffer.mWorldView.transpose();

	//color
	cbuffer.vColor =  material.Lighting ? SColorf(sceneStateServices->getAmbientLight()) : SColorf(1.0f, 1.0f, 1.0f, 1.0f);

	//fog
	cbuffer.fogParams[0] = (float)fogParam.FogType;
	cbuffer.fogParams[1] = fogParam.FogStart;
	cbuffer.fogParams[2] = fogParam.FogEnd;
	cbuffer.fogParams[3] = fogParam.FogDensity;

	//tex
	cbuffer.Params[0] = 0.0f;
	bool useAnimTex = material.TextureLayer[0].UseTextureMatrix;
	cbuffer.Params[1] = useAnimTex ? 1.0f : 0.0f;
	cbuffer.Params[2] = material.FogEnable ? 1.0f : 0.0f;
	if (useAnimTex)
	{
		cbuffer.mTexture = *material.TextureLayer[0].TextureMatrix;
		cbuffer.mTexture.transpose();
	}

	u32 size = useAnimTex ? sizeof(SCDiffuse) : sizeof(SCDiffuse) - sizeof(cbuffer.mTexture);
	services->setShaderUniformF("g_vsbuffer", (const f32*)&cbuffer, size);
}

void CGLES2_VS::DiffuseT1_setShaderConst( IVertexShader* vs, const SMaterial& material, u32 pass )
{
	CGLES2Driver* driver = static_cast<CGLES2Driver*>(g_Engine->getDriver());
	CGLES2ShaderServices* services = static_cast<CGLES2ShaderServices*>(driver->getShaderServices());
	CGLES2SceneStateServices* sceneStateServices = static_cast<CGLES2SceneStateServices*>(driver->getSceneStateServices());
	SFogParam fogParam = sceneStateServices->getFog();

	const SRenderUnit* unit = g_Engine->getSceneRenderServices()->getCurrentUnit();
	SBoneMatrixArray* boneMats = unit->u.boneMatrixArray;
	ASSERT(boneMats);

	// para
	SDiffuseT cbuffer;

	//wvp
	driver->getWVPMatrix(cbuffer.mWorldViewProjection);
	cbuffer.mWorldViewProjection.transpose();

	driver->getWVMatrix(cbuffer.mWorldView);
	cbuffer.mWorldView.transpose();

	//color
	cbuffer.vColor =  material.Lighting ? SColorf(sceneStateServices->getAmbientLight()) * material.AmbientColor : material.EmissiveColor;

	//fog
	cbuffer.fogParams[0] = (float)fogParam.FogType;
	cbuffer.fogParams[1] = fogParam.FogStart;
	cbuffer.fogParams[2] = fogParam.FogEnd;
	cbuffer.fogParams[3] = fogParam.FogDensity;

	//tex
	cbuffer.Params[0] = unit->u.useBoneMatrix ? (float)boneMats->maxWeights: 0.0f;
	bool useAnimTex = material.TextureLayer[0].UseTextureMatrix;
	cbuffer.Params[1] = useAnimTex ? 1.0f : 0.0f;
	cbuffer.Params[2] = material.FogEnable ? 1.0f : 0.0f;
	if (useAnimTex)
	{
		cbuffer.mTexture = *material.TextureLayer[0].TextureMatrix;
		cbuffer.mTexture.transpose();
	}

	u32 size = sizeof(SDiffuseT) - sizeof(cbuffer.boneMatrices);

	//matrix array
	if(unit->u.useBoneMatrix)
	{
		ASSERT(boneMats->count <= MAX_BONE_NUM);

		for(u32 i=0; i<boneMats->count; ++i)
		{
			matrix4 m = boneMats->matrices[i];
			m.transpose();
			Q_memcpy(cbuffer.boneMatrices[i], 12 * sizeof(float), m.M, 12 * sizeof(float)); 
		}

		size += boneMats->count * 12 * sizeof(float);
	}
	
	services->setShaderUniformF("g_vsbuffer", (const f32*)&cbuffer, size);
}

void CGLES2_VS::DiffuseEnv_setShaderConst( IVertexShader* vs, const SMaterial& material, u32 pass )
{
	CGLES2Driver* driver = static_cast<CGLES2Driver*>(g_Engine->getDriver());
	CGLES2ShaderServices* services = static_cast<CGLES2ShaderServices*>(driver->getShaderServices());
	CGLES2SceneStateServices* sceneStateServices = static_cast<CGLES2SceneStateServices*>(driver->getSceneStateServices());
	SFogParam fogParam = sceneStateServices->getFog();

	const SRenderUnit* unit = g_Engine->getSceneRenderServices()->getCurrentUnit();
	SBoneMatrixArray* boneMats = unit->u.boneMatrixArray;
	ASSERT(boneMats);

	//param
	SDiffuseEnv cbuffer;

	//wvp
	driver->getWVPMatrix(cbuffer.mWorldViewProjection);
	cbuffer.mWorldViewProjection.transpose();

	driver->getWVMatrix(cbuffer.mWorldView);
	cbuffer.mWorldView.transpose();

	//color
	cbuffer.vColor =  material.Lighting ? SColorf(sceneStateServices->getAmbientLight()) * material.AmbientColor : material.EmissiveColor;

	//fog
	cbuffer.fogParams[0] = (float)fogParam.FogType;
	cbuffer.fogParams[1] = fogParam.FogStart;
	cbuffer.fogParams[2] = fogParam.FogEnd;
	cbuffer.fogParams[3] = fogParam.FogDensity;

	//tex
	cbuffer.Params[0] = unit->u.useBoneMatrix ? (float)boneMats->maxWeights: 0.0f;
	cbuffer.Params[1] = 0.0f;
	cbuffer.Params[2] = material.FogEnable ? 1.0f : 0.0f;

	u32 size = sizeof(SDiffuseEnv) - sizeof(cbuffer.boneMatrices);

	//matrix array
	if(unit->u.useBoneMatrix)
	{
		ASSERT(boneMats->count <= MAX_BONE_NUM);

		for(u32 i=0; i<boneMats->count; ++i)
		{
			matrix4 m = boneMats->matrices[i];
			m.transpose();
			Q_memcpy(cbuffer.boneMatrices[i], 12 * sizeof(float), m.M, 12 * sizeof(float)); 
		}

		size += boneMats->count * 12 * sizeof(float);
	}

	services->setShaderUniformF("g_vsbuffer", (const f32*)&cbuffer, size);
}

void CGLES2_VS::Terrain_setShaderConst( IVertexShader* vs, const SMaterial& material, u32 pass )
{
	CGLES2Driver* driver = static_cast<CGLES2Driver*>(g_Engine->getDriver());
	CGLES2ShaderServices* services = static_cast<CGLES2ShaderServices*>(driver->getShaderServices());
	CGLES2SceneStateServices* sceneStateServices = static_cast<CGLES2SceneStateServices*>(driver->getSceneStateServices());
	SFogParam fogParam = sceneStateServices->getFog();
	ISceneEnvironment* sceneEnv = g_Engine->getSceneEnvironment();

	//para
	SParamTerrain	cbuffer;

	//wvp
	driver->getWVPMatrix(cbuffer.mWorldViewProjection);
	cbuffer.mWorldViewProjection.transpose();
	driver->getWVMatrix(cbuffer.mWorldView);
	cbuffer.mWorldView.transpose();

	const SLight* l = sceneStateServices->getDynamicLight(ISceneEnvironment::INDEX_DIR_LIGHT);
	cbuffer.LightDir[0] = l->Direction.X;
	cbuffer.LightDir[1] = l->Direction.Y;
	cbuffer.LightDir[2] = l->Direction.Z;

	cbuffer.LightColor = SColorf(l->DiffuseColor);

	cbuffer.MaterialParams[0] = sceneEnv->TerrainMaterialParams[0];
	cbuffer.MaterialParams[1] = sceneEnv->TerrainMaterialParams[1];
	cbuffer.MaterialParams[2] = sceneEnv->TerrainMaterialParams[2];

	//fog
	cbuffer.fogParams[0] = (float)fogParam.FogType;
	cbuffer.fogParams[1] = fogParam.FogStart;
	cbuffer.fogParams[2] = fogParam.FogEnd;
	cbuffer.fogParams[3] = fogParam.FogDensity;

	//tex
	bool useAnimTex = material.TextureLayer[0].UseTextureMatrix;
	cbuffer.Params[0] =  useAnimTex ? 1.0f : 0.0f;
	cbuffer.Params[1] = material.FogEnable ? 1.0f : 0.0f;
	if (useAnimTex)
	{
		cbuffer.mTexture = *material.TextureLayer[0].TextureMatrix;
		cbuffer.mTexture.transpose();
	}

	u32 size = useAnimTex ? sizeof(SParamTerrain) : sizeof(SParamTerrain) - sizeof(cbuffer.mTexture);
	services->setShaderUniformF("g_vsbuffer", (const f32*)&cbuffer, size);
}

void CGLES2_VS::MapObjDiffuse_setShaderConst( IVertexShader* vs, const SMaterial& material, u32 pass )
{
	CGLES2Driver* driver = static_cast<CGLES2Driver*>(g_Engine->getDriver());
	CGLES2ShaderServices* services = static_cast<CGLES2ShaderServices*>(driver->getShaderServices());
	CGLES2SceneStateServices* sceneStateServices = static_cast<CGLES2SceneStateServices*>(driver->getSceneStateServices());
	SFogParam fogParam = sceneStateServices->getFog();
	ISceneEnvironment* sceneEnv = g_Engine->getSceneEnvironment();

	//para
	SParamMapObjDiffuse	cbuffer;

	//wvp
	driver->getWVPMatrix(cbuffer.mWorldViewProjection);
	cbuffer.mWorldViewProjection.transpose();
	driver->getWVMatrix(cbuffer.mWorldView);
	cbuffer.mWorldView.transpose();

	const SLight* l = sceneStateServices->getDynamicLight(ISceneEnvironment::INDEX_DIR_LIGHT);
	cbuffer.LightDir[0] = l->Direction.X;
	cbuffer.LightDir[1] = l->Direction.Y;
	cbuffer.LightDir[2] = l->Direction.Z;

	cbuffer.LightColor = SColorf(l->DiffuseColor);

	cbuffer.MaterialParams[0] = sceneEnv->WmoMaterialParams[0];
	cbuffer.MaterialParams[1] = sceneEnv->WmoMaterialParams[1];
	cbuffer.MaterialParams[2] = sceneEnv->WmoMaterialParams[2];

	//fog
	cbuffer.fogParams[0] = (float)fogParam.FogType;
	cbuffer.fogParams[1] = fogParam.FogStart;
	cbuffer.fogParams[2] = fogParam.FogEnd;
	cbuffer.fogParams[3] = fogParam.FogDensity;

	//tex
	bool useAnimTex = material.TextureLayer[0].UseTextureMatrix;
	cbuffer.Params[0] =  useAnimTex ? 1.0f : 0.0f;
	cbuffer.Params[1] = material.FogEnable ? 1.0f : 0.0f;
	if (useAnimTex)
	{
		cbuffer.mTexture = *material.TextureLayer[0].TextureMatrix;
		cbuffer.mTexture.transpose();
	}

	u32 size = useAnimTex ? sizeof(SParamMapObjDiffuse) : sizeof(SParamMapObjDiffuse) - sizeof(cbuffer.mTexture);
	services->setShaderUniformF("g_vsbuffer", (const f32*)&cbuffer, size);
}

void CGLES2_VS::MapObjSpecular_setShaderConst( IVertexShader* vs, const SMaterial& material, u32 pass )
{
	CGLES2Driver* driver = static_cast<CGLES2Driver*>(g_Engine->getDriver());
	CGLES2ShaderServices* services = static_cast<CGLES2ShaderServices*>(driver->getShaderServices());
	CGLES2SceneStateServices* sceneStateServices = static_cast<CGLES2SceneStateServices*>(driver->getSceneStateServices());
	SFogParam fogParam = sceneStateServices->getFog();
	ISceneEnvironment* sceneEnv = g_Engine->getSceneEnvironment();

	//para
	SParamMapObjSpecular	cbuffer;

	//wvp
	driver->getWVPMatrix(cbuffer.mWorldViewProjection);
	cbuffer.mWorldViewProjection.transpose();
	driver->getWVMatrix(cbuffer.mWorldView);
	cbuffer.mWorldView.transpose();

	const SLight* l = sceneStateServices->getDynamicLight(ISceneEnvironment::INDEX_DIR_LIGHT);
	cbuffer.LightDir[0] = l->Direction.X;
	cbuffer.LightDir[1] = l->Direction.Y;
	cbuffer.LightDir[2] = l->Direction.Z;

	cbuffer.LightColor = SColorf(l->DiffuseColor);

	cbuffer.MaterialParams[0] = sceneEnv->WmoMaterialParams[0];
	cbuffer.MaterialParams[1] = sceneEnv->WmoMaterialParams[1];
	cbuffer.MaterialParams[2] = sceneEnv->WmoMaterialParams[2];

	//fog
	cbuffer.fogParams[0] = (float)fogParam.FogType;
	cbuffer.fogParams[1] = fogParam.FogStart;
	cbuffer.fogParams[2] = fogParam.FogEnd;
	cbuffer.fogParams[3] = fogParam.FogDensity;

	//tex
	bool useAnimTex = material.TextureLayer[0].UseTextureMatrix;
	cbuffer.Params[0] =  useAnimTex ? 1.0f : 0.0f;
	cbuffer.Params[1] = material.FogEnable ? 1.0f : 0.0f;
	if (useAnimTex)
	{
		cbuffer.mTexture = *material.TextureLayer[0].TextureMatrix;
		cbuffer.mTexture.transpose();
	}

	u32 size = useAnimTex ? sizeof(SParamMapObjSpecular) : sizeof(SParamMapObjSpecular) - sizeof(cbuffer.mTexture);
	services->setShaderUniformF("g_vsbuffer", (const f32*)&cbuffer, size);
}

#endif
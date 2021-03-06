#include "stdafx.h"
#include "CD3D9_VS30.h"
#include "mywow.h"

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D9

#include "CD3D9Shader.h"
#include "CD3D9Driver.h"
#include "CD3D9ShaderServices.h"
#include "CD3D9SceneStateServices.h"

static CD3D9_VS30::SVertexShaderEntry g_d3d9vs30_vertexShaderEntries[] =
{
	{EVST_TERRAIN, "Terrain", CD3D9_VS30::Terrain_setShaderConst},
	{EVST_DEFAULT_P, "Default_P", CD3D9_VS30::Default_WCol_setShaderConst},
	{EVST_DEFAULT_PC, "Default_PC", CD3D9_VS30::Default_W_setShaderConst},
	{EVST_DEFAULT_PCT, "Default_PCT", CD3D9_VS30::Default_WTex_setShaderConst},
	{EVST_DEFAULT_PN, "Default_PN", CD3D9_VS30::Default_WCol_setShaderConst},
	{EVST_DEFAULT_PNC, "Default_PNC", CD3D9_VS30::Default_W_setShaderConst},
	{EVST_DEFAULT_PNCT, "Default_PNCT", CD3D9_VS30::Default_WTex_setShaderConst},
	{EVST_DEFAULT_PNCT2, "Default_PNCT2", CD3D9_VS30::Default_WTex_setShaderConst},
	{EVST_DEFAULT_PNT, "Default_PNT", CD3D9_VS30::Default_WColTex_setShaderConst},
	{EVST_DEFAULT_PT, "Default_PT", CD3D9_VS30::Default_WColTex_setShaderConst},

	{EVST_CDIFFUSE_T1, "CDiffuse_T1", CD3D9_VS30::CDiffuseT1_setShaderConst},
	{EVST_DIFFUSE_T1, "Diffuse_T1", CD3D9_VS30::DiffuseT1_setShaderConst},
	{EVST_DIFFUSE_ENV, "Diffuse_Env", CD3D9_VS30::DiffuseEnv_setShaderConst},
	{EVST_DIFFUSE_T1_T1, "Diffuse_T1_T1", CD3D9_VS30::DiffuseT1_setShaderConst},
	{EVST_DIFFUSE_T1_T2, "Diffuse_T1_T2", CD3D9_VS30::DiffuseT1_setShaderConst},
	{EVST_DIFFUSE_T1_ENV, "Diffuse_T1_Env", CD3D9_VS30::DiffuseT1_setShaderConst},
	{EVST_DIFFUSE_ENV_T1, "Diffuse_Env_T1", CD3D9_VS30::DiffuseEnv_setShaderConst},
	{EVST_DIFFUSE_ENV_ENV, "Diffuse_Env_Env", CD3D9_VS30::DiffuseEnv_setShaderConst},
	{EVST_DIFFUSE_T1_T1_T1, "Diffuse_T1_T1_T1", CD3D9_VS30::DiffuseT1_setShaderConst},
	{EVST_DIFFUSE_T1_T2_T1, "Diffuse_T1_T2_T1", CD3D9_VS30::DiffuseT1_setShaderConst},
	{EVST_DIFFUSE_T1_ENV_T1, "Diffuse_T1_Env_T1", CD3D9_VS30::DiffuseT1_setShaderConst},
	{EVST_DIFFUSE_T1_ENV_T2, "Diffuse_T1_Env_T2", CD3D9_VS30::DiffuseT1_setShaderConst},

	{EVST_MAPOBJ_DIFFUSE_T1, "MapObjDiffuse_T1", CD3D9_VS30::MapObjDiffuse_setShaderConst},
	{EVST_MAPOBJ_SPECULAR_T1, "MapObjSpecular_T1", CD3D9_VS30::MapObjSpecular_setShaderConst},
	{EVST_MAPOBJ_DIFFUSE_T1_T2, "MapObjDiffuse_T1_T2", CD3D9_VS30::MapObjDiffuse_setShaderConst},
	{EVST_MAPOBJ_DIFFUSE_T1_ENV_T2, "MapObjDiffuse_T1_Env_T2", CD3D9_VS30::MapObjDiffuse_setShaderConst},
	{EVST_MAPOBJ_DIFFUSE_T1_REFL, "MapObjDiffuse_T1_Refl", CD3D9_VS30::MapObjDiffuse_setShaderConst},
};

void CD3D9_VS30::loadAll(CD3D9ShaderServices* shaderServices)
{
	uint32_t num = sizeof(g_d3d9vs30_vertexShaderEntries) / sizeof(SVertexShaderEntry);
	for (uint32_t i=0; i<num; ++i)
	{
		loadVShaderHLSL(shaderServices, g_d3d9vs30_vertexShaderEntries[i].vsType);
	}
}

bool CD3D9_VS30::loadVShaderHLSL( CD3D9ShaderServices* shaderServices, E_VS_TYPE type )
{
	const char* profile = "vs_3_0";

	//load

	char basePath[128];
	Q_sprintf(basePath, 128, "Vertex\\%s\\", profile);
	
	int32_t index = -1;
	uint32_t num = sizeof(g_d3d9vs30_vertexShaderEntries) / sizeof(SVertexShaderEntry);
	for (uint32_t i=0; i<num; ++i)
	{
		if(g_d3d9vs30_vertexShaderEntries[i].vsType == type)
		{
			index = (int32_t)i;
			break;
		}
	}

	if (index == -1)
		return false;


	string128 path(basePath);
	path.append(g_d3d9vs30_vertexShaderEntries[index].path.c_str());

#ifdef DIRECTX_USE_COMPILED_SHADER
	path.append(".bls");
#else
	path.append(".fx");
#endif

	path.normalize();

	shaderServices->loadVShaderHLSL(path.c_str(), "main", type, g_d3d9vs30_vertexShaderEntries[index].callback);

	return true;
}

void CD3D9_VS30::CDiffuseT1_setShaderConst( IVertexShader* vs, const SMaterial& material, uint32_t pass )
{
	CD3D9VertexShader* d3d9vs = static_cast<CD3D9VertexShader*>(vs);
	CD3D9Driver* driver = static_cast<CD3D9Driver*>(g_Engine->getDriver());
	CD3D9SceneStateServices* sceneStateServices = static_cast<CD3D9SceneStateServices*>(driver->getSceneStateServices());
	const SFogParam& fogParam = sceneStateServices->getFog();

	// para
	SCDiffuse cbuffer;

	//wvp
	driver->getWVPMatrix(cbuffer.mWorldViewProjection);
	driver->getWVMatrix(cbuffer.mWorldView);

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
	}

	//set
	uint32_t size = useAnimTex ? sizeof(SCDiffuse) : sizeof(SCDiffuse) - sizeof(matrix4);
	d3d9vs->setShaderConstant("g_cbuffer", &cbuffer, size);
}

void CD3D9_VS30::DiffuseT1_setShaderConst( IVertexShader* vs, const SMaterial& material, uint32_t pass )
{
	CD3D9VertexShader* d3d9vs = static_cast<CD3D9VertexShader*>(vs);
	CD3D9Driver* driver = static_cast<CD3D9Driver*>(g_Engine->getDriver());
	CD3D9SceneStateServices* sceneStateServices = static_cast<CD3D9SceneStateServices*>(driver->getSceneStateServices());
	const SFogParam& fogParam = sceneStateServices->getFog();

	const SRenderUnit* unit = g_Engine->getSceneRenderServices()->getCurrentUnit();
	const SBoneMatrixArray* boneMats = unit->u.boneMatrixArray;
	ASSERT(boneMats);

	// para
	SDiffuseT cbuffer;

	//wvp
	driver->getWVPMatrix(cbuffer.mWorldViewProjection);
	driver->getWVMatrix(cbuffer.mWorldView);

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
	}

	uint32_t size = sizeof(SDiffuseT) - sizeof(cbuffer.boneMatrices);
	if(unit->u.useBoneMatrix)
	{
		Q_memcpy(cbuffer.boneMatrices, sizeof(cbuffer.boneMatrices), boneMats->matrices, boneMats->count * sizeof(matrix4));
		size += boneMats->count * sizeof(matrix4);
	}
	
	//set
	d3d9vs->setShaderConstant("g_cbuffer", &cbuffer, size);
}

void CD3D9_VS30::DiffuseEnv_setShaderConst( IVertexShader* vs, const SMaterial& material, uint32_t pass )
{
	CD3D9VertexShader* d3d9vs = static_cast<CD3D9VertexShader*>(vs);
	CD3D9Driver* driver = static_cast<CD3D9Driver*>(g_Engine->getDriver());
	CD3D9SceneStateServices* sceneStateServices = static_cast<CD3D9SceneStateServices*>(driver->getSceneStateServices());
	const SFogParam& fogParam = sceneStateServices->getFog();

	const SRenderUnit* unit = g_Engine->getSceneRenderServices()->getCurrentUnit();
	const SBoneMatrixArray* boneMats = unit->u.boneMatrixArray;
	ASSERT(boneMats);

	// para
	SDiffuseEnv cbuffer;

	//wvp
	driver->getWVPMatrix(cbuffer.mWorldViewProjection);
	driver->getWVMatrix(cbuffer.mWorldView);

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

	uint32_t size = sizeof(SDiffuseEnv) - sizeof(cbuffer.boneMatrices);
	if(unit->u.useBoneMatrix)
	{
		Q_memcpy(cbuffer.boneMatrices, sizeof(cbuffer.boneMatrices), boneMats->matrices, boneMats->count * sizeof(matrix4));
		size += boneMats->count * sizeof(matrix4);
	}

	//set
	d3d9vs->setShaderConstant("g_cbuffer", &cbuffer, size);
}

void CD3D9_VS30::Default_W_setShaderConst( IVertexShader* vs, const SMaterial& material, uint32_t pass)
{
	CD3D9VertexShader* d3d9vs = static_cast<CD3D9VertexShader*>(vs);
	CD3D9Driver* driver = static_cast<CD3D9Driver*>(g_Engine->getDriver());

	// para
	SParamW cbuffer;

	//wvp
	driver->getWVPMatrix(cbuffer.mWorldViewProjection);

	d3d9vs->setShaderConstant("g_cbuffer", &cbuffer, sizeof(cbuffer));
}


void CD3D9_VS30::Default_WCol_setShaderConst( IVertexShader* vs, const SMaterial& material, uint32_t pass )
{
	CD3D9VertexShader* d3d9vs = static_cast<CD3D9VertexShader*>(vs);
	CD3D9Driver* driver = static_cast<CD3D9Driver*>(g_Engine->getDriver());

	// para
	SParamWCol cbuffer;

	//wvp
	driver->getWVPMatrix(cbuffer.mWorldViewProjection);

	//color
	cbuffer.vColor =  material.Lighting ? SColorf(1.0f, 1.0f, 1.0f) : material.EmissiveColor;

	d3d9vs->setShaderConstant("g_cbuffer", &cbuffer, sizeof(cbuffer));
}

void CD3D9_VS30::Default_WColTex_setShaderConst( IVertexShader* vs, const SMaterial& material, uint32_t pass )
{
	CD3D9VertexShader* d3d9vs = static_cast<CD3D9VertexShader*>(vs);
	CD3D9Driver* driver = static_cast<CD3D9Driver*>(g_Engine->getDriver());

	// para
	SParamWColTex cbuffer;

	//wvp
	driver->getWVPMatrix(cbuffer.mWorldViewProjection);

	//color
	cbuffer.vColor =  material.Lighting ? SColorf(1.0f, 1.0f, 1.0f) : material.EmissiveColor;

	//tex
	bool useAnimTex = material.TextureLayer[0].UseTextureMatrix;
	cbuffer.Params[1] =  useAnimTex ? 1.0f : 0.0f;
	if (useAnimTex)
	{
		cbuffer.mTexture = *material.TextureLayer[0].TextureMatrix;
	}

	//set
	uint32_t size = useAnimTex ? sizeof(SParamWColTex) : (sizeof(SParamWColTex) - sizeof(matrix4));
	d3d9vs->setShaderConstant("g_cbuffer", &cbuffer, size);
}

void CD3D9_VS30::Default_WTex_setShaderConst( IVertexShader* vs, const SMaterial& material, uint32_t pass )
{
	CD3D9VertexShader* d3d9vs = static_cast<CD3D9VertexShader*>(vs);
	CD3D9Driver* driver = static_cast<CD3D9Driver*>(g_Engine->getDriver());

	// para
	SParamWTex cbuffer;

	//wvp
	driver->getWVPMatrix(cbuffer.mWorldViewProjection);

	//tex
	bool useAnimTex = material.TextureLayer[0].UseTextureMatrix;
	cbuffer.Params[1] =  useAnimTex ? 1.0f : 0.0f;
	if (useAnimTex)
	{
		cbuffer.mTexture = *material.TextureLayer[0].TextureMatrix;
	}

	//set
	uint32_t size = useAnimTex ? sizeof(SParamWTex) : (sizeof(SParamWTex) - sizeof(matrix4));
	d3d9vs->setShaderConstant("g_cbuffer", &cbuffer, size);
}

void CD3D9_VS30::Terrain_setShaderConst( IVertexShader* vs, const SMaterial& material, uint32_t pass )
{
	CD3D9VertexShader* d3d9vs = static_cast<CD3D9VertexShader*>(vs);
	CD3D9Driver* driver = static_cast<CD3D9Driver*>(g_Engine->getDriver());
	CD3D9SceneStateServices* sceneStateServices = static_cast<CD3D9SceneStateServices*>(driver->getSceneStateServices());
	const SFogParam& fogParam = sceneStateServices->getFog();
	ISceneEnvironment* sceneEnv = g_Engine->getSceneEnvironment();

	// para
	SParamTerrain cbuffer;

	//wvp
	driver->getWVPMatrix(cbuffer.mWorldViewProjection);
	driver->getWVMatrix(cbuffer.mWorldView);

	const SLight* l = sceneStateServices->getDynamicLight(ISceneEnvironment::INDEX_DIR_LIGHT);
	cbuffer.LightDir[0] = l->Direction.X;
	cbuffer.LightDir[1] = l->Direction.Y;
	cbuffer.LightDir[2] = l->Direction.Z;

	cbuffer.LightColor = SColorf(l->LightColor);

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
	}

	//set
	uint32_t size = useAnimTex ? sizeof(cbuffer) : (sizeof(cbuffer) - sizeof(matrix4));
	d3d9vs->setShaderConstant("g_cbuffer", &cbuffer, size);
}

void CD3D9_VS30::MapObjDiffuse_setShaderConst( IVertexShader* vs, const SMaterial& material, uint32_t pass )
{
	CD3D9VertexShader* d3d9vs = static_cast<CD3D9VertexShader*>(vs);
	CD3D9Driver* driver = static_cast<CD3D9Driver*>(g_Engine->getDriver());
	CD3D9SceneStateServices* sceneStateServices = static_cast<CD3D9SceneStateServices*>(driver->getSceneStateServices());
	const SFogParam& fogParam = sceneStateServices->getFog();
	ISceneEnvironment* sceneEnv = g_Engine->getSceneEnvironment();
	const vector3df& viewPos = g_Engine->getSceneManager()->getActiveCameraPos();

	// para
	SParamMapObjDiffuse cbuffer;

	//wvp
	driver->getWVPMatrix(cbuffer.mWorldViewProjection);
	driver->getWVMatrix(cbuffer.mWorldView);
	driver->getWMatrix(cbuffer.mWorld);

	//viewPos
	cbuffer.ViewPos[0] = viewPos.X;
	cbuffer.ViewPos[0] = viewPos.Y;
	cbuffer.ViewPos[0] = viewPos.Z;

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
	}

	//set
	uint32_t size = useAnimTex ? sizeof(SParamMapObjDiffuse) : (sizeof(SParamMapObjDiffuse) - sizeof(matrix4));
	d3d9vs->setShaderConstant("g_cbuffer", &cbuffer, size);
}

void CD3D9_VS30::MapObjSpecular_setShaderConst( IVertexShader* vs, const SMaterial& material, uint32_t pass )
{
	CD3D9VertexShader* d3d9vs = static_cast<CD3D9VertexShader*>(vs);
	CD3D9Driver* driver = static_cast<CD3D9Driver*>(g_Engine->getDriver());
	CD3D9SceneStateServices* sceneStateServices = static_cast<CD3D9SceneStateServices*>(driver->getSceneStateServices());
	const SFogParam& fogParam = sceneStateServices->getFog();
	ISceneEnvironment* sceneEnv = g_Engine->getSceneEnvironment();
	const vector3df& viewPos = g_Engine->getSceneManager()->getActiveCameraPos();

	// para
	SParamMapObjSpecular cbuffer;

	//wvp
	driver->getWVPMatrix(cbuffer.mWorldViewProjection);
	driver->getWVMatrix(cbuffer.mWorldView);
	driver->getWMatrix(cbuffer.mWorld);

	//viewPos
	cbuffer.ViewPos[0] = viewPos.X;
	cbuffer.ViewPos[0] = viewPos.Y;
	cbuffer.ViewPos[0] = viewPos.Z;

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
	}

	//set
	uint32_t size = useAnimTex ? sizeof(SParamMapObjSpecular) : (sizeof(SParamMapObjSpecular) - sizeof(matrix4));
	d3d9vs->setShaderConstant("g_cbuffer", &cbuffer, size);
}

#endif
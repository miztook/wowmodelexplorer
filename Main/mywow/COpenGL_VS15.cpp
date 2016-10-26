#include "stdafx.h"
#include "COpenGL_VS15.h"
#include "mywow.h"

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_OPENGL

#include "COpenGLShader.h"
#include "COpenGLDriver.h"
#include "COpenGLShaderServices.h"
#include "COpenGLSceneStateServices.h"

static COpenGL_VS15::SVertexShaderEntry g_openglvs15_vertexShaderEntries[] =
{
	{EVST_TERRAIN, "Terrain", COpenGL_VS15::Terrain_setShaderConst},
	{EVST_DEFAULT_P, "Default_P", COpenGL_VS15::Default_WCol_setShaderConst},
	{EVST_DEFAULT_PC, "Default_PC", COpenGL_VS15::Default_W_setShaderConst},
	{EVST_DEFAULT_PCT, "Default_PCT", COpenGL_VS15::Default_WTex_setShaderConst},
	{EVST_DEFAULT_PN, "Default_PN", COpenGL_VS15::Default_WCol_setShaderConst},
	{EVST_DEFAULT_PNC, "Default_PNC", COpenGL_VS15::Default_W_setShaderConst},
	{EVST_DEFAULT_PNCT, "Default_PNCT", COpenGL_VS15::Default_WTex_setShaderConst},
	{EVST_DEFAULT_PNCT2, "Default_PNCT2", COpenGL_VS15::Default_WTex_setShaderConst},
	{EVST_DEFAULT_PNT, "Default_PNT", COpenGL_VS15::Default_WColTex_setShaderConst},
	{EVST_DEFAULT_PT, "Default_PT", COpenGL_VS15::Default_WColTex_setShaderConst},

	{EVST_CDIFFUSE_T1, "CDiffuse_T1", COpenGL_VS15::CDiffuseT1_setShaderConst},
	{EVST_DIFFUSE_T1, "Diffuse_T1", COpenGL_VS15::DiffuseT1_setShaderConst},
	{EVST_DIFFUSE_ENV, "Diffuse_Env", COpenGL_VS15::DiffuseEnv_setShaderConst},
	{EVST_DIFFUSE_T1_T1, "Diffuse_T1_T1", COpenGL_VS15::DiffuseT1_setShaderConst},
	{EVST_DIFFUSE_T1_T2, "Diffuse_T1_T2", COpenGL_VS15::DiffuseT1_setShaderConst},
	{EVST_DIFFUSE_T1_ENV, "Diffuse_T1_Env", COpenGL_VS15::DiffuseT1_setShaderConst},
	{EVST_DIFFUSE_ENV_T1, "Diffuse_Env_T1", COpenGL_VS15::DiffuseEnv_setShaderConst},
	{EVST_DIFFUSE_ENV_ENV, "Diffuse_Env_Env", COpenGL_VS15::DiffuseEnv_setShaderConst},
	{EVST_DIFFUSE_T1_T1_T1, "Diffuse_T1_T1_T1", COpenGL_VS15::DiffuseT1_setShaderConst},
	{EVST_DIFFUSE_T1_T2_T1, "Diffuse_T1_T2_T1", COpenGL_VS15::DiffuseT1_setShaderConst},
	{EVST_DIFFUSE_T1_ENV_T1, "Diffuse_T1_Env_T1", COpenGL_VS15::DiffuseT1_setShaderConst},
	{EVST_DIFFUSE_T1_ENV_T2, "Diffuse_T1_Env_T2", COpenGL_VS15::DiffuseT1_setShaderConst},

	{EVST_MAPOBJ_DIFFUSE_T1, "MapObjDiffuse_T1", COpenGL_VS15::MapObjDiffuse_setShaderConst},
	{EVST_MAPOBJ_SPECULAR_T1, "MapObjSpecular_T1", COpenGL_VS15::MapObjSpecular_setShaderConst},
	{EVST_MAPOBJ_DIFFUSE_T1_T2, "MapObjDiffuse_T1_T2", COpenGL_VS15::MapObjDiffuse_setShaderConst},
	{EVST_MAPOBJ_DIFFUSE_T1_ENV_T2, "MapObjDiffuse_T1_Env_T2", COpenGL_VS15::MapObjDiffuse_setShaderConst},
	{EVST_MAPOBJ_DIFFUSE_T1_REFL, "MapObjDiffuse_T1_Refl", COpenGL_VS15::MapObjDiffuse_setShaderConst},
};

void COpenGL_VS15::loadAll(COpenGLShaderServices* shaderServices)
{
	u32 num = sizeof(g_openglvs15_vertexShaderEntries) / sizeof(SVertexShaderEntry);
	for (u32 i=0; i<num; ++i)
	{
		loadVShaderHLSL(shaderServices, g_openglvs15_vertexShaderEntries[i].vsType);
	}
}

bool COpenGL_VS15::loadVShaderHLSL(COpenGLShaderServices* shaderServices, E_VS_TYPE type)
{
	const c8* profile = "glvs_1_5";

	c8 basePath[128];
	Q_sprintf(basePath, 128, "Vertex\\%s\\", profile);

	s32 index = -1;
	u32 num = sizeof(g_openglvs15_vertexShaderEntries) / sizeof(SVertexShaderEntry);
	for (u32 i=0; i<num; ++i)
	{
		if(g_openglvs15_vertexShaderEntries[i].vsType == type)
		{
			index = (s32)i;
			break;
		}
	}

	if (index == -1)
		return false;

	string128 path(basePath);
	path.append(g_openglvs15_vertexShaderEntries[index].path.c_str());

	path.append(".fx");

	path.normalize();

	return shaderServices->loadVShaderHLSL(path.c_str(), "main", profile, g_openglvs15_vertexShaderEntries[index].vsType, g_openglvs15_vertexShaderEntries[index].callback);
}

void COpenGL_VS15::Default_W_setShaderConst( IVertexShader* vs, const SMaterial& material, u32 pass )
{
	COpenGLDriver* driver = static_cast<COpenGLDriver*>(g_Engine->getDriver());
	COpenGLShaderServices* services = static_cast<COpenGLShaderServices*>(driver->getShaderServices());

	// para
	SParamW cbuffer;

	//wvp
	driver->getWVPMatrix(cbuffer.mWorldViewProjection);
	cbuffer.mWorldViewProjection.transpose();

	services->setShaderUniformF("g_vsbuffer", (const f32*)&cbuffer, sizeof(cbuffer));
}

void COpenGL_VS15::Default_WCol_setShaderConst( IVertexShader* vs, const SMaterial& material, u32 pass )
{
	COpenGLDriver* driver = static_cast<COpenGLDriver*>(g_Engine->getDriver());
	COpenGLShaderServices* services = static_cast<COpenGLShaderServices*>(driver->getShaderServices());

	// para
	SParamWCol cbuffer;

	//wvp
	driver->getWVPMatrix(cbuffer.mWorldViewProjection);
	cbuffer.mWorldViewProjection.transpose();

	//color
	cbuffer.vColor =  material.Lighting ? SColorf(1.0f, 1.0f, 1.0f) : material.EmissiveColor;

	services->setShaderUniformF("g_vsbuffer", (const f32*)&cbuffer, sizeof(cbuffer));
}

void COpenGL_VS15::Default_WColTex_setShaderConst( IVertexShader* vs, const SMaterial& material, u32 pass )
{
	COpenGLDriver* driver = static_cast<COpenGLDriver*>(g_Engine->getDriver());
	COpenGLShaderServices* services = static_cast<COpenGLShaderServices*>(driver->getShaderServices());

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

void COpenGL_VS15::Default_WTex_setShaderConst( IVertexShader* vs, const SMaterial& material, u32 pass )
{
	COpenGLDriver* driver = static_cast<COpenGLDriver*>(g_Engine->getDriver());
	COpenGLShaderServices* services = static_cast<COpenGLShaderServices*>(driver->getShaderServices());

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

void COpenGL_VS15::CDiffuseT1_setShaderConst( IVertexShader* vs, const SMaterial& material, u32 pass )
{
	COpenGLDriver* driver = static_cast<COpenGLDriver*>(g_Engine->getDriver());
	COpenGLShaderServices* services = static_cast<COpenGLShaderServices*>(driver->getShaderServices());
	COpenGLSceneStateServices* sceneStateServices = static_cast<COpenGLSceneStateServices*>(driver->getSceneStateServices());
	const SFogParam& fogParam = sceneStateServices->getFog();

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

void COpenGL_VS15::DiffuseT1_setShaderConst( IVertexShader* vs, const SMaterial& material, u32 pass )
{
	COpenGLDriver* driver = static_cast<COpenGLDriver*>(g_Engine->getDriver());
	COpenGLShaderServices* services = static_cast<COpenGLShaderServices*>(driver->getShaderServices());
	COpenGLSceneStateServices* sceneStateServices = static_cast<COpenGLSceneStateServices*>(driver->getSceneStateServices());
	const SFogParam& fogParam = sceneStateServices->getFog();

	const SRenderUnit* unit = g_Engine->getSceneRenderServices()->getCurrentUnit();
	const SBoneMatrixArray* boneMats = unit->u.boneMatrixArray;
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

void COpenGL_VS15::DiffuseEnv_setShaderConst( IVertexShader* vs, const SMaterial& material, u32 pass )
{
	COpenGLDriver* driver = static_cast<COpenGLDriver*>(g_Engine->getDriver());
	COpenGLShaderServices* services = static_cast<COpenGLShaderServices*>(driver->getShaderServices());
	COpenGLSceneStateServices* sceneStateServices = static_cast<COpenGLSceneStateServices*>(driver->getSceneStateServices());
	const SFogParam& fogParam = sceneStateServices->getFog();

	const SRenderUnit* unit = g_Engine->getSceneRenderServices()->getCurrentUnit();
	const SBoneMatrixArray* boneMats = unit->u.boneMatrixArray;
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

void COpenGL_VS15::Terrain_setShaderConst( IVertexShader* vs, const SMaterial& material, u32 pass )
{
	COpenGLDriver* driver = static_cast<COpenGLDriver*>(g_Engine->getDriver());
	COpenGLShaderServices* services = static_cast<COpenGLShaderServices*>(driver->getShaderServices());
	COpenGLSceneStateServices* sceneStateServices = static_cast<COpenGLSceneStateServices*>(driver->getSceneStateServices());
	const SFogParam& fogParam = sceneStateServices->getFog();
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

	//clip plane
	plane3df plane;
	if (sceneStateServices->getClipPlane(0, plane))
	{
		cbuffer.ClipPlane0[0] = plane.Normal.X;
		cbuffer.ClipPlane0[1] = plane.Normal.Y;
		cbuffer.ClipPlane0[2] = plane.Normal.Z;
		cbuffer.ClipPlane0[3] = plane.D;
	}

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

void COpenGL_VS15::MapObjDiffuse_setShaderConst( IVertexShader* vs, const SMaterial& material, u32 pass )
{
	COpenGLDriver* driver = static_cast<COpenGLDriver*>(g_Engine->getDriver());
	COpenGLShaderServices* services = static_cast<COpenGLShaderServices*>(driver->getShaderServices());
	COpenGLSceneStateServices* sceneStateServices = static_cast<COpenGLSceneStateServices*>(driver->getSceneStateServices());
	const SFogParam& fogParam = sceneStateServices->getFog();
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

	//clip plane
	plane3df plane;
	if (sceneStateServices->getClipPlane(0, plane))
	{
		cbuffer.ClipPlane0[0] = plane.Normal.X;
		cbuffer.ClipPlane0[1] = plane.Normal.Y;
		cbuffer.ClipPlane0[2] = plane.Normal.Z;
		cbuffer.ClipPlane0[3] = plane.D;
	}

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

void COpenGL_VS15::MapObjSpecular_setShaderConst( IVertexShader* vs, const SMaterial& material, u32 pass )
{
	COpenGLDriver* driver = static_cast<COpenGLDriver*>(g_Engine->getDriver());
	COpenGLShaderServices* services = static_cast<COpenGLShaderServices*>(driver->getShaderServices());
	COpenGLSceneStateServices* sceneStateServices = static_cast<COpenGLSceneStateServices*>(g_Engine->getDriver()->getSceneStateServices());
	const SFogParam& fogParam = sceneStateServices->getFog();
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

	//clip plane
	plane3df plane;
	if (sceneStateServices->getClipPlane(0, plane))
	{
		cbuffer.ClipPlane0[0] = plane.Normal.X;
		cbuffer.ClipPlane0[1] = plane.Normal.Y;
		cbuffer.ClipPlane0[2] = plane.Normal.Z;
		cbuffer.ClipPlane0[3] = plane.D;
	}

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
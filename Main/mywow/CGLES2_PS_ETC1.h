#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_GLES2

#include "core.h"
#include "SColor.h"
#include "SMaterial.h"
#include "ITexture.h"
#include "IShader.h"

#define USE_ETC1_REPERAT_ADDRESS	2.0f
#define USE_ETC1	1.0f

class CGLES2ShaderServices;

class CGLES2_PS_ETC1
{
private:
	DISALLOW_COPY_AND_ASSIGN(CGLES2_PS_ETC1);

public:
	CGLES2_PS_ETC1() {}

	static void Terrain_setShaderConst(IPixelShader* ps, const SMaterial& material, u32 pass);
	static void UI_setShaderConst(IPixelShader* ps, const SMaterial& material, u32 pass);
	static void Default_setShaderConst_T1(IPixelShader* ps, const SMaterial& material, u32 pass);
	static void Default_setShaderConst_T2(IPixelShader* ps, const SMaterial& material, u32 pass);

	static void DiffuseT1_setShaderConst(IPixelShader* ps, const SMaterial& material, u32 pass);
	static void DiffuseT2_setShaderConst(IPixelShader* ps, const SMaterial& material, u32 pass);
	static void DiffuseT3_setShaderConst(IPixelShader* ps, const SMaterial& material, u32 pass);

	static void MapObjOpaque_setShaderConst(IPixelShader* ps, const SMaterial& material, u32 pass);
	static void MapObj_setShaderConst(IPixelShader* ps, const SMaterial& material, u32 pass);
	static void MapObjTwoLayer_setShaderConst(IPixelShader* ps, const SMaterial& material, u32 pass);
	static void MapObjTwoLayerOpaque_setShaderConst(IPixelShader* ps, const SMaterial& material, u32 pass);

public:
	void loadAll(CGLES2ShaderServices* shaderServices);

	bool loadPShaderHLSL(CGLES2ShaderServices* shaderServices, E_PS_TYPE type, E_PS_MACRO macro);

	static float getTextureParams(const SMaterial& material, u32 index, ITexture* tex);

	struct SPixelShaderEntry
	{
		E_PS_TYPE psType;
		string64	path;
		PSHADERCONSTCALLBACK callback;
	};

private:

	struct Common_CB
	{
		float				TextureParams[4];
		float				params[4];		//0: alphatest, 1: alpharef
	};

	struct Terrain_CB
	{
		float	params[4];		//0: numTextures
		SColorf		fogColor;
		float				TextureParams[4];
	};

	struct Diffuse_CB
	{
		SColorf			FogColor;	
		float				TextureParams[4];
		float				params[4];		//0: alphatest, 1: alpharef
	};

	struct MapObjOpaque_CB
	{
		SColorf			AmbientColor;
		SColorf			FogColor;
		float				TextureParams[4];
	};

	struct MapObjAlphaTest_CB
	{
		SColorf			AmbientColor;
		SColorf			FogColor;
		float				TextureParams[4];
		float			params[4];			//0: alphatest, 1: ref
	};
};

inline float CGLES2_PS_ETC1::getTextureParams( const SMaterial& material, u32 index, ITexture* tex )
{
	if (index >= MATERIAL_MAX_TEXTURES || !tex)
		return 0.0f;

	if (tex->getColorFormat() == ECF_ETC1_RGBA)
		return material.TextureLayer[index].TextureWrapV == ETC_REPEAT ? USE_ETC1_REPERAT_ADDRESS : USE_ETC1;
	else
		return 0.0f;
}

#endif
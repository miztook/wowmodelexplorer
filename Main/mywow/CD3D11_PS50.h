#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D11

#include "core.h"
#include "SColor.h"
#include "SMaterial.h"
#include "IShader.h"

class CD3D11ShaderServices;

class CD3D11_PS50
{
private:
	DISALLOW_COPY_AND_ASSIGN(CD3D11_PS50);

public:
	CD3D11_PS50() {}

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
	void loadAll(CD3D11ShaderServices* shaderServices);

	bool loadPShaderHLSL(CD3D11ShaderServices* shaderServices, E_PS_TYPE type, E_PS_MACRO macro);

	struct SPixelShaderEntry
	{
		E_PS_TYPE psType;
		string64	path;
		PSHADERCONSTCALLBACK callback;
	};

private:

	struct Common_CB
	{
		float				params[4];		//0: alphatest, 1: alpharef
	};

	struct Terrain_CB
	{
		float params[4];		//0: numTextures
		SColorf			fogColor;
	};

	struct Diffuse_CB
	{
		float				params[4];		//0: alphatest, 1: alpharef
		SColorf			fogColor;
	};

	struct MapObjOpaque_CB
	{
		SColorf			AmbientColor;
		SColorf			FogColor;
	};

	struct MapObjAlphaTest_CB
	{
		SColorf			AmbientColor;
		SColorf			FogColor;
		float			params[4];			//0: alphatest, 1: ref
	};

};

#endif
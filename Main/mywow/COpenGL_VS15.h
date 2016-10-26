#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_OPENGL

#include "core.h"
#include "SColor.h"
#include "SMaterial.h"
#include "IShader.h"

class COpenGLShaderServices;

class COpenGL_VS15
{
private:
	DISALLOW_COPY_AND_ASSIGN(COpenGL_VS15);

public:
	COpenGL_VS15() {}

	static void Terrain_setShaderConst(IVertexShader* vs, const SMaterial& material, u32 pass);
	static void Default_W_setShaderConst(IVertexShader* vs, const SMaterial& material, u32 pass);
	static void Default_WCol_setShaderConst(IVertexShader* vs, const SMaterial& material, u32 pass);
	static void Default_WColTex_setShaderConst(IVertexShader* vs, const SMaterial& material, u32 pass);
	static void Default_WTex_setShaderConst(IVertexShader* vs, const SMaterial& material, u32 pass);

	static void CDiffuseT1_setShaderConst(IVertexShader* vs, const SMaterial& material, u32 pass);
	static void DiffuseT1_setShaderConst(IVertexShader* vs, const SMaterial& material, u32 pass);
	static void DiffuseEnv_setShaderConst(IVertexShader* vs, const SMaterial& material, u32 pass);

	static void MapObjDiffuse_setShaderConst(IVertexShader* vs, const SMaterial& material, u32 pass);
	static void MapObjSpecular_setShaderConst(IVertexShader* vs, const SMaterial& material, u32 pass);

public:
	void loadAll(COpenGLShaderServices* shaderServices);

	bool loadVShaderHLSL(COpenGLShaderServices* shaderServices, E_VS_TYPE type);

	struct SVertexShaderEntry
	{
		E_VS_TYPE vsType;
		string64	path;
		VSHADERCONSTCALLBACK callback;
	};

private:

	struct SParamW
	{
		matrix4 		mWorldViewProjection;    // World * View * Projection
	};

	struct SParamWCol
	{
		matrix4 		mWorldViewProjection;    // World * View * Projection
		SColorf			vColor;
	};

	struct SParamWTex
	{
		matrix4 		mWorldViewProjection;    // World * View * Projection
		float				Params[4];		//0: numBones, 1: animTexture
		matrix4			mTexture;
	};

	struct SParamWColTex
	{
		matrix4 		mWorldViewProjection;    // World * View * Projection
		SColorf			vColor;
		float				Params[4];		//0: numBones, 1: animTexture
		matrix4			mTexture;
	};

	struct SCDiffuse
	{
		matrix4 		mWorldViewProjection;    // World * View * Projection
		matrix4			mWorldView;		//World * View
		SColorf			vColor;
		float		fogParams[4];		//0: fogMode, 1: fogStart, 2: fogEnd, 3: fogDensity
		float				Params[4];		//0: numBones, 1: animTexture, 2: fogEnable
		matrix4			mTexture;
	};

	struct SDiffuseT
	{
		matrix4 		mWorldViewProjection;    // World * View * Projection
		matrix4			mWorldView;		//World * View
		SColorf			vColor;
		float		fogParams[4];		//0: fogMode, 1: fogStart, 2: fogEnd, 3: fogDensity
		float				Params[4];		//0: numBones, 1: animTexture, 2: fogEnable
		matrix4			mTexture;

		float		boneMatrices[MAX_BONE_NUM][12];
	};

	struct SDiffuseEnv
	{
		matrix4 		mWorldViewProjection;    // World * View * Projection
		matrix4			mWorldView;		//World * View
		SColorf			vColor;
		float		fogParams[4];		//0: fogMode, 1: fogStart, 2: fogEnd, 3: fogDensity
		float				Params[4];		//0: numBones, 1: animTexture, 2: fogEnable

		float		boneMatrices[MAX_BONE_NUM][12];
	};

	struct SParamMapObjDiffuse
	{
		matrix4 		mWorldViewProjection;    // World * View * Projection
		matrix4			mWorldView;		//World * View
		float		LightDir[4];				//light dir
		SColorf		LightColor;
		float		MaterialParams[4];	//0: ambient, 1: diffuse, 2: specular
		float		fogParams[4];		//0: fogMode, 1: fogStart, 2: fogEnd, 3: fogDensity
		float		ClipPlane0[4];
		float				Params[4];		//0: animTexture, 1: fogEnable
		matrix4			mTexture;
	};

	struct SParamMapObjSpecular
	{
		matrix4 		mWorldViewProjection;    // World * View * Projection
		matrix4			mWorldView;		//World * View
		float		LightDir[4];				//light dir
		SColorf		LightColor;
		float		MaterialParams[4];	//0: ambient, 1: diffuse, 2: specular
		float		fogParams[4];		//0: fogMode, 1: fogStart, 2: fogEnd, 3: fogDensity
		float		ClipPlane0[4];
		float				Params[4];		//0: animTexture, 1: fogEnable
		matrix4			mTexture;
	};

	struct SParamTerrain
	{
		matrix4 		mWorldViewProjection;    // World * View * Projection
		matrix4			mWorldView;		//World * View
		float		LightDir[4];				//light dir
		SColorf		LightColor;
		float		MaterialParams[4];	//0: ambient, 1: diffuse, 2: specular
		float		fogParams[4];		//0: fogMode, 1: fogStart, 2: fogEnd, 3: fogDensity
		float		ClipPlane0[4];
		float				Params[4];		//0: animTexture, 1: fogEnable
		matrix4			mTexture;
	};
	
};

#endif
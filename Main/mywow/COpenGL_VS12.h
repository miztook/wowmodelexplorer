#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_OPENGL

#include "core.h"
#include "SColor.h"
#include "SMaterial.h"
#include "IShader.h"

class COpenGLShaderServices;

class COpenGL_VS12
{
private:
	DISALLOW_COPY_AND_ASSIGN(COpenGL_VS12);

public:
	COpenGL_VS12() {}

	static void Default_W_setShaderConst(IVertexShader* vs, const SMaterial& material, u32 pass);
	static void Default_WCol_setShaderConst(IVertexShader* vs, const SMaterial& material, u32 pass);
	static void Default_WColTex_setShaderConst(IVertexShader* vs, const SMaterial& material, u32 pass);
	static void Default_WTex_setShaderConst(IVertexShader* vs, const SMaterial& material, u32 pass);

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
};

#endif
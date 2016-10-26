#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_OPENGL

#include "core.h"
#include "SColor.h"
#include "SMaterial.h"
#include "IShader.h"

class COpenGLShaderServices;

class COpenGL_PS33
{
private:
	DISALLOW_COPY_AND_ASSIGN(COpenGL_PS33);

public:
	COpenGL_PS33() {}

	static void Default_setShaderConst_T1(IPixelShader* ps, const SMaterial& material, u32 pass);
	static void Default_setShaderConst_T2(IPixelShader* ps, const SMaterial& material, u32 pass);

public:
	void loadAll(COpenGLShaderServices* shaderServices);

private:
	struct SPixelShaderEntry
	{
		E_PS_TYPE psType;
		string64	path;
		PSHADERCONSTCALLBACK callback;
	};


	struct Terrain_CB
	{
		float params[4];		//0: numTextures
		SColorf		fogColor;
	};

	struct UI_CB
	{
		float params[4];	//0: alpha, 1: alphachannel
	};

	struct Diffuse_CB
	{
		SColorf		FogColor;
	};

	struct MapObj_CB
	{
		SColorf		AmbientColor;
		SColorf		FogColor;
	};

};


#endif
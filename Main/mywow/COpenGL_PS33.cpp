#include "stdafx.h"
#include "COpenGL_PS33.h"
#include "mywow.h"

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_OPENGL

#include "COpenGLShader.h"
#include "COpenGLShaderServices.h"

void COpenGL_PS33::loadAll( COpenGLShaderServices* shaderServices )
{
	static SPixelShaderEntry pixelShaderEntries[] =
	{
		{EPST_DEFAULT_P, "Default_P", NULL},
		{EPST_DEFAULT_PC, "Default_PC", NULL},
		{EPST_DEFAULT_PCT, "Default_PCT", Default_setShaderConst_T1},
		{EPST_DEFAULT_PN, "Default_PN", NULL},
		{EPST_DEFAULT_PNC, "Default_PNC", NULL},
		{EPST_DEFAULT_PNCT, "Default_PNCT", Default_setShaderConst_T1},
		{EPST_DEFAULT_PNCT2, "Default_PNCT2", Default_setShaderConst_T2},
		{EPST_DEFAULT_PNT, "Default_PNT", Default_setShaderConst_T1},
		{EPST_DEFAULT_PT, "Default_PT", Default_setShaderConst_T1},
	};

	const c8* profile = "glps_3_3";

	//load

	c8 basePath[128];
	sprintf_s(basePath, 128, "Pixel\\%s\\", profile);

	u32 num = sizeof(pixelShaderEntries) / sizeof(SPixelShaderEntry);
	for (u32 i=0; i<num; ++i)
	{
		string128 path(basePath);
		path.append(pixelShaderEntries[i].path.c_str());

		path.append(".fx");

		shaderServices->loadPShaderHLSL(path.c_str(), "main", profile, pixelShaderEntries[i].psType, pixelShaderEntries[i].callback);
	}
}

void COpenGL_PS33::Default_setShaderConst_T1( IPixelShader* ps, const SMaterial& material, u32 pass )
{

}

void COpenGL_PS33::Default_setShaderConst_T2( IPixelShader* ps, const SMaterial& material, u32 pass )
{

}

#endif
#include "stdafx.h"
#include "COpenGL_PS12.h"
#include "mywow.h"

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_OPENGL

#include "COpenGLShader.h"
#include "COpenGLShaderServices.h"
#include "COpenGLDriver.h"
#include "COpenGLMaterialRenderServices.h"
#include "COpenGLSceneStateServices.h"

static COpenGL_PS12::SPixelShaderEntry g_openglps12_pixelShaderEntries[] =
{
	{EPST_DEFAULT_P, "Default_P", NULL_PTR},
	{EPST_DEFAULT_PC, "Default_PC", NULL_PTR},
	{EPST_DEFAULT_PCT, "Default_PCT", COpenGL_PS12::Default_setShaderConst_T1},
	{EPST_DEFAULT_PN, "Default_PN", NULL_PTR},
	{EPST_DEFAULT_PNC, "Default_PNC", NULL_PTR},
	{EPST_DEFAULT_PNCT, "Default_PNCT", COpenGL_PS12::Default_setShaderConst_T1},
	{EPST_DEFAULT_PNCT2, "Default_PNCT2", COpenGL_PS12::Default_setShaderConst_T2},
	{EPST_DEFAULT_PNT, "Default_PNT", COpenGL_PS12::Default_setShaderConst_T1},
	{EPST_DEFAULT_PT, "Default_PT", COpenGL_PS12::Default_setShaderConst_T1},

	{EPST_UI, "UI", COpenGL_PS12::UI_setShaderConst},
};

void COpenGL_PS12::loadAll(COpenGLShaderServices* shaderServices)
{
	u32 num = sizeof(g_openglps12_pixelShaderEntries) / sizeof(SPixelShaderEntry);
	for (u32 i=0; i<num; ++i)
	{
		loadPShaderHLSL(shaderServices, g_openglps12_pixelShaderEntries[i].psType, PS_Macro_None);
	}
}

bool COpenGL_PS12::loadPShaderHLSL(COpenGLShaderServices* shaderServices, E_PS_TYPE type, E_PS_MACRO macro)
{
	const c8* profile = "glps_1_2";

	c8 basePath[128];
	Q_sprintf(basePath, 128, "Pixel\\%s\\", profile);

	s32 index = -1;
	u32 num = sizeof(g_openglps12_pixelShaderEntries) / sizeof(SPixelShaderEntry);
	for (u32 i=0; i<num; ++i)
	{
		if (g_openglps12_pixelShaderEntries[i].psType == type)
		{
			index = (s32)i;
			break;
		}
	}

	if (index == -1)
		return false;

	string128 path(basePath);
	path.append(g_openglps12_pixelShaderEntries[index].path.c_str());

	path.append(".fx");

	path.normalize();

	shaderServices->loadPShaderHLSL(path.c_str(), "main", profile, g_openglps12_pixelShaderEntries[index].psType, macro, g_openglps12_pixelShaderEntries[index].callback);

	return true;
}

void COpenGL_PS12::Default_setShaderConst_T1( IPixelShader* ps, const SMaterial& material, u32 pass )
{
	
}

void COpenGL_PS12::Default_setShaderConst_T2( IPixelShader* ps, const SMaterial& material, u32 pass )
{
	
}

void COpenGL_PS12::UI_setShaderConst( IPixelShader* ps, const SMaterial& material, u32 pass )
{
	
}

#endif


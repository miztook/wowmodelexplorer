#include "stdafx.h"
#include "COpenGL_VS33.h"
#include "mywow.h"

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_OPENGL

#include "COpenGLShader.h"
#include "COpenGLShaderServices.h"

void COpenGL_VS33::loadAll( COpenGLShaderServices* shaderServices )
{
	static SVertexShaderEntry vertexShaderEntries[] =
	{
		{EVST_DEFAULT_P, "Default_P", Default_WCol_setShaderConst},
		{EVST_DEFAULT_PC, "Default_PC", Default_W_setShaderConst},
		{EVST_DEFAULT_PCT, "Default_PCT", Default_WTex_setShaderConst},
		{EVST_DEFAULT_PN, "Default_PN", Default_WCol_setShaderConst},
		{EVST_DEFAULT_PNC, "Default_PNC", Default_W_setShaderConst},
		{EVST_DEFAULT_PNCT, "Default_PNCT", Default_WTex_setShaderConst},
		{EVST_DEFAULT_PNCT2, "Default_PNCT2", Default_WTex_setShaderConst},
		{EVST_DEFAULT_PNT, "Default_PNT", Default_WColTex_setShaderConst},
		{EVST_DEFAULT_PT, "Default_PT", Default_WColTex_setShaderConst},
	};

	const c8* profile = "glvs_3_3";

	//load

	c8 basePath[128];
	sprintf_s(basePath, 128, "Vertex\\%s\\", profile);

	u32 num = sizeof(vertexShaderEntries) / sizeof(SVertexShaderEntry);
	for (u32 i=0; i<num; ++i)
	{
		string128 path(basePath);
		path.append(vertexShaderEntries[i].path.c_str());

		path.append(".fx");

		shaderServices->loadVShaderHLSL(path.c_str(), "main", profile, vertexShaderEntries[i].vsType, vertexShaderEntries[i].callback);
	}
}

void COpenGL_VS33::Default_W_setShaderConst( IVertexShader* vs, const SMaterial& material, u32 pass )
{

}

void COpenGL_VS33::Default_WCol_setShaderConst( IVertexShader* vs, const SMaterial& material, u32 pass )
{

}

void COpenGL_VS33::Default_WColTex_setShaderConst( IVertexShader* vs, const SMaterial& material, u32 pass )
{

}

void COpenGL_VS33::Default_WTex_setShaderConst( IVertexShader* vs, const SMaterial& material, u32 pass )
{

}

#endif
#include "stdafx.h"
#include "COpenGL_VS12.h"
#include "mywow.h"

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_OPENGL

#include "COpenGLShader.h"
#include "COpenGLShaderServices.h"
#include "COpenGLDriver.h"

static COpenGL_VS12::SVertexShaderEntry g_openglvs12_vertexShaderEntries[] =
{
	{EVST_DEFAULT_P, "Default_P", COpenGL_VS12::Default_WCol_setShaderConst},
	{EVST_DEFAULT_PC, "Default_PC", COpenGL_VS12::Default_W_setShaderConst},
	{EVST_DEFAULT_PCT, "Default_PCT", COpenGL_VS12::Default_WTex_setShaderConst},
	{EVST_DEFAULT_PN, "Default_PN", COpenGL_VS12::Default_WCol_setShaderConst},
	{EVST_DEFAULT_PNC, "Default_PNC", COpenGL_VS12::Default_W_setShaderConst},
	{EVST_DEFAULT_PNCT, "Default_PNCT", COpenGL_VS12::Default_WTex_setShaderConst},
	{EVST_DEFAULT_PNCT2, "Default_PNCT2", COpenGL_VS12::Default_WTex_setShaderConst},
	{EVST_DEFAULT_PNT, "Default_PNT", COpenGL_VS12::Default_WColTex_setShaderConst},
	{EVST_DEFAULT_PT, "Default_PT", COpenGL_VS12::Default_WColTex_setShaderConst},
};

void COpenGL_VS12::loadAll(COpenGLShaderServices* shaderServices)
{
	u32 num = sizeof(g_openglvs12_vertexShaderEntries) / sizeof(SVertexShaderEntry);
	for (u32 i=0; i<num; ++i)
	{
		loadVShaderHLSL(shaderServices, g_openglvs12_vertexShaderEntries[i].vsType);
	}
}

bool COpenGL_VS12::loadVShaderHLSL(COpenGLShaderServices* shaderServices, E_VS_TYPE type)
{
	const c8* profile = "glvs_1_2";

	//load

	c8 basePath[128];
	Q_sprintf(basePath, 128, "Vertex\\%s\\", profile);

	s32 index = -1;
	u32 num = sizeof(g_openglvs12_vertexShaderEntries) / sizeof(SVertexShaderEntry);
	for (u32 i=0; i<num; ++i)
	{
		if(g_openglvs12_vertexShaderEntries[i].vsType == type)
		{
			index = (s32)i;
			break;
		}
	}

	if (index == -1)
		return false;

	string128 path(basePath);
	path.append(g_openglvs12_vertexShaderEntries[index].path.c_str());

	path.append(".fx");

	path.normalize();

	return shaderServices->loadVShaderHLSL(path.c_str(), "main", profile, g_openglvs12_vertexShaderEntries[index].vsType, g_openglvs12_vertexShaderEntries[index].callback);
}

void COpenGL_VS12::Default_W_setShaderConst( IVertexShader* vs, const SMaterial& material, u32 pass )
{
	COpenGLDriver* driver = static_cast<COpenGLDriver*>(g_Engine->getDriver());
	COpenGLShaderServices* services = static_cast<COpenGLShaderServices*>(driver->getShaderServices());

	// para
	SParamW cbuffer;

	//wvp
	driver->getWVPMatrix(cbuffer.mWorldViewProjection);

	services->setShaderUniformF("g_vsbuffer.mWorldViewProjection", (const f32*)&cbuffer.mWorldViewProjection, sizeof(cbuffer.mWorldViewProjection));

}

void COpenGL_VS12::Default_WCol_setShaderConst( IVertexShader* vs, const SMaterial& material, u32 pass )
{
	COpenGLDriver* driver = static_cast<COpenGLDriver*>(g_Engine->getDriver());
	COpenGLShaderServices* services = static_cast<COpenGLShaderServices*>(driver->getShaderServices());

	// para
	SParamWCol cbuffer;

	//wvp
	driver->getWVPMatrix(cbuffer.mWorldViewProjection);

	//color
	cbuffer.vColor =  material.Lighting ? SColorf(1.0f, 1.0f, 1.0f) : material.EmissiveColor;

	services->setShaderUniformF("g_vsbuffer.mWorldViewProjection", (const f32*)&cbuffer.mWorldViewProjection, sizeof(cbuffer.mWorldViewProjection));

	services->setShaderUniformF("g_vsbuffer.vColor", (const f32*)&cbuffer.vColor, sizeof(cbuffer.vColor));
}

void COpenGL_VS12::Default_WColTex_setShaderConst( IVertexShader* vs, const SMaterial& material, u32 pass )
{
	COpenGLDriver* driver = static_cast<COpenGLDriver*>(g_Engine->getDriver());
	COpenGLShaderServices* services = static_cast<COpenGLShaderServices*>(driver->getShaderServices());

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

	services->setShaderUniformF("g_vsbuffer.mWorldViewProjection", (const f32*)&cbuffer.mWorldViewProjection, sizeof(cbuffer.mWorldViewProjection));

	services->setShaderUniformF("g_vsbuffer.vColor", (const f32*)&cbuffer.vColor, sizeof(cbuffer.vColor));

	services->setShaderUniformF("g_vsbuffer.Params", cbuffer.Params, sizeof(cbuffer.Params));

	if (useAnimTex)
	{
		services->setShaderUniformF("g_vsbuffer.mTexture", (const f32*)&cbuffer.mTexture, sizeof(cbuffer.mTexture));
	}
}

void COpenGL_VS12::Default_WTex_setShaderConst( IVertexShader* vs, const SMaterial& material, u32 pass )
{
	COpenGLDriver* driver = static_cast<COpenGLDriver*>(g_Engine->getDriver());
	COpenGLShaderServices* services = static_cast<COpenGLShaderServices*>(driver->getShaderServices());

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

	services->setShaderUniformF("g_vsbuffer.mWorldViewProjection", (const f32*)&cbuffer.mWorldViewProjection, sizeof(cbuffer.mWorldViewProjection));

	services->setShaderUniformF("g_vsbuffer.Params", cbuffer.Params, sizeof(cbuffer.Params));

	if (useAnimTex)
	{
		services->setShaderUniformF("g_vsbuffer.mTexture", (const f32*)&cbuffer.mTexture, sizeof(cbuffer.mTexture));
	}
}

#endif
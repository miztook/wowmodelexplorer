#include "stdafx.h"
#include "CD3D9ShaderServices.h"
#include "mywow.h"
#include "CD3D9Shader.h"
#include "CD3D9Texture.h"

struct AlphaTest_CB
{
	float params[4];		//0: alphascale
};

void AlphaTest_setShaderConst(IPixelShader* ps, const SMaterial& material);

void CD3D9ShaderServices::loadAllPixelShaders(const c8* profile)
{
	c8 basePath[128];
	sprintf_s(basePath, 128, "Pixel\\%s\\", profile);

	//alphatest
	{
		string128 path(basePath);
		path.append("AlphaTest.fx");

		loadPShaderHLSL(path.c_str(), "main", profile, EPST_ALPHATEST, AlphaTest_setShaderConst);
	}
	
}

void AlphaTest_setShaderConst( IPixelShader* ps, const SMaterial& material )
{
	CD3D9PixelShader* d3d9ps = static_cast<CD3D9PixelShader*>(ps);
	IVideoDriver* driver = g_Engine->getDriver();
	IDirect3DDevice9*	pID3DDevice = (IDirect3DDevice9*)driver->getD3DDevice();

	AlphaTest_CB cbuffer;
	cbuffer.params[0] = material.getMaterialAlpha();

	//set
	pID3DDevice->SetPixelShaderConstantF(0, (const float*)&cbuffer, 1);

	//texture
	u32 idx = d3d9ps->getRegIndex("g_TexSampler");
	CD3D9Texture* tex = static_cast<CD3D9Texture*>(driver->getTexture(0));
	pID3DDevice->SetTexture(idx, tex ? tex->getDXTexture() : NULL);
}

#include "stdafx.h"
#include "CD3D9ShaderServices.h"
#include "mywow.h"
#include "CD3D9Shader.h"
#include "CD3D9Texture.h"

struct Terrain_CB
{
	float params[4];		//0: numTextures
};

void Terrain_setShaderConst(IPixelShader* ps, const SMaterial& material);

void CD3D9ShaderServices::loadAllPixelShaders(const c8* profile)
{
	c8 basePath[128];
	sprintf_s(basePath, 128, "Pixel\\%s\\", profile);
	
	//terrain
	{
		string128 path(basePath);
		path.append("Terrain.fx");

		loadPShaderHLSL(path.c_str(), "main", profile, EPST_TERRAIN, Terrain_setShaderConst);
	}
}

void Terrain_setShaderConst(IPixelShader* ps, const SMaterial& material)
{
	CD3D9PixelShader* d3d9ps = static_cast<CD3D9PixelShader*>(ps);
	IVideoDriver* driver = g_Engine->getDriver();
	IDirect3DDevice9*	pID3DDevice = (IDirect3DDevice9*)driver->getD3DDevice();
	const SMapChunk* chunk = (SMapChunk*)g_Engine->getSceneRenderServices()->getCurrentUnit()->u.chunk;
	
	// param
	Terrain_CB cbuffer;
	cbuffer.params[0] =  (float)chunk->numTextures;

	//set
	pID3DDevice->SetPixelShaderConstantF(0, (const float*)&cbuffer, 1);

	//texture
	u32 idx = d3d9ps->getRegIndex("g_TexSampler0");
	driver->setTexture(idx, chunk->textures[0]);

	if(chunk->numTextures > 1)
	{
		idx = d3d9ps->getRegIndex("g_TexSampler1");
		driver->setTexture(idx, chunk->textures[1]);
	}

	if(chunk->numTextures > 2)
	{
		idx = d3d9ps->getRegIndex("g_TexSampler2");
		driver->setTexture(idx, chunk->textures[2]);
	}

	if(chunk->numTextures > 3)
	{
		idx = d3d9ps->getRegIndex("g_TexSampler3");
		driver->setTexture(idx, chunk->textures[3]);
	}
	
	idx = d3d9ps->getRegIndex("g_TexSampler4");
	driver->setTexture(idx, chunk->blendmap);
}
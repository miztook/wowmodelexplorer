#pragma once

#include "base.h"
#include "IMaterialRenderer.h"
#include <map>

class CD3D9MaterialRenderer_OneColor : public IMaterialRenderer
{
public:
	CD3D9MaterialRenderer_OneColor()
	{
		MaterialBlock.textureUnits[0].colorOp = ETO_ARG1;
		MaterialBlock.textureUnits[0].colorArg1 = ETA_DIFFUSE;
		MaterialBlock.textureUnits[0].alphaOp = ETO_DISABLE;

		MaterialBlock.textureUnits[1].alphaOp = ETO_DISABLE;
		MaterialBlock.textureUnits[1].colorOp = ETO_DISABLE;

		MaterialBlock.alphaBlendEnabled = false;
		MaterialBlock.alphaTestEnabled = false;
	}

	virtual void OnSetMaterial( const SMaterial& material, const SMaterial& lastMaterial, bool resetAllRenderStates );
};

class CD3D9MaterialRenderer_Solid : public IMaterialRenderer
{
public:
	CD3D9MaterialRenderer_Solid()
	{
		MaterialBlock.textureUnits[0].colorOp = ETO_MODULATE;
		MaterialBlock.textureUnits[0].colorArg1 = ETA_TEXTURE;
		MaterialBlock.textureUnits[0].colorArg2 = ETA_DIFFUSE;
		MaterialBlock.textureUnits[0].alphaOp = ETO_DISABLE;

		MaterialBlock.textureUnits[1].alphaOp = ETO_DISABLE;
		MaterialBlock.textureUnits[1].colorOp = ETO_DISABLE;

		MaterialBlock.alphaBlendEnabled = false;
		MaterialBlock.alphaTestEnabled = false;
	}

	virtual void OnSetMaterial( const SMaterial& material, const SMaterial& lastMaterial, bool resetAllRenderStates );
};

class CD3D9MaterialRenderer_LightMap : public IMaterialRenderer
{
public:
	virtual void OnSetMaterial( const SMaterial& material, const SMaterial& lastMaterial, bool resetAllRenderStates );
};

class CD3D9MaterialRenderer_DetailMap : public IMaterialRenderer
{
public:
	virtual void OnSetMaterial( const SMaterial& material, const SMaterial& lastMaterial, bool resetAllRenderStates );
};

class CD3D9MaterialRenderer_Transparent_Alpha_Blend : public IMaterialRenderer
{
public:
	CD3D9MaterialRenderer_Transparent_Alpha_Blend()
	{
		MaterialBlock.textureUnits[0].colorOp = ETO_MODULATE;
		MaterialBlock.textureUnits[0].colorArg1 = ETA_TEXTURE;
		MaterialBlock.textureUnits[0].colorArg2 = ETA_DIFFUSE;
		MaterialBlock.textureUnits[0].alphaOp = ETO_MODULATE;
		MaterialBlock.textureUnits[0].alphaArg1 = ETA_TEXTURE;
		MaterialBlock.textureUnits[0].alphaArg2 = ETA_DIFFUSE;

		MaterialBlock.textureUnits[1].alphaOp = ETO_DISABLE;
		MaterialBlock.textureUnits[1].colorOp = ETO_DISABLE;

		MaterialBlock.alphaBlendEnabled = true;
		MaterialBlock.alphaTestEnabled = false;
	}

	virtual void OnSetMaterial( const SMaterial& material, const SMaterial& lastMaterial, bool resetAllRenderStates );
	virtual void OnRender(const SMaterial& material, u32 pass);
};

class CD3D9MaterialRenderer_Transparent_Alpha_Test : public IMaterialRenderer
{
public:
	CD3D9MaterialRenderer_Transparent_Alpha_Test()
	{
		MaterialBlock.textureUnits[0].colorOp = ETO_MODULATE;
		MaterialBlock.textureUnits[0].colorArg1 = ETA_TEXTURE;
		MaterialBlock.textureUnits[0].colorArg2 = ETA_DIFFUSE;
		MaterialBlock.textureUnits[0].alphaOp = ETO_MODULATE;
		MaterialBlock.textureUnits[0].alphaArg1 = ETA_TEXTURE;
		MaterialBlock.textureUnits[0].alphaArg2 = ETA_DIFFUSE;

		MaterialBlock.textureUnits[1].alphaOp = ETO_DISABLE;
		MaterialBlock.textureUnits[1].colorOp = ETO_DISABLE;

		MaterialBlock.alphaBlendEnabled = false;
		MaterialBlock.alphaTestEnabled = true;
		MaterialBlock.alphaTestFunc = ECFN_GREATEREQUAL;
		MaterialBlock.alphaTestRef = 204;
	}

	virtual void OnSetMaterial( const SMaterial& material, const SMaterial& lastMaterial, bool resetAllRenderStates );
	virtual void OnRender(const SMaterial& material, u32 pass);
};

class CD3D9MaterialRenderer_Terrain_MultiPass : public IMaterialRenderer
{
public:
	virtual u32 getNumPasses() const;
	virtual void OnSetMaterial( const SMaterial& material, const SMaterial& lastMaterial, bool resetAllRenderStates);
	virtual void OnUnsetMaterial();
	virtual void OnRender(const SMaterial& material, u32 pass);

private:
	SOverrideMaterial		OldOverrideMaterial;
	E_TEXTURE_CLAMP		OldTextureWrap[MATERIAL_MAX_TEXTURES][2];
};
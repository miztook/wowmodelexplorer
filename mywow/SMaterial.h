#pragma once

#include "base.h"
#include "SColor.h"

class ITexture;
class IVertexShader;
class IPixelShader;
class IEffect;

//纹理层次，混合
struct SMaterialLayer
{
	E_TEXTURE_CLAMP TextureWrapU;
	E_TEXTURE_CLAMP TextureWrapV;
	bool	UseTextureMatrix : 1;
	matrix4* TextureMatrix;

	SMaterialLayer()
		: TextureWrapU(ETC_CLAMP),
		TextureWrapV(ETC_CLAMP),
		UseTextureMatrix(false),
		TextureMatrix(NULL)
	{
	}

	inline bool operator!=(const SMaterialLayer& b) const
	{
		if (!UseTextureMatrix)
		{
			return TextureWrapU != b.TextureWrapU ||
				TextureWrapV != b.TextureWrapV ||
				UseTextureMatrix != b.UseTextureMatrix;
		}
		else
		{
			if (!TextureMatrix)
			{
				return b.TextureMatrix ||
					TextureWrapU != b.TextureWrapU ||
					TextureWrapV != b.TextureWrapV ||
					UseTextureMatrix != b.UseTextureMatrix;
			}
			else
			{
				return !b.TextureMatrix ||
					TextureWrapU != b.TextureWrapU ||
					TextureWrapV != b.TextureWrapV ||
					UseTextureMatrix != b.UseTextureMatrix ||
					memcmp(TextureMatrix, b.TextureMatrix, sizeof(matrix4)) != 0;
			}
		}
	}

	inline bool operator==(const SMaterialLayer& b) const
	{ return !(b!=*this); }

};

//texture, render state
struct SMaterial
{
	//basic
	SColor	AmbientColor;
	SColor	DiffuseColor;
	SColor	EmissiveColor;
	SColor	SpecularColor;
	f32		Shininess;

	//vs
	bool		GouraudShading : 1;
	bool		Lighting : 1;
	bool		FogEnable : 1;
	IVertexShader*		VertexShader;
	IEffect*		Effect;

	//rasterize
	bool		Wireframe : 1;
	E_CULL_MODE		Cull;
	E_ANTI_ALIASING_MODE		AntiAliasing;

	//sampler state
	SMaterialLayer		TextureLayer[MATERIAL_MAX_TEXTURES];

	//depth stencil
	u8		ZBuffer;
	bool		ZWriteEnable : 1;
	bool		StencilEnable : 1;

	//ps, blend
	E_MATERIAL_TYPE	MaterialType;

	SMaterial()
		: MaterialType((E_MATERIAL_TYPE)-1),
		AmbientColor(255,255,255,255),
		DiffuseColor(255,255,255,255),
		EmissiveColor(0,0,0,0),
		SpecularColor(0,0,0,0),
		Shininess(0.0f),
		ZBuffer(ECFN_LESSEQUAL),
		Wireframe(false),
		GouraudShading(true), Lighting(true),
		ZWriteEnable(true),
		StencilEnable(false),
		Cull(ECM_NONE),
		FogEnable(false),
		AntiAliasing(EAAM_SIMPLE),
		VertexShader(NULL),
		Effect(NULL)
	{

	}

	float getMaterialAlpha() const
	{
		if (Lighting)
			return AmbientColor.getAlpha() / 255.0f;
		else
			return EmissiveColor.getAlpha() / 255.0f;
	}

	SColor getMaterialColor() const
	{
		if (Lighting)
			return AmbientColor;
		else
			return EmissiveColor;
	}

	void setMaterialAlpha(float alpha)
	{
		if (Lighting)
			AmbientColor.setAlpha((u32)(AmbientColor.getAlpha() * alpha));
		else
			EmissiveColor.setAlpha((u32)(EmissiveColor.getAlpha() * alpha));

		//DiffuseColor.setAlpha((u32)(DiffuseColor.getAlpha() * alpha));
	}

	void setMaterialColor(SColor color)
	{
		//不改变alpha
		if (Lighting)
		{
			AmbientColor.setRed(color.getRed());
			AmbientColor.setGreen(color.getGreen());
			AmbientColor.setBlue(color.getBlue());
		}
		else
		{
			EmissiveColor.setRed(color.getRed());
			EmissiveColor.setGreen(color.getGreen());
			EmissiveColor.setBlue(color.getBlue());
		}
	}

	inline bool operator!=(const SMaterial& b) const
	{
		return memcmp(this, &b, sizeof(SMaterial)) != 0;
	}

	inline bool operator==(const SMaterial& b) const
	{ return !(b!=*this); }

	inline bool isTransparent() const 
	{
		return MaterialType > EMT_ALPHA_TEST;
	}

};

//blend state
struct  SRenderStateBlock
{
	struct STextureStageBlock
	{
		STextureStageBlock() :
		colorOp(ETO_DISABLE), colorArg1(ETA_TEXTURE), colorArg2(ETA_DIFFUSE),
		alphaOp(ETO_DISABLE), alphaArg1(ETA_TEXTURE), alphaArg2(ETA_DIFFUSE) { }

		E_TEXTURE_OP		colorOp;
		E_TEXTURE_ARG		colorArg1;
		E_TEXTURE_ARG		colorArg2;
		E_TEXTURE_OP		alphaOp;
		E_TEXTURE_ARG		alphaArg1;
		E_TEXTURE_ARG		alphaArg2;
		u8	texcoordIndex;
	};

	SRenderStateBlock()
		: alphaBlendEnabled(false),
		srcBlend(EBF_ONE), destBlend(EBF_ZERO),
		alphaTestEnabled(false),
		alphaTestFunc(ECFN_GREATEREQUAL),
		alphaTestRef(0),
		alphaToCoverage(false),
		pixelShader(NULL)
	{
		for (u8 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
		{
			textureUnits[i].texcoordIndex = i;
		}
	}

	STextureStageBlock		textureUnits[MATERIAL_MAX_TEXTURES];
	bool alphaBlendEnabled;
	E_BLEND_FACTOR	srcBlend;
	E_BLEND_FACTOR	destBlend;
	bool  alphaTestEnabled;
	E_COMPARISON_FUNC	alphaTestFunc;
	u8	alphaTestRef;
	bool		alphaToCoverage;

	IPixelShader*		pixelShader;

	inline bool operator!=(const SRenderStateBlock& b) const
	{
		return memcmp(this, &b, sizeof(SRenderStateBlock)) != 0;
	}

	inline bool operator==(const SRenderStateBlock& b) const
	{ return !(b!=*this); }
};

//global 
struct SOverrideMaterial
{
	SOverrideMaterial() 
	{
		for(u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
		{	
			TextureFilters[i] = ETF_BILINEAR;
			MipMapLodBias[i] = 0;
		}
	}

	E_TEXTURE_FILTER	TextureFilters[MATERIAL_MAX_TEXTURES];
	s32		MipMapLodBias[MATERIAL_MAX_TEXTURES];

	inline bool operator!=(const SOverrideMaterial& b) const
	{
		return memcmp(this, &b, sizeof(SOverrideMaterial)) != 0;
	}

	inline bool operator==(const SOverrideMaterial& b) const
	{ return !(b!=*this); }
};

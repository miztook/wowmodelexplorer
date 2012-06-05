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
	E_TEXTURE_FILTER	TextureFilter;
	bool	UseTextureMatrix : 1;
	matrix4* TextureMatrix;

	SMaterialLayer()
		: TextureWrapU(ETC_CLAMP),
		TextureWrapV(ETC_CLAMP),
		TextureFilter(ETF_BILINEAR),
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
					TextureFilter != b.TextureFilter ||
					UseTextureMatrix != b.UseTextureMatrix;
			}
			else
			{
				return !b.TextureMatrix ||
					TextureWrapU != b.TextureWrapU ||
					TextureWrapV != b.TextureWrapV ||
					TextureFilter != b.TextureFilter ||
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
	SMaterialLayer		TextureLayer[MATERIAL_MAX_TEXTURES];
	E_MATERIAL_TYPE	MaterialType;
	SColor	AmbientColor;
	SColor	DiffuseColor;
	SColor	EmissiveColor;
	SColor	SpecularColor;
	f32			Shininess;
	u8			ZBuffer;
	bool		Wireframe : 1;
	bool		GouraudShading : 1;
	bool		Lighting : 1;
	bool		ZWriteEnable : 1;
	bool		StencilEnable : 1;
	bool		BackfaceCulling : 1;
	bool		FrontfaceCulling : 1;
	bool		FogEnable : 1;
	bool		AntiAliasing : 1;
	IVertexShader*		VertexShader;
	IEffect*		Effect;

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
		BackfaceCulling(false),FrontfaceCulling(false),
		FogEnable(false),
		AntiAliasing(true),
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

struct  SRenderStateBlock
{
	struct STextureStageBlock
	{
		STextureStageBlock() :
		colorOp(ETO_MODULATE), colorArg1(ETA_TEXTURE), colorArg2(ETA_DIFFUSE),
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

	IPixelShader*		pixelShader;

	inline bool operator!=(const SRenderStateBlock& b) const
	{
		return memcmp(this, &b, sizeof(SRenderStateBlock)) != 0;
	}

	inline bool operator==(const SRenderStateBlock& b) const
	{ return !(b!=*this); }
};

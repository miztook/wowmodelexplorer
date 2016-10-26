#pragma once

#include "base.h"
#include "SColor.h"

class ITexture;
class IVertexShader;
class IPixelShader;

//sampler
struct SMaterialLayer
{	
	const matrix4* TextureMatrix;
	E_TEXTURE_CLAMP TextureWrapU;
	E_TEXTURE_CLAMP TextureWrapV;
	E_TEXTURE_CLAMP TextureWrapW;
	bool	UseTextureMatrix;

	SMaterialLayer()
		: TextureMatrix(NULL_PTR),
		TextureWrapU(ETC_CLAMP),
		TextureWrapV(ETC_CLAMP),
		TextureWrapW(ETC_CLAMP),
		UseTextureMatrix(false)
	{
	}

	bool operator!=(const SMaterialLayer& b) const
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
					TextureMatrix != b.TextureMatrix;
			}
		}
	}

	bool operator==(const SMaterialLayer& b) const
	{ return !(b!=*this); }

};

//global sampler
struct SOverrideMaterial
{
	SOverrideMaterial() 
	{
		for(u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
		{	
			TextureFilters[i] = ETF_TRILINEAR;
			MipMapLodBias[i] = 0;
		}
	}

	E_TEXTURE_FILTER	TextureFilters[MATERIAL_MAX_TEXTURES];
	s32		MipMapLodBias[MATERIAL_MAX_TEXTURES];

	bool operator!=(const SOverrideMaterial& b) const
	{
		for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
		{
			if (TextureFilters[i] != b.TextureFilters[i] ||
				MipMapLodBias[i] != b.MipMapLodBias[i])
			{
				return true;
			}
		}
		return false;
	}

	bool operator==(const SOverrideMaterial& b) const
	{
		for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
		{
			if (TextureFilters[i] != b.TextureFilters[i] ||
				MipMapLodBias[i] != b.MipMapLodBias[i])
			{
				return false;
			}
		}
		return true;
	}
};

/*
//basic
SColorf		AmbientColor;
SColorf		DiffuseColor;
SColorf		EmissiveColor;
SColorf		SpecularColor;
f32		Shininess;

//vs
bool		GouraudShading : 1;
bool		Lighting : 1;
bool		FogEnable : 1;
IVertexShader*		VertexShader;

//rasterize
bool		Wireframe : 1;
E_CULL_MODE		Cull;
E_ANTI_ALIASING_MODE		AntiAliasing;

//sampler state
SMaterialLayer		TextureLayer[MATERIAL_MAX_TEXTURES];

//depth stencil
u8			ZBuffer;
bool		ZWriteEnable : 1;
bool		StencilEnable : 1;

//ps(texture stage), blend
E_MATERIAL_TYPE	MaterialType;	//blend
E_PS_TYPE	PsType;			//ps
*/

//render state
struct SMaterial
{
	//basic
	SColorf		AmbientColor;
	SColorf		DiffuseColor;
	SColorf		EmissiveColor;
	SColorf		SpecularColor;
	f32		Shininess;

	IVertexShader*		VertexShader;
	SMaterialLayer		TextureLayer[MATERIAL_MAX_TEXTURES];

	E_COMPARISON_FUNC		ZBuffer;
	E_CULL_MODE		Cull;
	E_ANTI_ALIASING_MODE		AntiAliasing;
	E_MATERIAL_TYPE	MaterialType;	//blend
	E_PS_TYPE	PsType;			//ps

	bool		GouraudShading;
	bool		Lighting;
	bool		FogEnable;
	bool		Wireframe;
	bool		ZWriteEnable;
	bool		StencilEnable;
	bool		ScissorEnable;

	SMaterial()
		: AmbientColor(1.0f,1.0f,1.0f,1.0f),
		DiffuseColor(1.0f,1.0f,1.0f,1.0f),
		EmissiveColor(0,0,0,0),
		SpecularColor(0,0,0,0),
		Shininess(0.0f),
		VertexShader(NULL_PTR),
		ZBuffer(ECFN_LESSEQUAL),
		Cull(ECM_NONE),
		AntiAliasing(EAAM_SIMPLE),
		MaterialType((E_MATERIAL_TYPE)-1),
		PsType((E_PS_TYPE)-1),
		GouraudShading(true), 
		Lighting(true), 
		FogEnable(false),
		Wireframe(false),
		ZWriteEnable(true),
		StencilEnable(false),
		ScissorEnable(false)
	{
	}

	float getMaterialAlpha() const
	{
		if (Lighting)
			return AmbientColor.getAlpha();
		else
			return EmissiveColor.getAlpha();
	}

	SColor getMaterialColor() const
	{
		if (Lighting)
			return AmbientColor.toSColor();
		else
			return EmissiveColor.toSColor();
	}

	void setMaterialAlpha(float alpha)
	{
		if (Lighting)
			AmbientColor.setAlpha(AmbientColor.getAlpha() * alpha);
		else
			EmissiveColor.setAlpha(EmissiveColor.getAlpha() * alpha);
	}

	void setMaterialColor(SColor color)
	{
		//不改变alpha
		if (Lighting)
		{
			AmbientColor.setRed(color.getRed() / 255.0f);
			AmbientColor.setGreen(color.getGreen() / 255.0f);
			AmbientColor.setBlue(color.getBlue() / 255.0f);
		}
		else
		{
			EmissiveColor.setRed(color.getRed() / 255.0f);
			EmissiveColor.setGreen(color.getGreen() / 255.0f);
			EmissiveColor.setBlue(color.getBlue() / 255.0f);
		}
	}

	bool operator!=(const SMaterial& b) const
	{
		bool equal = AmbientColor == b.AmbientColor &&
			DiffuseColor == b.DiffuseColor &&
			EmissiveColor == b.EmissiveColor &&
			SpecularColor == b.SpecularColor &&
			Shininess == b.Shininess &&
			VertexShader == b.VertexShader &&
			ZBuffer == b.ZBuffer &&
			Cull == b.Cull &&
			AntiAliasing == b.AntiAliasing &&
			MaterialType == b.MaterialType &&
			PsType == b.PsType &&
			GouraudShading == b.GouraudShading &&
			Lighting == b.Lighting &&
			FogEnable == b.FogEnable &&
			Wireframe == b.Wireframe &&
			ZWriteEnable == b.ZWriteEnable &&
			StencilEnable == b.StencilEnable &&
			ScissorEnable == b.ScissorEnable;

		if (!equal)
			return true;

		for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
		{
			if (TextureLayer[i] != b.TextureLayer[i])
				return true;
		}
		
		return false;
	}

	bool operator==(const SMaterial& b) const
	{ return !(b!=*this); }

	bool isTransparent() const 
	{
		return MaterialType > EMT_ALPHA_TEST &&
			MaterialType < EMT_COUNT;
	}

	bool isAlphaTest() const
	{
		return MaterialType == EMT_ALPHA_TEST;
	}
};

//blend state
struct  SRenderStateBlock
{
	struct STextureStageBlock
	{
		STextureStageBlock() :
		colorOp(ETO_DISABLE), colorArg1(ETA_TEXTURE), colorArg2(ETA_DIFFUSE),
		alphaOp(ETO_DISABLE), alphaArg1(ETA_TEXTURE), alphaArg2(ETA_DIFFUSE),
		texcoordIndex(0) { }

		E_TEXTURE_OP		colorOp;
		E_TEXTURE_ARG		colorArg1;
		E_TEXTURE_ARG		colorArg2;
		E_TEXTURE_OP		alphaOp;
		E_TEXTURE_ARG		alphaArg1;
		E_TEXTURE_ARG		alphaArg2;
		u8	texcoordIndex;

		bool operator!=(const STextureStageBlock& b) const
		{
			return colorOp != b.colorOp ||
				colorArg1 != b.colorArg1 ||
				colorArg2 != b.colorArg2 ||
				alphaOp != b.alphaOp ||
				alphaArg1 != b.alphaArg1 ||
				alphaArg2 != b.alphaArg2 ||
				texcoordIndex != b.texcoordIndex;
		}

		bool operator==(const STextureStageBlock& b) const
		{ return !(*this != b); }
	};

	SRenderStateBlock()
		: pixelShader(NULL_PTR),
		blendOp(EBO_ADD),
		srcBlend(EBF_ONE), destBlend(EBF_ZERO),
		alphaTestEnabled(false),
		alphaTestFunc(ECFN_GREATEREQUAL),
		alphaTestRef(0),
		alphaToCoverage(false),		
		alphaBlendEnabled(false)
	{
		for (u8 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
		{
			textureUnits[i].texcoordIndex = i;
		}
	}

	STextureStageBlock		textureUnits[MATERIAL_MAX_TEXTURES];
	IPixelShader*		pixelShader;		//ps通常取代STextureStageBlock

	E_BLEND_OP		blendOp;
	E_BLEND_FACTOR	srcBlend;
	E_BLEND_FACTOR	destBlend;
	bool  alphaTestEnabled;
	E_COMPARISON_FUNC	alphaTestFunc;
	u8	alphaTestRef;
	bool	alphaToCoverage;
	bool	alphaBlendEnabled;

	bool operator!=(const SRenderStateBlock& b) const
	{
		bool equal = pixelShader == b.pixelShader &&
			blendOp == b.blendOp &&
			srcBlend == b.srcBlend &&
			destBlend == b.destBlend &&
			alphaTestEnabled == b.alphaTestEnabled &&
			alphaTestFunc == b.alphaTestFunc &&
			alphaTestRef == b.alphaTestRef &&
			alphaToCoverage == b.alphaToCoverage &&
			alphaBlendEnabled == b.alphaBlendEnabled;
		if (!equal)
			return true;

		for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
		{
			if (textureUnits[i] != b.textureUnits[i])
				return true;
		}
		return false;
	}

	bool operator==(const SRenderStateBlock& b) const
	{ return !(b!=*this); }
};



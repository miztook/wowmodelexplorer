#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_OPENGL

#include "base.h"
#include "IMaterialRenderServices.h"
#include "IShader.h"

class ITexture;
class COpenGLDriver;
class COpenGLExtension;
class COpenGLShaderServices;

class COpenGLMaterialRenderServices : public IMaterialRenderServices
{
private:
	DISALLOW_COPY_AND_ASSIGN(COpenGLMaterialRenderServices);

public:
	COpenGLMaterialRenderServices();
	~COpenGLMaterialRenderServices();

public:
	//
	void setBasicRenderStates( const SMaterial& material,
		const SMaterial& lastMaterial,
		bool resetAllRenderstates );

	void setOverrideRenderStates( const SOverrideMaterial& overrideMaterial,
		bool resetAllRenderStates );

	IMaterialRenderer* getMaterialRenderer( E_MATERIAL_TYPE type ) { return MaterialRenderers[type]; }

	void set2DRenderStates( const S2DBlendParam& blendParam, bool resetAllRenderStates);

	const S2DBlendParam& getCurrent2DBlendParam() const { return Last2DBlendParam; }
	IPixelShader* getCurrentPixelShader() { return LastMaterialBlock.pixelShader; }

	void applyMaterialBlock( const SRenderStateBlock& block, bool resetAllRenderStates );

	void setPixelShaderMaterialBlock(SRenderStateBlock& block, E_PS_TYPE psType, E_VERTEX_TYPE vType);

	void applyMaterialChanges();

	const SRenderStateBlock& getRenderStateBlock() const { return LastMaterialBlock; }

	ITexture* getSampler_Texture(u32 st) const { return CurrentRenderState.TextureUnits[st].texture; }
	void setSampler_Texture(u32 st, ITexture* tex);
	void setTextureWrap(u32 st, E_TEXTURE_ADDRESS address, E_TEXTURE_CLAMP wrap);
	E_TEXTURE_CLAMP getTextureWrap(u32 st, E_TEXTURE_ADDRESS address) const;
	
	//
	void setZWriteEnable(bool enable);
	bool getZWriteEnable() const;
	void setActiveTexture(u32 st);
	void setTextureEnable(u32 st, bool enable);

	void applyTextureFilter(u32 st, E_TEXTURE_FILTER filter);
	void applyTextureMipMap(u32 st, bool mipmap);
	void applySamplerTexture(u32 st, ITexture* tex);
	void applySamplerTextureMultiSample(u32 st, ITexture* tex);
	void applyTextureWrap(u32 st, E_TEXTURE_ADDRESS address, E_TEXTURE_CLAMP wrap);

private:
	void resetRSCache();

	E_PS_MACRO getPSMacro(const SRenderStateBlock& block);

	void set2DPixelShaderMaterialBlock(SRenderStateBlock& block, bool alpha, bool alphaChannel);

private:
	struct SRenderStateCache 
	{
		GLint		ActiveTextureIndex;

		struct STextureUnit
		{
			ITexture*	texture;
			GLint		texEnable;

#ifdef FIXPIPELINE
			GLint			envMode;
			GLfloat		colorOp;
			GLfloat		colorArg1;
			GLfloat		colorArg2;
			GLfloat		alphaOp;
			GLfloat		alphaArg1;
			GLfloat		alphaArg2;
			GLfloat		colorScale;
			GLfloat		alphaScale;
#endif

			GLint	addressU;
			GLint	addressV;
			GLint	addressW;

			GLint	baseLevel;
			GLint	maxLevel;
			GLint	magFilter;
			GLint	minFilter;
			GLint	mipFilter;
			GLint	maxAniso;
		};

#ifdef FIXPIPELINE
		GLint	 ShadeMode;
		GLint	 Lighting;
		GLint	 ModelColorControl;	
		GLint	 FogEnable;
#endif

		GLint	 ZEnable;	
		GLint	 ZFunc;	
		GLint	 FillMode;
		GLint	 ZWriteEnable;
		GLint	 StencilEnable;
		GLint	 CullEnable;	
		GLint	 CullMode;
		GLint	 FrontFace;
		GLint	 MultiSampleAntiAlias;
		GLint	 AntiAliasedLineEnable;

		GLint	 AlphaBlendEnable;
		GLint	AlphaTestEnable;
		GLint	 SrcBlend;
		GLint	 DestBlend;
		GLint	 AlphaFunc;
		GLfloat	 AlphaRef;

		GLint	 AlphaToCoverage;

		STextureUnit		TextureUnits[MATERIAL_MAX_TEXTURES];
	};

private:
	IMaterialRenderer*		MaterialRenderers[EMT_COUNT];

	COpenGLDriver*	Driver;
	COpenGLExtension*		Extension;
	COpenGLShaderServices*		ShaderServices;

	SRenderStateCache	CurrentRenderState;

	SRenderStateCache	RsCache;

	SRenderStateBlock		LastMaterialBlock;					//»º´æmaterial block

	SOverrideMaterial		LastOverrideMaterial;

	S2DBlendParam	Last2DBlendParam;

};

inline void COpenGLMaterialRenderServices::setSampler_Texture( u32 st, ITexture* tex )
{
	if (st < MATERIAL_MAX_TEXTURES)
		CurrentRenderState.TextureUnits[st].texture = tex;
}

inline E_PS_MACRO COpenGLMaterialRenderServices::getPSMacro( const SRenderStateBlock& block )
{
	if(block.alphaTestEnabled)
		return PS_Macro_AlphaTest;

	return PS_Macro_None;
}


#endif
#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_GLES2

#include "base.h"
#include "IMaterialRenderServices.h"
#include "IShader.h"

class ITexture;
class CGLES2Driver;
class CGLES2Extension;
class CGLES2ShaderServices;

class CGLES2MaterialRenderServices : public IMaterialRenderServices
{
private:
	DISALLOW_COPY_AND_ASSIGN(CGLES2MaterialRenderServices);

public:
	CGLES2MaterialRenderServices();
	~CGLES2MaterialRenderServices();

public:
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

			GLint	addressU;
			GLint	addressV;

			GLint	magFilter;
			GLint	minFilter;
			GLint	mipFilter;
		};

		GLint	 ZEnable;	
		GLint	 ZFunc;	
		GLint	 ZWriteEnable;
		GLint	 StencilEnable;
		GLint	 CullEnable;	
		GLint	 CullMode;
		GLint	 FrontFace;

		GLint	 AlphaBlendEnable;
		GLint	AlphaTestEnable;
		GLint	 SrcBlend;
		GLint	 DestBlend;

		GLint	 AlphaToCoverage;

		STextureUnit		TextureUnits[MATERIAL_MAX_TEXTURES];
	};

private:
	IMaterialRenderer*		MaterialRenderers[EMT_COUNT];

	CGLES2Driver*	Driver;
	CGLES2Extension*		Extension;
	CGLES2ShaderServices*		ShaderServices;

	SRenderStateCache	CurrentRenderState;

	SRenderStateCache	RsCache;

	SRenderStateBlock		LastMaterialBlock;					//»º´æmaterial block

	SOverrideMaterial		LastOverrideMaterial;

	S2DBlendParam	Last2DBlendParam;

};

inline void CGLES2MaterialRenderServices::setSampler_Texture( u32 st, ITexture* tex )
{
	if (st < MATERIAL_MAX_TEXTURES)
		CurrentRenderState.TextureUnits[st].texture = tex;
}

inline E_PS_MACRO CGLES2MaterialRenderServices::getPSMacro( const SRenderStateBlock& block )
{
	if(block.alphaTestEnabled)
		return PS_Macro_AlphaTest;

	return PS_Macro_None;
}

#endif
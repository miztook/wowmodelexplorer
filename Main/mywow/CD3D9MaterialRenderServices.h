#pragma  once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D9

#include "base.h"
#include "IMaterialRenderServices.h"
#include "IShader.h"

class ITexture;
class CD3D9Driver;
class CD3D9ShaderServices;

class CD3D9MaterialRenderServices : public IMaterialRenderServices
{
private:
	DISALLOW_COPY_AND_ASSIGN(CD3D9MaterialRenderServices);

public:
	CD3D9MaterialRenderServices();

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

	//render states
	void setDepthBias(f32 depthbias);
	f32 getDepthBias() const;

private:
	void resetRSCache();

	void set2DPixelShaderMaterialBlock(SRenderStateBlock& block, bool alpha, bool alphaChannel);

	E_PS_MACRO getPSMacro(const SRenderStateBlock& block);

private:
	struct SRenderStateCache		
	{
		struct STextureUnit
		{
			ITexture*	texture;

#ifdef FIXPIPELINE
			DWORD		textureTransformFlags;
			DWORD		colorOp;
			DWORD		colorArg1;
			DWORD		colorArg2;
			DWORD		alphaOp;
			DWORD		alphaArg1;
			DWORD		alphaArg2;
#endif
			DWORD		texcoordIndex;

			DWORD		addressU;
			DWORD		addressV;
			DWORD		addressW;
			DWORD		maxAniso;
			DWORD		magFilter;
			DWORD		minFilter;
			DWORD		mipFilter;
			DWORD		mipLodBias;
		};

#ifdef FIXPIPELINE
		DWORD	 SpecularEnable;
		DWORD	 ShadeMode;
		DWORD	 Lighting;
		DWORD	 FogEnable;
#endif

		DWORD	 ZEnable;
		DWORD	 ZFunc;
		DWORD	 FillMode;	
		DWORD	 ZWriteEnable;
		DWORD	 StencilEnable;
		DWORD	 CullMode;
		DWORD	 MultiSampleAntiAlias;
		DWORD	 AntiAliasedLineEnable;
		DWORD	 SlopeScaleDepthBias;
		DWORD	 DepthBias;
		DWORD	 AlphaBlendEnable;
		DWORD	 BlendOp;
		DWORD	 SrcBlend;
		DWORD	 DestBlend;
		DWORD	 AlphaTestEnable;
		DWORD	 AlphaFunc;
		DWORD	 AlphaRef;
		DWORD	 AdaptivetessY;
		DWORD	 PointSize;

		STextureUnit	TextureUnits[MATERIAL_MAX_TEXTURES];
	};

private:
	IMaterialRenderer*		MaterialRenderers[EMT_COUNT];

	CD3D9Driver*	Driver;
	IDirect3DDevice9*	pID3DDevice;
	CD3D9ShaderServices*	ShaderServices;

	SRenderStateCache	CurrentRenderState;

	SRenderStateCache	RsCache;

	SRenderStateBlock		LastMaterialBlock;					//»º´æmaterial block

	SOverrideMaterial		LastOverrideMaterial;

	S2DBlendParam	Last2DBlendParam;
};

inline void CD3D9MaterialRenderServices::setSampler_Texture( u32 st, ITexture* tex )
{
	if (st < MATERIAL_MAX_TEXTURES)
		CurrentRenderState.TextureUnits[st].texture = tex;
}

inline E_PS_MACRO CD3D9MaterialRenderServices::getPSMacro( const SRenderStateBlock& block )
{
	if(block.alphaTestEnabled)
		return PS_Macro_AlphaTest;

	return PS_Macro_None;
}

#endif
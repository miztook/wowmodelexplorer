#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D11

#include "IMaterialRenderServices.h"
#include "CD3D11Structs.h"
#include "IShader.h"
#include <unordered_map>

class CD3D11Driver;
class CD3D11ShaderServices;

class CD3D11MaterialRenderServices : public IMaterialRenderServices
{
private:
	DISALLOW_COPY_AND_ASSIGN(CD3D11MaterialRenderServices);

public:
	CD3D11MaterialRenderServices();
	~CD3D11MaterialRenderServices();

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

public:
	//bind states to pipeline
	ID3D11BlendState*  getBlendState();
	ID3D11RasterizerState* getRasterizerState();
	ID3D11DepthStencilState* getDepthStencilState();
	ID3D11SamplerState* getSamplerState(u32 index);

	//state
private:
	typedef std::unordered_map<SD3D11_BLEND_DESC, ID3D11BlendState*, SD3D11_BLEND_DESC_Hash> T_BlendStateMap;
	typedef std::unordered_map<SD3D11_RASTERIZER_DESC, ID3D11RasterizerState*, SD3D11_RASTERIZER_DESC_Hash>	T_RasterizerStateMap;
	typedef std::unordered_map<SD3D11_DEPTH_STENCIL_DESC, ID3D11DepthStencilState*, SD3D11_DEPTH_STENCIL_DESC_Hash> T_DepthStencilStateMap;
	typedef std::unordered_map<SD3D11_SAMPLER_DESC, ID3D11SamplerState*, SD3D11_SAMPLER_DESC_Hash> T_SamplerStateMap;

	T_BlendStateMap	BlendStateMap;
	T_RasterizerStateMap	RasterizerStateMap;
	T_DepthStencilStateMap  DepthStencilStateMap;
	T_SamplerStateMap		SamplerStateMap;

private:
	void resetRSCache();
	void clearStateMap();

	void set2DPixelShaderMaterialBlock(SRenderStateBlock& block, bool alpha, bool alphaChannel);

	E_PS_MACRO getPSMacro(const SRenderStateBlock& block);

private:
	struct SRenderState			//current render state
	{
		SRenderState()
		{
			for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
			{
				TextureUnits[i].texture = NULL_PTR;
				TextureUnits[i].SamplerDesc.reset();
			}
			BlendDesc.reset();
			RasterizerDesc.reset();
			DepthStencilDesc.reset();
		}

		struct STextureUnit
		{
			ITexture*	texture;

			SD3D11_SAMPLER_DESC		SamplerDesc;
		};

		SD3D11_BLEND_DESC	BlendDesc;
		SD3D11_RASTERIZER_DESC	RasterizerDesc;
		SD3D11_DEPTH_STENCIL_DESC DepthStencilDesc;

		STextureUnit	TextureUnits[MATERIAL_MAX_TEXTURES];
	};

	struct SRenderStateCache			//render stateµÄ»º´æ
	{
		//
		ID3D11BlendState*		BlendState;
		ID3D11RasterizerState*	RasterizerState;
		ID3D11DepthStencilState*	DepthStencilState;
	};

private:
	IMaterialRenderer*		MaterialRenderers[EMT_COUNT];
	CD3D11Driver*	Driver;
	CD3D11ShaderServices*	ShaderServices;

	SRenderState		CurrentRenderState;

	SRenderStateCache	RsCache;

	SRenderStateBlock		LastMaterialBlock;					//»º´æmaterial block

	SOverrideMaterial		LastOverrideMaterial;

	S2DBlendParam	Last2DBlendParam;
};

inline void CD3D11MaterialRenderServices::setSampler_Texture( u32 st, ITexture* tex )
{
	if (st < MATERIAL_MAX_TEXTURES)
		CurrentRenderState.TextureUnits[st].texture = tex;
}

inline E_PS_MACRO CD3D11MaterialRenderServices::getPSMacro( const SRenderStateBlock& block )
{
	if(block.alphaTestEnabled)
		return PS_Macro_AlphaTest;

	return PS_Macro_None;
}

#endif
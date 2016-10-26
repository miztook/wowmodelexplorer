#pragma once

#include "base.h"
#include "SMaterial.h"

class IMaterialRenderer;

/*
SMaterial			//vs, per unit
SOverrideMaterial		//vs, global
SRenderStageBlock		//ps, for IMaterialRenderer

then applyMaterialChanges, apply to render pipeline
*/

class IMaterialRenderServices
{
public:
	virtual ~IMaterialRenderServices(){}

#ifdef FULL_INTERFACE

	virtual void setBasicRenderStates( const SMaterial& material,
		const SMaterial& lastMaterial,
		bool resetAllRenderstates ) = 0;

	virtual void setOverrideRenderStates( const SOverrideMaterial& overrideMaterial,
		bool resetAllRenderStates ) = 0;

	virtual IMaterialRenderer* getMaterialRenderer( E_MATERIAL_TYPE type )= 0;

	virtual void set2DRenderStates( const S2DBlendParam& blendParam, bool resetAllREnderStates ) = 0;

	virtual const S2DBlendParam& getCurrent2DBlendParam() const = 0;
	virtual IPixelShader* getCurrentPixelShader() = 0;

	virtual void applyMaterialBlock( const SRenderStateBlock& block, bool resetAllRenderStates ) = 0;

	virtual void setPixelShaderMaterialBlock(SRenderStateBlock& block, E_PS_TYPE psType, E_VERTEX_TYPE vType) = 0;

	virtual void applyMaterialChanges() = 0;

	virtual const SRenderStateBlock& getRenderStateBlock() const = 0;

	virtual ITexture* getSampler_Texture(u32 st) const = 0;
	virtual void setSampler_Texture(u32 st, ITexture* tex) = 0;
	virtual void setTextureWrap(u32 st, E_TEXTURE_ADDRESS address, E_TEXTURE_CLAMP wrap) = 0;
	virtual E_TEXTURE_CLAMP getTextureWrap(u32 st, E_TEXTURE_ADDRESS address) const = 0;

#endif 
};
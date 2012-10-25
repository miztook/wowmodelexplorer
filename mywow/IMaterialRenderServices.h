#pragma once

#include "base.h"
#include "SMaterial.h"

class IMaterialRenderer;

class IMaterialRenderServices
{
public:
	virtual ~IMaterialRenderServices(){}

	virtual void setBasicRenderStates( const SMaterial& material,
		const SMaterial& lastMaterial,
		bool resetAllRenderstates ) = 0;

	virtual void setOverrideRenderStates( const SOverrideMaterial& overrideMaterial,
		bool resetAllRenderStates ) = 0;

	virtual bool isFFPipeline() const  = 0;

	virtual IMaterialRenderer* getMaterialRenderer( E_MATERIAL_TYPE type ) = 0;

	virtual void set2DRenderStates( bool alpha, bool alphaChannel, E_BLEND_FACTOR srcBlend, E_BLEND_FACTOR destBlend, bool resetAllREnderStates ) = 0;

	virtual void applyMaterialBlock( const SRenderStateBlock& block, bool resetAllRenderStates ) = 0;

	//for FP and PP
	virtual ITexture* getSampler_Texture(u32 st) const = 0;
	virtual void setSampler_Texture(u32 st, ITexture* tex) = 0;
	virtual bool getZWriteEnable() const = 0;
	virtual void setZWriteEnable(bool enable) = 0;
	virtual void setTextureWrap(u32 st, E_TEXTURE_ADDRESS address, E_TEXTURE_CLAMP wrap) = 0;
	virtual E_TEXTURE_CLAMP getTextureWrap(u32 st, E_TEXTURE_ADDRESS address) const = 0;
	virtual void setDepthBias(f32 depthbias) = 0;
	virtual f32 getDepthBias() const = 0;
};
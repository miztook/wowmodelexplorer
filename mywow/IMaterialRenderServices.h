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

	virtual bool isFFPipeline() const  = 0;

	virtual IMaterialRenderer* getMaterialRenderer( E_MATERIAL_TYPE type ) = 0;

	virtual void set2DRenderStates( bool alpha, bool texture, bool alphaChannel, bool resetAllREnderStates ) = 0;

	virtual void applyMaterialBlock( const SRenderStateBlock& block, bool resetAllRenderStates ) = 0;

	//for FP and PP
	virtual ITexture* getSampler_Texture(u32 st) const = 0;
	virtual void setSampler_Texture(u32 st, ITexture* tex) = 0;
};
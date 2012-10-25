#pragma  once

#include "base.h"
#include "IMaterialRenderServices.h"
#include <map>

class ITexture;
class CD3D9Driver;

class CD3D9MaterialRenderServices : public IMaterialRenderServices
{
public:
	CD3D9MaterialRenderServices(bool ppipeline);

public:
	virtual void setBasicRenderStates( const SMaterial& material,
		const SMaterial& lastMaterial,
		bool resetAllRenderstates );

	virtual void setOverrideRenderStates( const SOverrideMaterial& overrideMaterial,
		bool resetAllRenderStates );

	virtual bool isFFPipeline() const { return !PPipeline; }

	virtual IMaterialRenderer* getMaterialRenderer( E_MATERIAL_TYPE type ) { return MaterialRenderersMap[type]; }

	virtual void set2DRenderStates( bool alpha, bool alphaChannel, E_BLEND_FACTOR srcBlend, E_BLEND_FACTOR destBlend, bool resetAllRenderStates);

	virtual void applyMaterialBlock( const SRenderStateBlock& block, bool resetAllRenderStates );

	virtual ITexture* getSampler_Texture(u32 st) const { return RsCache.TextureUnits[st].texture; }
	virtual void setSampler_Texture(u32 st, ITexture* tex);

	//render states
	virtual bool getZWriteEnable() const;
	virtual void setZWriteEnable(bool enable);
	virtual void setTextureWrap(u32 st, E_TEXTURE_ADDRESS address, E_TEXTURE_CLAMP wrap);
	virtual E_TEXTURE_CLAMP getTextureWrap(u32 st, E_TEXTURE_ADDRESS address) const;
	virtual void setDepthBias(f32 depthbias);
	virtual f32 getDepthBias() const;

private:
	void ResetRSCache();

private:
	typedef std::map<E_MATERIAL_TYPE, IMaterialRenderer*, std::less<E_MATERIAL_TYPE>, qzone_allocator<std::pair<E_MATERIAL_TYPE, IMaterialRenderer*>>> T_MaterialRenderersMap;
	T_MaterialRenderersMap		MaterialRenderersMap;

	CD3D9Driver*	Driver;
	IDirect3DDevice9*	pID3DDevice;
	bool	PPipeline;

	struct SRenderStateCache			//BasicRenderStateµÄ»º´æ
	{
		struct STextureUnit
		{
			ITexture*	texture;

			DWORD		textureTransformFlags;
			DWORD		colorOp;
			DWORD		colorArg1;
			DWORD		colorArg2;
			DWORD		alphaOp;
			DWORD		alphaArg1;
			DWORD		alphaArg2;
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

		DWORD	 Lighting;
		DWORD	 SpecularEnable;
		DWORD	 ZEnable;
		DWORD	 ZFunc;
		DWORD	 FillMode;
		DWORD	 ShadeMode;
		DWORD	 ZWriteEnable;
		DWORD	 StencilEnable;
		DWORD	 CullMode;
		DWORD	 FogEnable;
		DWORD	 MultiSampleAntiAlias;
		DWORD	 AntiAliasedLineEnable;
		DWORD	 SlopeScaleDepthBias;
		DWORD	 DepthBias;
		DWORD	 AlphaBlendEnable;
		DWORD	 SrcBlend;
		DWORD	 DestBlend;
		DWORD	 AlphaTestEnable;
		DWORD	 AlphaFunc;
		DWORD	 AlphaRef;
		DWORD	 AdaptivetessY;
		DWORD	 PointSize;

		STextureUnit	TextureUnits[MATERIAL_MAX_TEXTURES];
	};

	SRenderStateCache	RsCache;

	SRenderStateBlock		LastMaterialBlock;					//»º´æmaterial block

	SOverrideMaterial		LastOverrideMaterial;
};
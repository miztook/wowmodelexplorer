#pragma once

#include "IShaderServices.h"
#include <map>

class CD3D9VertexShader;
class CD3D9PixelShader;
class CD3D9Effect;
class CD3D9Driver;
class CD3D9Texture;

class CD3D9ShaderServices : public IShaderServices, public ILostResetCallback
{
public:
	CD3D9ShaderServices();
	~CD3D9ShaderServices();

private:
	virtual void loadAllVertexShaders(const c8* profile);
	virtual void loadAllPixelShaders(const c8* profile);
	virtual void loadAllEffects();

public:
	virtual void onLost();
	virtual void onReset();

public:
	virtual bool loadVShader( const c8* filename, E_VS_TYPE type, VSHADERCONSTCALLBACK callback );
	virtual bool loadPShader( const c8* filename, E_PS_TYPE type, PSHADERCONSTCALLBACK callback );
	virtual bool loadVShaderHLSL( const c8* filename, const c8* entry, const c8* profile, E_VS_TYPE type, VSHADERCONSTCALLBACK callback );
	virtual bool loadPShaderHLSL( const c8* filename, const c8* entry, const c8* profile, E_PS_TYPE type, PSHADERCONSTCALLBACK callback );
	virtual bool loadEffect( const c8* filename, const c8* technique, E_EFFECT_TYPE type, EFFECTCONSTCALLBACK callback );

	virtual void useVertexShader(IVertexShader* vshader);
	virtual void usePixelShader(IPixelShader* pshader);
	virtual void useEffect(IEffect* effect);
	virtual void setShaderConstants(IVertexShader* vs, const SMaterial& material);
	virtual void setShaderConstants(IPixelShader* ps, const SMaterial& material);
	virtual void setShaderConstants(IEffect* effect, const SMaterial& material);

	virtual void getWVPMatrix(matrix4& mat) const;

public:
	virtual IVertexShader* getVertexShader(E_VS_TYPE type);
	virtual IPixelShader* getPixelShader(E_PS_TYPE type);
	virtual IEffect* getEffect(E_EFFECT_TYPE type);

private:
	void loadAll();

	struct SShaderCache
	{
		void reset()
		{
			vshader = NULL;
			pshader = NULL;
			effect = NULL;
		}

		IDirect3DVertexShader9*		vshader;
		IDirect3DPixelShader9*	pshader;
		ID3DXEffect*	effect;
	};

	SShaderCache ShaderCache;

private:
	typedef std::map<E_VS_TYPE, CD3D9VertexShader*, std::less<E_VS_TYPE>, qzone_allocator<std::pair<E_VS_TYPE, CD3D9VertexShader*>>> T_VertexShaderMap;
	T_VertexShaderMap		VertexShaderMap;

	typedef std::map<E_PS_TYPE, CD3D9PixelShader*, std::less<E_PS_TYPE>, qzone_allocator<std::pair<E_PS_TYPE, CD3D9PixelShader*>>> T_PixelShaderMap;
	T_PixelShaderMap		PixelShaderMap;

	typedef std::map<E_EFFECT_TYPE, CD3D9Effect*, std::less<E_EFFECT_TYPE>, qzone_allocator<std::pair<E_EFFECT_TYPE, CD3D9Effect*>>> T_EffectMap;
	T_EffectMap		EffectMap;

	IDirect3DDevice9*		Device;
	CD3D9Driver*	Driver;
};
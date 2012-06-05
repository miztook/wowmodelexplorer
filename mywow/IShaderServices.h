#pragma once

#include "base.h"
#include "SMaterial.h"
#include "IShader.h"

class matrix4A16;

//提供所有shader相关的辅助函数，参数设置等
//管理所有加载的shader 
class IShaderServices
{
public:
	virtual ~IShaderServices() {}

public:
	virtual void loadAllVertexShaders(const c8* profile) = 0;
	virtual void loadAllPixelShaders(const c8* profile) = 0;
	virtual void loadAllEffects() = 0;

public:
	virtual IVertexShader* getVertexShader(E_VS_TYPE type) = 0;
	virtual IPixelShader* getPixelShader(E_PS_TYPE type) = 0;
	virtual IEffect* getEffect(E_EFFECT_TYPE type) = 0;

	virtual bool loadVShader( const c8* filename, E_VS_TYPE type, VSHADERCONSTCALLBACK callback ) = 0;
	virtual bool loadPShader( const c8* filename, E_PS_TYPE type, PSHADERCONSTCALLBACK callback ) = 0;
	virtual bool loadVShaderHLSL( const c8* filename, const c8* entry, const c8* profile, E_VS_TYPE type, VSHADERCONSTCALLBACK callback ) = 0;
	virtual bool loadPShaderHLSL( const c8* filename, const c8* entry, const c8* profile, E_PS_TYPE type, PSHADERCONSTCALLBACK callback ) = 0;
	virtual bool loadEffect( const c8* filename, const c8* technique, E_EFFECT_TYPE type, EFFECTCONSTCALLBACK callback ) = 0;

	virtual void useVertexShader(IVertexShader* vshader) = 0;
	virtual void usePixelShader(IPixelShader* pshader) = 0;
	virtual void useEffect(IEffect* effect) = 0;
	virtual void setShaderConstants(IVertexShader* vs, const SMaterial& material) = 0;
	virtual void setShaderConstants(IPixelShader* ps, const SMaterial& material) = 0;
	virtual void setShaderConstants(IEffect* effect, const SMaterial& material) = 0;

	virtual void getWVPMatrix(matrix4& mat) const = 0;
};
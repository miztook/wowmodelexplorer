#pragma once

#include "base.h"
#include "SMaterial.h"
#include "IShader.h"
#include "ILostResetCallback.h"

#define DIRECTX_USE_COMPILED_SHADER


//提供所有shader相关的辅助函数，参数设置等
//管理所有加载的shader 
class IShaderServices : public ILostResetCallback
{
public:
	IShaderServices()
	{
		memset(VertexShaders, 0, sizeof(VertexShaders));
		memset(PixelShaders, 0, sizeof(PixelShaders));
	}
	virtual ~IShaderServices() {}

public:
	virtual void loadAll() = 0;

	IVertexShader* getVertexShader(E_VS_TYPE type) const { return VertexShaders[type]; }
	virtual IPixelShader* getPixelShader(E_PS_TYPE type, E_PS_MACRO macro = PS_Macro_None) = 0;

#ifdef FULL_INTERFACE

	virtual IVertexShader* getDefaultVertexShader(E_VERTEX_TYPE vType) const= 0;
	virtual IPixelShader* getDefaultPixelShader(E_VERTEX_TYPE vType) const = 0;

	virtual bool loadVShader( const c8* filename, E_VS_TYPE type, VSHADERCONSTCALLBACK callback ) = 0;
	virtual bool loadPShader( const c8* filename, E_PS_TYPE type, E_PS_MACRO macro, PSHADERCONSTCALLBACK callback ) = 0;
	virtual bool loadVShaderHLSL( const c8* filename, const c8* entry, const c8* profile, E_VS_TYPE type, VSHADERCONSTCALLBACK callback ) = 0;
	virtual bool loadPShaderHLSL( const c8* filename, const c8* entry, const c8* profile, E_PS_TYPE type, E_PS_MACRO macro, PSHADERCONSTCALLBACK callback ) = 0;

	virtual void useVertexShader(IVertexShader* vshader) = 0;
	virtual void usePixelShader(IPixelShader* pshader) = 0;
	virtual void applyShaders() = 0;
	virtual void setShaderConstants(IVertexShader* vs, const SMaterial& material, u32 pass) = 0;
	virtual void setShaderConstants(IPixelShader* ps, const SMaterial& material, u32 pass) = 0;

#endif

protected:
	IVertexShader*		VertexShaders[EVST_COUNT];
	IPixelShader*		PixelShaders[EPST_COUNT][PS_Macro_Num];
};
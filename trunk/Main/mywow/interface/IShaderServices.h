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

protected:
	IVertexShader*		VertexShaders[EVST_COUNT];
	IPixelShader*		PixelShaders[EPST_COUNT][PS_Macro_Num];
};
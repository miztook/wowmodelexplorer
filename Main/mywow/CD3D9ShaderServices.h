#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D9

#include "IShaderServices.h"
#include "CD3D9_VS30.h"
#include "CD3D9_PS30.h"

class CD3D9VertexShader;
class CD3D9PixelShader;
class CD3D9Driver;

class CD3D9ShaderServices : public IShaderServices
{
private:
	DISALLOW_COPY_AND_ASSIGN(CD3D9ShaderServices);

public:
	CD3D9ShaderServices();
	~CD3D9ShaderServices();

public:
	virtual void loadAll();

	virtual IPixelShader* getPixelShader(E_PS_TYPE type, E_PS_MACRO macro = PS_Macro_None) { return PixelShaders[type][PS_Macro_None]; }

public:
	bool loadVShader( const char* filename, E_VS_TYPE type, VSHADERCONSTCALLBACK callback );
	bool loadPShader( const char* filename, E_PS_TYPE type, E_PS_MACRO macro, PSHADERCONSTCALLBACK callback );

	bool loadVShaderHLSL( const char* filename, const char* entry, const char* profile, E_VS_TYPE type, VSHADERCONSTCALLBACK callback );
	bool loadPShaderHLSL( const char* filename, const char* entry, const char* profile, E_PS_TYPE type, E_PS_MACRO macro, PSHADERCONSTCALLBACK callback );

	void useVertexShader(IVertexShader* vshader) { ShaderState.vshader = vshader; }
	void usePixelShader(IPixelShader* pshader) { ShaderState.pshader = pshader; }
	void applyShaders();
	void setShaderConstants(IVertexShader* vs, const SMaterial& material, uint32_t pass);
	void setShaderConstants(IPixelShader* ps, const SMaterial& material, uint32_t pass);

public:
	IVertexShader* getDefaultVertexShader(E_VERTEX_TYPE vType) const;
	IPixelShader* getDefaultPixelShader(E_VERTEX_TYPE vType, E_PS_MACRO macro = PS_Macro_None);

private:

	struct SShaderState
	{
		 void reset()
		{
			vshader = nullptr;
			pshader = nullptr;
		}

		IVertexShader*		vshader;
		IPixelShader*	pshader;
	};

private:
	SShaderState	ShaderState;
	SShaderState	LastShaderState;

	CD3D9_VS30 vs30Loader;
	CD3D9_PS30 ps30Loader;	
	
private:
	IDirect3DDevice9*		Device;
	CD3D9Driver*	Driver;
};

#endif
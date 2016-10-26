#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D9

#include "IShaderServices.h"
#include "CD3D9_VS20.h"
#include "CD3D9_VS30.h"
#include "CD3D9_PS20.h"
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
	virtual void onLost();
	virtual void onReset();

	virtual void loadAll();

	virtual IPixelShader* getPixelShader(E_PS_TYPE type, E_PS_MACRO macro = PS_Macro_None) { return PixelShaders[type][PS_Macro_None]; }

public:
	bool loadVShader( const c8* filename, E_VS_TYPE type, VSHADERCONSTCALLBACK callback );
	bool loadPShader( const c8* filename, E_PS_TYPE type, E_PS_MACRO macro, PSHADERCONSTCALLBACK callback );

	bool loadVShaderHLSL( const c8* filename, const c8* entry, const c8* profile, E_VS_TYPE type, VSHADERCONSTCALLBACK callback );
	bool loadPShaderHLSL( const c8* filename, const c8* entry, const c8* profile, E_PS_TYPE type, E_PS_MACRO macro, PSHADERCONSTCALLBACK callback );

	void useVertexShader(IVertexShader* vshader) { ShaderState.vshader = vshader; }
	void usePixelShader(IPixelShader* pshader) { ShaderState.pshader = pshader; }
	void applyShaders();
	void setShaderConstants(IVertexShader* vs, const SMaterial& material, u32 pass);
	void setShaderConstants(IPixelShader* ps, const SMaterial& material, u32 pass);

public:
	IVertexShader* getDefaultVertexShader(E_VERTEX_TYPE vType) const;
	IPixelShader* getDefaultPixelShader(E_VERTEX_TYPE vType, E_PS_MACRO macro = PS_Macro_None);

private:

	struct SShaderState
	{
		 void reset()
		{
			vshader = NULL_PTR;
			pshader = NULL_PTR;
		}

		IVertexShader*		vshader;
		IPixelShader*	pshader;
	};

private:
	SShaderState	ShaderState;
	SShaderState	LastShaderState;

	CD3D9_VS20 vs20Loader;
	CD3D9_VS30 vs30Loader;
	CD3D9_PS20 ps20Loader;
	CD3D9_PS30 ps30Loader;	
	
	bool		ResetShaders;

private:
	IDirect3DDevice9*		Device;
	CD3D9Driver*	Driver;
};

#endif
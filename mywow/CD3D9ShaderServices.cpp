#include "stdafx.h"
#include "CD3D9ShaderServices.h"
#include "mywow.h"
#include "CD3D9Helper.h"
#include "CD3D9Driver.h"
#include "CD3D9Shader.h"

CD3D9ShaderServices::CD3D9ShaderServices()
{
	Driver = (CD3D9Driver*)g_Engine->getDriver();
	Device = (IDirect3DDevice9*)Driver->getD3DDevice();
	Driver->registerLostReset(this);

	loadAll();

	ShaderCache.reset();
}

void CD3D9ShaderServices::loadAll()
{
	//vs
// 	if (Driver->queryFeature(EVDF_VERTEX_SHADER_3_0))
// 	{
// 		loadAllVertexShaders("vs_3_0");
// 	}
// 	else 
	if (Driver->queryFeature(EVDF_VERTEX_SHADER_2_0))
	{
		loadAllVertexShaders("vs_2_0");
	}
	else if (Driver->queryFeature(EVDF_VERTEX_SHADER_1_1))
	{
		loadAllVertexShaders("vs_1_1");
	}

	//ps
// 	if (Driver->queryFeature(EVDF_PIXEL_SHADER_3_0))
// 	{
// 		loadAllPixelShaders("ps_3_0");
// 	}
// 	else 
	if (Driver->queryFeature(EVDF_PIXEL_SHADER_2_0))
	{
		loadAllPixelShaders("ps_2_0");
	}
	else if (Driver->queryFeature(EVDF_PIXEL_SHADER_1_1))
	{
		loadAllPixelShaders("ps_1_1");
	}

	//effect
// 	if (Driver->queryFeature(EVDF_VERTEX_SHADER_2_0) && Driver->queryFeature(EVDF_PIXEL_SHADER_2_0))
// 	{
// 		loadAllEffects();
// 	}
}

CD3D9ShaderServices::~CD3D9ShaderServices()
{
	useEffect(NULL);

	for (T_EffectMap::iterator i = EffectMap.begin(); i != EffectMap.end(); ++i)
	{
		delete i->second;
	}

	for (T_VertexShaderMap::iterator i = VertexShaderMap.begin(); i != VertexShaderMap.end(); ++i)
	{
		delete i->second;
	}

	for (T_PixelShaderMap::iterator i = PixelShaderMap.begin(); i != PixelShaderMap.end(); ++i)
	{
		delete i->second;
	}
}


void CD3D9ShaderServices::onLost()
{
	for (T_EffectMap::iterator i=EffectMap.begin(); i!=EffectMap.end(); ++i)
	{
		CD3D9Effect* pEffect = i->second;
		pEffect->getEffect()->OnLostDevice();
	}
}

void CD3D9ShaderServices::onReset()
{
	for (T_EffectMap::iterator i=EffectMap.begin(); i!=EffectMap.end(); ++i)
	{
		CD3D9Effect* pEffect = i->second;
		pEffect->getEffect()->OnResetDevice();
	}

	ShaderCache.reset();
}

bool CD3D9ShaderServices::loadVShader( const c8* filename, E_VS_TYPE type, VSHADERCONSTCALLBACK callback )
{
	string256 absFileName = g_Engine->getFileSystem()->getShaderBaseDirectory();
	absFileName.append(filename);
	c16	absFileNameW[MAX_PATH];
	str8to16(absFileName.c_str(), absFileNameW, MAX_PATH);

	DWORD dwShaderFlags;
// #ifdef DEBUG
// 	dwShaderFlags = D3DXSHADER_DEBUG;
// #else
	dwShaderFlags = D3DXSHADER_OPTIMIZATION_LEVEL3;
//#endif

	LPD3DXBUFFER pCode;
	HRESULT hr = CD3D9Helper::d3dxAssembleShaderFromFileW( absFileNameW, NULL, NULL, dwShaderFlags, &pCode, NULL );
	if (FAILED(hr))
	{
		_ASSERT(false);
		SAFE_RELEASE(pCode);
		return false;
	}

	IDirect3DVertexShader9* vertexShader;
	hr = Device->CreateVertexShader((DWORD*)pCode->GetBufferPointer(), &vertexShader);
	if (FAILED(hr))
	{
		_ASSERT(false);
		SAFE_RELEASE(pCode);
		return false;
	}

	VertexShaderMap[type] = new CD3D9VertexShader(type, vertexShader, NULL, callback);

	return true;
}

bool CD3D9ShaderServices::loadPShader( const c8* filename, E_PS_TYPE type, PSHADERCONSTCALLBACK callback )
{
	string256 absFileName = g_Engine->getFileSystem()->getShaderBaseDirectory();
	absFileName.append(filename);
	c16	absFileNameW[MAX_PATH];
	str8to16(absFileName.c_str(), absFileNameW, MAX_PATH);

	DWORD dwShaderFlags;
// #ifdef DEBUG
// 	dwShaderFlags = D3DXSHADER_DEBUG;
// #else
	dwShaderFlags = D3DXSHADER_OPTIMIZATION_LEVEL3;
//#endif
	

	LPD3DXBUFFER pCode;
	HRESULT hr = CD3D9Helper::d3dxAssembleShaderFromFileW( absFileNameW, NULL, NULL, dwShaderFlags, &pCode, NULL );
	if (FAILED(hr))
	{
		_ASSERT(false);
		SAFE_RELEASE(pCode);
		return false;
	}

	IDirect3DPixelShader9* pixelShader;
	hr = Device->CreatePixelShader((DWORD*)pCode->GetBufferPointer(), &pixelShader);
	if (FAILED(hr))
	{
		_ASSERT(false);
		SAFE_RELEASE(pCode);
		return false;
	}

	PixelShaderMap[type] = new CD3D9PixelShader(type, pixelShader, NULL, callback);

	return true;
}

bool CD3D9ShaderServices::loadVShaderHLSL( const c8* filename, const c8* entry, const c8* profile, E_VS_TYPE type, VSHADERCONSTCALLBACK callback )
{
	string256 absFileName = g_Engine->getFileSystem()->getShaderBaseDirectory();
	absFileName.append(filename);
	c16	absFileNameW[MAX_PATH];
	str8to16(absFileName.c_str(), absFileNameW, MAX_PATH);

	DWORD dwShaderFlags;
#ifdef _DEBUG
	dwShaderFlags = D3DXSHADER_DEBUG;
#else
	dwShaderFlags = D3DXSHADER_OPTIMIZATION_LEVEL3;
#endif

	LPD3DXBUFFER pCode;
	LPD3DXCONSTANTTABLE constantsTable;
	HRESULT hr = CD3D9Helper::d3dxCompileShaderFromFileW( absFileNameW, NULL, NULL, entry, profile, dwShaderFlags, &pCode, NULL, &constantsTable );

	if (FAILED(hr))
	{
		_ASSERT(false);
		SAFE_RELEASE(pCode);
		SAFE_RELEASE(constantsTable);
		return false;
	}

	IDirect3DVertexShader9* vertexShader;
	hr = Device->CreateVertexShader((DWORD*)pCode->GetBufferPointer(), &vertexShader);
	if (FAILED(hr))
	{
		_ASSERT(false);
		SAFE_RELEASE(pCode);
		SAFE_RELEASE(constantsTable);
		return false;
	}

	VertexShaderMap[type] = new CD3D9VertexShader(type, vertexShader, constantsTable, callback);

	return true;
}

bool CD3D9ShaderServices::loadPShaderHLSL( const c8* filename, const c8* entry, const c8* profile, E_PS_TYPE type, PSHADERCONSTCALLBACK callback )
{
	string256 absFileName = g_Engine->getFileSystem()->getShaderBaseDirectory();
	absFileName.append(filename);
	c16	absFileNameW[MAX_PATH];
	str8to16(absFileName.c_str(), absFileNameW, MAX_PATH);

	DWORD dwShaderFlags;
#ifdef _DEBUG
	dwShaderFlags = D3DXSHADER_DEBUG;
#else
	dwShaderFlags = D3DXSHADER_OPTIMIZATION_LEVEL3;
#endif

	LPD3DXBUFFER pCode;
	LPD3DXCONSTANTTABLE constantsTable;
	HRESULT hr = CD3D9Helper::d3dxCompileShaderFromFileW( absFileNameW, NULL, NULL, entry, profile, dwShaderFlags, &pCode, NULL, &constantsTable );

	if (FAILED(hr))
	{
		_ASSERT(false);
		SAFE_RELEASE(pCode);
		SAFE_RELEASE(constantsTable);
		return false;
	}

	IDirect3DPixelShader9* pixelShader;
	hr = Device->CreatePixelShader((DWORD*)pCode->GetBufferPointer(), &pixelShader);
	if (FAILED(hr))
	{
		_ASSERT(false);
		SAFE_RELEASE(pCode);
		SAFE_RELEASE(constantsTable);
		return false;
	}

	PixelShaderMap[type] = new CD3D9PixelShader(type, pixelShader, constantsTable, callback);

	return true;
}

bool CD3D9ShaderServices::loadEffect( const c8* filename, const c8* technique, E_EFFECT_TYPE type, EFFECTCONSTCALLBACK callback )
{
	string256 absFileName = g_Engine->getFileSystem()->getShaderBaseDirectory();
	absFileName.append(filename);
	c16	absFileNameW[MAX_PATH];
	str8to16(absFileName.c_str(), absFileNameW, MAX_PATH);

	HRESULT hr;
	DWORD dwShaderFlags = D3DXFX_NOT_CLONEABLE;

	LPD3DXBUFFER pErrors = NULL;
	ID3DXEffect* pEffect;
	hr = CD3D9Helper::d3dxCreateEffectFromFileW( Device, absFileNameW, NULL, NULL, dwShaderFlags, NULL, &pEffect, &pErrors );

	if (FAILED(hr))
	{
		if ( pErrors )
			MessageBoxA( NULL, (char*)pErrors->GetBufferPointer(), "Compile Error", MB_OK | MB_ICONEXCLAMATION );

		_ASSERT(false);
		return false;
	}

	SAFE_RELEASE(pErrors);

	EffectMap[type] = new CD3D9Effect(type, pEffect, technique, callback);

	return true;
}

void CD3D9ShaderServices::useVertexShader(IVertexShader* vshader)
{
	if (!vshader)
	{
		if (ShaderCache.vshader != NULL)
		{
			Device->SetVertexShader(NULL);
			ShaderCache.vshader = NULL;
		}
	}
	else
	{
		IDirect3DVertexShader9* vs = ((CD3D9VertexShader*)vshader)->getDXVShader();
		if (ShaderCache.vshader != vs)
		{
			Device->SetVertexShader(vs);
			ShaderCache.vshader = vs;
		}
	}
}

void CD3D9ShaderServices::usePixelShader(IPixelShader* pshader)
{
	if (!pshader)
	{
		if (ShaderCache.pshader != NULL)
		{
			Device->SetPixelShader(NULL);
			ShaderCache.pshader = NULL;
		}
	}
	else
	{
		IDirect3DPixelShader9* ps =  ((CD3D9PixelShader*)pshader)->getDXPShader();
		if (ShaderCache.pshader != ps)
		{
			Device->SetPixelShader(ps);
			ShaderCache.pshader = ps;
		}
	}
}

void CD3D9ShaderServices::useEffect( IEffect* effect)
{
	useVertexShader(NULL);
	usePixelShader(NULL);

	if (!effect)
	{
		if (ShaderCache.effect != NULL)
		{
			ShaderCache.effect->End();
			ShaderCache.effect = NULL;
		}
	}
	else
	{
		CD3D9Effect* pEffect = (CD3D9Effect*)effect;
		ID3DXEffect* dxEffect = pEffect->getEffect();

		dxEffect->End();
		dxEffect->SetTechnique(pEffect->getActiveTechnique());
		dxEffect->Begin(&pEffect->NumPasses, 0);

		ShaderCache.effect = dxEffect;
	}
}

IVertexShader* CD3D9ShaderServices::getVertexShader( E_VS_TYPE type )
{
	T_VertexShaderMap::iterator i = VertexShaderMap.find(type);
	if (i == VertexShaderMap.end())
		return NULL;
	return i->second;
}

IPixelShader* CD3D9ShaderServices::getPixelShader( E_PS_TYPE type )
{
	T_PixelShaderMap::iterator i = PixelShaderMap.find(type);
	if (i == PixelShaderMap.end())
		return NULL;
	return i->second;
}

IEffect* CD3D9ShaderServices::getEffect( E_EFFECT_TYPE type )
{
	T_EffectMap::iterator i = EffectMap.find(type);
	if (i == EffectMap.end())
		return NULL;
	return i->second;
}

void CD3D9ShaderServices::getWVPMatrix( matrix4& mat ) const
{
	const matrix4& w = Driver->getTransform(ETS_WORLD);
	const matrix4& v = Driver->getTransform(ETS_VIEW);
	const matrix4& p = Driver->getTransform(ETS_PROJECTION);
	mat = w * v * p;
}

void CD3D9ShaderServices::getVPMatrix( matrix4& mat ) const
{
	const matrix4& v = Driver->getTransform(ETS_VIEW);
	const matrix4& p = Driver->getTransform(ETS_PROJECTION);
	mat = v * p;
}

void CD3D9ShaderServices::setShaderConstants( IVertexShader* vs, const SMaterial& material )
{
	if (vs && vs->ShaderConstCallback)
		vs->ShaderConstCallback(vs, material);
}

void CD3D9ShaderServices::setShaderConstants( IPixelShader* ps, const SMaterial& material )
{
	if (ps && ps->ShaderConstCallback)
		ps->ShaderConstCallback(ps, material);
}

void CD3D9ShaderServices::setShaderConstants( IEffect* effect, const SMaterial& material )
{
	if (effect && effect->EffectConstCallback)
		effect->EffectConstCallback(effect, material);
}


#include "stdafx.h"
#include "CD3D9Helper.h"
#include "CD3D9Driver.h"
#include "mywow.h"

D3DMATRIX	CD3D9Helper::UnitMatrixD3D9;
D3DMATRIX	CD3D9Helper::View2DMatrixD3D9;
D3DMATRIX	CD3D9Helper::SphereMapMatrixD3D9;


D3DXASSEMBLESHADERFROMFILEW	CD3D9Helper::d3dxAssembleShaderFromFileW;
D3DXCOMPILESHADERFROMFILEW CD3D9Helper::d3dxCompileShaderFromFileW;
D3DXCREATEEFFECTFROMFILEW		CD3D9Helper::d3dxCreateEffectFromFileW;
D3DXASSEMBLESHADERFROMFILEA	CD3D9Helper::d3dxAssembleShaderFromFileA;
D3DXCOMPILESHADERFROMFILEA CD3D9Helper::d3dxCompileShaderFromFileA;
D3DXCREATEEFFECTFROMFILEA		CD3D9Helper::d3dxCreateEffectFromFileA;
D3DXCREATEBUFFER			CD3D9Helper::d3dxCreateBuffer;
D3DXDISASSEMBLESHADER		CD3D9Helper::d3dxDisassembleShader;

CD3D9Helper g_d3dHelper;

CD3D9Helper::CD3D9Helper()
{
	memset(&UnitMatrixD3D9, 0, sizeof(D3DMATRIX));
	UnitMatrixD3D9._11 = UnitMatrixD3D9._22 = UnitMatrixD3D9._33 = UnitMatrixD3D9._44 = 1.0f;

	View2DMatrixD3D9 = UnitMatrixD3D9;
	View2DMatrixD3D9._41 = View2DMatrixD3D9._42 = 0.5f;

	SphereMapMatrixD3D9._11 = 0.5f; SphereMapMatrixD3D9._12 = 0.0f;
	SphereMapMatrixD3D9._13 = 0.0f; SphereMapMatrixD3D9._14 = 0.0f;
	SphereMapMatrixD3D9._21 = 0.0f; SphereMapMatrixD3D9._22 =-0.5f;
	SphereMapMatrixD3D9._23 = 0.0f; SphereMapMatrixD3D9._24 = 0.0f;
	SphereMapMatrixD3D9._31 = 0.0f; SphereMapMatrixD3D9._32 = 0.0f;
	SphereMapMatrixD3D9._33 = 1.0f; SphereMapMatrixD3D9._34 = 0.0f;
	SphereMapMatrixD3D9._41 = 0.5f; SphereMapMatrixD3D9._42 = 0.5f;
	SphereMapMatrixD3D9._43 = 0.0f; SphereMapMatrixD3D9._44 = 1.0f;

	//imports
	c8 libName[DEFAULT_SIZE];
	HMODULE library = NULL;
	for ( int version = 43; version >= 24; --version )
	{
		sprintf_s(libName, DEFAULT_SIZE, "d3dx9_%d.dll", version);
		library = LoadLibraryA( libName );
		if (library)
			break;
	}

	if (!library)
	{
		MessageBoxA(NULL, "d3dx9_xx.dll无法加载，程序退出!", "初始化错误", MB_OK);
		exit(0);
	}

	d3dxAssembleShaderFromFileW = (D3DXASSEMBLESHADERFROMFILEW)GetProcAddress(library, "D3DXAssembleShaderFromFileW");
	_ASSERT(d3dxAssembleShaderFromFileW);

	d3dxCompileShaderFromFileW = (D3DXCOMPILESHADERFROMFILEW)GetProcAddress(library, "D3DXCompileShaderFromFileW");
	_ASSERT(d3dxCompileShaderFromFileW);

	d3dxCreateEffectFromFileW = (D3DXCREATEEFFECTFROMFILEW)GetProcAddress(library, "D3DXCreateEffectFromFileW");
	_ASSERT(d3dxCreateEffectFromFileW);

	d3dxAssembleShaderFromFileA = (D3DXASSEMBLESHADERFROMFILEA)GetProcAddress(library, "D3DXAssembleShaderFromFileA");
	_ASSERT(d3dxAssembleShaderFromFileA);

	d3dxCompileShaderFromFileA = (D3DXCOMPILESHADERFROMFILEA)GetProcAddress(library, "D3DXCompileShaderFromFileA");
	_ASSERT(d3dxCompileShaderFromFileA);

	d3dxCreateEffectFromFileA = (D3DXCREATEEFFECTFROMFILEA)GetProcAddress(library, "D3DXCreateEffectFromFileA");
	_ASSERT(d3dxCreateEffectFromFileA);

	d3dxCreateBuffer = (D3DXCREATEBUFFER)GetProcAddress(library, "D3DXCreateBuffer");
	_ASSERT(d3dxCreateBuffer);

	d3dxDisassembleShader = (D3DXDISASSEMBLESHADER)GetProcAddress(library, "D3DXDisassembleShader");
	_ASSERT(d3dxDisassembleShader);
}




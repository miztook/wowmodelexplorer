#pragma once

#include "core.h"
#include <d3d11.h>
#include <D3DX11.h>
#include <D3Dcompiler.h>

//
void funcShader11VS(const c8* filename, void* args);
void funcShader11PS(const c8* filename, void* args);
void funcShader11GS(const c8* filename, void* args);

void buildShaders_40(bool vs, bool ps, bool gs);
void buildShaders_50(bool vs, bool ps, bool gs, bool ds, bool hs);

class CD3D11ShaderInclude : public ID3DInclude
{
public:
	explicit CD3D11ShaderInclude(const c8* dir) 
	{
		Q_strcpy(CurrentDir, MAX_TEXT_LENGTH, dir);
		normalizeDirName(CurrentDir);
	}

public:
	STDMETHOD				(Open)( D3D_INCLUDE_TYPE IncludeType, 
		LPCSTR pFileName,
		LPCVOID pParentData,
		LPCVOID* ppData,
		UINT* pBytes);
	STDMETHOD				(Close)(LPCVOID pData);

private:
	c8			CurrentDir[MAX_TEXT_LENGTH];
};



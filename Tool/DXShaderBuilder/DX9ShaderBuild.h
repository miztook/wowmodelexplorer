#pragma once

#include "core.h"
#include <d3d9.h>
#include <d3dx9.h>

//
void funcShader9VS( const char* filename, void* args );
void funcShader9PS( const char* filename, void* args );

void buildShaders_20(bool vs, bool ps);
void buildShaders_30(bool vs, bool ps);

class CD3D9ShaderInclude : public ID3DXInclude
{
public:
	explicit CD3D9ShaderInclude(const char* dir) 
	{
		Q_strcpy(CurrentDir, MAX_TEXT_LENGTH, dir);
		normalizeDirName(CurrentDir);
	}

public:
	STDMETHOD				(Open)(D3DXINCLUDE_TYPE Type,LPCSTR Name,LPCVOID ParentData,LPCVOID* Data,UINT* Bytes);
	STDMETHOD				(Close)(LPCVOID Data);

private:
	char			CurrentDir[MAX_TEXT_LENGTH];
};
#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D9

#include "base.h"
#include <d3d9.h>

class CD3D9VertexDeclaration
{
public:
	static D3DVERTEXELEMENT9	Decl_P[];
	static D3DVERTEXELEMENT9	Decl_PC[];
	static D3DVERTEXELEMENT9	Decl_PCT[];
	static D3DVERTEXELEMENT9	Decl_PN[];
	static D3DVERTEXELEMENT9	Decl_PNC[];
	static D3DVERTEXELEMENT9	Decl_PNT[];
	static D3DVERTEXELEMENT9	Decl_PT[];
	static D3DVERTEXELEMENT9	Decl_PNCT[];
	static D3DVERTEXELEMENT9	Decl_PNCT2[];
	static D3DVERTEXELEMENT9	Decl_PNT2W_M[];

public:
	explicit CD3D9VertexDeclaration(E_VERTEX_TYPE vtype);
	~CD3D9VertexDeclaration();

	 IDirect3DVertexDeclaration9* getDx9Declaration() const { return Declaration; }

private:
	E_VERTEX_TYPE VertexType;
	IDirect3DVertexDeclaration9*		Declaration;
};

#endif
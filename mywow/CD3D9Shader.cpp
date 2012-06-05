#include "stdafx.h"
#include "CD3D9Shader.h"
#include "mywow.h"
#include "CD3D9Helper.h"


CD3D9VertexShader::CD3D9VertexShader( E_VS_TYPE type, IDirect3DVertexShader9* vs, LPD3DXCONSTANTTABLE vsConstantTable, VSHADERCONSTCALLBACK callback )
	: IVertexShader(callback), VertexShader(vs), VSConstantTable(vsConstantTable), Type(type)
{
	if (VSConstantTable)
	{
		D3DXCONSTANTTABLE_DESC table_desc;
		VSConstantTable->GetDesc(&table_desc);
		for (u32 i=0; i<table_desc.Constants; ++i)
		{
			D3DXCONSTANT_DESC desc;
			D3DXHANDLE handle = VSConstantTable->GetConstant(NULL, i);
			if (handle)
			{
				VSConstantTable->GetConstantDesc(handle, &desc, NULL);
				ConstMap[desc.Name] = desc.RegisterIndex;
			}
		}
	}
}

CD3D9VertexShader::~CD3D9VertexShader()
{
	SAFE_RELEASE(VertexShader); 
}

u32 CD3D9VertexShader::getRegIndex( const c8* name )
{
	T_ConstMap::iterator itr = ConstMap.find(name);
	_ASSERT(itr != ConstMap.end());
	return itr->second;
}

CD3D9PixelShader::CD3D9PixelShader( E_PS_TYPE type, IDirect3DPixelShader9* ps, LPD3DXCONSTANTTABLE psConstantTable, PSHADERCONSTCALLBACK callback )
	: IPixelShader(callback), PixelShader(ps), PSConstantTable(psConstantTable), Type(type)
{
	D3DXCONSTANTTABLE_DESC table_desc;
	PSConstantTable->GetDesc(&table_desc);
	for (u32 i=0; i<table_desc.Constants; ++i)
	{
		D3DXCONSTANT_DESC desc;
		D3DXHANDLE handle = PSConstantTable->GetConstant(NULL, i);
		if (handle)
		{
			PSConstantTable->GetConstantDesc(handle, &desc, NULL);
			ConstMap[desc.Name] = desc.RegisterIndex;
		}
	}
}

CD3D9PixelShader::~CD3D9PixelShader()
{
	SAFE_RELEASE(PixelShader);
}

u32 CD3D9PixelShader::getRegIndex( const c8* name )
{
	T_ConstMap::iterator itr = ConstMap.find(name);
	_ASSERT(itr != ConstMap.end());
	return itr->second;
}

CD3D9Effect::~CD3D9Effect()
{
	SAFE_RELEASE(Effect); 
}
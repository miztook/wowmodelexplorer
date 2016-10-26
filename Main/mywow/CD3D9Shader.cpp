#include "stdafx.h"
#include "CD3D9Shader.h"
#include "mywow.h"

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D9

#include "CD3D9Driver.h"

CD3D9VertexShader::CD3D9VertexShader( E_VS_TYPE type, IDirect3DVertexShader9* vs, VSHADERCONSTCALLBACK callback )
	: IVertexShader(type, callback), VertexShader(vs)
{
	Driver = static_cast<CD3D9Driver*>(g_Engine->getDriver());
}

CD3D9VertexShader::~CD3D9VertexShader()
{
	SAFE_RELEASE(VertexShader); 
}

void CD3D9VertexShader::setShaderConstant( const c8* name, const void* srcData, u32 size )
{
	const SDx9ConstDesc* desc = getConstantDesc(name);
	ASSERT(desc);
	if (desc)
	{
		setShaderConstant(desc, srcData, size);
	}
}

const SDx9ConstDesc* CD3D9VertexShader::getConstantDesc( const c8* name ) const
{
	for (T_ConstList::const_iterator itr = ConstList.begin(); itr != ConstList.end(); ++itr)
	{
		if (strcmp(name, itr->name) == 0)
			return &(*itr);
	}
	return NULL_PTR;
}

void CD3D9VertexShader::setShaderConstant( const SDx9ConstDesc* desc, const void* srcData, u32 size )
{
	//if(desc->index == 0)			//骨骼矩阵例外，sizeof(matrix4X3) != desc.size(列优先)
	//	ASSERT(size <= desc->size);

	ASSERT(size % 16 == 0);

	Driver->pID3DDevice->SetVertexShaderConstantF(desc->index, (const float*)srcData, size / 16);
}


CD3D9PixelShader::CD3D9PixelShader( E_PS_TYPE type, E_PS_MACRO macro, IDirect3DPixelShader9* ps, PSHADERCONSTCALLBACK callback )
	: IPixelShader(type, macro, callback), PixelShader(ps)
{
	Driver = static_cast<CD3D9Driver*>(g_Engine->getDriver());
}

CD3D9PixelShader::~CD3D9PixelShader()
{
	SAFE_RELEASE(PixelShader);
}

void CD3D9PixelShader::setShaderConstant( const c8* name, const void* srcData, u32 size )
{
	const SDx9ConstDesc* desc = getConstantDesc(name);
	ASSERT(desc);
	if (desc)
	{
		setShaderConstant(desc, srcData, size);
	}
}

void CD3D9PixelShader::setTextureConstant( const c8* name, ITexture* texture )
{
	const SDx9ConstDesc* desc = getConstantDesc(name);
	ASSERT(desc);
	if (desc)
	{
		Driver->setTexture(desc->index, texture);
	}
}

const SDx9ConstDesc* CD3D9PixelShader::getConstantDesc(const c8* name) const
{
	for (T_ConstList::const_iterator itr = ConstList.begin(); itr != ConstList.end(); ++itr)
	{
		if (strcmp(name, itr->name) == 0)
			return &(*itr);
	}
	return NULL_PTR;
}

void CD3D9PixelShader::setShaderConstant( const SDx9ConstDesc* desc, const void* srcData, u32 size )
{
	ASSERT(size <= desc->size);
	ASSERT(size % 16 == 0);

	Driver->pID3DDevice->SetPixelShaderConstantF(desc->index, (const float*)srcData, size / 16);
}


#endif
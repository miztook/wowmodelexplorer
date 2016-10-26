#include "stdafx.h"
#include "CD3D11Shader.h"
#include "mywow.h"

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D11

#include "CD3D11Driver.h"

CD3D11VertexShader::CD3D11VertexShader( E_VS_TYPE type, ID3D11VertexShader* vs, VSHADERCONSTCALLBACK callback, const void* codeBuffer, u32 codeSize )
	: IVertexShader(type, callback), VertexShader(vs), CodeSize(codeSize)
{
	Driver = static_cast<CD3D11Driver*>(g_Engine->getDriver());

	CodeBuffer = new u8[CodeSize];
	Q_memcpy(CodeBuffer, CodeSize, codeBuffer, CodeSize);
}

CD3D11VertexShader::~CD3D11VertexShader()
{
	delete[] CodeBuffer;
	SAFE_RELEASE_STRICT(VertexShader); 
}

const SDx11ConstDesc* CD3D11VertexShader::getConstantDesc( const c8* name ) const
{
	for (T_BufferList::const_iterator itr = CBufferList.begin(); itr != CBufferList.end(); ++itr)
	{
		if (strcmp(name, itr->name) == 0)
			return &(*itr);
	}

	for (T_BufferList::const_iterator itr = TBufferList.begin(); itr != TBufferList.end(); ++itr)
	{
		if (strcmp(name, itr->name) == 0)
			return &(*itr);
	}

	return NULL_PTR;
}

SDx11ConstDesc* CD3D11VertexShader::getConstantDesc( u32 index )
{
	if (index >= getConstantCount())
		return NULL_PTR;

	if (index < CBufferList.size())
	{
		T_BufferList::iterator itr = CBufferList.begin();
		while(index)
		{
			++itr;
			--index;
		}
		return &(*itr);
	}
	else
	{
		T_BufferList::iterator itr = TBufferList.begin();
		while(index - CBufferList.size() > 0)
		{
			++itr;
			--index;
		}
		return &(*itr);
	}
}

const STextureDesc11* CD3D11VertexShader::getTextureDesc( const c8* name ) const
{
	for (T_TextureList::const_iterator itr = TextureList.begin(); itr != TextureList.end(); ++itr)
	{
		if (strcmp(name, itr->name) == 0)
			return &(*itr);
	}
	return NULL_PTR;
}

void CD3D11VertexShader::setShaderConstant( const c8* name, const void* srcData, u32 size )
{
	const SDx11ConstDesc* desc = getConstantDesc(name);
	ASSERT(desc);
	if (desc)
	{
		setShaderConstant(desc, srcData, size);
	}
}

const SSamplerDesc11* CD3D11VertexShader::getSamplerDesc( const c8* name ) const
{
	for (T_SamplerList::const_iterator itr = SamplerList.begin(); itr != SamplerList.end(); ++itr)
	{
		if (strcmp(name, itr->name) == 0)
			return &(*itr);
	}
	return NULL_PTR;
}

void CD3D11VertexShader::setShaderConstant( const SDx11ConstDesc* desc, const void* srcData, u32 size )
{
	ASSERT(size % 16 == 0);
	
	D3D11_MAPPED_SUBRESOURCE mapOut;

	void* pDest = NULL_PTR;
	//ImmediateContext

	HRESULT hr = Driver->ImmediateContext->Map(desc->constBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapOut);
	if(FAILED(hr))
	{
		ASSERT(false);
		return;
	}

	pDest = mapOut.pData;
	Q_memcpy(pDest, size, (const void*)srcData, size);

	//ImmediateContext
	Driver->ImmediateContext->Unmap(desc->constBuffer, 0);
}

void CD3D11VertexShader::onShaderUsed()
{
	if (!CBufferList.empty())
	{
		ID3D11Buffer** buffers = (ID3D11Buffer**)Z_AllocateTempMemory(sizeof(ID3D11Buffer*) * CBufferList.size());
		u32 count = 0;
		for (T_BufferList::const_iterator itr = CBufferList.begin(); itr != CBufferList.end(); ++itr)
		{
			buffers[count] = itr->constBuffer;
			++count;
		}
		u32 start = CBufferList.front().index;

		//ImmediateContext
		Driver->ImmediateContext->VSSetConstantBuffers(start, count, buffers);
	
		Z_FreeTempMemory(buffers);
	}

	if (!TBufferList.empty())
	{
		ID3D11ShaderResourceView** shaderRVs = (ID3D11ShaderResourceView**)Z_AllocateTempMemory(sizeof(ID3D11ShaderResourceView*) * TBufferList.size());
		u32 count = 0;
		for (T_BufferList::const_iterator itr = TBufferList.begin(); itr != TBufferList.end(); ++itr)
		{
			shaderRVs[count] = itr->shaderResourceView;
			++count;
		}
		u32 start = TBufferList.front().index;

		//ImmediateContext
		Driver->ImmediateContext->VSSetShaderResources(start, count, shaderRVs);

		Z_FreeTempMemory(shaderRVs);
	}
}

CD3D11PixelShader::CD3D11PixelShader( E_PS_TYPE type, E_PS_MACRO macro, ID3D11PixelShader* ps, PSHADERCONSTCALLBACK callback )
	: IPixelShader(type, macro, callback), PixelShader(ps)
{
	Driver = static_cast<CD3D11Driver*>(g_Engine->getDriver());
}

CD3D11PixelShader::~CD3D11PixelShader()
{
	SAFE_RELEASE_STRICT(PixelShader);
}

const SDx11ConstDesc* CD3D11PixelShader::getConstantDesc( const c8* name ) const
{
	for (T_BufferList::const_iterator itr = CBufferList.begin(); itr != CBufferList.end(); ++itr)
	{
		if (strcmp(name, itr->name) == 0)
			return &(*itr);
	}

	for (T_BufferList::const_iterator itr = TBufferList.begin(); itr != TBufferList.end(); ++itr)
	{
		if (strcmp(name, itr->name) == 0)
			return &(*itr);
	}

	return NULL_PTR;
}

SDx11ConstDesc* CD3D11PixelShader::getConstantDesc( u32 index )
{
	if (index >= getConstantCount())
		return NULL_PTR;

	if (index < CBufferList.size())
	{
		T_BufferList::iterator itr = CBufferList.begin();
		while(index)
		{
			++itr;
			--index;
		}
		return &(*itr);
	}
	else
	{
		T_BufferList::iterator itr = TBufferList.begin();
		while(index - CBufferList.size() > 0)
		{
			++itr;
			--index;
		}
		return &(*itr);
	}
}

const STextureDesc11* CD3D11PixelShader::getTextureDesc( const c8* name ) const
{
	for (T_TextureList::const_iterator itr = TextureList.begin(); itr != TextureList.end(); ++itr)
	{
		if (strcmp(name, itr->name) == 0)
			return &(*itr);
	}
	return NULL_PTR;
}

const SSamplerDesc11* CD3D11PixelShader::getSamplerDesc( const c8* name ) const
{
	for (T_SamplerList::const_iterator itr = SamplerList.begin(); itr != SamplerList.end(); ++itr)
	{
		if (strcmp(name, itr->name) == 0)
			return &(*itr);
	}
	return NULL_PTR;
}

void CD3D11PixelShader::setShaderConstant( const SDx11ConstDesc* desc, const void* srcData, u32 size )
{
	ASSERT(size <= desc->size);
	ASSERT(size % 16 == 0);

	D3D11_MAPPED_SUBRESOURCE mapOut;

	void* pDest = NULL_PTR;

	//ImmediateContext
	HRESULT hr = Driver->ImmediateContext->Map(desc->constBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapOut);
	if(FAILED(hr))
	{
		ASSERT(false);
		return;
	}

	pDest = mapOut.pData;
	Q_memcpy(pDest, size, (const void*)srcData, size);

	//ImmediateContext
	Driver->ImmediateContext->Unmap(desc->constBuffer, 0);
}

void CD3D11PixelShader::setShaderConstant( const c8* name, const void* srcData, u32 size )
{
	const SDx11ConstDesc* desc = getConstantDesc(name);
	//ASSERT(desc);
	if (desc)
	{
		setShaderConstant(desc, srcData, size);
	}
}

void CD3D11PixelShader::setTextureConstant( const c8* name, ITexture* texture )
{
	const STextureDesc11* desc = getTextureDesc(name);
	ASSERT(desc);
	if (desc)
	{
		Driver->setTexture(desc->index, texture);
	}
}

void CD3D11PixelShader::onShaderUsed()
{
	if (!CBufferList.empty())
	{
		ID3D11Buffer** buffers = (ID3D11Buffer**)Z_AllocateTempMemory(sizeof(ID3D11Buffer*) * CBufferList.size());
		u32 count = 0;
		for (T_BufferList::const_iterator itr = CBufferList.begin(); itr != CBufferList.end(); ++itr)
		{
			buffers[count] = itr->constBuffer;
			++count;
		}
		u32 start = CBufferList.front().index;

		//ImmediateContext
		Driver->ImmediateContext->PSSetConstantBuffers(start, count, buffers);

		Z_FreeTempMemory(buffers);
	}

	if (!TBufferList.empty())
	{
		ID3D11ShaderResourceView** shaderRVs = (ID3D11ShaderResourceView**)Z_AllocateTempMemory(sizeof(ID3D11ShaderResourceView*) * TBufferList.size());
		u32 count = 0;
		for (T_BufferList::const_iterator itr = TBufferList.begin(); itr != TBufferList.end(); ++itr)
		{
			shaderRVs[count] = itr->shaderResourceView;
			++count;
		}
		u32 start = TBufferList.front().index;

		//ImmediateContext
		Driver->ImmediateContext->PSSetShaderResources(start, count, shaderRVs);

		Z_FreeTempMemory(shaderRVs);
	}
}


#endif

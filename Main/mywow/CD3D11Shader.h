#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D11

#include "core.h"
#include "IShader.h"
#include <list>

class CD3D11Driver;
class ITexture;

struct SDx11ConstDesc 
{
	bool operator<(const SDx11ConstDesc& other) const
	{
		return index < other.index;
	}

	c8 name[32];
	u32 index;		//register index in dx9, cbuffer/tbuffer index in dx11
	u32 size;			//const size
	ID3D11Buffer*	constBuffer;
	ID3D11ShaderResourceView*	shaderResourceView;		//for tbuffer
	bool tbuffer;		//is tbuffer ? 
};

struct SSamplerDesc11
{
	bool operator<(const SSamplerDesc11& other) const
	{
		return index < other.index;
	}

	c8 name[32];
	u32 index;
	u32 count;
};

struct STextureDesc11
{
	bool operator<(const STextureDesc11& other) const
	{
		return index < other.index;
	}

	c8 name[32];
	u32 index;
	u32 count;
};

class CD3D11VertexShader : public IVertexShader
{
private:
	CD3D11VertexShader(E_VS_TYPE type, ID3D11VertexShader* vs, VSHADERCONSTCALLBACK callback, const void* codeBuffer, u32 codeSize);

	~CD3D11VertexShader();

	typedef std::list<SDx11ConstDesc, qzone_allocator<SDx11ConstDesc> >		T_BufferList;
	typedef std::list<SSamplerDesc11, qzone_allocator<SSamplerDesc11> >		T_SamplerList;
	typedef std::list<STextureDesc11, qzone_allocator<STextureDesc11> >		T_TextureList;

	 T_BufferList& getCBufferList() { return CBufferList; }
	 T_BufferList& getTBufferList() { return TBufferList; }
	 T_SamplerList& getSamplerList() { return SamplerList; }
	 T_TextureList& getTextureList() { return TextureList; }

	SDx11ConstDesc* getConstantDesc(u32 index);

public:
	bool isValid() const { return VertexShader != NULL_PTR; }
	void onShaderUsed();

	u32 getConstantCount() const { return (u32)CBufferList.size() + (u32)TBufferList.size(); }
	u32 getTextureCount() const { return (u32)TextureList.size(); }
	u32 getSamplerCount() const { return (u32)SamplerList.size(); }
	ID3D11VertexShader* getDXVShader() const { return VertexShader; }

	void setShaderConstant(const c8* name, const void* srcData, u32 size);

private:
	const SDx11ConstDesc* getConstantDesc(const c8* name) const;
	const STextureDesc11* getTextureDesc(const c8* name) const;
	const SSamplerDesc11* getSamplerDesc(const c8* name) const;

	const void* getCodeBuffer() const { return CodeBuffer; }
	u32 getCodeSize() const { return CodeSize; }
	void setShaderConstant(const SDx11ConstDesc* desc, const void* srcData, u32 size);

private:
	CD3D11Driver*		Driver;
	ID3D11VertexShader*			VertexShader;

	T_BufferList		CBufferList;
	T_BufferList		TBufferList;
	T_SamplerList		SamplerList;
	T_TextureList		TextureList;

	void*	CodeBuffer;
	u32		CodeSize;

	friend class CD3D11ShaderServices;
	friend class CD3D11VertexDeclaration;
};

class CD3D11PixelShader : public IPixelShader
{
private:
	CD3D11PixelShader(E_PS_TYPE type, E_PS_MACRO macro, ID3D11PixelShader* ps, PSHADERCONSTCALLBACK callback); 

	~CD3D11PixelShader();

	typedef std::list<SDx11ConstDesc, qzone_allocator<SDx11ConstDesc> >		T_BufferList;
	typedef std::list<SSamplerDesc11, qzone_allocator<SSamplerDesc11> >	T_SamplerList;
	typedef std::list<STextureDesc11, qzone_allocator<STextureDesc11> >		T_TextureList;

	 T_BufferList& getCBufferList() { return CBufferList; }
	 T_BufferList& getTBufferList() { return TBufferList; }
	 T_SamplerList& getSamplerList() { return SamplerList; }
	 T_TextureList& getTextureList() { return TextureList; }

	SDx11ConstDesc* getConstantDesc(u32 index);

public:
	bool isValid() const { return PixelShader != NULL_PTR; }
	void onShaderUsed();

	void setShaderConstant(const c8* name, const void* srcData, u32 size);
	void setTextureConstant(const c8* name, ITexture* texture);

	u32 getConstantCount() const { return (u32)CBufferList.size() + (u32)TBufferList.size(); }
	u32 getTextureCount() const { return (u32)TextureList.size(); }
	u32 getSamplerCount() const { return (u32)SamplerList.size(); }
	ID3D11PixelShader* getDXPShader() const { return PixelShader; }

private:
	const SDx11ConstDesc* getConstantDesc(const c8* name) const;
	const STextureDesc11* getTextureDesc(const c8* name) const;
	const SSamplerDesc11* getSamplerDesc(const c8* name) const;
	
	void setShaderConstant(const SDx11ConstDesc* desc, const void* srcData, u32 size);

private:
	CD3D11Driver*	Driver;
	ID3D11PixelShader*		PixelShader;

	T_BufferList		CBufferList;
	T_BufferList		TBufferList;
	T_SamplerList		SamplerList;
	T_TextureList		TextureList;

	friend class CD3D11ShaderServices;

};

#endif
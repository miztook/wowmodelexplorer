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

	char name[32];
	uint32_t index;		//register index in dx9, cbuffer/tbuffer index in dx11
	uint32_t size;			//const size
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

	char name[32];
	uint32_t index;
	uint32_t count;
};

struct STextureDesc11
{
	bool operator<(const STextureDesc11& other) const
	{
		return index < other.index;
	}

	char name[32];
	uint32_t index;
	uint32_t count;
};

class CD3D11VertexShader : public IVertexShader
{
private:
	CD3D11VertexShader(E_VS_TYPE type, ID3D11VertexShader* vs, VSHADERCONSTCALLBACK callback, const void* codeBuffer, uint32_t codeSize);

	~CD3D11VertexShader();

	typedef std::list<SDx11ConstDesc, qzone_allocator<SDx11ConstDesc> >		T_BufferList;
	typedef std::list<SSamplerDesc11, qzone_allocator<SSamplerDesc11> >		T_SamplerList;
	typedef std::list<STextureDesc11, qzone_allocator<STextureDesc11> >		T_TextureList;

	 T_BufferList& getCBufferList() { return CBufferList; }
	 T_BufferList& getTBufferList() { return TBufferList; }
	 T_SamplerList& getSamplerList() { return SamplerList; }
	 T_TextureList& getTextureList() { return TextureList; }

	SDx11ConstDesc* getConstantDesc(uint32_t index);

public:
	bool isValid() const { return VertexShader != nullptr; }
	void onShaderUsed();

	uint32_t getConstantCount() const { return (uint32_t)CBufferList.size() + (uint32_t)TBufferList.size(); }
	uint32_t getTextureCount() const { return (uint32_t)TextureList.size(); }
	uint32_t getSamplerCount() const { return (uint32_t)SamplerList.size(); }
	ID3D11VertexShader* getDXVShader() const { return VertexShader; }

	void setShaderConstant(const char* name, const void* srcData, uint32_t size);

private:
	const SDx11ConstDesc* getConstantDesc(const char* name) const;
	const STextureDesc11* getTextureDesc(const char* name) const;
	const SSamplerDesc11* getSamplerDesc(const char* name) const;

	const void* getCodeBuffer() const { return CodeBuffer; }
	uint32_t getCodeSize() const { return CodeSize; }
	void setShaderConstant(const SDx11ConstDesc* desc, const void* srcData, uint32_t size);

private:
	CD3D11Driver*		Driver;
	ID3D11VertexShader*			VertexShader;

	T_BufferList		CBufferList;
	T_BufferList		TBufferList;
	T_SamplerList		SamplerList;
	T_TextureList		TextureList;

	void*	CodeBuffer;
	uint32_t		CodeSize;

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

	SDx11ConstDesc* getConstantDesc(uint32_t index);

public:
	bool isValid() const { return PixelShader != nullptr; }
	void onShaderUsed();

	void setShaderConstant(const char* name, const void* srcData, uint32_t size);
	void setTextureConstant(const char* name, ITexture* texture);

	uint32_t getConstantCount() const { return (uint32_t)CBufferList.size() + (uint32_t)TBufferList.size(); }
	uint32_t getTextureCount() const { return (uint32_t)TextureList.size(); }
	uint32_t getSamplerCount() const { return (uint32_t)SamplerList.size(); }
	ID3D11PixelShader* getDXPShader() const { return PixelShader; }

private:
	const SDx11ConstDesc* getConstantDesc(const char* name) const;
	const STextureDesc11* getTextureDesc(const char* name) const;
	const SSamplerDesc11* getSamplerDesc(const char* name) const;
	
	void setShaderConstant(const SDx11ConstDesc* desc, const void* srcData, uint32_t size);

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
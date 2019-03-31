#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_DIRECT3D9

#include "IShader.h"
#include "core.h"
#include <list>

class ITexture;
class CD3D9Driver;

struct SDx9ConstDesc
{
	 bool operator<(const SDx9ConstDesc& other) const
	{
		return index < other.index;
	}

	char		name[32];
	uint32_t index;		//register index in dx9
	uint32_t size;			//const size
};

class CD3D9VertexShader : public IVertexShader
{
private:
	CD3D9VertexShader(E_VS_TYPE type, IDirect3DVertexShader9* vs, VSHADERCONSTCALLBACK callback);
	
	~CD3D9VertexShader();

public:
	bool isValid() const { return VertexShader != nullptr; }
	void onShaderUsed() {}

	 uint32_t getConstantCount() const { return (uint32_t)ConstList.size(); }
	 IDirect3DVertexShader9* getDXVShader() const { return VertexShader; }

	void setShaderConstant(const char* name, const void* srcData, uint32_t size);

public:
	typedef std::list<SDx9ConstDesc, qzone_allocator<SDx9ConstDesc> >		T_ConstList;

	 T_ConstList& getConstList() { return ConstList; }

private:
	void setShaderConstant(const SDx9ConstDesc* desc, const void* srcData, uint32_t size);
	const SDx9ConstDesc* getConstantDesc(const char* name) const;

private:
	CD3D9Driver*	Driver;
	IDirect3DVertexShader9*	VertexShader;
	T_ConstList		ConstList;

	friend class CD3D9ShaderServices;
};

class CD3D9PixelShader : public IPixelShader
{
private:
	CD3D9PixelShader(E_PS_TYPE type, E_PS_MACRO macro, IDirect3DPixelShader9* ps, PSHADERCONSTCALLBACK callback); 

	~CD3D9PixelShader();

public:
	bool isValid() const { return PixelShader != nullptr; }
	void onShaderUsed() {}

	 uint32_t getConstantCount() const { return (uint32_t)ConstList.size(); }
	 IDirect3DPixelShader9* getDXPShader() const { return PixelShader; }

	void setShaderConstant(const char* name, const void* srcData, uint32_t size);
	void setTextureConstant(const char* name, ITexture* texture);

public:
	typedef std::list<SDx9ConstDesc, qzone_allocator<SDx9ConstDesc> >		T_ConstList;

	 T_ConstList& getConstList() { return ConstList; }

private:
	void setShaderConstant(const SDx9ConstDesc* desc, const void* srcData, uint32_t size);
	const SDx9ConstDesc* getConstantDesc(const char* name) const;

private:
	CD3D9Driver*	Driver;
	IDirect3DPixelShader9*		PixelShader;

	T_ConstList		ConstList;

	friend class CD3D9ShaderServices;
};

#endif
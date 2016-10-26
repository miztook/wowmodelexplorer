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

	c8		name[32];
	u32 index;		//register index in dx9
	u32 size;			//const size
};

class CD3D9VertexShader : public IVertexShader
{
private:
	CD3D9VertexShader(E_VS_TYPE type, IDirect3DVertexShader9* vs, VSHADERCONSTCALLBACK callback);
	
	~CD3D9VertexShader();

public:
	bool isValid() const { return VertexShader != NULL_PTR; }
	void onShaderUsed() {}

	 u32 getConstantCount() const { return (u32)ConstList.size(); }
	 IDirect3DVertexShader9* getDXVShader() const { return VertexShader; }

	void setShaderConstant(const c8* name, const void* srcData, u32 size);

public:
	typedef std::list<SDx9ConstDesc, qzone_allocator<SDx9ConstDesc> >		T_ConstList;

	 T_ConstList& getConstList() { return ConstList; }

private:
	void setShaderConstant(const SDx9ConstDesc* desc, const void* srcData, u32 size);
	const SDx9ConstDesc* getConstantDesc(const c8* name) const;

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
	bool isValid() const { return PixelShader != NULL_PTR; }
	void onShaderUsed() {}

	 u32 getConstantCount() const { return (u32)ConstList.size(); }
	 IDirect3DPixelShader9* getDXPShader() const { return PixelShader; }

	void setShaderConstant(const c8* name, const void* srcData, u32 size);
	void setTextureConstant(const c8* name, ITexture* texture);

public:
	typedef std::list<SDx9ConstDesc, qzone_allocator<SDx9ConstDesc> >		T_ConstList;

	 T_ConstList& getConstList() { return ConstList; }

private:
	void setShaderConstant(const SDx9ConstDesc* desc, const void* srcData, u32 size);
	const SDx9ConstDesc* getConstantDesc(const c8* name) const;

private:
	CD3D9Driver*	Driver;
	IDirect3DPixelShader9*		PixelShader;

	T_ConstList		ConstList;

	friend class CD3D9ShaderServices;
};

#endif
#pragma once

#include "IShader.h"
#include "core.h"
#include <map>

class CD3D9VertexShader : public IVertexShader
{
private:
	CD3D9VertexShader(E_VS_TYPE type, IDirect3DVertexShader9* vs, LPD3DXCONSTANTTABLE vsConstantTable, VSHADERCONSTCALLBACK callback);
	
	~CD3D9VertexShader();

public:
	virtual E_VS_TYPE getType() const { return Type; }
	virtual bool isValid() const { return VertexShader != NULL; }
	virtual u32 getRegIndex(const c8* name);
	IDirect3DVertexShader9* getDXVShader() const { return VertexShader; }
	LPD3DXCONSTANTTABLE getDXVSConstantTable() const { return VSConstantTable; }

private:
	IDirect3DVertexShader9*			VertexShader;
	LPD3DXCONSTANTTABLE			VSConstantTable;
	E_VS_TYPE	Type;
	typedef std::map<string64, u32, std::less<string64>, qzone_allocator<std::pair<string64, u32>>>	 T_ConstMap;
	T_ConstMap		ConstMap;

	friend class CD3D9ShaderServices;
};

class CD3D9PixelShader : public IPixelShader
{
private:
	CD3D9PixelShader(E_PS_TYPE type, IDirect3DPixelShader9* ps, LPD3DXCONSTANTTABLE psConstantTable, PSHADERCONSTCALLBACK callback); 

	~CD3D9PixelShader();

public:
	virtual E_PS_TYPE getType() const { return Type; }
	virtual bool isValid() const { return PixelShader != NULL; }
	virtual u32 getRegIndex(const c8* name);
	IDirect3DPixelShader9* getDXPShader() const { return PixelShader; }
	LPD3DXCONSTANTTABLE getDXPSConstantTable() const { return PSConstantTable; }

private:
	IDirect3DPixelShader9*		PixelShader;
	LPD3DXCONSTANTTABLE			PSConstantTable;
	E_PS_TYPE	Type;

	typedef std::map<string64, u32, std::less<string64>, qzone_allocator<std::pair<string64, u32>>>	 T_ConstMap;
	T_ConstMap		ConstMap;

	friend class CD3D9ShaderServices;
};


class CD3D9Effect : public IEffect
{
public:
	CD3D9Effect(E_EFFECT_TYPE type, ID3DXEffect* eff, const c8* technique, EFFECTCONSTCALLBACK callback)
		: IEffect(callback), Effect(eff), Type(type), NumPasses(0) 
	{
		Technique = technique;
	}

	~CD3D9Effect();

public:
	virtual E_EFFECT_TYPE getType() const { return Type; }
	virtual bool isValid() const { return Effect != NULL; }

	ID3DXEffect* getEffect() const { return Effect; }
	const c8* getActiveTechnique() const { return Technique.c_str(); }

	u32		NumPasses;
private:
	ID3DXEffect*				Effect;
	E_EFFECT_TYPE		Type;

	string128		Technique;

	friend class CD3D9ShaderServices;
};
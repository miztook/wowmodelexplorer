#pragma once

#include "base.h"
#include "str.h"

class IVertexShader;
class IPixelShader;
class IEffect;
struct SMaterial;

typedef void (*VSHADERCONSTCALLBACK)(IVertexShader* vshader, const SMaterial& material);
typedef void (*PSHADERCONSTCALLBACK)(IPixelShader* vshader, const SMaterial& material);
typedef void (*EFFECTCONSTCALLBACK)(IEffect* vshader, const SMaterial& material);

class IVertexShader
{
public:
	IVertexShader(VSHADERCONSTCALLBACK callback) : ShaderConstCallback(callback) {}
	virtual ~IVertexShader() {}

public:
	virtual bool isValid() const  = 0;
	virtual E_VS_TYPE getType() const  = 0;
	virtual u32 getRegIndex(const c8* name) = 0;

	VSHADERCONSTCALLBACK		ShaderConstCallback;
};

class IPixelShader
{
public:
	IPixelShader(PSHADERCONSTCALLBACK callback) : ShaderConstCallback(callback) {}
	virtual ~IPixelShader() {}

public:
	virtual bool isValid() const  = 0;
	virtual E_PS_TYPE getType() const  = 0;
	virtual u32 getRegIndex(const c8* name) = 0;
	PSHADERCONSTCALLBACK		ShaderConstCallback;
};

class IEffect
{
public:
	IEffect(EFFECTCONSTCALLBACK callback) : EffectConstCallback(callback) {}
	virtual ~IEffect() {}

public:
	virtual bool isValid() const  = 0;
	virtual E_EFFECT_TYPE getType() const = 0;

	EFFECTCONSTCALLBACK		EffectConstCallback;
};
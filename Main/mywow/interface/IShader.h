#pragma once

#include "base.h"

/*
	bls file format:

	BlsHeader

	//macro0
	BlsBlockHeader

	BlsBlockContent

	//macro1

	BlsBlockHeader

	BlsBlockContent

	...
*/

#	pragma pack (1)

struct BlsHeader
{
	char    _magic[4];  /// shader type,must be "SPXG" or "SVXG"
	u16  _minor;        /// minor version
	u16  _major;			/// major version
	u32  shaderCount;    /// shader count in each shader block
};

struct BlsBlockHeader
{
	u32 constSize;
	u32 samplerSize;
	u32 textureSize;
	u32 codeSize;
};

struct BlsConst
{
	u32 index;
	u32 size;
	u32 tbuffer;
	c8 name[1];
};

struct BlsSampler 
{
	u32 index;
	c8 name[1];
};

struct BlsTexture
{
	u32 index;
	c8 name[1];
};

#	pragma pack ()

enum E_PS_MACRO
{
	PS_Macro_None = 0,
	PS_Macro_AlphaTest,

	PS_Macro_Num,
};

inline const c8* getPSMacroString(E_PS_MACRO macro)
{
	switch(macro)
	{
	case PS_Macro_AlphaTest:
		return "_ALPHATEST_";
	default:
		return "";
	}
}

class IVertexShader;
class IPixelShader;
struct SMaterial;

typedef void (*VSHADERCONSTCALLBACK)(IVertexShader* vshader, const SMaterial& material, u32 pass);
typedef void (*PSHADERCONSTCALLBACK)(IPixelShader* pshader, const SMaterial& material, u32 pass);

class IVertexShader
{
public:
	IVertexShader(E_VS_TYPE	type, VSHADERCONSTCALLBACK callback) 
		: Type(type), ShaderConstCallback(callback) {}
	virtual ~IVertexShader() {}

public:
	E_VS_TYPE getType() const { return Type; }

#ifdef FULL_INTERFACE
	virtual bool isValid() const  = 0;
	virtual void onShaderUsed() = 0;			//when shader is used
#endif

	VSHADERCONSTCALLBACK		ShaderConstCallback;
protected:
	E_VS_TYPE	Type;
};

class IPixelShader
{
public:
	IPixelShader(E_PS_TYPE type, E_PS_MACRO macro, PSHADERCONSTCALLBACK callback) 
		: Type(type), Macro(macro), ShaderConstCallback(callback) {}
	virtual ~IPixelShader() {}

public:
	E_PS_TYPE getType() const { return Type; }
	E_PS_MACRO getMacro() const { return Macro; }

#ifdef FULL_INTERFACE
	virtual bool isValid() const  = 0;
	virtual void onShaderUsed() = 0;		//when shader is used
#endif

	PSHADERCONSTCALLBACK		ShaderConstCallback;
protected:
	E_PS_TYPE	Type;
	E_PS_MACRO	Macro;
};

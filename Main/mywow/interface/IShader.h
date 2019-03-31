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
	uint16_t  _minor;        /// minor version
	uint16_t  _major;			/// major version
	uint32_t  shaderCount;    /// shader count in each shader block
};

struct BlsBlockHeader
{
	uint32_t constSize;
	uint32_t samplerSize;
	uint32_t textureSize;
	uint32_t codeSize;
};

struct BlsConst
{
	uint32_t index;
	uint32_t size;
	uint32_t tbuffer;
	char name[1];
};

struct BlsSampler 
{
	uint32_t index;
	char name[1];
};

struct BlsTexture
{
	uint32_t index;
	char name[1];
};

#	pragma pack ()

enum E_PS_MACRO
{
	PS_Macro_None = 0,
	PS_Macro_AlphaTest,

	PS_Macro_Num,
};

inline const char* getPSMacroString(E_PS_MACRO macro)
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

typedef void (*VSHADERCONSTCALLBACK)(IVertexShader* vshader, const SMaterial& material, uint32_t pass);
typedef void (*PSHADERCONSTCALLBACK)(IPixelShader* pshader, const SMaterial& material, uint32_t pass);

class IVertexShader
{
public:
	IVertexShader(E_VS_TYPE	type, VSHADERCONSTCALLBACK callback) 
		: Type(type), ShaderConstCallback(callback) {}
	virtual ~IVertexShader() {}

public:
	E_VS_TYPE getType() const { return Type; }

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

	PSHADERCONSTCALLBACK		ShaderConstCallback;
protected:
	E_PS_TYPE	Type;
	E_PS_MACRO	Macro;
};

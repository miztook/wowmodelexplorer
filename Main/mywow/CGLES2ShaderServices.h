#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_GLES2

#include "IShaderServices.h"
#include <unordered_map>
#include <vector>
#include "gles2/gl2ext.h"
#include "CGLES2_VS.h"
#include "CGLES2_PS.h"
#include "CGLES2_PS_ETC1.h"
#include "CGLES2Extension.h"

class CGLES2VertexShader;
class CGLES2PixelShader;
class CGLES2Driver;
class ITexture;

struct SGLUniformInfo
{
	GLenum type;
	s32 location;
	s32 dimension;
};

struct SGLProgram
{
	SGLProgram() : program(0), key(0), samplerCount(0) {}

	GLuint		program;
	u32		key;
	u32		samplerCount;

	//uniform info
	std::vector<SGLUniformInfo>	uniforms;

	typedef std::unordered_map<string_cs32, u32, string_cs32::string_cs_hash> T_UniformMap;
	T_UniformMap	uniformMap;

};

class CGLES2ShaderServices : public IShaderServices
{
private:
	DISALLOW_COPY_AND_ASSIGN(CGLES2ShaderServices);

public:
	CGLES2ShaderServices();
	~CGLES2ShaderServices();

public:
	virtual void onLost();
	virtual void onReset();

	virtual void loadAll();

	virtual IPixelShader* getPixelShader(E_PS_TYPE type, E_PS_MACRO macro = PS_Macro_None);

public:
	bool loadVShader( const c8* filename, E_VS_TYPE type, VSHADERCONSTCALLBACK callback );
	bool loadPShader( const c8* filename, E_PS_TYPE type, E_PS_MACRO macro, PSHADERCONSTCALLBACK callback );

	bool loadVShaderHLSL( const c8* filename, const c8* entry, const c8* profile, E_VS_TYPE type, VSHADERCONSTCALLBACK callback );
	bool loadPShaderHLSL( const c8* filename, const c8* entry, const c8* profile, E_PS_TYPE type, E_PS_MACRO macro, PSHADERCONSTCALLBACK callback );

	void useVertexShader(IVertexShader* vshader) { ShaderState.vshader = (CGLES2VertexShader*)vshader; }
	void usePixelShader(IPixelShader* pshader) { ShaderState.pshader = (CGLES2PixelShader*)pshader; }
	void applyShaders();
	void setShaderConstants(IVertexShader* vs, const SMaterial& material, u32 pass);
	void setShaderConstants(IPixelShader* ps, const SMaterial& material, u32 pass);

public:
	IVertexShader* getDefaultVertexShader(E_VERTEX_TYPE vType) const;
	IPixelShader* getDefaultPixelShader(E_VERTEX_TYPE vType, E_PS_MACRO macro = PS_Macro_None);

private:
	GLuint createHLShader(GLenum shaderType, const char* shader);

	struct SShaderState
	{
		void reset()
		{
			vshader = NULL;
			pshader = NULL;
		}

		CGLES2VertexShader*		vshader;
		CGLES2PixelShader*		pshader;
	};

private:
	const SGLUniformInfo* getUniformInfo(const c8* name) const { return getUniformInfo(CurrentProgram, name); }
	const SGLUniformInfo* getUniformInfo(const SGLProgram* p, const c8* name) const;

	void setShaderUniformF(u32 location, GLenum type, const f32* srcData, u32 size);
	void setTextureUniform(u32 location, GLenum type, u32 index);

public:	
	const SGLProgram* getCurrentGlProgram() const { return CurrentProgram; }
	s32 getAttribLocation(const SGLProgram* p, const c8* name) const;

	void setShaderUniformF(const c8* name, const f32* srcData, u32 size);
	void setTextureUniform(const c8* name, u32 index);

	u32 getSamplerCount() const { return CurrentProgram ? CurrentProgram->samplerCount : MATERIAL_MAX_TEXTURES; }

private:
	SGLProgram* createGlProgram(IVertexShader* vshader, IPixelShader* pshader);
	SGLProgram* getGlProgram(IVertexShader* vshader, IPixelShader* pshader);
	u32 makeKey(IVertexShader* vs, IPixelShader* ps);

	void makeMacroString(string1024& macroString, const c8* strMacro);

	bool linkGlProgram(SGLProgram* p);
	bool getGlProgramUniformInfo(SGLProgram* p);
	void removeGlProgram(SGLProgram* p);

private:
	typedef std::unordered_map<u32, SGLProgram> T_ProgramMap;
	T_ProgramMap		ProgramMap;

	CGLES2Extension*		Extension;
	CGLES2Driver*		Driver;
	SGLProgram*			CurrentProgram;

private:
	SShaderState	ShaderState;
	SShaderState	LastShaderState;

	CGLES2_VS	vsLoader;

#if defined(USE_PVR)
	CGLES2_PS	psLoader;
#elif defined(USE_KTX)
	CGLES2_PS_ETC1		psLoader;
#endif
	
	bool ResetShaders;
	u8	Padding[3];
};

inline const SGLUniformInfo* CGLES2ShaderServices::getUniformInfo( const SGLProgram* p, const c8* name ) const
{
	ASSERT(p);
	SGLProgram::T_UniformMap::const_iterator itr = p->uniformMap.find(name);
	if (itr == p->uniformMap.end())
		return NULL;

	u32 i = itr->second;
	return &p->uniforms[i];
}

inline s32 CGLES2ShaderServices::getAttribLocation( const SGLProgram* p, const c8* name ) const
{
	s32 loc = (s32)Extension->extGlGetAttribLocation(p->program, name);
	//ASSERT(loc >= 0);
	return loc;
}

inline void CGLES2ShaderServices::setShaderUniformF( const c8* name, const f32* srcData, u32 size )
{
	const SGLUniformInfo* info = getUniformInfo(name);
	ASSERT(info);
	if (info)
	{
		setShaderUniformF(info->location, info->type, srcData, size);
	}
}

inline void CGLES2ShaderServices::setTextureUniform( const c8* name, u32 index)
{
	const SGLUniformInfo* info = getUniformInfo(name);
	ASSERT(info);
	if (info)
	{
		setTextureUniform(info->location, info->type, index);
	}
}

inline void CGLES2ShaderServices::setShaderUniformF( u32 location, GLenum type, const f32* srcData, u32 size )
{
	u32 count = size / sizeof(f32);

	switch (type)
	{
	case GL_FLOAT:
		Extension->extGlUniform1fv(location, count, srcData);
		break;
	case GL_FLOAT_VEC2:
		Extension->extGlUniform2fv(location, count/2, srcData);
		break;
	case GL_FLOAT_VEC3:
		Extension->extGlUniform3fv(location, count/3, srcData);
		break;
	case GL_FLOAT_VEC4:
		Extension->extGlUniform4fv(location, count/4, srcData);
		break;
	case GL_FLOAT_MAT2:
		Extension->extGlUniformMatrix2fv(location, count/4, GL_FALSE, srcData);
		break;
	case GL_FLOAT_MAT3:
		Extension->extGlUniformMatrix3fv(location, count/9, GL_FALSE, srcData);
		break;
	case GL_FLOAT_MAT4:
		Extension->extGlUniformMatrix4fv(location, count/16, GL_FALSE, srcData);
		break;
	default:
		ASSERT(false);
		Extension->extGlUniform4fv(location, count/4, srcData);
		break;
	}
}

#endif
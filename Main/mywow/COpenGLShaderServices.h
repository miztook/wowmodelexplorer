#pragma once

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_OPENGL

#include "IShaderServices.h"
#include <unordered_map>
#include <map>
#include <vector>
#include "gl/glext.h"
#include "COpenGL_VS15.h"
#include "COpenGL_PS15.h"

#include "COpenGLExtension.h"

class COpenGLVertexShader;
class COpenGLPixelShader;
class COpenGLDriver;
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

	u32 key;
	GLhandleARB		program;
	u32 samplerCount;

	//uniform info
	std::vector<SGLUniformInfo>	uniforms;

#ifdef USE_QALLOCATOR
	typedef std::map<string_cs32, u32, std::less<string_cs32>, qzone_allocator<std::pair<string_cs32, u32>>> T_UniformMap;
#else
	typedef std::unordered_map<string_cs32, u32, string_cs32::string_cs_hash> T_UniformMap;
#endif

	T_UniformMap	uniformMap;

};

class COpenGLShaderServices : public IShaderServices
{
private:
	DISALLOW_COPY_AND_ASSIGN(COpenGLShaderServices);

public:
	COpenGLShaderServices();
	~COpenGLShaderServices();

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

	void useVertexShader(IVertexShader* vshader) { ShaderState.vshader = (COpenGLVertexShader*)vshader; }
	void usePixelShader(IPixelShader* pshader) { ShaderState.pshader = (COpenGLPixelShader*)pshader; }
	void applyShaders();
	void setShaderConstants(IVertexShader* vs, const SMaterial& material, u32 pass);
	void setShaderConstants(IPixelShader* ps, const SMaterial& material, u32 pass);

public:
	IVertexShader* getDefaultVertexShader(E_VERTEX_TYPE vType) const;
	IPixelShader* getDefaultPixelShader(E_VERTEX_TYPE vType, E_PS_MACRO macro = PS_Macro_None);

private:
	GLhandleARB createHLShader(GLenum shaderType, const char* shader);

	struct SShaderState
	{
		void reset()
		{
			vshader = NULL_PTR;
			pshader = NULL_PTR;
		}

		COpenGLVertexShader*		vshader;
		COpenGLPixelShader*		pshader;
	};

private:
	const SGLUniformInfo* getUniformInfo(const c8* name) const { return getUniformInfo(CurrentProgram, name); }
	const SGLUniformInfo* getUniformInfo(const SGLProgram* p, const c8* name) const;

	void setShaderUniformF(s32 location, GLenum type, const f32* srcData, u32 size);
	void setTextureUniform(s32 location, GLenum type, u32 index);

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

	bool checkError(const c8* type);
	bool linkGlProgram(SGLProgram* p);
	bool getGlProgramUniformInfo(SGLProgram* p);
	void removeGlProgram(SGLProgram* p);

private:
#ifdef USE_QALLOCATOR
	typedef std::map<u32, SGLProgram, std::less<u32>, qzone_allocator<std::pair<u32, SGLProgram>>> T_ProgramMap;
#else
	typedef std::unordered_map<u32, SGLProgram> T_ProgramMap;
#endif

	T_ProgramMap		ProgramMap;

	COpenGLExtension*		Extension;
	COpenGLDriver*	Driver;
	SGLProgram*			CurrentProgram;
	
private:
	SShaderState	ShaderState;
	SShaderState	LastShaderState;

	COpenGL_VS15	vs15Loader;
	COpenGL_PS15	ps15Loader;	
	
	bool ResetShaders;
};

inline const SGLUniformInfo* COpenGLShaderServices::getUniformInfo( const SGLProgram* p, const c8* name ) const
{
	ASSERT(p);
	SGLProgram::T_UniformMap::const_iterator itr = p->uniformMap.find(name);
	if (itr == p->uniformMap.end())
		return NULL_PTR;

	u32 i = itr->second;
	return &p->uniforms[i];
}

inline s32 COpenGLShaderServices::getAttribLocation( const SGLProgram* p, const c8* name ) const
{
	s32 loc = (s32)Extension->extGlGetAttribLocationARB(p->program, name);
	//ASSERT(loc >= 0);
	return loc;
}

inline void COpenGLShaderServices::setShaderUniformF( const c8* name, const f32* srcData, u32 size )
{
	const SGLUniformInfo* info = getUniformInfo(name);
	ASSERT(info);
	if (info)
	{
		setShaderUniformF(info->location, info->type, srcData, size);
	}
}

inline void COpenGLShaderServices::setTextureUniform( const c8* name,  u32 index )
{
	const SGLUniformInfo* info = getUniformInfo(name);
	ASSERT(info);
	if (info)
	{
		//ASSERT(info->location < MATERIAL_MAX_TEXTURES);

		setTextureUniform(info->location, info->type, index);
	}
}

inline void COpenGLShaderServices::setShaderUniformF( s32 location, GLenum type, const f32* srcData, u32 size )
{
	u32 count = size / sizeof(f32);

	switch (type)
	{
	case GL_FLOAT:
		Extension->extGlUniform1fv(location, count, srcData);
		break;
	case GL_FLOAT_VEC2_ARB:
		Extension->extGlUniform2fv(location, count/2, srcData);
		break;
	case GL_FLOAT_VEC3_ARB:
		Extension->extGlUniform3fv(location, count/3, srcData);
		break;
	case GL_FLOAT_VEC4_ARB:
		Extension->extGlUniform4fv(location, count/4, srcData);
		break;
	case GL_FLOAT_MAT2_ARB:
		Extension->extGlUniformMatrix2fv(location, count/4, GL_FALSE, srcData);
		break;
	case GL_FLOAT_MAT3_ARB:
		Extension->extGlUniformMatrix3fv(location, count/9, GL_FALSE, srcData);
		break;
	case GL_FLOAT_MAT4_ARB:
		Extension->extGlUniformMatrix4fv(location, count/16, GL_FALSE, srcData);
		break;
	default:
		ASSERT(false);
		Extension->extGlUniform4fv(location, count/4, srcData);
		break;
	}
}

#endif
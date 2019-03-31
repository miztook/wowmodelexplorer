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
	int32_t location;
	int32_t dimension;
};

struct SGLProgram
{
	SGLProgram() : program(0), key(0), samplerCount(0) {}

	uint32_t key;
	GLhandleARB		program;
	uint32_t samplerCount;

	//uniform info
	std::vector<SGLUniformInfo>	uniforms;

#ifdef USE_QALLOCATOR
	typedef std::map<string_cs32, uint32_t, std::less<string_cs32>, qzone_allocator<std::pair<string_cs32, uint32_t>>> T_UniformMap;
#else
	typedef std::unordered_map<string_cs32, uint32_t, string_cs32::string_cs_hash> T_UniformMap;
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
	virtual void loadAll();

	virtual IPixelShader* getPixelShader(E_PS_TYPE type, E_PS_MACRO macro = PS_Macro_None);

public:
	bool loadVShader( const char* filename, E_VS_TYPE type, VSHADERCONSTCALLBACK callback );
	bool loadPShader( const char* filename, E_PS_TYPE type, E_PS_MACRO macro, PSHADERCONSTCALLBACK callback );

	bool loadVShaderHLSL( const char* filename, const char* entry, const char* profile, E_VS_TYPE type, VSHADERCONSTCALLBACK callback );
	bool loadPShaderHLSL( const char* filename, const char* entry, const char* profile, E_PS_TYPE type, E_PS_MACRO macro, PSHADERCONSTCALLBACK callback );

	void useVertexShader(IVertexShader* vshader) { ShaderState.vshader = (COpenGLVertexShader*)vshader; }
	void usePixelShader(IPixelShader* pshader) { ShaderState.pshader = (COpenGLPixelShader*)pshader; }
	void applyShaders();
	void setShaderConstants(IVertexShader* vs, const SMaterial& material, uint32_t pass);
	void setShaderConstants(IPixelShader* ps, const SMaterial& material, uint32_t pass);

public:
	IVertexShader* getDefaultVertexShader(E_VERTEX_TYPE vType) const;
	IPixelShader* getDefaultPixelShader(E_VERTEX_TYPE vType, E_PS_MACRO macro = PS_Macro_None);

private:
	GLhandleARB createHLShader(GLenum shaderType, const char* shader);

	struct SShaderState
	{
		void reset()
		{
			vshader = nullptr;
			pshader = nullptr;
		}

		COpenGLVertexShader*		vshader;
		COpenGLPixelShader*		pshader;
	};

private:
	const SGLUniformInfo* getUniformInfo(const char* name) const { return getUniformInfo(CurrentProgram, name); }
	const SGLUniformInfo* getUniformInfo(const SGLProgram* p, const char* name) const;

	void setShaderUniformF(int32_t location, GLenum type, const float* srcData, uint32_t size);
	void setTextureUniform(int32_t location, GLenum type, uint32_t index);

public:	
	const SGLProgram* getCurrentGlProgram() const { return CurrentProgram; }
	int32_t getAttribLocation(const SGLProgram* p, const char* name) const;

	void setShaderUniformF(const char* name, const float* srcData, uint32_t size);
	void setTextureUniform(const char* name, uint32_t index);

	uint32_t getSamplerCount() const { return CurrentProgram ? CurrentProgram->samplerCount : MATERIAL_MAX_TEXTURES; }

private:
	SGLProgram* createGlProgram(IVertexShader* vshader, IPixelShader* pshader);
	SGLProgram* getGlProgram(IVertexShader* vshader, IPixelShader* pshader);
	uint32_t makeKey(IVertexShader* vs, IPixelShader* ps);

	bool checkError(const char* type);
	bool linkGlProgram(SGLProgram* p);
	bool getGlProgramUniformInfo(SGLProgram* p);
	void removeGlProgram(SGLProgram* p);

private:
#ifdef USE_QALLOCATOR
	typedef std::map<uint32_t, SGLProgram, std::less<uint32_t>, qzone_allocator<std::pair<uint32_t, SGLProgram>>> T_ProgramMap;
#else
	typedef std::unordered_map<uint32_t, SGLProgram> T_ProgramMap;
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

inline const SGLUniformInfo* COpenGLShaderServices::getUniformInfo( const SGLProgram* p, const char* name ) const
{
	ASSERT(p);
	SGLProgram::T_UniformMap::const_iterator itr = p->uniformMap.find(name);
	if (itr == p->uniformMap.end())
		return nullptr;

	uint32_t i = itr->second;
	return &p->uniforms[i];
}

inline int32_t COpenGLShaderServices::getAttribLocation( const SGLProgram* p, const char* name ) const
{
	int32_t loc = (int32_t)Extension->extGlGetAttribLocationARB(p->program, name);
	//ASSERT(loc >= 0);
	return loc;
}

inline void COpenGLShaderServices::setShaderUniformF( const char* name, const float* srcData, uint32_t size )
{
	const SGLUniformInfo* info = getUniformInfo(name);
	ASSERT(info);
	if (info)
	{
		setShaderUniformF(info->location, info->type, srcData, size);
	}
}

inline void COpenGLShaderServices::setTextureUniform( const char* name,  uint32_t index )
{
	const SGLUniformInfo* info = getUniformInfo(name);
	ASSERT(info);
	if (info)
	{
		//ASSERT(info->location < MATERIAL_MAX_TEXTURES);

		setTextureUniform(info->location, info->type, index);
	}
}

inline void COpenGLShaderServices::setShaderUniformF( int32_t location, GLenum type, const float* srcData, uint32_t size )
{
	uint32_t count = size / sizeof(float);

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
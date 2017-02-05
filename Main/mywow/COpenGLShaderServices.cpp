#include "stdafx.h"
#include "COpenGLShaderServices.h"
#include "mywow.h"

#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_OPENGL

#include "COpenGLDriver.h"
#include "COpenGLExtension.h"
#include "COpenGLShader.h"
#include "COpenGLHelper.h"
#include "COpenGLMaterialRenderServices.h"

COpenGLShaderServices::COpenGLShaderServices()
	: CurrentProgram(NULL_PTR)
{
	Driver = static_cast<COpenGLDriver*>(g_Engine->getDriver());
	Extension = Driver->getGLExtension();
	Driver->registerLostReset(this);

	LastShaderState.reset();
	ShaderState.reset();
	ResetShaders = true;
}

COpenGLShaderServices::~COpenGLShaderServices()
{
	Driver->removeLostReset(this);

	Extension->extGlUseProgramObject(0);

	for (T_ProgramMap::const_iterator i = ProgramMap.begin(); i != ProgramMap.end(); ++i)
	{
		Extension->extGlDeleteObject(i->second.program);
	}

	for (u32 i=0; i < EVST_COUNT; ++i)
	{
		delete VertexShaders[i];
	}

	for (u32 i=0; i < EPST_COUNT; ++i)
	{
		for (u32 k=PS_Macro_None; k<PS_Macro_Num; ++k)
			delete PixelShaders[i][k];
	}
}

void COpenGLShaderServices::onLost()
{

}

void COpenGLShaderServices::onReset()
{
	ResetShaders = true;
}

void COpenGLShaderServices::loadAll()
{
	const c8* profile = "";

	//vs
	g_Engine->getFileSystem()->writeLog(ELOG_GX, "begin loading shaders ...");

	if (Extension->ShaderLanguageVersion >= 303)
	{
		profile = "vs_1_5";
		vs15Loader.loadAll(this);
	}
	else 
	{
		ASSERT(false);
		profile = "";
	}

	g_Engine->getFileSystem()->writeLog(ELOG_GX, "finish loading shaders %s.", profile);

	//ps
	g_Engine->getFileSystem()->writeLog(ELOG_GX, "begin loading shaders ...");

	if (Extension->ShaderLanguageVersion >= 303)
	{
		profile = "ps_1_5";
		ps15Loader.loadAll(this);
	}
	else
	{
		ASSERT(false);
		profile = "";
	}

	g_Engine->getFileSystem()->writeLog(ELOG_GX, "finish loading shaders %s.", profile);
}

IPixelShader* COpenGLShaderServices::getPixelShader( E_PS_TYPE type, E_PS_MACRO macro /*= PS_Macro_None*/ )
{
	IPixelShader* ps = PixelShaders[type][macro];
	if (!ps)
	{
		if (Extension->ShaderLanguageVersion >= 303)
		{
			bool success = ps15Loader.loadPShaderHLSL(this, type, macro);
			ASSERT(success);
		}
		else
		{
			ASSERT(false);
		}
	}
	return PixelShaders[type][macro];
}

bool COpenGLShaderServices::loadVShader( const c8* filename, E_VS_TYPE type, VSHADERCONSTCALLBACK callback )
{
	/*
	string256 absFileName = g_Engine->getFileSystem()->getShaderBaseDirectory();
	absFileName.append(filename);

	IReadFile* rfile = g_Engine->getFileSystem()->createAndOpenFile(absFileName.c_str(), true);
	if (!rfile)
	{
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "COpenGLShaderServices::loadVShader Failed: Shader missing: %s", absFileName.c_str());
		ASSERT(false);
		return false;
	}

	GLuint vertexShader;
	Extension->extGlGenPrograms(1, &vertexShader);

	if (Extension->queryOpenGLFeature(IRR_ARB_vertex_program))
		Extension->extGlBindProgram(GL_VERTEX_PROGRAM_ARB, vertexShader);
	else if (Extension->queryOpenGLFeature(IRR_NV_vertex_program))
		Extension->extGlBindProgram(GL_VERTEX_PROGRAM_NV, vertexShader);
	else
		ASSERT(false);

	c8* buffer = (c8*)Z_AllocateTempMemory(rfile->getSize());
	u32 size = rfile->read(buffer, rfile->getSize());

	// compile
	if (Extension->queryOpenGLFeature(IRR_ARB_vertex_program))
		Extension->extGlProgramString(GL_VERTEX_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, (GLsizei)size, buffer);
	else if (Extension->queryOpenGLFeature(IRR_NV_vertex_program))
		Extension->extGlLoadProgram(GL_VERTEX_PROGRAM_NV, vertexShader, (GLsizei)size, (const GLubyte*)buffer);
	else
		ASSERT(false);

	Z_FreeTempMemory(buffer);

	delete rfile;

	//check error
	if (checkError("Vertex shader"))
	{
		Extension->extGlDeletePrograms(1, &vertexShader);
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "COpenGLShaderServices::loadVShader Failed");
		ASSERT(false);
		return false;
	}

	ASSERT(VertexShaderMap.find(type) == VertexShaderMap.end());
	VertexShaderMap[type] = new COpenGLVertexShader(type, vertexShader, false, callback);
	*/

	ASSERT(false);
	return false;
}

bool COpenGLShaderServices::loadPShader( const c8* filename, E_PS_TYPE type, E_PS_MACRO macro, PSHADERCONSTCALLBACK callback )
{
	/*
	string256 absFileName = g_Engine->getFileSystem()->getShaderBaseDirectory();
	absFileName.append(filename);

	IReadFile* rfile = g_Engine->getFileSystem()->createAndOpenFile(absFileName.c_str(), true);
	if (!rfile)
	{
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "COpenGLShaderServices::loadPShader Failed: Shader missing: %s", absFileName.c_str());
		ASSERT(false);
		return false;
	}

	GLuint pixelShader;
	Extension->extGlGenPrograms(1, &pixelShader);

	if (Extension->queryOpenGLFeature(IRR_ARB_vertex_program))
		Extension->extGlBindProgram(GL_FRAGMENT_PROGRAM_ARB, pixelShader);
	else if (Extension->queryOpenGLFeature(IRR_NV_vertex_program))
		Extension->extGlBindProgram(GL_FRAGMENT_PROGRAM_NV, pixelShader);
	else
		ASSERT(false);

	c8* buffer = (c8*)Z_AllocateTempMemory(rfile->getSize());
	u32 size = rfile->read(buffer, rfile->getSize());

	// compile
	if (Extension->queryOpenGLFeature(IRR_ARB_vertex_program))
		Extension->extGlProgramString(GL_FRAGMENT_PROGRAM_ARB, GL_PROGRAM_FORMAT_ASCII_ARB, (GLsizei)size, buffer);
	else if (Extension->queryOpenGLFeature(IRR_NV_vertex_program))
		Extension->extGlLoadProgram(GL_FRAGMENT_PROGRAM_NV, pixelShader, (GLsizei)size, (const GLubyte*)buffer);
	else
		ASSERT(false);

	Z_FreeTempMemory(buffer);

	delete rfile;

	//check error
	if (checkError("Pixel shader"))
	{
		Extension->extGlDeletePrograms(1, &pixelShader);
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "COpenGLShaderServices::loadPShader Failed");
		ASSERT(false);
		return false;
	}

	ASSERT(PixelShaderMap.find(type) == PixelShaderMap.end());
	PixelShaderMap[type] = new COpenGLPixelShader(type, pixelShader, false, callback);
	*/

	ASSERT(false);
	return false;
}

bool COpenGLShaderServices::loadVShaderHLSL( const c8* filename, const c8* entry, const c8* profile, E_VS_TYPE type, VSHADERCONSTCALLBACK callback )
{
	string_path absFileName = g_Engine->getFileSystem()->getShaderBaseDirectory();
	absFileName.append(filename);

	IReadFile* rfile = g_Engine->getFileSystem()->createAndOpenFile(absFileName.c_str(), false);
	if (!rfile)
	{
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "COpenGLShaderServices::loadVShaderHLSL Failed: Shader missing: %s", absFileName.c_str());
		ASSERT(false);
		return false;
	}

	c8* buffer = (c8*)Z_AllocateTempMemory(rfile->getSize());
	rfile->seek(0);

	c8* p = buffer;
	while(!rfile->isEof())
	{
		u32 size = rfile->readText(p, rfile->getSize() - (u32)(p - buffer));

		if(size>1 && !isComment(p))			//skip comment
			p += size;
	}

	ASSERT(p <= buffer + rfile->getSize());

	//compile
	GLhandleARB vertexShader = createHLShader(GL_VERTEX_SHADER_ARB, buffer);

	Z_FreeTempMemory(buffer);

	delete rfile;

	if (!vertexShader)
	{
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "COpenGLShaderServices::loadVShaderHLSL Failed");
		ASSERT(false);
		return false;
	}

	ASSERT(VertexShaders[type] == NULL_PTR);
	VertexShaders[type] = new COpenGLVertexShader(type, (GLuint)vertexShader, callback);

	return true;
}

bool COpenGLShaderServices::loadPShaderHLSL( const c8* filename, const c8* entry, const c8* profile, E_PS_TYPE type, E_PS_MACRO macro, PSHADERCONSTCALLBACK callback )
{
	string_path absFileName = g_Engine->getFileSystem()->getShaderBaseDirectory();
	absFileName.append(filename);

	IReadFile* rfile = g_Engine->getFileSystem()->createAndOpenFile(absFileName.c_str(), false);
	if (!rfile)
	{
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "COpenGLShaderServices::loadPShaderHLSL Failed: Shader missing: %s", absFileName.c_str());
		ASSERT(false);
		return false;
	}

	//macro
	string1024 macroString;
	makeMacroString(macroString, getPSMacroString(macro));

	u32 macroLength = macroString.length();

	//log
	if (macroLength)
	{
		CSysUtility::outputDebug("compiling shader with macro: %s, macro: %s", filename, getPSMacroString(macro));
	}

	c8* buffer = (c8*)Z_AllocateTempMemory(macroLength + rfile->getSize());
	rfile->seek(0);

	c8* p = buffer;

	bool version = false;
	bool macroAdd = false;
	while(!rfile->isEof())
	{
		u32 size = rfile->readText(p, rfile->getSize() - (u32)(p - buffer));

		if(size>1 && !isComment(p))			//skip comment
		{
			if(!version)
				version = strstr(p, "#version") != NULL_PTR;

			p += size;
		}

		if (version && !macroAdd && macroLength)			//!NOTICE, macro must be after #version
		{
			Q_strcpy(p, macroString.length()+1, macroString.c_str());
			p += macroLength;

			macroAdd = true;
		}
	}

	ASSERT(p <= buffer + macroLength + rfile->getSize());

	//compile
	GLhandleARB pixelShader = createHLShader(GL_FRAGMENT_SHADER_ARB, buffer);

	Z_FreeTempMemory(buffer);

	delete rfile;

	if (!pixelShader)
	{
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "COpenGLShaderServices::loadPShaderHLSL Failed");
		ASSERT(false);
		return false;
	}

	ASSERT(PixelShaders[type][macro] == NULL_PTR);
	PixelShaders[type][macro] = new COpenGLPixelShader(type, macro, (GLuint)pixelShader, callback);

	return true;
}

void COpenGLShaderServices::applyShaders()
{
	if (ResetShaders || LastShaderState.vshader != ShaderState.vshader || LastShaderState.pshader != ShaderState.pshader)
	{
		if (ShaderState.vshader && ShaderState.pshader)
		{
			CurrentProgram = getGlProgram(ShaderState.vshader, ShaderState.pshader);
		}
		else
		{
#ifndef FIXPIPELINE
			ASSERT(false);
#endif
			CurrentProgram = NULL_PTR;
		}

		if (CurrentProgram)
		{
			Extension->extGlUseProgramObject(CurrentProgram->program);

			for (u32 i=0; i<CurrentProgram->samplerCount; ++i)
			{
				c8 name[16];
				Q_sprintf(name, 16, "g_TexSampler%u", i);
				setTextureUniform(name, i);
			}
		}
		else
		{
			Extension->extGlUseProgramObject(0);
		}
	}

	LastShaderState = ShaderState;
	ResetShaders = false;
}

void COpenGLShaderServices::setShaderConstants( IVertexShader* vs, const SMaterial& material, u32 pass )
{
#ifndef FIXPIPELINE
	ASSERT(vs);
#endif
	if (vs && vs->ShaderConstCallback)
		vs->ShaderConstCallback(vs, material, pass);
}

void COpenGLShaderServices::setShaderConstants( IPixelShader* ps, const SMaterial& material, u32 pass )
{
#ifndef FIXPIPELINE
	ASSERT(ps);
#endif
	if (ps && ps->ShaderConstCallback)
		ps->ShaderConstCallback(ps, material, pass);
}

IVertexShader* COpenGLShaderServices::getDefaultVertexShader( E_VERTEX_TYPE vType ) const
{
	switch (vType)
	{
	case EVT_P:
		return getVertexShader(EVST_DEFAULT_P);
	case EVT_PC:
		return getVertexShader(EVST_DEFAULT_PC);
	case EVT_PCT:
		return getVertexShader(EVST_DEFAULT_PCT);
	case EVT_PN:
		return getVertexShader(EVST_DEFAULT_PN);
	case EVT_PNC:
		return getVertexShader(EVST_DEFAULT_PNC);
	case EVT_PNCT:
		return getVertexShader(EVST_DEFAULT_PNCT);
	case EVT_PNCT2:
		return getVertexShader(EVST_DEFAULT_PNCT2);
	case EVT_PNT:
		return getVertexShader(EVST_DEFAULT_PNT);
	case EVT_PNT2W:
		return getVertexShader(EVST_DIFFUSE_T1);
	case EVT_PT:
		return getVertexShader(EVST_DEFAULT_PT);
	default:
		return NULL_PTR;
	}
}

IPixelShader* COpenGLShaderServices::getDefaultPixelShader( E_VERTEX_TYPE vType, E_PS_MACRO macro )
{
	switch (vType)
	{
	case EVT_P:
		return getPixelShader(EPST_DEFAULT_P, macro);
	case EVT_PC:
		return getPixelShader(EPST_DEFAULT_PC, macro);
	case EVT_PCT:
		return getPixelShader(EPST_DEFAULT_PCT, macro);
	case EVT_PN:
		return getPixelShader(EPST_DEFAULT_PN, macro);
	case EVT_PNC:
		return getPixelShader(EPST_DEFAULT_PNC, macro);
	case EVT_PNCT:
		return getPixelShader(EPST_DEFAULT_PNCT, macro);
	case EVT_PNCT2:
		return getPixelShader(EPST_DEFAULT_PNCT2, macro);
	case EVT_PNT:
	case EVT_PNT2W:
		return getPixelShader(EPST_DEFAULT_PNT, macro);
	case EVT_PT:
		return getPixelShader(EPST_DEFAULT_PT, macro);
	default:
		return NULL_PTR;
	}
}

SGLProgram* COpenGLShaderServices::getGlProgram( IVertexShader* vshader, IPixelShader* pshader )
{
	u32 key = makeKey(vshader, pshader);
	T_ProgramMap::iterator itr = ProgramMap.find(key);
	if (itr == ProgramMap.end())
	{
		return createGlProgram(vshader, pshader);
	}
	return &itr->second;
}

u32 COpenGLShaderServices::makeKey( IVertexShader* vs, IPixelShader* ps )
{
	return ((vs->getType() & 0xff)<<24) | ((ps->getType() & 0xff)<<8) | (ps->getMacro() & 0xff);
}

SGLProgram* COpenGLShaderServices::createGlProgram( IVertexShader* vshader, IPixelShader* pshader )
{
	COpenGLVertexShader* vs = static_cast<COpenGLVertexShader*>(vshader);
	COpenGLPixelShader* ps = static_cast<COpenGLPixelShader*>(pshader);

	//高级语言的shader才用program
	if (!vs || !ps)
	{
		ASSERT(false);
		return NULL_PTR;
	}

	u32 key = makeKey(vs, ps);
	
	ASSERT(ProgramMap.find(key) == ProgramMap.end());

	ProgramMap[key] = SGLProgram();
	SGLProgram* p = &ProgramMap[key];
	p->key = key;
	p->program = Extension->extGlCreateProgramObject();

	Extension->extGlAttachObject(p->program, (GLhandleARB)vs->getGLVShader());
	Extension->extGlAttachObject(p->program, (GLhandleARB)ps->getGLPShader());

	//bind ps output
	Extension->extGlBindFragDataLocation(p->program, 0, "COLOR0");

	//link
	if (!linkGlProgram(p))
	{
		removeGlProgram(p);
		ASSERT(false);
		return NULL_PTR;
	}

	//get uniform
	Extension->extGlUseProgramObject(p->program);
	if (!getGlProgramUniformInfo(p))
	{
		removeGlProgram(p);
		ASSERT(false);
		Extension->extGlUseProgramObject(0);
		return NULL_PTR;
	}
	Extension->extGlUseProgramObject(0);

	return p;
}

void COpenGLShaderServices::removeGlProgram( SGLProgram* p )
{
	T_ProgramMap::iterator itr = ProgramMap.find(p->key);
	if (itr == ProgramMap.end())
		return;

	Extension->extGlDeleteObject(p->program);
	ProgramMap.erase(itr);
}

bool COpenGLShaderServices::linkGlProgram( SGLProgram* p )
{
	//link
	Extension->extGlLinkProgramARB(p->program);

	GLint status = 0;

	Extension->extGlGetObjectParameteriv(p->program, GL_OBJECT_LINK_STATUS_ARB, &status);

	if (!status)
	{
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "GLSL shader program failed to link: ");
		// check error message and log it
		GLint maxLength=0;
		GLsizei length;

		Extension->extGlGetObjectParameteriv(p->program, GL_OBJECT_INFO_LOG_LENGTH_ARB, &maxLength);

		GLcharARB *infoLog = (GLcharARB*)Z_AllocateTempMemory(sizeof(GLcharARB) * maxLength);
		Extension->extGlGetInfoLog(p->program, maxLength, &length, infoLog);
		g_Engine->getFileSystem()->writeLog(ELOG_GX, reinterpret_cast<const c8*>(infoLog));
		Z_FreeTempMemory(infoLog);
		ASSERT(false);

		return false;
	}

	return true;
}

bool COpenGLShaderServices::getGlProgramUniformInfo( SGLProgram* p )
{
	GLint num = 0;

	Extension->extGlGetObjectParameteriv(p->program, GL_OBJECT_ACTIVE_UNIFORMS_ARB, &num);

	if (num == 0)
		return true;

	GLint maxlen = 0;

	Extension->extGlGetObjectParameteriv(p->program, GL_OBJECT_ACTIVE_UNIFORM_MAX_LENGTH_ARB, &maxlen);

	if (maxlen == 0)
	{
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "GLSL: failed to retrieve uniform information");
		ASSERT(false);
		return false;
	}

	p->uniforms.resize(num);

	// seems that some implementations use an extra null terminator
	++maxlen;

	p->samplerCount = 0;
	c8 *buf = (c8*)Z_AllocateTempMemory(maxlen);

	for (u32 i=0; i < (u32)num; ++i)
	{
		SGLUniformInfo ui;

		GLenum type;
		GLint length;
		GLint size;
		Extension->extGlGetActiveUniformARB(p->program, i, maxlen, &length, &size, &type, reinterpret_cast<GLcharARB*>(buf));
		buf[length] = '\0';
		string_cs32 name = buf;
		ui.dimension = (u8)size;
		ui.type = type;

		if (type == GL_SAMPLER_1D_ARB || type == GL_SAMPLER_2D_ARB || type == GL_SAMPLER_3D_ARB)
		{
			++p->samplerCount;
		}

		ui.location = Extension->extGlGetUniformLocationARB(p->program, buf);

		//delete "[]" in array names
		string_cs32 out;
		s32 index = name.findNext('[', 0);
		if (index >= 0)
		{
			name.subString(0, index, out);
			name = out;
		}

		p->uniformMap[name] = i;

		p->uniforms[i] = ui;
	}

	Z_FreeTempMemory(buf);

	return true;
}

bool COpenGLShaderServices::checkError( const c8* type )
{
#if defined(GL_ARB_vertex_program) || defined(GL_NV_vertex_program) || defined(GL_ARB_fragment_program) || defined(GL_NV_fragment_program)
	GLenum g = glGetError();
	if (g == GL_NO_ERROR)
		return false;

	GLint errPos=-1;
#if defined(GL_ARB_vertex_program) || defined(GL_ARB_fragment_program)
	glGetIntegerv( GL_PROGRAM_ERROR_POSITION_ARB, &errPos );
#else
	glGetIntegerv( GL_PROGRAM_ERROR_POSITION_NV, &errPos );
#endif

	string256 str;
#if defined(GL_ARB_vertex_program) || defined(GL_ARB_fragment_program)
	str = reinterpret_cast<const char*>(glGetString(GL_PROGRAM_ERROR_STRING_ARB));
#else
	str = reinterpret_cast<const char*>(glGetString(GL_PROGRAM_ERROR_STRING_NV));
#endif

	g_Engine->getFileSystem()->writeLog(ELOG_GX, "%s compilation failed at position %d:\n %s", 
		type, errPos, str.c_str());

#else
	g_Engine->getFileSystem()->writeLog(ELOG_GX, "Shaders not supported.");
#endif

	return true;
}

GLhandleARB COpenGLShaderServices::createHLShader( GLenum shaderType, const char* shader )
{
	GLhandleARB shaderHandle = Extension->extGlCreateShaderObject(shaderType);

	Extension->extGlShaderSourceARB(shaderHandle, 1, &shader, NULL_PTR);
	Extension->extGlCompileShaderARB(shaderHandle);

	GLint status = 0;

	Extension->extGlGetObjectParameteriv(shaderHandle, GL_OBJECT_COMPILE_STATUS_ARB, &status);

	if (!status)
	{
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "GLSL shader failed to compile");
		// check error message and log it
		GLint maxLength=0;
		GLsizei length;

		Extension->extGlGetObjectParameteriv(shaderHandle, GL_OBJECT_INFO_LOG_LENGTH_ARB, &maxLength);

		GLcharARB* infoLog = (GLcharARB*)Z_AllocateTempMemory(sizeof(GLcharARB) * maxLength);
		Extension->extGlGetInfoLog(shaderHandle, maxLength, &length, infoLog);
		g_Engine->getFileSystem()->writeLog(ELOG_GX, reinterpret_cast<const c8*>(infoLog));
		Z_FreeTempMemory(infoLog);

		shaderHandle = 0;
	}

	return shaderHandle;
}

void COpenGLShaderServices::setTextureUniform( s32 location, GLenum type, u32 index )
{
	switch(type)
	{
	case GL_SAMPLER_1D_ARB:
	case GL_SAMPLER_2D_ARB:
	case GL_SAMPLER_3D_ARB:
		{
			GLint i = (GLint)index;
			Extension->extGlUniform1iv(location, 1, &i);
			break;
		}	
	default:
		ASSERT(false);
	}
}



#endif


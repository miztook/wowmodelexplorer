#include "stdafx.h"
#include "CGLES2ShaderServices.h"
#include "mywow.h"
#include "compileconfig.h"

#ifdef MW_COMPILE_WITH_GLES2

#include "CGLES2Driver.h"
#include "CGLES2Extension.h"
#include "CGLES2Shader.h"
#include "CGLES2Helper.h"
#include "CGLES2MaterialRenderServices.h"

CGLES2ShaderServices::CGLES2ShaderServices()
	: CurrentProgram(NULL)
{
	Driver = static_cast<CGLES2Driver*>(g_Engine->getDriver());
	Extension = Driver->getGLExtension();
	Driver->registerLostReset(this);

	LastShaderState.reset();
	ShaderState.reset();
	ResetShaders = true;
}

CGLES2ShaderServices::~CGLES2ShaderServices()
{
	Driver->removeLostReset(this);

	Extension->extGlUseProgram(0);

	for (T_ProgramMap::const_iterator i = ProgramMap.begin(); i != ProgramMap.end(); ++i)
	{
		Extension->extGlDeleteProgram(i->second.program);
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

void CGLES2ShaderServices::onLost()
{

}

void CGLES2ShaderServices::onReset()
{
	ResetShaders = true;
}

void CGLES2ShaderServices::loadAll()
{
	const c8* profile;

	//vs
	g_Engine->getFileSystem()->writeLog(ELOG_GX, "begin loading shaders ...");

	profile = "gles2_vs";

	vsLoader.loadAll(this);

	g_Engine->getFileSystem()->writeLog(ELOG_GX, "finish loading shaders %s.", profile);

	//ps
	g_Engine->getFileSystem()->writeLog(ELOG_GX, "begin loading shaders ...");

	profile = "gles2_ps";

	psLoader.loadAll(this);

	g_Engine->getFileSystem()->writeLog(ELOG_GX, "finish loading shaders %s.", profile);
}

IPixelShader* CGLES2ShaderServices::getPixelShader( E_PS_TYPE type, E_PS_MACRO macro /*= PS_Macro_None*/ )
{
	IPixelShader* ps = PixelShaders[type][macro];
	if (!ps)
	{
		psLoader.loadPShaderHLSL(this, type, macro);
	}
	return PixelShaders[type][macro];
}

bool CGLES2ShaderServices::loadVShader( const c8* filename, E_VS_TYPE type, VSHADERCONSTCALLBACK callback )
{
	ASSERT(false);

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

	return false;
}

bool CGLES2ShaderServices::loadPShader( const c8* filename, E_PS_TYPE type, E_PS_MACRO macro, PSHADERCONSTCALLBACK callback )
{
	ASSERT(false);

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
	return false;
}

bool CGLES2ShaderServices::loadVShaderHLSL( const c8* filename, const c8* entry, const c8* profile, E_VS_TYPE type, VSHADERCONSTCALLBACK callback )
{
	string_path absFileName = g_Engine->getFileSystem()->getShaderBaseDirectory();
	absFileName.append(filename);

	IReadFile* rfile = g_Engine->getFileSystem()->createAndOpenFile(absFileName.c_str(), false);
	if (!rfile)
	{
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "CGLES2ShaderServices::loadVShaderHLSL Failed: Shader missing: %s", absFileName.c_str());
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
	GLuint vertexShader = createHLShader(GL_VERTEX_SHADER, buffer);

	Z_FreeTempMemory(buffer);

	delete rfile;

	if (!vertexShader)
	{
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "CGLES2ShaderServices::loadVShaderHLSL Failed");
		ASSERT(false);
		return false;
	}

	ASSERT(VertexShaders[type]== NULL);
	VertexShaders[type]= new CGLES2VertexShader(type, (GLuint)vertexShader, callback);

	return true;
}

bool CGLES2ShaderServices::loadPShaderHLSL( const c8* filename, const c8* entry, const c8* profile, E_PS_TYPE type, E_PS_MACRO macro, PSHADERCONSTCALLBACK callback )
{
	string_path absFileName = g_Engine->getFileSystem()->getShaderBaseDirectory();
	absFileName.append(filename);

	IReadFile* rfile = g_Engine->getFileSystem()->createAndOpenFile(absFileName.c_str(), false);
	if (!rfile)
	{
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "CGLES2ShaderServices::loadPShaderHLSL Failed: Shader missing: %s", absFileName.c_str());
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
				version = strstr(p, "#version") != NULL;

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
	GLuint pixelShader = createHLShader(GL_FRAGMENT_SHADER, buffer);

	Z_FreeTempMemory(buffer);

	delete rfile;

	if (!pixelShader)
	{
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "CGLES2ShaderServices::loadPShaderHLSL Failed");
		ASSERT(false);
		return false;
	}

	ASSERT(PixelShaders[type][macro] == NULL);
	PixelShaders[type][macro] = new CGLES2PixelShader(type, macro, (GLuint)pixelShader, callback);

	return true;
}

void CGLES2ShaderServices::makeMacroString( string1024& macroString, const c8* strMacro )
{
	macroString.clear();
	u32 len = (u32)strlen(strMacro);
	if (len == 0)
		return;
    
	u32 p = 0;
	for (u32 i=0; i<len; ++i)
	{
		if (i>0 && strMacro[i] == '#')	//split
		{
			if(i - p > 0)
			{
				string256 str(&strMacro[p], i-p);
				p = i+1;
				macroString.append("#define ");
				macroString.append(str.c_str());
				macroString.append("\n");
			}
		}
	}
    
	//last
	if (len - p > 0)
	{
		string256 str(&strMacro[p], len-p);
		macroString.append("#define ");
		macroString.append(str.c_str());
		macroString.append("\n");
	}
	
}

void CGLES2ShaderServices::applyShaders()
{
	if (ResetShaders || LastShaderState.vshader != ShaderState.vshader || LastShaderState.pshader != ShaderState.pshader)
	{
		if (ShaderState.vshader && ShaderState.pshader)
		{
			CurrentProgram = getGlProgram(ShaderState.vshader, ShaderState.pshader);
		}
		else
		{
			ASSERT(false);
			CurrentProgram = NULL;
		}

		if (CurrentProgram)
		{
			Extension->extGlUseProgram(CurrentProgram->program);

			for (u32 i=0; i<CurrentProgram->samplerCount; ++i)
			{
				c8 name[16];
				Q_sprintf(name, 16, "g_TexSampler%u", i);
				setTextureUniform(name, i);
			}
		}
		else
		{
			Extension->extGlUseProgram(0);
		}
	}

	LastShaderState = ShaderState;
	ResetShaders = false;
}

void CGLES2ShaderServices::setShaderConstants( IVertexShader* vs, const SMaterial& material, u32 pass )
{
	ASSERT(vs);
	if (vs && vs->ShaderConstCallback)
		vs->ShaderConstCallback(vs, material, pass);
}

void CGLES2ShaderServices::setShaderConstants( IPixelShader* ps, const SMaterial& material, u32 pass )
{
	ASSERT(ps);
	if (ps && ps->ShaderConstCallback)
		ps->ShaderConstCallback(ps, material, pass);
}

IVertexShader* CGLES2ShaderServices::getDefaultVertexShader( E_VERTEX_TYPE vType ) const
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
		return NULL;
	}
}

IPixelShader* CGLES2ShaderServices::getDefaultPixelShader( E_VERTEX_TYPE vType, E_PS_MACRO macro )
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
		return NULL;
	}
}

SGLProgram* CGLES2ShaderServices::getGlProgram( IVertexShader* vshader, IPixelShader* pshader )
{
	u32 key = makeKey(vshader, pshader);
	T_ProgramMap::iterator itr = ProgramMap.find(key);
	if (itr == ProgramMap.end())
	{
		return createGlProgram(vshader, pshader);
	}
	return &itr->second;
}

u32 CGLES2ShaderServices::makeKey( IVertexShader* vs, IPixelShader* ps )
{
	return ((vs->getType() & 0xff)<<24) | ((ps->getType() & 0xff)<<8) | (ps->getMacro() & 0xff);
}

SGLProgram* CGLES2ShaderServices::createGlProgram( IVertexShader* vshader, IPixelShader* pshader )
{
	CGLES2VertexShader* vs = static_cast<CGLES2VertexShader*>(vshader);
	CGLES2PixelShader* ps = static_cast<CGLES2PixelShader*>(pshader);

	//高级语言的shader才用program
	if (!vs || !ps)
	{
		ASSERT(false);
		return NULL;
	}

	u32 key = makeKey(vs, ps);
	
	ASSERT(ProgramMap.find(key) == ProgramMap.end());

	ProgramMap[key] = SGLProgram();
	SGLProgram* p = &ProgramMap[key];
	p->key = key;
	p->program = Extension->extGlCreateProgram();

	Extension->extGlAttachShader(p->program, vs->getGLVShader());
	Extension->extGlAttachShader(p->program, ps->getGLPShader());

	//link
	if (!linkGlProgram(p))
	{
		removeGlProgram(p);
		ASSERT(false);
		return NULL;
	}

	//get uniform
	Extension->extGlUseProgram(p->program);
	if (!getGlProgramUniformInfo(p))
	{
		removeGlProgram(p);
		ASSERT(false);
		Extension->extGlUseProgram(0);
		return NULL;
	}
	Extension->extGlUseProgram(0);

	return p;
}

void CGLES2ShaderServices::removeGlProgram( SGLProgram* p )
{
	T_ProgramMap::iterator itr = ProgramMap.find(p->key);
	if (itr == ProgramMap.end())
		return;

	Extension->extGlDeleteProgram(p->program);
	ProgramMap.erase(itr);
}

bool CGLES2ShaderServices::linkGlProgram( SGLProgram* p )
{

	//link
	Extension->extGlLinkProgram(p->program);

	GLint status = 0;

	Extension->extGlGetProgramiv(p->program, GL_LINK_STATUS, &status);

	if (!status)
	{
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "GLSL shader program failed to link: ");
		// check error message and log it
		GLint maxLength=0;
		GLsizei length;

		Extension->extGlGetProgramiv(p->program, GL_INFO_LOG_LENGTH, &maxLength);

		GLchar* infoLog = (GLchar*)Z_AllocateTempMemory(sizeof(GLchar) * maxLength);
		Extension->extGlGetProgramInfoLog(p->program, maxLength, &length, infoLog);
		string1024 str(infoLog, 1024);
		Z_FreeTempMemory(infoLog);

		g_Engine->getFileSystem()->writeLog(ELOG_GX, str.c_str());

		ASSERT(false);
		return false;
	}

	return true;
}

bool CGLES2ShaderServices::getGlProgramUniformInfo( SGLProgram* p )
{
	GLint num = 0;

	Extension->extGlGetProgramiv(p->program, GL_ACTIVE_UNIFORMS, &num);

	if (num == 0)
		return true;

	GLint maxlen = 0;

	Extension->extGlGetProgramiv(p->program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxlen);

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
	c8* buf = (c8*)Z_AllocateTempMemory(maxlen);

	for (u32 i=0; i < (u32)num; ++i)
	{
		SGLUniformInfo ui;

		GLenum type;
		GLint length;
		GLint size;
		Extension->extGlGetActiveUniform(p->program, i, maxlen, &length, &size, &type, reinterpret_cast<GLchar*>(buf));
		buf[length] = '\0';
		string_cs32 name = buf;
		ui.dimension = (u8)size;
		ui.type = type;

		if (type == GL_SAMPLER_2D)
		{
			++p->samplerCount;
		}

		ui.location = Extension->extGlGetUniformLocation(p->program, buf);

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

GLuint CGLES2ShaderServices::createHLShader( GLenum shaderType, const char* shader )
{
	
	GLuint shaderHandle = Extension->extGlCreateShader(shaderType);

	Extension->extGlShaderSource(shaderHandle, 1, &shader, NULL);
	Extension->extGlCompileShader(shaderHandle);

	GLint status = 0;

	Extension->extGlGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &status);

	if (!status)
	{
		g_Engine->getFileSystem()->writeLog(ELOG_GX, "GLSL shader failed to compile");
		// check error message and log it
		GLint maxLength=0;
		GLsizei length;

		Extension->extGlGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &maxLength);

		GLchar* infoLog = (GLchar*)Z_AllocateTempMemory(sizeof(GLchar) * (maxLength+1));
		Extension->extGlGetShaderInfoLog(shaderHandle, maxLength, &length, infoLog);
		infoLog[maxLength] = '\0';
		string1024 str(infoLog, 1024);
		Z_FreeTempMemory(infoLog);

		g_Engine->getFileSystem()->writeLog(ELOG_GX, str.c_str());

		ASSERT(false);
		shaderHandle = NULL;
	}
	
	return shaderHandle;
}

void CGLES2ShaderServices::setTextureUniform( u32 location, GLenum type, u32 index )
{
	switch(type)
	{
	case GL_SAMPLER_2D:
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
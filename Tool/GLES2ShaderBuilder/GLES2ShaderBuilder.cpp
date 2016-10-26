#include "mywow.h"
#include "CFileSystem.h"

#include <GLES2/gl2.h>
#include <egl/egl.h>

#pragma comment(lib, "mywow.lib")
#pragma comment(lib, "libEGL.lib")
#pragma comment(lib, "libGLESv2.lib")

bool initGLES2(HWND hwnd, dimension2du windowSize);
void releaseGLES2(HWND hwnd);
void funcShaderVS(const c8* filename, void* args);
void funcShaderPS(const c8* filename, void* args);
void buildShaders( bool vs, bool ps );

GLuint createHLShader( GLenum shaderType, const char* shader );

void makeMacroString( string1024& macroString, const c8* strMacro );

IFileSystem* g_fs = NULL;
EGLNativeWindowType		g_hWnd = NULL;
HDC		g_hDC = NULL;
EGLDisplay		g_EglDisplay = NULL;
EGLSurface		g_EglSurface = NULL;

int nSucceed = 0;
int nFailed = 0;

int main()
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	initGlobal();

	QMem_Init(0, 0, 40);

	g_fs = new CFileSystem("", "", false);

	SWindowInfo wndInfo = Engine::createWindow("GLES2ShaderBuilder", dimension2du(800,600), 1.0f, false, true);
	g_hWnd = wndInfo.hwnd;

	if (!initGLES2(g_hWnd, dimension2du(800,600)))
	{
		goto fail;
	}

	buildShaders(true, true);

fail:

	releaseGLES2(g_hWnd);

	delete g_fs;

	QMem_End();

	deleteGlobal();

	printf("compile completed. %d Succeed, %d Failed\n", nSucceed, nFailed);
	CSysUtility::outputDebug("compile completed. %d Succeed, %d Failed\n", nSucceed, nFailed);
	getchar();
	return 0;
}

bool initGLES2( HWND hwnd, dimension2du windowSize )
{
	_ASSERT(::IsWindow(hwnd));

	g_hDC = ::GetDC(hwnd);

	//step 1
	g_EglDisplay = eglGetDisplay(g_hDC);
	if (g_EglDisplay == EGL_NO_DISPLAY)
		g_EglDisplay = eglGetDisplay((EGLNativeDisplayType)EGL_DEFAULT_DISPLAY);

	//step 2
	GLint major, minor;
	if (!eglInitialize(g_EglDisplay, &major, &minor))
	{
		_ASSERT(false);
		return false;
	}

	//step 3, context
	if(!eglBindAPI(EGL_OPENGL_ES_API))
	{
		_ASSERT(false);
		return false;
	}

	const u8 depthBits = 24;
	const u8 stencilBits = 8;

	//step 4 choose format
	const EGLint pi32ConfigAttribs[] =
	{
		EGL_LEVEL,				0,
		EGL_RENDERABLE_TYPE,	EGL_OPENGL_ES2_BIT,
		EGL_RED_SIZE,			8,
		EGL_GREEN_SIZE,		8,
		EGL_BLUE_SIZE,			8,
		EGL_DEPTH_SIZE,		depthBits,
		EGL_STENCIL_SIZE,	stencilBits,
		EGL_NONE
	};

	int iConfigs;
	EGLConfig config;
	if (!eglChooseConfig(g_EglDisplay, pi32ConfigAttribs, &config, 1, &iConfigs) || (iConfigs != 1))
	{
		_ASSERT(false);
		return false;
	}

	//step 5 create surface
	g_EglSurface = eglCreateWindowSurface(g_EglDisplay, config, hwnd, NULL);
	if (g_EglSurface == EGL_NO_SURFACE)
	{
		_ASSERT(false);
		return false;
	}

	//create context
	EGLContext context;
	EGLint ai32ContextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
	context = eglCreateContext(g_EglDisplay, config, NULL, ai32ContextAttribs);
	if (context == EGL_NO_CONTEXT)
	{
		_ASSERT(false);
		return false;
	}

	//apply context
	if (!eglMakeCurrent(g_EglDisplay, g_EglSurface, g_EglSurface, context))
	{
		_ASSERT(false);
		return false;
	}

	return true;
}

void releaseGLES2( HWND hwnd )
{
	if (g_EglDisplay)
	{
		if(!eglMakeCurrent(g_EglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT))
		{
			_ASSERT(false);
		}

		if (!eglTerminate(g_EglDisplay))
		{
			_ASSERT(false);
		}
	}

	if (g_hDC && g_hWnd)
		::ReleaseDC(g_hWnd, g_hDC);
}

void funcShaderVS( const c8* filename, void* args )
{
	string256 absfilename = filename;

	IReadFile* rfile = g_fs->createAndOpenFile(absfilename.c_str(), false);
	if (!rfile)
	{
		printf("\n");
		printf("cannot open file, compile failed!!! %s\n", absfilename.c_str());
		CSysUtility::outputDebug("cannot open file, compile failed!!! %s\n", absfilename.c_str());

		_ASSERT(false);
		++nFailed;
		return;
	}


	printf("compiling: %s\n", absfilename.c_str());
	CSysUtility::outputDebug("compiling: %s\n", absfilename.c_str());

	c8* buffer = new c8[rfile->getSize()];
	rfile->seek(0);

	c8* p = buffer;
	while(!rfile->isEof())
	{
		u32 size = rfile->readText(p, rfile->getSize());

	//	if(size>1 && !isComment(p))			//skip comment
			p += size;
	}

	_ASSERT(p <= buffer + rfile->getSize());

	//compile
	GLuint vertexShader = createHLShader(GL_VERTEX_SHADER, buffer);

	delete[] buffer;

	if (!vertexShader)
	{
		printf("\n");
		printf("compile failed!!! %s\n ", absfilename.c_str());
		CSysUtility::outputDebug("compile failed!!! %s\n ", absfilename.c_str());
		++nFailed;

		delete rfile;
		return;
	}

	if (vertexShader)
		glDeleteShader(vertexShader);
	
	++nSucceed;

	delete rfile;
}

void funcShaderPS( const c8* filename, void* args )
{
	string256 absfilename = filename;

	IReadFile* rfile = g_fs->createAndOpenFile(absfilename.c_str(), false);
	if (!rfile)
	{
		printf("\n");
		printf("cannot open file, compile failed!!! %s\n", absfilename.c_str());
		CSysUtility::outputDebug("cannot open file, compile failed!!! %s\n", absfilename.c_str());
	
		_ASSERT(false);
		++nFailed;
		return;
	}

	for (u32 i=PS_Macro_None; i<PS_Macro_Num; ++i)
	{
		//macro
		string1024 macroString;
		const c8* strMacro = getPSMacroString((E_PS_MACRO)i);
		makeMacroString(macroString, strMacro);

		printf("compiling: %s, macro: %s\n", absfilename.c_str(), strMacro);
		CSysUtility::outputDebug("compiling: %s, macro: %s\n", absfilename.c_str(), strMacro);

		u32 macroLength = macroString.length();
		c8* buffer = new c8[macroLength + rfile->getSize()];
		rfile->seek(0);

		c8* p = buffer;

		bool version = false;
		bool macroAdd = false;
		while(!rfile->isEof())
		{
			u32 size = rfile->readText(p, rfile->getSize());

			if(size>1 /*&& !isComment(p)*/)			//skip comment
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

		_ASSERT(p <= buffer + macroLength + rfile->getSize());

		//compile
		GLuint pixelShader = createHLShader(GL_FRAGMENT_SHADER, buffer);

		delete[] buffer;

		if (!pixelShader)
		{
			CSysUtility::outputDebug("compile failed!!! %s", absfilename.c_str());
			++nFailed;

	//		_ASSERT(false);
			return;
		}

		if (pixelShader)
			glDeleteShader(pixelShader);

		++nSucceed;
	}
		
	delete rfile;
}

GLuint createHLShader( GLenum shaderType, const char* shader )
{
	GLuint shaderHandle = glCreateShader(shaderType);

	glShaderSource(shaderHandle, 1, &shader, NULL);
	glCompileShader(shaderHandle);

	GLint status = 0;

	glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &status);

	if (!status)
	{
		// check error message and log it
		GLint maxLength=0;
		GLsizei length;

		glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &maxLength);

		GLchar* infoLog = new GLchar[maxLength + 1];
		glGetShaderInfoLog(shaderHandle, maxLength, &length, infoLog);
		infoLog[maxLength] = '\0';
		printf("%s\n", reinterpret_cast<const c8*>(infoLog));
		CSysUtility::outputDebug("%s", reinterpret_cast<const c8*>(infoLog));
		delete[] infoLog;

		shaderHandle = NULL;

		_ASSERT(false);			//compile fail
	}

	return shaderHandle;
}

void buildShaders( bool vs, bool ps )
{
	const c8* basedir = g_fs->getShaderBaseDirectory();

	printf("compiling shaders.................................................................\n\n");

	if(vs)
	{
		string256 dirname = basedir;
		dirname.append("Vertex\\gles2_vs");
		Q_iterateFiles(dirname.c_str(), "*.fx", funcShaderVS, (void*)dirname.c_str());
	}

	if(ps)
	{
		string256 dirname = basedir;
		dirname.append("Pixel\\gles2_ps");
		Q_iterateFiles(dirname.c_str(), "*.fx", funcShaderPS, (void*)dirname.c_str());
	}

	if(ps)
	{
		string256 dirname = basedir;
		dirname.append("Pixel\\gles2_ps_etc1");
		Q_iterateFiles(dirname.c_str(), "*.fx", funcShaderPS, (void*)dirname.c_str());
	}

	printf("\n\n");
}

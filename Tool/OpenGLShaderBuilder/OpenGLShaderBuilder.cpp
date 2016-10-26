#include "mywow.h"
#include "CFileSystem.h"

#include <gl/GL.h>
#include <gl/glext.h>

#include "COpenGLExtension.h"

#pragma comment(lib, "mywow.lib")

bool initOpenGL(HWND hwnd, dimension2du windowSize);
void releaseOpenGL(HWND hwnd);
void funcShaderVS(const c8* filename, void* args);
void funcShaderPS(const c8* filename, void* args);
void buildShaders_12(bool vs, bool ps);
void buildShaders_15(bool vs, bool ps);

GLhandleARB createHLShader(GLenum shaderType, const char* shader);

IFileSystem* g_fs = NULL;
HWND g_hWnd = NULL;
HDC	g_hDC = NULL;
HGLRC	g_hGrc = NULL;
COpenGLExtension* g_GLExtension = NULL;

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

	SWindowInfo wndInfo = Engine::createWindow("OpenGLShaderBuilder", dimension2du(800,600), 1.0f, false, true);
	g_hWnd = wndInfo.hwnd;

	if (!initOpenGL(g_hWnd, dimension2du(800,600)))
	{
		goto fail;
	}

	buildShaders_12(true, true);

	buildShaders_15(true, true);

fail:

	releaseOpenGL(g_hWnd);

	delete g_fs;

	QMem_End();

	deleteGlobal();

	printf("compile completed. %d Succeed, %d Failed\n", nSucceed, nFailed);
	getchar();
	return 0;
}

bool initOpenGL(HWND hwnd, dimension2du windowSize)
{
	g_hDC = ::GetDC(hwnd);

	const u8 depthBits = 24;
	const u8 stencilBits = 8;

	// Set up pixel format descriptor with desired parameters
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),             // Size Of This Pixel Format Descriptor
		1,                                         // Version Number
		PFD_DRAW_TO_WINDOW |                       // Format Must Support Window
		PFD_SUPPORT_OPENGL |                       // Format Must Support OpenGL
		PFD_DOUBLEBUFFER | // Must Support Double Buffering
		//	PFD_STEREO,        // Must Support Stereo Buffer
		PFD_TYPE_RGBA,                             // Request An RGBA Format
		32,                               // Select Our Color Depth
		8, 0, 8, 0, 8, 0,                          // Color Bits Ignored
		0,                                         // No Alpha Buffer
		0,                                         // Shift Bit Ignored
		0,                                         // No Accumulation Buffer
		0, 0, 0, 0,	                               // Accumulation Bits Ignored
		depthBits,                        // Z-Buffer (Depth Buffer)
		stencilBits,              // Stencil Buffer Depth
		0,                                         // No Auxiliary Buffer
		PFD_MAIN_PLANE,                            // Main Drawing Layer
		0,                                         // Reserved
		0, 0, 0                                    // Layer Masks Ignored
	};

	//choose pixel format
	int pixelFormat;

	//depth stencil
	pixelFormat = ChoosePixelFormat(g_hDC, &pfd);
	if (!pixelFormat)
	{
		pfd.cDepthBits = 15;
		pfd.cStencilBits = 1;

		pixelFormat = ChoosePixelFormat(g_hDC, &pfd);
	}

	if (!pixelFormat)
	{
		_ASSERT(false);
		return false;
	}

	if (!SetPixelFormat(g_hDC, pixelFormat, &pfd))
	{
		_ASSERT(false);
		return false;
	}

	g_hGrc = wglCreateContext(g_hDC);
	if (!g_hGrc)
	{
		::ReleaseDC(hwnd, g_hDC);

		_ASSERT(false);
		return false;
	}

	if (!wglMakeCurrent(g_hDC, g_hGrc))
	{
		wglDeleteContext(g_hGrc);
		::ReleaseDC(hwnd, g_hDC);

		return false;
	}

	g_GLExtension = new COpenGLExtension;
	if (!g_GLExtension->initExtensions())
	{
		MessageBoxA(NULL, "Video Card Extension does not support some features. Application will now exit!", "Warnning", MB_ICONEXCLAMATION);
		return false;
	}

	return true;
}

void releaseOpenGL( HWND hwnd )
{
	delete g_GLExtension;

	if (!wglMakeCurrent(NULL, NULL))
	{
		_ASSERT(false);
	}

	if (g_hGrc && !wglDeleteContext(g_hGrc))
	{
		_ASSERT(false);
	}

	if (g_hDC && hwnd)
		::ReleaseDC(hwnd, g_hDC);
}


void funcShaderVS( const c8* filename, void* args )
{
	string256 absfilename = filename;

	IReadFile* rfile = g_fs->createAndOpenFile(absfilename.c_str(), false);
	if (!rfile)
	{
		printf("\n");
		printf("cannot open file, compile failed!!! %s\n", absfilename.c_str());
		++nFailed;
		return;
	}

	//macro
	printf("compiling: %s\n", absfilename.c_str());

	c8* buffer = (c8*)Z_AllocateTempMemory(rfile->getSize());
	rfile->seek(0);

	c8* p = buffer;
	while(!rfile->isEof())
	{
		u32 size = rfile->readText(p, rfile->getSize());

		//if(!isComment(p))			//skip comment
		p += size;
	}

	_ASSERT(p <= buffer + rfile->getSize());

	//compile
	GLhandleARB vertexShader = createHLShader(GL_VERTEX_SHADER_ARB, buffer);

	Z_FreeTempMemory(buffer);

	if (!vertexShader)
	{
		printf("\n");
		printf("compile failed!!! %s\n ", absfilename.c_str());
		++nFailed;

		delete rfile;
		return;
	}

	if (vertexShader)
		g_GLExtension->extGlDeleteObject((GLhandleARB)vertexShader);

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

		u32 macroLength = macroString.length();
		c8* buffer = (c8*)Z_AllocateTempMemory(macroLength + rfile->getSize());
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
		GLhandleARB pixelShader = createHLShader(GL_FRAGMENT_SHADER_ARB, buffer);

		Z_FreeTempMemory(buffer);

		if (!pixelShader)
		{
			printf("\n");
			printf("compile failed!!! %s, macro: %s\n ", absfilename.c_str(), strMacro);
			++nFailed;

			break;
		}

		if (pixelShader)
			g_GLExtension->extGlDeleteObject((GLhandleARB)pixelShader);

		++nSucceed;
	}

	delete rfile;
}

GLhandleARB createHLShader( GLenum shaderType, const char* shader )
{
	GLhandleARB shaderHandle = g_GLExtension->extGlCreateShaderObject(shaderType);

	g_GLExtension->extGlShaderSourceARB(shaderHandle, 1, &shader, NULL);
	g_GLExtension->extGlCompileShaderARB(shaderHandle);

	GLint status = 0;

	g_GLExtension->extGlGetObjectParameteriv(shaderHandle, GL_OBJECT_COMPILE_STATUS_ARB, &status);

	if (!status)
	{
		// check error message and log it
		GLint maxLength=0;
		GLsizei length;
		g_GLExtension->extGlGetObjectParameteriv(shaderHandle,
			GL_OBJECT_INFO_LOG_LENGTH_ARB, &maxLength);
		GLcharARB* infoLog = (GLcharARB*)Z_AllocateTempMemory(sizeof(GLcharARB) * maxLength);
		g_GLExtension->extGlGetInfoLog(shaderHandle, maxLength, &length, infoLog);
		printf("%s\n", reinterpret_cast<const c8*>(infoLog));
		Z_FreeTempMemory(infoLog);

		shaderHandle = NULL;
	}

	return shaderHandle;
}

void buildShaders_12( bool vs, bool ps )
{
	const c8* basedir = g_fs->getShaderBaseDirectory();

	printf("1_2.................................................................\n\n");

	if(vs)
	{
		string256 dirname = basedir;
		dirname.append("Vertex/glvs_1_2");
		Q_iterateFiles(dirname.c_str(), "*.fx", funcShaderVS, (void*)dirname.c_str());
	}

	if(ps)
	{
		string256 dirname = basedir;
		dirname.append("Pixel/glps_1_2");
		Q_iterateFiles(dirname.c_str(), "*.fx", funcShaderPS, (void*)dirname.c_str());
	}

	printf("\n\n");
}

void buildShaders_15( bool vs, bool ps )
{
	const c8* basedir = g_fs->getShaderBaseDirectory();

	printf("1_5.................................................................\n\n");

	if(vs)
	{
		string256 dirname = basedir;
		dirname.append("Vertex/glvs_1_5");
		Q_iterateFiles(dirname.c_str(), "*.fx", funcShaderVS, (void*)dirname.c_str());
	}

	if(ps)
	{
		string256 dirname = basedir;
		dirname.append("Pixel/glps_1_5");
		Q_iterateFiles(dirname.c_str(), "*.fx", funcShaderPS, (void*)dirname.c_str());
	}

	printf("\n\n");
}

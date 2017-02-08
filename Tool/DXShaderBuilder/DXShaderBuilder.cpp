#include "DXShaderBuilder.h"
#include "mywow.h"
#include "CFileSystem.h"
#include "DX9ShaderBuild.h"
#include "DX11ShaderBuild.h"

#pragma comment(lib, "mywow.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dcompiler.lib")

IFileSystem* g_fs = NULL;
string512 g_BaseDir = "";

int nSucceed = 0;
int nFailed = 0;

int main(int argc, const char* argvs[])
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif
	
	initGlobal();
	
	QMem_Init(0, 0, 40);

	g_fs = new CFileSystem("", "", false);

	if (argc == 2)
	{
		char tmp[512];
		Q_fullpath(argvs[1], tmp, 512);
		g_BaseDir = tmp;
		g_BaseDir.normalizeDir();
	}
	else
	{
		g_BaseDir = g_fs->getShaderBaseDirectory();
	}

	printf("BaseShaderDir: %s\n", g_BaseDir.c_str());

	buildShaders_20(true, true);

	buildShaders_30(true, true);

	buildShaders_40(true, true, true);

	buildShaders_50(true, true, true, true, true);

	delete g_fs;

	QMem_End();

	deleteGlobal();

	printf("compile completed. %d Succeed, %d Failed\n", nSucceed, nFailed);
	//getchar();
	return 0;
}


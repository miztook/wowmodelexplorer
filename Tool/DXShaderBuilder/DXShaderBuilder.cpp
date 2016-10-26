#include "DXShaderBuilder.h"
#include "mywow.h"
#include "CFileSystem.h"
#include "DX9ShaderBuild.h"
#include "DX11ShaderBuild.h"

#pragma comment(lib, "mywow.lib")

IFileSystem* g_fs = NULL;

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

	buildShaders_20(true, true);

	buildShaders_30(true, true);

	buildShaders_40(true, true, true);

	buildShaders_50(true, true, true, true, true);

	delete g_fs;

	QMem_End();

	deleteGlobal();

	printf("compile completed. %d Succeed, %d Failed\n", nSucceed, nFailed);
	getchar();
	return 0;
}


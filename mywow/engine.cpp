#include "StdAfx.h"
#include "mywow.h"
#include "CFileSystem.h"

//dx9
#include "CD3D9Driver.h"
#include "CD3D9HardwareBufferServices.h"
#include "CD3D9DrawServices.h"
#include "CD3D9ResourceLoader.h"
#include "CD3D9ManualTextureServices.h"

#include "CFTFont.h"
#include "CGeometryCreator.h"
#include "CManualMeshServices.h"
#include "CParticleSystemServices.h"
#include "CRibbonEmitterServices.h"
#include "CSceneRenderServices.h"
#include "CSceneManager.h"

Engine*		g_Engine = NULL;

Engine::Engine()
{
	CurrentThreadId = ::GetCurrentThreadId();

#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	_CrtMemCheckpoint(&oldState);

	QMem_Init(8, 30, 30);		//30m

	FileSystem = new CFileSystem;
	WowEnvironment = new wowEnvironment(FileSystem, ELI_ZH_CN);
	WowDatabase = new wowDatabase(WowEnvironment);

	Driver = NULL;
	HardwareBufferServices = NULL;
	DrawServices = NULL;
	Font = NULL;
	ResourceLoader = NULL;
	GeometryCreator = NULL;
	ManualMeshServices = NULL;
	ManualTextureServices = NULL;
	ParticleSystemServices = NULL;
	RibbonEmitterServices = NULL;

	SceneRenderServices = NULL;
	SceneManager = NULL;
}

Engine::~Engine()
{
	if(ResourceLoader)
		ResourceLoader->endLoadingM2();

	delete SceneManager;
	delete SceneRenderServices;

	delete RibbonEmitterServices;
	delete ParticleSystemServices;
	delete ManualMeshServices;
	delete GeometryCreator;
	delete Font;

	delete ManualTextureServices;
	delete ResourceLoader;
	delete DrawServices;
	delete HardwareBufferServices;
	delete Driver;

	delete WowDatabase;
	delete WowEnvironment;
	delete FileSystem;
	QMem_End();

	_CrtMemCheckpoint(&newState);

	s32 diff = _CrtMemDifference(&diffState, &oldState, &newState);
	_ASSERT(diff == 0);

	/*
	int low, high;
	QMem_TempInfo(low, high);
	char msg[128];
	sprintf_s(msg, 128, "最大临时内存使用: %d", high);
	MessageBoxA(NULL, msg, "", 0);
	*/
}

HWND Engine::createWindow( const char* caption, WNDPROC proc, dimension2du &windowSize, bool fullscreen, bool hide )
{
	if (fullscreen)
		hide = false;

	HINSTANCE hInstance = ::GetModuleHandle(NULL);
	HWND hwnd;

	const char* className = "mywowWindow";

	WNDCLASSEX wcex;
	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= proc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= ::LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)COLOR_BACKGROUND;
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= className;
	wcex.hIconSm		= 0;
	wcex.hIcon			= ::LoadIcon(NULL, IDI_APPLICATION);

	::RegisterClassEx( &wcex );

	RECT clientSize;
	clientSize.top = 0;
	clientSize.left = 0;
	clientSize.right = windowSize.Width;
	clientSize.bottom = windowSize.Height;

	DWORD style = WS_POPUP;

	if (!fullscreen && !hide)
		style = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

	if (hide)
	{
		style &= (~WS_VISIBLE);
	}

	AdjustWindowRect(&clientSize, style, FALSE);

	s32 width = clientSize.right - clientSize.left;
	s32 height = clientSize.bottom - clientSize.top;

	s32 windowLeft = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
	s32 windowTop = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

	if ( windowLeft < 0 )
		windowLeft = 0;
	if ( windowTop < 0 )
		windowTop = 0;	// make sure window menus are in screen on creation

	hwnd = ::CreateWindow( className, caption, style, windowLeft, windowTop,
		width, height, NULL, NULL, hInstance, NULL);

	::ShowWindow(hwnd, hide? SW_HIDE : SW_SHOW);
	::UpdateWindow(hwnd);

	::MoveWindow(hwnd, windowLeft, windowTop, width, height, TRUE);

	if (!hide)
	{
		::SetActiveWindow(hwnd);
		::SetForegroundWindow(hwnd);
	}

	return hwnd;
}

bool Engine::initDriver( HWND hwnd, E_DRIVER_TYPE driverType, bool fullscreen, bool vsync, bool multithread, u32 bits, u8 antialias, E_CSAA csaa )
{
	if (bits != 32 && bits != 16)
	{
		bits = 32;
	}

	switch (driverType)
	{
	case EDT_DIRECT3D9:
		Driver = new CD3D9Driver;
		if(!static_cast<CD3D9Driver*>(Driver)->initDriver(hwnd, fullscreen, vsync, multithread, bits, antialias, csaa))
			return false;
 		HardwareBufferServices = new CD3D9HardwareBufferServices;
 		DrawServices = new CD3D9DrawServices;
 		ResourceLoader = new CD3D9ResourceLoader(multithread);
 		ManualTextureServices = new CD3D9ManualTextureServices;
		break;
	default:
		_ASSERT(false);
	}

	Font = new CFTFont("ARIALUNI.ttf", 10);
 	GeometryCreator = new CGeometryCreator;
 	ManualMeshServices = new CManualMeshServices;
 	ParticleSystemServices = new CParticleSystemServices(2000, 1000, 0.5f);
 	RibbonEmitterServices = new CRibbonEmitterServices(2000, 1000);
 
 	SceneRenderServices = new CSceneRenderServices;

	return true;
}

void Engine::initSceneManager()
{
	SceneManager = new CSceneManager();
}

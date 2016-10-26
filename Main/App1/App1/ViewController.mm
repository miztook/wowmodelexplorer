//
//  ViewController.m
//  App1
//
//  Created by miao yu on 6/4/14.
//  Copyright (c) 2014 miao yu. All rights reserved.
//

#import "ViewController.h"

#include "mywow.h"

EAGLContext* g_GLKViewContext = nil;

@interface ViewController () {

}
@property (strong, nonatomic) EAGLContext *context;
@property (strong, nonatomic) GLKBaseEffect *effect;

- (void)setupGL;
- (void)tearDownGL;
- (void)createScene;
- (void)destroyScene;

@end

@implementation ViewController

- (BOOL)shouldAutorotate
{
    return TRUE;
}

- (NSUInteger)supportedInterfaceOrientations
{
    // return UIInterfaceOrientationMaskAll;
    return UIInterfaceOrientationMaskLandscapeRight | UIInterfaceOrientationMaskLandscapeLeft;
}

- (void)willRotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation duration:(NSTimeInterval)duration
{
    
}

- (void)willAnimateRotationToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation duration:(NSTimeInterval)duration
{
    CGFloat scale = [UIScreen mainScreen].scale;
    CGRect clientRect = [UIScreen mainScreen].applicationFrame;
    
    int width = clientRect.size.width * scale;
    int height = clientRect.size.height * scale;
    
    u32 wndWidth;
    u32 wndHeight;
    if (self.interfaceOrientation == UIInterfaceOrientationPortrait || self.interfaceOrientation == UIInterfaceOrientationPortraitUpsideDown)
    {
        wndWidth = (u32)width;
        wndHeight = (u32)height;
    }
    else
    {
        wndWidth = (u32)height;
        wndHeight = (u32)width;
    }
    
    dimension2du dim(wndWidth, wndHeight);
    
    _ASSERT(g_Engine && g_Engine->getDriver() && g_Engine->getSceneManager());
    
    IVideoDriver* driver = g_Engine->getDriver();
    if (driver)
    {
        driver->setDisplayMode(dim);
        
        ISceneManager* smgr = g_Engine->getSceneManager();
        if(smgr)
        {
            smgr->onWindowSizeChanged(dim);
            
            ICamera* cam = smgr->getActiveCamera();
			if(cam)
				cam->recalculateAll();
        }
    }
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];

    if (!self.context) {
        NSLog(@"Failed to create ES context");
    }
    
    GLKView *view = (GLKView *)self.view;
    view.context = self.context;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    
    [view bindDrawable];        //bind default fbo
    
    g_GLKViewContext = self.context;
    
    [self setupGL];
}

- (void)dealloc
{    
    [self tearDownGL];
    
    if ([EAGLContext currentContext] == self.context) {
        [EAGLContext setCurrentContext:nil];
    }
    
    [super dealloc];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];

    if ([self isViewLoaded] && ([[self view] window] == nil)) {
        self.view = nil;
        
        [self tearDownGL];
        
        if ([EAGLContext currentContext] == self.context) {
            [EAGLContext setCurrentContext:nil];
        }
        self.context = nil;
    }

    // Dispose of any resources that can be recreated.
}

- (void)setupGL
{
    [EAGLContext setCurrentContext:self.context];
    
    
    NSBundle* myBundle = [NSBundle mainBundle];
    NSString* path = [myBundle bundlePath];
    NSString* bundleDir = @"/iossource.bundle";
    NSString* bundleDirectory = [path stringByAppendingString:bundleDir];
    
    NSString* homePath = NSHomeDirectory();
    NSString* docDir = @"/Documents";
    NSString* docDirectory = [homePath stringByAppendingString:docDir];
    
    CGFloat scale = [UIScreen mainScreen].scale;
    CGRect clientRect = [UIScreen mainScreen].applicationFrame;
    
    int width = clientRect.size.width * scale;
    int height = clientRect.size.height * scale;
    
    u32 wndWidth;
    u32 wndHeight;
    if (self.interfaceOrientation == UIInterfaceOrientationPortrait || self.interfaceOrientation == UIInterfaceOrientationPortraitUpsideDown)
    {
        wndWidth = (u32)width;
        wndHeight = (u32)height;
    }
    else
    {
        wndWidth = (u32)height;
        wndHeight = (u32)width;
    }
    
    SEngineInitParam param;
    strcpy(param.baseDir, [bundleDirectory UTF8String]);
    strcpy(param.logDir, [docDirectory UTF8String]);
    param.osversion = [[[UIDevice currentDevice]systemVersion]floatValue];
    
    SWindowInfo wndInfo = Engine::createWindow("App1", dimension2du(wndWidth, wndHeight));
    
    createEngine(param, wndInfo);
    
    bool success = g_Engine->initDriver(EDT_GLES2, 0, false, true, 1, true);
    
    _ASSERT(success);
    
    g_Engine->initSceneManager();
    
    ISceneManager* sceneMgr = g_Engine->getSceneManager();
    sceneMgr->addCamera(vector3df(0, 3, -5), vector3df(0,1,0), vector3df(0,1,0), 1.0f, 2500.0f, PI/4.0f);
    
    [self createScene];
}

- (void)tearDownGL
{
    [EAGLContext setCurrentContext:self.context];

    [self destroyScene];
    
    destroyEngine();
}

- (void)createScene
{
    g_Engine->getManualMeshServices()->addGridLineMesh("$grid20", 20, 1,SColor(128,128,128) );
    
    IMeshSceneNode* gridNode = g_Engine->getSceneManager()->addMeshSceneNode("$grid20", NULL);
    
    IFileM2* m2Human = g_Engine->getResourceLoader()->loadM2("Character\\HUMAN\\MALE\\HumanMale.m2");
	IM2SceneNode*  humanNode = g_Engine->getSceneManager()->addM2SceneNode(m2Human, NULL);
	humanNode->getM2Appearance()->loadSet(1189);
	humanNode->updateCharacter();
	humanNode->getM2Move()->setDir(-vector3df::UnitZ());
	humanNode->playAnimationByName("Stand", 0, true);
}

- (void)destroyScene
{
    g_Engine->getSceneManager()->removeAllSceneNodes();
}

#pragma mark - GLKView and GLKViewController delegate methods

- (void)update
{
    
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    _ASSERT(g_Engine && g_Engine->getSceneManager());
    
    ISceneManager* smgr = g_Engine->getSceneManager();
    if (smgr->beginFrame(true))
    {
        smgr->drawAll(true);
        smgr->endFrame();
    }
}

@end

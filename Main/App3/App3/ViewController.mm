//
//  ViewController.m
//  App1
//
//  Created by miao yu on 6/4/14.
//  Copyright (c) 2014 miao yu. All rights reserved.
//

#import "ViewController.h"

#include "mywow.h"
#include "mywowclient.h"
#include "game.h"

EAGLContext* g_GLKViewContext = nil;

@interface ViewController () {
    MyMessageHandler    m_messageHandler;
}
@property (strong, nonatomic) EAGLContext *context;
@property (strong, nonatomic) GLKBaseEffect *effect;

- (CGRect)fixedNativeBounds;
- (void)setupGL;
- (void)tearDownGL;

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
    if (!g_Engine) return;
    IMessageHandler* handler = g_Engine->getMessageHandler();
    if (!handler)  return;
    
    CGRect clientRect = [self fixedNativeBounds];
    
    int wndWidth = (int)clientRect.size.width;
    int wndHeight = (int)clientRect.size.height;
    
    handler->onSize(g_Engine->getWindowInfo().hwnd, wndWidth, wndHeight);
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

- (CGRect)fixedNativeBounds
{
    BOOL OSIsBelowIOS8 = [[[UIDevice currentDevice] systemVersion] floatValue] < 8.0;
    
    BOOL IsPortrait = self.interfaceOrientation == UIInterfaceOrientationPortrait || self.interfaceOrientation == UIInterfaceOrientationPortraitUpsideDown;
    
    
    if (OSIsBelowIOS8)
    {
        CGRect clientRect = [UIScreen mainScreen].bounds;
        CGFloat scale = [UIScreen mainScreen].scale;
        CGFloat width = IsPortrait ? clientRect.size.width * scale : clientRect.size.height * scale;
        CGFloat height = IsPortrait ? clientRect.size.height * scale : clientRect.size.width * scale;
        
        return CGRectMake(clientRect.origin.x, clientRect.origin.y, width, height);
    }
    else
    {
        CGRect clientRect = [UIScreen mainScreen].nativeBounds;
		CGFloat nativeScale = [UIScreen mainScreen].nativeScale;
		CGFloat scale = [UIScreen mainScreen].scale;
		
        CGFloat width = IsPortrait ? clientRect.size.width / nativeScale * scale : clientRect.size.height / nativeScale * scale;
        CGFloat height = IsPortrait ? clientRect.size.height / nativeScale * scale : clientRect.size.width / nativeScale * scale;
        
        return CGRectMake(clientRect.origin.x, clientRect.origin.y, width, height);
    }
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
    CGRect clientRect = [self fixedNativeBounds];
    
    int wndWidth = clientRect.size.width;
    int wndHeight = clientRect.size.height;
    
    SEngineInitParam param;
    strcpy(param.baseDir, [bundleDirectory UTF8String]);
    strcpy(param.logDir, [docDirectory UTF8String]);
    param.osversion = [[[UIDevice currentDevice]systemVersion]floatValue];
    
    SWindowInfo wndInfo = Engine::createWindow("App1", dimension2du(wndWidth, wndHeight), scale);
    
    createEngine(param, wndInfo);
    
    bool success = g_Engine->initDriver(EDT_GLES2, 0, false, true, 1, true);
    
    _ASSERT(success);
    
    g_Engine->setMessageHandler(&m_messageHandler);
    
    g_Engine->initSceneManager();
    
    createClient();
    
    ISceneManager* sceneMgr = g_Engine->getSceneManager();
    sceneMgr->addCamera(vector3df(0, 3, -5), vector3df(0,1,0), vector3df(0,1,0), 1.0f, 2500.0f, PI/4.0f);
    
    createScene();
    
    createInput();
}

- (void)tearDownGL
{
    [EAGLContext setCurrentContext:self.context];
    
    destroyInput();
    
    destroyScene();
    
    destroyClient();
    
    destroyEngine();
}


#pragma mark - GLKView and GLKViewController delegate methods

- (void)update
{
    processInput();
    
    idleTick();
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

#import "UncaughtExceptionHandler.h"
#include <libkern/OSAtomic.h>
#include <execinfo.h>

NSString * const UncaughtExceptionHandlerSignalExceptionName = @"UncaughtExceptionHandlerSignalExceptionName";

NSString * const UncaughtExceptionHandlerSignalKey = @"UncaughtExceptionHandlerSignalKey";

volatile int32_t UncaughtExceptionCount = 0;

const int32_t UncaughtExceptionMaximum = 10;

const NSInteger UncaughtExceptionHandlerSkipAddressCount = 4;

const NSInteger UncaughtExceptionHandlerReportAddressCount = 5;

@implementation UncaughtExceptionHandler

+ (NSArray *)backtrace
{
    void* callstack[128];
    
    int frames = backtrace(callstack, 128);
    
    char **strs = backtrace_symbols(callstack, frames);
    
    int i;
    
    NSMutableArray *_backtrace = [NSMutableArray arrayWithCapacity:frames];
    
    for ( i = UncaughtExceptionHandlerSkipAddressCount; i < UncaughtExceptionHandlerSkipAddressCount + UncaughtExceptionHandlerReportAddressCount; ++i )
    {
        [_backtrace addObject:[NSString stringWithUTF8String:strs[i]]];
    }
    
    free(strs);
    
    return _backtrace;
}

- (void)alertView : (UIAlertView *)anAlertView clickedButtonAtIndex : (NSInteger)anIndex
{
    if (anIndex == 0)
    {
        dismissed = YES; 
    } 
}

- (void)handleException : (NSException *)exception
{
    NSMutableString *string = [[NSMutableString alloc] init];
    NSArray *callStack = [UncaughtExceptionHandler backtrace];
    int i=0;
    for (NSString *obj in callStack)
    {
        if(i>4)
            break;
        [string appendString: obj];
        [string appendString:@"\n"];
        
        ++i;
    }
    
    UIAlertView *alert = [[UIAlertView alloc] initWithTitle : NSLocalizedString(@"Unhandled exception", nil)
      message:[NSString stringWithFormat:NSLocalizedString(@"%@\n%@", nil),          
               [exception reason],   
               string]
      delegate:self
      cancelButtonTitle : NSLocalizedString(@"Quit", nil)
      otherButtonTitles : nil];
    
    [alert show];
    
    CFRunLoopRef runLoop = CFRunLoopGetCurrent();
    
    CFArrayRef allModes = CFRunLoopCopyAllModes(runLoop);
    
    while (!dismissed) 
    {      
        for (NSString *mode in (__bridge NSArray *)allModes)     
        { 
            CFRunLoopRunInMode((CFStringRef)mode, 0.001, false);
        }  
    }
    
    CFRelease(allModes);
    
    NSSetUncaughtExceptionHandler(NULL);
    
    signal(SIGABRT, SIG_DFL);
    
    signal(SIGILL, SIG_DFL);
    
    signal(SIGSEGV, SIG_DFL);
    
    signal(SIGFPE, SIG_DFL);
    
    signal(SIGBUS, SIG_DFL);
    
    signal(SIGPIPE, SIG_DFL);
    
    signal(SIGBUS, SIG_DFL);
    
    signal(SIGTRAP, SIG_DFL);
    
    signal(SIGTERM, SIG_DFL);
    
    signal(SIGKILL, SIG_DFL);
    
    if ([[exception name] isEqual:UncaughtExceptionHandlerSignalExceptionName])
    {
        kill(getpid(), [[[exception userInfo] objectForKey:UncaughtExceptionHandlerSignalKey] intValue]); 
    }
    else   
    {
        [exception raise];
    }
}

@end

NSString* getAppInfo()
{
    NSString *appInfo = [NSString stringWithFormat:@"App : %@ %@(%@)\nDevice : %@\nOS Version : %@ %@\n",
                         [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleDisplayName"],         
                         [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleShortVersionString"],     
                         [[NSBundle mainBundle] objectForInfoDictionaryKey:@"CFBundleVersion"],            
                         [UIDevice currentDevice].model,           
                         [UIDevice currentDevice].systemName,          
                         [UIDevice currentDevice].systemVersion];
    
    NSLog(@"Crash!!!! %@", appInfo);
    return appInfo;
}

void MySignalHandler(int signal)
{ 
    NSArray *callStack = [UncaughtExceptionHandler backtrace];
    int32_t exceptionCount = OSAtomicIncrement32(&UncaughtExceptionCount);
    if (exceptionCount > UncaughtExceptionMaximum)  
    {
        return;  
    }
   
    [[[UncaughtExceptionHandler alloc] init]
     
     performSelectorOnMainThread:@selector(handleException:)
     
     withObject:   
     [NSException exceptionWithName : UncaughtExceptionHandlerSignalExceptionName
      
     reason:   
     [NSString stringWithFormat : NSLocalizedString(@"Signal %d was raised.\n" @"%@", nil), signal, getAppInfo()]
      
     userInfo:     
     [NSDictionary dictionaryWithObject : [NSNumber numberWithInt:signal] forKey:UncaughtExceptionHandlerSignalKey]]
     
     waitUntilDone : YES];
}

void InstallUncaughtExceptionHandler()
{
    signal(SIGABRT, MySignalHandler);
    
    signal(SIGILL, MySignalHandler);
    
    signal(SIGSEGV, MySignalHandler);
    
    signal(SIGFPE, MySignalHandler);
    
    signal(SIGBUS, MySignalHandler);
    
    signal(SIGPIPE, MySignalHandler);
    
    signal(SIGBUS, MySignalHandler);
    
    signal(SIGTRAP, MySignalHandler);
    
    signal(SIGTERM, MySignalHandler);
    
    signal(SIGKILL, MySignalHandler);
}

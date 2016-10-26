#ifndef _UncaughtExceptionHandler_h
#define _UncaughtExceptionHandler_h

#import <UIKit/UIKit.h>

@interface UncaughtExceptionHandler : NSObject
{
    BOOL dismissed;
}

+ (NSArray *)backtrace;

@end


void InstallUncaughtExceptionHandler();

#endif

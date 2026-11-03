#define GL_SILENCE_DEPRECATION

#include <stdlib.h>

#import <Foundation/Foundation.h>
#import <OpenGL/gl3.h>
#import <Cocoa/Cocoa.h>

#include "include/logger.h"
#include "include/mac_window.h"

/* global variables */
static int g_macWindowShouldClose = 0;

/* declarations */
@interface MachaApplication:NSApplication
- (void)run;
@end

@protocol MachaWindowProtocol <NSWindowDelegate>
- (BOOL)windowShouldClose:(id)sender;
@end

@interface MachaWindowDelegate:NSObject<MachaWindowProtocol>
@end

/* implementations */
@implementation MachaApplication
- (void)run {
    @autoreleasepool {
        NSEvent *event = [self
                            nextEventMatchingMask:NSEventMaskAny
                            untilDate:[NSDate distantFuture]
                            inMode:NSDefaultRunLoopMode
                            dequeue:YES];
        [self sendEvent:event];
        [self updateWindows];
    }
}
@end

@implementation MachaWindowDelegate
- (BOOL)windowShouldClose:(id)sender {
    g_macWindowShouldClose = 1;
    return NO;
}
@end

/* now time for some regular C... */
struct mach_mac_window_t {
    void *window;
    NSWindow *ns_window;
    NSOpenGLContext *ns_glctx;
};

mach_mac_window_t *mach_mac_window_create(mach_logger_t *logger, int width,
            int height, char *title) {
    mach_mac_window_t *window = malloc(sizeof(mach_mac_window_t));

    @autoreleasepool {
        [MachaApplication sharedApplication];

        /* add the app to the dock when we first run the app */
        [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
        [NSApp activateIgnoringOtherApps:YES];

        NSUInteger styles = NSWindowStyleMaskClosable |
                            NSWindowStyleMaskTitled |
                            NSWindowStyleMaskResizable |
                            NSWindowStyleMaskMiniaturizable;

        /* create the window */
        NSRect windowRect = NSMakeRect(0, 0, width, height);
        window->ns_window = [[NSWindow alloc] initWithContentRect:windowRect
                                                styleMask:styles
                                                backing:NSBackingStoreBuffered
                                                defer:NO];
        [window->ns_window center];

        /* error handling */
        if (window->ns_window == nil) {
            mach_logger_log(logger, MACH_LOG_CRITICAL,
                    "Objective-C: Failed to create an NSWindow!");
            mach_logger_set_flag(logger, "WINDOW_CREATION_FAILED");
            return NULL;
        }

        /* initialize and properly set the delegate */
        MachaWindowDelegate *delegate = [[MachaWindowDelegate alloc] init];
        [window->ns_window setDelegate:delegate];

        /* window configuration */
        [window->ns_window setTitle:[NSString stringWithUTF8String:title]];
        [window->ns_window makeKeyAndOrderFront:nil];
        [window->ns_window orderFrontRegardless];

        window->window = (void*)window->ns_window;
  }

    return window;
}

/* creates and initializes an OpenGL 3.2 Core context for the window */
void mach_mac_window_gl_init(mach_logger_t *logger,
        mach_mac_window_t *window) {
    /* define pixel format attributes & create pixel format */
    NSOpenGLPixelFormatAttribute attribs[] = {
        NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
        NSOpenGLPFAColorSize, 24,
        NSOpenGLPFAAlphaSize, 8,
        NSOpenGLPFADepthSize, 24,
        NSOpenGLPFAStencilSize, 8,
        NSOpenGLPFADoubleBuffer,
        NSOpenGLPFAAccelerated,
        NSOpenGLPFANoRecovery,
        NSOpenGLPFAClosestPolicy,
        0
    };

    NSOpenGLPixelFormat *pf = [[[NSOpenGLPixelFormat alloc]
                                initWithAttributes:attribs]
                                autorelease];

    if (pf == nil) {
        mach_logger_log(logger, MACH_LOG_CRITICAL, "Failed "
                "to create a pixel format descriptor for OpenGL (3.2 Core)");
        mach_logger_set_flag(logger, "WINDOW_GL_PF_FAILED");
        return;
    }

    /* now, create the actual OpenGL context */
    window->ns_glctx = [[NSOpenGLContext alloc]
                        initWithFormat:pf
                        shareContext:nil];
    if (window->ns_glctx == nil) {
        mach_logger_log(logger, MACH_LOG_CRITICAL, "Failed "
                "to create an NSOpenGLContext (3.2 Core)");
        mach_logger_set_flag(logger, "WINDOW_GL_CTX_FAILED");
        return;
    }

    /* configure the newly created context */
    [window->ns_glctx setView:[window->ns_window contentView]];
    [window->ns_glctx makeCurrentContext];

    /* set v-sync */
    GLint swapInt = 1;
    [window->ns_glctx setValues:&swapInt
                   forParameter:NSOpenGLContextParameterSwapInterval];
}

/* swaps the front and back buffers for rendering */
void mach_mac_window_gl_swap(mach_mac_window_t *window) {
    [window->ns_glctx flushBuffer];
}

/* gets the drawable size in pixels; this is due to the behavior of Apple's
   retina displays */
void mach_mac_window_fb_size(mach_mac_window_t *window, int *width,
        int *height) {
    @autoreleasepool {
        NSRect window_frame = [window->ns_window frame];
        NSRect content_rect = [window->ns_window
            contentRectForFrameRect:window_frame];
        NSSize logical_size = content_rect.size;

        /* account for high-DPI screens */
        CGFloat scale_factor = [window->ns_window backingScaleFactor];
        NSSize physical_size = NSMakeSize(logical_size.width * scale_factor,
                logical_size.height * scale_factor);

        *width = physical_size.width;
        *height = physical_size.height;
    }
}

/* helper function */
int mach_mac_window_should_close(mach_mac_window_t *window) {
    return g_macWindowShouldClose;
}

void mach_mac_window_poll(mach_mac_window_t *window) {
    @autoreleasepool {
        for (;;) {
            NSEvent *event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                    untilDate:nil
                                    inMode:NSDefaultRunLoopMode
                                    dequeue:(YES)];

            if (event == nil) {
                break;
            }

            [NSApp sendEvent:event];
        }
    }
}

void mach_mac_window_destroy(mach_mac_window_t *window) {
    if (window == NULL) return;

    @autoreleasepool {
        if (window->ns_glctx) {
            [NSOpenGLContext clearCurrentContext];
            window->ns_glctx = nil;
        }

        if (window->ns_window) {
            [window->ns_window setDelegate:nil];
            [window->ns_window close];
            window->ns_window = nil;
        }
    }

    free(window);
}


#include <AppKit/AppKit.h>
#include <MacTypes.h>
#include <stdlib.h>

#import <Foundation/Foundation.h>
#include <sys/resource.h>
#import <Cocoa/Cocoa.h>

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
#ifdef __OBJC__
	NSWindow *ns_window;
#endif
};

mach_mac_window_t *mach_mac_window_create(int width, int height, char *title) {
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
			NSLog(@"ERROR: Failed to create an NSWindow!");
			exit(EXIT_FAILURE);
		}

		/* window configuration */
		[window->ns_window setDelegate:[MachaWindowDelegate alloc]];
		[window->ns_window setTitle:[NSString stringWithUTF8String:title]];
		[window->ns_window makeKeyAndOrderFront:nil];
		[window->ns_window orderFrontRegardless];

		window->window = (void*)window->ns_window;
	}

	return window;
}

/* helper function */
int mach_mac_window_should_close(mach_mac_window_t *window) {
	return g_macWindowShouldClose;
}

void mach_mac_window_poll(mach_mac_window_t *window) {
	NSEvent *event = [NSApp nextEventMatchingMask:NSEventMaskAny
							untilDate:nil
							inMode:NSDefaultRunLoopMode
						    dequeue:(YES)];
	[NSApp sendEvent:event];
	[NSApp updateWindows];
}

void mach_mac_window_destroy(mach_mac_window_t *window) {
	if (![window->ns_window isVisible]) {
		[window->ns_window close];
		[NSApp terminate:nil];
	}
}


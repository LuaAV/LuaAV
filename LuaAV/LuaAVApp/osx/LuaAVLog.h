#ifndef _LUAAV_LOG_H_
#define _LUAAV_LOG_H_ 1

#import <Cocoa/Cocoa.h>
#include "luaav_app.h"

/*
	Modified from sources in HandBrake:
*/

/**
 * This class is used to redirect @c stdout and @c stderr outputs. It is never
 * created directly; @c stdoutRedirect and @c stderrRedirect class methods
 * should be use instead.
 *
 * @note Redirection is done by replacing @c _write functions for @c stdout and
 *		 @c stderr streams. Because of this messages written by NSLog(), for
 *		 example are not redirected. I consider this a good thing, but if more
 *		 universal redirecting is needed, it can be done at file descriptor
 *		 level.
 */
@interface LuaAVLog : NSObject
{
	/// Set that contains all registered listeners for this output.
	NSMutableSet *listeners;
	
	/// Selector that is called on listeners to forward the output.
	SEL forwardingSelector;

	/// Output stream (@c stdout or @c stderr) redirected by this object.
	FILE *stream;
	FILE *readStream;
	int spipe[2];
	
	NSLock *lock;
	
	/// true while redirection is occuring
	bool active;
}

+ (void)flush;
+ (id)stdoutRedirect;
+ (id)stderrRedirect;

- (bool)isActive;
- (void)readPipe;
- (void)readPipeDirect;
- (void)addListener:(id)aListener;
- (void)removeListener:(id)aListener;

@end


#endif	//_LUAAV_LOG_H_
#include "LuaAVLog.h"
#include "luaav.h"
#include "al_pq.hpp"
//#include "allocore/types/al_MsgQueue.hpp"

#include <string>


#define LUAAV_LOG_POLL_PERIOD	(1./30.)


/// Global pointer to LuaAVLog object that manages redirects for stdout.
static LuaAVLog *g_stdoutRedirect = nil;

/// Global pointer to LuaAVLog object that manages redirects for stderr.
static LuaAVLog *g_stderrRedirect = nil;

@interface LuaAVLog (Private)
- (id)initWithStream:(FILE *)aStream selector:(SEL)aSelector;
- (void)startRedirect;
- (void)stopRedirect;
- (void)forwardOutput:(NSData *)data;
@end

static void readData(FILE *stream, std::string &str) {
	int res = 0;
	
	do {
		char buffer[128];
		res = fread(buffer, 1, 127, stream);
		buffer[res] = '\0';
		
		str.append(buffer);
	} while(res > 0);
}

static int stdout_readpipe(al_sec now, LuaAVLog * log, luaav_state * S) {
	[g_stdoutRedirect readPipe];
	
	if ([log isActive]) {
		al::PQHelper queue(luaav_state_mainpq(S));
		queue.send(LUAAV_LOG_POLL_PERIOD, /*LUAAV_LOG_POLL_PERIOD*/0, stdout_readpipe, log, S);
	}
	return 0;
}

static int stderr_readpipe(al_sec now, LuaAVLog * log, luaav_state * S) {
	[g_stderrRedirect readPipe];
	
	if ([log isActive]) {
		al::PQHelper queue(luaav_state_mainpq(S));
		queue.send(LUAAV_LOG_POLL_PERIOD, /*LUAAV_LOG_POLL_PERIOD*/0, stderr_readpipe, log, S);
	}
	return 0;
}


@implementation LuaAVLog

+ (void)flush
{
	if(g_stderrRedirect) {
		[g_stdoutRedirect readPipeDirect];
	}
	if(g_stderrRedirect) {
		[g_stderrRedirect readPipeDirect];
	}
}

/**
 * Returns LuaAVLog object used to redirect stdout.
 */
+ (id)stdoutRedirect
{
	if (!g_stdoutRedirect)
		g_stdoutRedirect = [[[LuaAVLog alloc] initWithStream:stdout selector:@selector(stdoutRedirect:)] retain];
	
	return g_stdoutRedirect;
}

/**
 * Returns LuaAVLog object used to redirect stderr.
 */
+ (id)stderrRedirect
{
	if (!g_stderrRedirect)
		g_stderrRedirect = [[[LuaAVLog alloc] initWithStream:stderr selector:@selector(stderrRedirect:)] retain];
	
	return g_stderrRedirect;
}

/**
 * Adds specified object as listener for this output. Method @c stdoutRedirect:
 * or @c stderrRedirect: of the listener is called to redirect the output.
 */
- (void)addListener:(id)aListener
{
	NSAssert2([aListener respondsToSelector:forwardingSelector], @"Object %@ doesn't respond to selector \"%@\"", aListener, NSStringFromSelector(forwardingSelector));

	if (![listeners containsObject:aListener])
	{
		[listeners addObject:aListener];
		[aListener release];
	}
	
	if ([listeners count] > 0)
		[self startRedirect];
}

/**
 * Stops forwarding for this output to the specified listener object.
 */
- (void)removeListener:(id)aListener
{
	if ([listeners containsObject:aListener])
	{
		[aListener retain];
		[listeners removeObject:aListener];
	}

	// If last listener is removed, stop redirecting output and autorelease
	// self. Remember to set proper global pointer to NULL so the object is
	// recreated again when needed.
	if ([listeners count] == 0)
	{
		[self stopRedirect];
		[self autorelease];

		if (self == g_stdoutRedirect)
			g_stdoutRedirect = NULL;
		else if (self == g_stderrRedirect)
			g_stderrRedirect = NULL;
	}
}

- (void)readPipe
{
	fflush(stream);

	std::string str;
	readData(readStream, str);
	int sz = str.size();
	if(sz > 0) {
		NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
		NSData *data = [[NSData alloc] initWithBytes:str.data() length:sz];
		[self performSelectorOnMainThread:@selector(forwardOutput:) withObject:data waitUntilDone:NO];
		[data release];
		[pool release];
	}
}

- (void)readPipeDirect
{
	fflush(stream);

	std::string str;
	readData(readStream, str);
	int sz = str.size();
	if(sz > 0) {
		NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
		NSData *data = [[NSData alloc] initWithBytes:str.data() length:sz];
		[self forwardOutput:data];
		[data release];
		[pool release];
	}
}

- (bool)isActive
{
	return self->active;
}

@end

@implementation LuaAVLog (Private)

/**
 * Private constructor which should not be called from outside. This is used to
 * initialize the class at @c stdoutRedirect and @c stderrRedirect.
 *
 * @param aStream	Stream that wil be redirected (stdout or stderr).
 * @param aSelector	Selector that will be called in listeners to redirect the stream.
 *
 * @return New LuaAVLog object.
 */
- (id)initWithStream:(FILE *)aStream selector:(SEL)aSelector
{
	if (self = [super init])
	{
		listeners = [[NSMutableSet alloc] init];
		forwardingSelector = aSelector;
		stream = aStream;
		
		if(pipe(spipe) != 0) {
			// error
		}
		
		for(int n=0; n < 2; n++) {
			int f = fcntl(spipe[n], F_GETFL, 0);
			f |= O_NONBLOCK;
			fcntl(spipe[n], F_SETFL, f);
		}
		
		readStream = fdopen(spipe[0], "r");
		
		lock = [[NSLock alloc] init];
		
		active = true;
	}
	return self;
}

/**
 * Frees all the listeners and deallocs the object.
 */
- (void)dealloc
{
	[self stopRedirect];
	[listeners release];
	[super dealloc];
}

/**
 * Starts redirecting the stream by redirecting its output to function
 * @c stdoutwrite() or @c stderrwrite(). Old _write function is stored to
 * @c oldWriteFunc so it can be restored. 
 */
- (void)startRedirect
{	
	dup2(spipe[1], fileno(stream));
	active = true;
	if(stream == stdout) {
		luaav_state * S = luaav_app_state();
		al::PQHelper queue(luaav_state_mainpq(S));
		queue.send(LUAAV_LOG_POLL_PERIOD, LUAAV_LOG_POLL_PERIOD, stdout_readpipe, self, S);
	}
	else {
		luaav_state * S = luaav_app_state();
		al::PQHelper queue(luaav_state_mainpq(S));
		queue.send(LUAAV_LOG_POLL_PERIOD, LUAAV_LOG_POLL_PERIOD, stderr_readpipe, self, S);
	}
}

/**
 * Stops redirecting of the stream by returning the stream's _write function
 * to original.
 */
- (void)stopRedirect
{
	active = false;
}


/**
 * Called from @c stdoutwrite() and @c stderrwrite() to forward the output to 
 * listeners.
 */ 
- (void)forwardOutput:(NSData *)data
{
	NSString *string = [[NSString alloc] initWithData:data encoding:NSASCIIStringEncoding];
	[listeners makeObjectsPerformSelector:forwardingSelector withObject:string];
	[string release];
}

@end

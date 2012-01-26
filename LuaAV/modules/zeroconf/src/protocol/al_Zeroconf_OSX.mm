#include "allocore/protocol/al_Zeroconf.hpp"

#import <Cocoa/Cocoa.h>
#import <sys/socket.h>
#import <netinet/in.h>
#import <arpa/inet.h>

// takes a std::string and returns an NSString *
#define CPP_STRING_TO_NSSTRING(STR) [NSString stringWithUTF8String:((STR).c_str())]

@interface ClientDelegate : NSObject {
	NSNetServiceBrowser *browser;
    NSNetService *connectedService;
    NSMutableArray *services;
	BOOL isConnected;
	NSRunLoop *loop;
	
	al::zero::Client * master;
}

-(void)search;

@end

@implementation ClientDelegate

-(id)initWithDomain:(NSString *) domain type:(NSString *)type master:(al::zero::Client *)ptr {
	if ((self = [super init])){
		master = ptr;
		services = [NSMutableArray new];
		//NSLog(@"allocated browsersssszzzzzzzzzzzzz");
		//browser = [[NSNetServiceBrowser new] autorelease];
		browser = [NSNetServiceBrowser new];
		browser.delegate = self;
		[browser searchForServicesOfType:type inDomain:domain];

		//[browser scheduleInRunLoop:loop forMode:NSDefaultRunLoopMode];
		//[loop run];		
		
		//[[NSRunLoop currentRunLoop] run];
		[browser scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
		
		isConnected = NO;
		//[self search];
		//[self performSelector:@selector(search) withObject:nil afterDelay:3];
//		[self performSelector:@selector(search) afterDelay:1];
		//[browser searchForServicesOfType:type inDomain:@""];
	}
	return self;
}

-(void)search {}

-(void)dealloc {
	[loop release];
	//self.connectedService = nil;
    //self.browser = nil;
	[browser stop];
    [browser release];
	[services release];
	[super dealloc];
}

#pragma mark Net Service Browser Delegate Methods
//- (void)netServiceBrowserWillSearch:(NSNetServiceBrowser *)aNetServiceBrowser {
//	NSLog(@"netServiceBrowserWillSearch: %@", aNetServiceBrowser);
//}
//- (void)netServiceBrowserDidStopSearch:(NSNetServiceBrowser *)aNetServiceBrowser {
//	NSLog(@"netServiceBrowserDidStopSearch: %@", aNetServiceBrowser);
//}
- (void)netServiceBrowser:(NSNetServiceBrowser *)aNetServiceBrowser didNotSearch:(NSDictionary *)errorDict {
	NSLog(@"didNotSearch: %@", errorDict);
}
//- (void)netServiceBrowser:(NSNetServiceBrowser *)aNetServiceBrowser didFindDomain:(NSString *)domainString moreComing:(BOOL)moreComing {
//	NSLog(@"didFindDomain: %@ %@", domainString, moreComing);
//}
- (void)netServiceBrowser:(NSNetServiceBrowser *)aNetServiceBrowser didFindService:(NSNetService *)service moreComing:(BOOL)moreComing {
	//NSLog(@"didFindService: %@ %@", aNetService, moreComing);
	[services addObject:service];
	//NSNetService *remoteService = [services objectAtIndex:0];
    service.delegate = self;
    [service resolveWithTimeout:0];
	master->onServiceNew([[service name] UTF8String]);
}
- (void)netServiceBrowser:(NSNetServiceBrowser *)aNetServiceBrowser didRemoveService:(NSNetService *)service moreComing:(BOOL)moreComing {
	//NSLog(@"didRemoveService: %@ %@", aNetService, moreComing);
	master->onServiceRemove([[service name] UTF8String]);
}
//- (void)netServiceBrowser:(NSNetServiceBrowser *)aNetServiceBrowser didRemoveDomain:(NSString *)domainString moreComing:(BOOL)moreComing {
//	NSLog(@"didRemoveDomain: %@ %@", domainString, moreComing);
//}

-(void)netServiceDidResolveAddress:(NSNetService *)service {
	//NSLog(@"CONNECTED");
	//NSLog(service.domain);
    //self.isConnected = YES;
    //self.connectedService = service;
	
	NSString *name = nil;
	int port;
	
	for(NSData *d in [service addresses]) {
		struct sockaddr_in *socketAddress  = (struct sockaddr_in *) [d bytes];
		name = [service name];
		socketAddress = (struct sockaddr_in *)[d bytes];
		char * ipaddress = inet_ntoa(socketAddress->sin_addr);
		port = ntohs(socketAddress->sin_port); // ntohs converts from network byte order to host byte order 
		master->onServiceResolved([name UTF8String], [[service hostName] UTF8String], port, ipaddress);
	}
}

@end



@interface ServerDelegate : NSObject {
    NSNetService * netService;
}
@end

@implementation ServerDelegate

- (id)initWithDomain:(NSString *)domain type:(NSString *)type name:(NSString *)name port:(int)port
{
	if ((self = [super init])){
		netService = [[NSNetService alloc] 
			initWithDomain:(domain) 
			type:(type) 
			name:(name) 
			port:(port)
		];
		netService.delegate = self;
		[netService publish];
	}
	return self;
}

-(void)dealloc {
    [netService stop];
    [netService release]; 
    netService = nil;
    [super dealloc];
}

#pragma mark Net Service Delegate Methods
-(void)netService:(NSNetService *)aNetService didNotPublish:(NSDictionary *)dict {
    NSLog(@"Failed to publish: %@", dict);
}
- (void)netServiceWillPublish:(NSNetService *)sender {
	//NSLog(@"Will publish: %@", sender);
}
- (void)netServiceWillResolve:(NSNetService *)sender { 
	//NSLog(@"Will resolve: %@", sender);
}
- (void)netServiceDidResolveAddress:(NSNetService *)sender {
	//NSLog(@"netServiceDidResolveAddress: %@", sender);
}
- (void)netService:(NSNetService *)sender didNotResolve:(NSDictionary *)errorDict {
	//NSLog(@"netServiceDidResolveAddress: %@", errorDict);
}
- (void)netServiceDidStop:(NSNetService *)sender {
	//NSLog(@"netServiceDidStop: %@", sender);
}

@end

class ImplBase {
public:

};


namespace al {
namespace zero {

#pragma mark private implementation

class Client::Impl : public ImplBase {
public:
	
	Impl(Client * master, const std::string& domain, const std::string& type) : ImplBase(), master(master) {
		NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
		delegate = [[ClientDelegate alloc]
			initWithDomain:CPP_STRING_TO_NSSTRING(domain)
			type:CPP_STRING_TO_NSSTRING(type)
			master:master
		];
		[pool release];
	}
	virtual ~Impl() {
		[delegate release]; 
		delegate = nil;
	}

	void poll(al_sec timeout) {}

	Client * master;
	ClientDelegate * delegate;
};

class Service::Impl : public ImplBase {
public:
	Impl(Service * master, const std::string& name, const std::string& host, uint16_t port, const std::string& type, const std::string& domain) 
	:	ImplBase(), name(name), host(host), type(type), domain(domain), port(port), master(master) {
		NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
		delegate = [[ServerDelegate alloc] 
			initWithDomain:CPP_STRING_TO_NSSTRING(domain) 
			type:CPP_STRING_TO_NSSTRING(type) 
			name:CPP_STRING_TO_NSSTRING(name) 
			port:(port)
		];
		[pool release];
	}
	
	virtual ~Impl() {
		[delegate release]; 
		delegate = nil;
	}

	void poll(al_sec timeout) {}
	
	std::string name, host, type, domain;
	uint16_t port;
	Service * master;
	
	ServerDelegate * delegate;
};

#pragma mark public interface

Client::Client(const std::string& type, const std::string& domain) 
:	type(type), domain(domain) {
	mImpl = new Impl(this, domain, type);
}

Client::~Client() {
	delete mImpl;
}

void Client::poll(al_sec interval) {
	NSDate * date = [[NSDate alloc] initWithTimeIntervalSinceNow:interval];
	[[NSRunLoop currentRunLoop] runUntilDate:date];
	[date release];
}

Service::Service(const std::string& name, uint16_t port, const std::string& type, const std::string& domain) {
	mImpl = new Impl(this, name, Socket::hostName(), port, type, domain);
}

Service::~Service() {
	delete mImpl;
}

} // ::zero
} // ::al

/*
	What would a basic socket class need?
	
	binding methods for UDP and TCP are very different
	receive-only or send-only or duplex?
*/
/*
Nomenclature:
Server side:
bind		Assigns a socket an address, i.e. a specified local port number and IP 
			address.
	
listen		After a socket has been associated with an address, listen 
			prepares it for incoming connections.

accept		Accepts a received incoming attempt to create a new TCP connection 
			from the remote client, and creates a new socket associated with the
			socket address pair of this connection.

Client side:
connect		Assigns a free local port number to a socket. In case of a TCP 
			socket, it causes an attempt to establish a new TCP connection.

Domain:
PF_INET			network protocol IPv4
PF_INET6		IPv6
PF_UNIX			local socket (using a file)

Type:
SOCK_STREAM		(reliable stream-oriented service or Stream Sockets)
SOCK_DGRAM		(datagram service or Datagram Sockets)
SOCK_SEQPACKET	(reliable sequenced packet service)
SOCK_RAW		(raw protocols atop the network layer)

Protocol:
IPPROTO_TCP
IPPROTO_SCTP
IPPROTO_UDP
IPPROTO_DCCP

TCP:
(PF_INET, PF_INET6), (SOCK_STREAM), (IPPROTO_TCP)
TCP Server:
1) Create a TCP socket
2) Bind socket to the listen port, with a call to bind()
3) Prepare socket to listen for connections with a call to listen()
4) Accepting incoming connections, via a call to accept()
5) Communicate with remote host, which can be done through, e.g., send()
6) Close each socket that was opened, once it is no longer needed, using close()
*/

#include <cstring>
#include "allocore/io/al_Socket.hpp"
#include "allocore/system/al_Config.h"

#include "../private/al_ImplAPR.h"
#ifdef AL_LINUX
#include "apr-1.0/apr_network_io.h"
#else
#include "apr-1/apr_network_io.h"
#endif

#define PRINT_SOCKADDR(s)\
	printf("%s %s\n", s->hostname, s->servname);

namespace al{

struct Socket::Impl : public ImplAPR {

	Impl()
	:	ImplAPR(), mPort(0), mAddress(""), mSockAddr(0), mSock(0), mSender(false)
	{}

	Impl(uint16_t port, const char * address, al_sec timeout_, bool sender)
	:	ImplAPR(), mPort(port), mAddress(address), mSockAddr(0), mSock(0), mSender(sender)
	{
//		// opens the socket also:
//		timeout(timeout_);
		open(port, address, timeout_, sender);
	}

	void close(){
		if(opened()){
			check_apr(apr_socket_close(mSock));
			mSock=0;
		}
	}

	#define BAILONFAIL(func)\
		if(APR_SUCCESS != check_apr(func)){\
			printf("failed to create socket at %s:%i\n", address.c_str(), port);\
			close();\
			return false;\
		}

	bool open(uint16_t port, std::string address, al_sec timeout, bool sender){
		close();

		/* @see http://dev.ariel-networks.com/apr/apr-tutorial/html/apr-tutorial-13.html */

		mPort = port;
		mSender = sender;
		mAddress = address;

		BAILONFAIL(
			apr_sockaddr_info_get(&mSockAddr, mAddress[0] ? mAddress.c_str() : 0, APR_INET, mPort, 0, mPool)
		);

		BAILONFAIL(apr_socket_create(&mSock, mSockAddr->family, SOCK_DGRAM, APR_PROTO_UDP, mPool));

		if(mSock){
			// Assign address to socket. If TCP, establish new connection.
			if(mSender){	BAILONFAIL(apr_socket_connect(mSock, mSockAddr)); }
			else{			BAILONFAIL(apr_socket_bind(mSock, mSockAddr)); }

			// Set timeout behavior
			if(timeout == 0){
				// non-blocking:			APR_SO_NONBLOCK==1(on),  then timeout=0
				BAILONFAIL(apr_socket_opt_set(mSock, APR_SO_NONBLOCK, 1));
				BAILONFAIL(apr_socket_timeout_set(mSock, 0));
			}
			else if(timeout > 0){
				// blocking-with-timeout:	APR_SO_NONBLOCK==0(off), then timeout>0
				BAILONFAIL(apr_socket_opt_set(mSock, APR_SO_NONBLOCK, 0));
				BAILONFAIL(apr_socket_timeout_set(mSock, (apr_interval_time_t)(timeout * 1.0e6)));

			}
			else{
				// blocking-forever:		APR_SO_NONBLOCK==0(off), then timeout<0
				BAILONFAIL(apr_socket_opt_set(mSock, APR_SO_NONBLOCK, 0));
				BAILONFAIL(apr_socket_timeout_set(mSock, -1));
			}
			
			mTimeout = timeout;
		}

		return true;
	}

	// note that setting timeout will close and re-open the socket:
	void timeout(al_sec v){
		open(mPort, mAddress, v, mSender);
	}
	
	bool opened() const { return 0!=mSock; }

	uint16_t mPort;
	std::string mAddress;
	apr_sockaddr_t * mSockAddr;
	apr_socket_t * mSock;
	al_sec mTimeout;
	bool mSender;
};



Socket::Socket()
:	mImpl(0)
{
	mImpl = new Impl;
}

Socket::Socket(uint16_t port, const char * address, al_sec timeout, bool sender)
: mImpl(0)
{
	mImpl = new Impl(port, address, timeout, sender);
}

Socket::~Socket(){
	close();
	delete mImpl;
}

const std::string& Socket::address() const { return mImpl->mAddress; }

bool Socket::opened() const { return mImpl->opened(); }

uint16_t Socket::port() const { return mImpl->mPort; }

al_sec Socket::timeout() const { return mImpl->mTimeout; }


void Socket::close(){ mImpl->close(); }

bool Socket::open(uint16_t port, const char * address, al_sec timeout, bool sender){
	return mImpl->open(port, address, timeout, sender);
}



void Socket::timeout(al_sec v){ mImpl->timeout(v); }

size_t Socket::recv(char * buffer, size_t maxlen) {
	apr_size_t len = maxlen;
	apr_status_t r = apr_socket_recv(mImpl->mSock, buffer, &len);
	
	// only error check if not error# 35: Resource temporarily unavailable
	if(len){ check_apr(r); }
	return len;
}

size_t Socket::send(const char * buffer, size_t len) {
	apr_size_t size = len;
	if (mImpl->opened()) {
		//check_apr(apr_socket_send(mSock, buffer, &size));
		apr_socket_send(mImpl->mSock, buffer, &size);
	} else {
		size = 0;
	}
	return size;
}


std::string Socket::hostIP(){
	ImplAPR apr;
	char * addr;
	apr_sockaddr_t * sa;
	check_apr(apr_sockaddr_info_get(&sa, hostName().c_str(), APR_INET, 8000, 0, apr.pool()));
	//printf("%p sa\n", sa);
	while(sa != 0) {
		check_apr(apr_sockaddr_ip_get(&addr, sa));
		//printf("%s %s %s %d %d\n", addr, sa->hostname, sa->servname, sa->port, sa->family);
		sa = sa->next;
	}
	return addr;
}

std::string Socket::hostName(){
	char buf[APRMAXHOSTLEN+1];
	ImplAPR apr;
	check_apr(apr_gethostname(buf, sizeof(buf), apr.pool()));
	//printf("host %s\n", buf);
	return buf;
}

} // al::

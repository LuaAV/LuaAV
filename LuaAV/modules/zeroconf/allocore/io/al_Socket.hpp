#ifndef INCLUDE_AL_IO_SOCKET_HPP
#define INCLUDE_AL_IO_SOCKET_HPP 1

/*	Allocore --
	Multimedia / virtual environment application class library
	
	Copyright (C) 2009. AlloSphere Research Group, Media Arts & Technology, UCSB.
	Copyright (C) 2006-2008. The Regents of the University of California (REGENTS). 
	All Rights Reserved.

	Permission to use, copy, modify, distribute, and distribute modified versions
	of this software and its documentation without fee and without a signed
	licensing agreement, is hereby granted, provided that the above copyright
	notice, the list of contributors, this paragraph and the following two paragraphs 
	appear in all copies, modifications, and distributions.

	IN NO EVENT SHALL REGENTS BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT,
	SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING
	OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF REGENTS HAS
	BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
	THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
	PURPOSE. THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED
	HEREUNDER IS PROVIDED "AS IS". REGENTS HAS  NO OBLIGATION TO PROVIDE
	MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.


	File description:
	Utilties for network socket sending/receiving

	File author(s):
	Graham Wakefield, 2010, grrrwaaa@gmail.com
	Lance Putnam, 2010, putnam.lance@gmail.com
*/


#include <string>
#include "allocore/system/al_Config.h"

namespace al{


/// Base class for network socket
class Socket{
public:

	/// Returns whether socket is open
	bool opened() const;

	/// Get IP address string
	const std::string& address() const;

	/// Get port number
	uint16_t port() const;

	/// Get timeout duration, in seconds
	al_sec timeout() const;


	/// Close the socket
	void close();

	/// Set timeout. < 0: block forever; = 0: no blocking; > 0 block with timeout

	/// Note that setting timeout will close and re-open the socket.
	///
	void timeout(al_sec v);


	/// Get name of current host
	static std::string hostName();
	
	/// IP address of current host
	static std::string hostIP();

protected:
	Socket();

	/// @sender: true if Socket will send
	Socket(uint16_t port, const char * address, al_sec timeout, bool sender);

	virtual ~Socket();

	bool open(uint16_t port, const char * address, al_sec timeout, bool sender);
	size_t recv(char * buffer, size_t maxlen);
	size_t send(const char * buffer, size_t len);

private:
	class Impl; Impl * mImpl;
};



/// Socket for sending data over a network
class SocketSend : public Socket {
public:
	SocketSend(){}

	/// @param[in] port		Port number (valid range is 0-65535)
	/// @param[in] address	IP address
	/// @param[in] timeout	< 0: block forever; = 0: no blocking; > 0 block with timeout
	SocketSend(uint16_t port, const char * address = "localhost", al_sec timeout=0)
	:	Socket(port, address, timeout, true)
	{}

	/// Open socket closing and reopening if currently open
	bool open(uint16_t port, const char * address, al_sec timeout=0){
		return Socket::open(port, address, timeout, true);
	}

	/// Send data over a network
	
	/// @param[in] buffer	The buffer of data to send
	/// @param[in] len		The length, in bytes, of the buffer
	size_t send(const char * buffer, size_t len){ return Socket::send(buffer, len); }
};

/// Socket for receiving data over a network
class SocketRecv : public Socket {
public:
	SocketRecv(){}

	/// @param[in] port		Port number (valid range is 0-65535)
	/// @param[in] address	IP address. If empty, will bind all network interfaces to socket.
	/// @param[in] timeout	< 0: block forever; = 0: no blocking; > 0 block with timeout
	SocketRecv(uint16_t port, const char * address = "", al_sec timeout=0)
	:	Socket(port, address, timeout, false)
	{}

	/// Open socket closing and reopening if currently open
	bool open(uint16_t port, const char * address = "", al_sec timeout=0){
		return Socket::open(port, address, timeout, false);
	}

	/// Read data from a network

	/// @param[in] buffer	A buffer to copy the received data into
	/// @param[in] maxlen	The maximum length, in bytes, of data to copy
	/// \returns bytes read
	//
	/// Note: to ensure receipt of all messages in the queue, use 
	/// while(recv()){}
	size_t recv(char * buffer, size_t maxlen){ return Socket::recv(buffer, maxlen); }
};


} // al::

#endif /* include guard */

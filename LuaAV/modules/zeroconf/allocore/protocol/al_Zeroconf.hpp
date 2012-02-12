#ifndef INCLUDE_AL_ZEROCONF_HPP
#define INCLUDE_AL_ZEROCONF_HPP

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
	Wrapper for mDNS zeroconf discovery

	File author(s):
	Charlie Roberts, 2012, charlie@charlie-roberts.com
	Graham Wakefield, 2012, grrrwaaa@gmail.com

*/

#include <string>
#include <vector>
#include "allocore/io/al_Socket.hpp"
#include "allocore/system/al_Thread.hpp"

#include <string>
#include <stdio.h>

namespace al{
namespace zero{

class Client {
public:
	class Impl;

	Client(const std::string& type = "_osc._udp.", const std::string& domain = "local.");
	virtual ~Client();

	///! check for new services:
	static void poll(al_sec interval = 0.01);

	///! called when a new service name is added:
	virtual void onServiceNew(const std::string& name) {}

	///! usually called after onServiceNew
	/// identifies the host/port/address(es) associated with the service name
	virtual void onServiceResolved(const std::string& name, const std::string& host_name, uint16_t port, const std::string& address) {}

	///! called when existing service name is removed:
	virtual void onServiceRemove(const std::string& name) {}

protected:	
	std::string type, domain;
	Impl * mImpl;
};

class Service {
public:
	class Impl;

	///! create and publish a new service
	/// the name should be unique
	Service(const std::string& name, uint16_t port=4110, const std::string& type="_osc._udp.", const std::string& domain="local.");

	virtual ~Service();

protected:
	Impl * mImpl;
};

} // zero::
} // al::
	
#endif
	

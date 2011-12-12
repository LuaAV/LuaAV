#ifndef INCLUDE_AL_THREAD_HPP
#define INCLUDE_AL_THREAD_HPP

/*
 *	Thread
 *  AlloSphere Research Group / Media Arts & Technology, UCSB, 2009
 */

/*
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
*/


namespace al{

typedef void * (*ThreadFunction)(void * userData);

class Thread{
public:
	Thread();
	Thread(ThreadFunction routine, void * userData = 0);

	//! The class destructor does not attempt to cancel or join a thread.
	~Thread();

	//! Begin execution of the thread \e routine.  Upon success, true is returned.
	/*!
	A data pointer can be supplied to the thread routine via the
	optional \e ptr argument.  If the thread cannot be created, the
	return value is false.
	*/
	bool start(ThreadFunction routine, void * userData = 0);

	//! Block the calling routine indefinitely until the thread terminates.
	/*!
	This function suspends execution of the calling routine until the thread has 
	terminated.  It will return immediately if the thread was already 
	terminated.  A \e true return value signifies successful termination. 
	A \e false return value indicates a problem with the wait call.
	*/
	bool wait();

protected:
	class Impl;
	Impl * mImpl;
};


} // al::

#endif /* include guard */

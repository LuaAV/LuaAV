#ifndef INCLUDE_AL_PRIVATE_IMPL_APR
#define INCLUDE_AL_PRIVATE_IMPL_APR

/*
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

#include <stdio.h>
#include <stdlib.h>

// need this to pick up AL_LINUX etc:
#include "allocore/system/al_Config.h"

#ifdef AL_LINUX
	#include "apr-1.0/apr_general.h"
	#include "apr-1.0/apr_errno.h"
	#include "apr-1.0/apr_pools.h"
#else
	#include "apr-1/apr_general.h"
	#include "apr-1/apr_errno.h"
	#include "apr-1/apr_pools.h"
#endif

namespace al{

/*
	Error handling utility
*/
static apr_status_t check_apr(apr_status_t err) {
	char errstr[1024];
	if (err != APR_SUCCESS) {
		apr_strerror(err, errstr, sizeof(errstr));
		fprintf(stderr, "%s\n", errstr);
	}
	return err;
}

/*
	This function MUST be called prior to using any APR
		(however, ImplAPR will do it for you)
*/
inline void initialize_apr() {
	static bool initialized = false;
	if (!initialized) {
		initialized = true;
		check_apr(apr_initialize());
		atexit(apr_terminate);	// FIXME - can we have multiple atexit calls?
	}
}

/*
	Base class for most APR wrapper objects
*/
class ImplAPR {
public:
	ImplAPR() {
		initialize_apr();
		check_apr(apr_pool_create(&mPool, NULL));
	}	

	~ImplAPR() {
		apr_pool_destroy(mPool);
	}
	
	apr_pool_t * pool() { return mPool; } 
	
protected:
	apr_pool_t * mPool;
};






} // ::al

#endif /* include guard */

#include "allocore/system/al_Thread.hpp"
#include "../private/al_ImplAPR.h"
#ifdef AL_LINUX
#include "apr-1.0/apr_general.h"
#include "apr-1.0/apr_thread_proc.h"
#else
#include "apr-1/apr_general.h"
#include "apr-1/apr_thread_proc.h"
#endif

namespace al {

struct Thread::Impl : public ImplAPR {
	Impl(void * ud = NULL)
	:	 ImplAPR(), mThread(0), mThreadAttr(0), mRoutine(0), mUserData(0)
	{
		check_apr(apr_threadattr_create(&mThreadAttr, mPool));
	}

	bool start(ThreadFunction routine, void * userData){
		if(mThread) return false;	// can't start already started!
		mRoutine = routine;
		mUserData = userData;
		apr_status_t rv = apr_thread_create(&mThread, mThreadAttr, cThreadFunc, this, mPool);
		check_apr(rv);
		return rv == APR_SUCCESS;
	}

	bool wait(){
		apr_status_t rv = APR_SUCCESS;
		rv = check_apr(apr_thread_join(&rv, mThread));
		mThread = 0;
		return rv == APR_SUCCESS;
	}

	~Impl(){
		if(mThread) wait();
	}

	apr_thread_t * mThread;
    apr_threadattr_t * mThreadAttr;
	ThreadFunction mRoutine;
	void * mUserData;

	static void * APR_THREAD_FUNC cThreadFunc(apr_thread_t *thread, void *data){
		//printf(".\n");
		Impl * impl = (Impl *)data;
		void * result = (impl->mRoutine)(impl->mUserData);
		apr_thread_exit(thread, APR_SUCCESS);
		return result;
	}
};



Thread::Thread()
:	mImpl(new Impl())
{}

Thread::Thread(ThreadFunction routine, void * userData)
:	mImpl(new Impl())
{
	start(routine, userData);
}

Thread::~Thread(){ delete mImpl; }

bool Thread::start(ThreadFunction routine, void * userData){
	return mImpl->start(routine, userData);
}

bool Thread::wait(){
	return mImpl->wait();
}

} // al::

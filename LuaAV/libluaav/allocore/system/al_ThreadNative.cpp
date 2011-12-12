#include "system/al_Thread.hpp"

/***************************************************/
/*! \class Thread
    \brief STK thread class.

    This class provides a uniform interface for cross-platform
    threads.  On unix systems, the pthread library is used.  Under
    Windows, the C runtime threadex functions are used.

    Each instance of the Thread class can be used to control a single
    thread process.  Routines are provided to signal cancelation
    and/or joining with a thread, though it is not possible for this
    class to know the running status of a thread once it is started.

    For cross-platform compatability, thread functions should be
    declared as follows:

    THREAD_FUNCTION(thread_function_name)

    by Perry R. Cook and Gary P. Scavone, 1995 - 2005.
*/
/***************************************************/


#define USE_PTHREAD		(defined (__APPLE__) || defined (OSX) || defined (__LINUX__) || defined (__UNIX__))
#define USE_THREADEX	(defined(WIN32))

namespace al {

#if USE_PTHREAD
#include <pthread.h>

//typedef pthread_t ThreadHandle;
//typedef void * (*ThreadFunction)(void *);
//#define THREAD_FUNCTION(name) void * name(void * user)

struct Thread::Impl{
	Impl(): mHandle(0){}
	
	bool start(ThreadFunction routine, void * userData){
		if(mHandle) return false;
		return 0 == pthread_create(&mHandle, NULL, *routine, userData);
	}

	bool cancel(){
		return 0 == pthread_cancel(mHandle);
	}

	void testCancel(){
		pthread_testcancel();
	}

	bool wait(){
		if(pthread_join(mHandle, NULL) == 0){
			mHandle = 0;
			return true;
		}
		return false;
	}

	pthread_t mHandle;
};

#elif USE_THREADEX

#include <windows.h>
#include <process.h>

//typedef unsigned long ThreadHandle;
//typedef unsigned (__stdcall *ThreadFunction)(void *);
//#define THREAD_FUNCTION(name) unsigned _stdcall * name(void * user)

struct Thread::Impl{
	Impl(): mHandle(0){}
	
	bool start(ThreadFunction routine, void * userData){
		if(mHandle) return false;
		unsigned thread_id;
		mHandle = _beginthreadex(NULL, 0, cThreadFunc, this, 0, &thread_id);
		if(mHandle) return true;
		return false;
	}

	bool cancel(){
		TerminateThread((HANDLE)mHandle, 0);
		return true;
	}

	void testCancel(){
	}

	bool wait(){
		long retval = WaitForSingleObject((HANDLE)mHandle, INFINITE);
		if(retval == WAIT_OBJECT_0){
			CloseHandle((HANDLE)mHandle);
			mHandle = 0;
			return true;
		}
		return false;
	}

	unsigned long mHandle;
	ThreadFunction mRoutine;
	
	static unsigned _stdcall * cThreadFunc(void * userData){
		Impl * impl = (Impl *)user;
		return (unsigned _stdcall *)user->mRoutine(userData);
	}
};

#endif


// typedef void * (*ThreadFunction)(void *);

Thread::Thread(): mImpl(new Impl()){}

Thread::Thread(ThreadFunction routine, void * userData)
:	mImpl(new Impl())
{
	start(routine, userData);
}

Thread::~Thread(){
	delete mImpl;
}

bool Thread::start(ThreadFunction routine, void * userData){
	return mImpl->start(routine, userData);
}

bool Thread::wait(){
	return mImpl->wait();
}

} // al::

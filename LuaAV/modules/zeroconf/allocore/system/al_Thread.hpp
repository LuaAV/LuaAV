#ifndef INCLUDE_AL_THREAD_HPP
#define INCLUDE_AL_THREAD_HPP

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
	Minimal thread class with similar interface to C++0x thread

	File author(s):
	Lance Putnam, 2010, putnam.lance@gmail.com
	Graham Wakefield, 2010, grrrwaaa@gmail.com
*/


namespace al{


/// Function object interface used by thread
struct ThreadFunction{
	virtual ~ThreadFunction(){}

	/// Routine called on thread execution start
	virtual void operator()() = 0;
};


/// C-style thread function with user data
struct CThreadFunction : public ThreadFunction{

	/// Prototype of thread execution function
	typedef void * (*CFunc)(void * userData);
	
	/// @param[in] func		thread execution function
	/// @param[in] user		user data passed into thread execution function
	CThreadFunction(CFunc func_=0, void * user_=0): func(func_), user(user_){}
	
	void operator()(){ func(user); }

	CFunc func;		///< Thread execution function
	void * user;	///< User data passed into thread execution function
};


/// Thread
class Thread{
public:

	/// Create thread without starting
	Thread();

	/// @param[in] func			thread function object
	Thread(ThreadFunction& func);
	
	/// @param[in] cFunc		thread C function
	/// @param[in] userData		user data passed to C function
	Thread(void * (*cFunc)(void * userData), void * userData);

	~Thread();

	
	/// Set whether thread will automatically join upon destruction
	Thread& joinOnDestroy(bool v){ mJoinOnDestroy=v; return *this; }

	/// Start executing thread function
	bool start(ThreadFunction& func);
	
	/// Start executing thread C function with user data
	bool start(void * (*threadFunc)(void * userData), void * userData);

	/// Block the calling routine indefinitely until the thread terminates

	///	This function suspends execution of the calling routine until the thread has 
	///	terminated.  It will return immediately if the thread was already 
	///	terminated.  A \e true return value signifies successful termination. 
	///	A \e false return value indicates a problem with the wait call.
	bool join();
	
	/// Return pointer to current OS thread object
	
	/// E.g., if using pthreads internally, will return the pthread_t.
	///
	static void * current();

protected:
	class Impl;
	Impl * mImpl;
	CThreadFunction mCFunc;
	bool mJoinOnDestroy;
};



/// Multiple threads acting as a single work unit
template <class ThreadFunction>
class Threads{
public:

	/// A thread and function
	struct Worker{
		Thread thread;
		ThreadFunction function;
	};


	/// @param[in] size		number of worker threads
	Threads(int size)
	:	mWorkers(0)
	{
		resize(size);
	}
	
	~Threads(){ clear(); }

	/// Returns number of workers
	int size() const { return mSize; }

	/// Resize number of workers
	void resize(int n){
		if(n != size()){
			mSize = n;
			clear();
			mWorkers = new Worker[size()];
		}
	}

	/// Start all worker threads
	void start(bool joinAll=true){
		for(int i=0; i<size(); ++i){
			thread(i).start(function(i));
		}
		if(joinAll) join();
	}
	
	/// Join all worker threads
	void join(){
		for(int i=0; i<size(); ++i) thread(i).join();
	}
	
	/// Get a worker
	Worker& worker(int i){ return mWorkers[i]; }
	
	/// Get a worker thread
	Thread& thread(int i){ return worker(i).thread; }
	
	/// Get a worker thread function
	ThreadFunction& function(int i){ return worker(i).function; }

	/// Get worker sub-interval range of a full interval [min, max)
	
	/// This is useful for determining how to break up for loops into 
	/// sub-intervals. E.g., if the full loop interval is [ 0, N ), then the 
	/// ith worker's interval is [ range(N)*i, range(N)*(i+1) ).
	template <class T>
	double range(T max, T min=T(0)){
		return (max-min) / double(size());
	}
	
	/// Get worker sub-interval of a full interval [min, max)
	
	/// @param[out] interval	two-element array containing endpoints of sub-interval
	/// @param[in]  i			worker index
	/// @param[in]  max			full interval max endpoint
	/// @param[in]  min			full interval min endpoint
	template <class T>
	void getInterval(T * interval, int i, T max, T min = T(0)){
		double diam = range(max,min);
		double left = diam * i + min;
		interval[0] = left;
		interval[1] = left + diam;
	}

protected:
	int mSize;
	Worker * mWorkers;
	
	void clear(){ if(mWorkers) delete[] mWorkers; }
};




// -----------------------------------------------------------------------------
// Inline implementation

inline bool Thread::start(void * (*threadFunc)(void * userData), void * userData){
	mCFunc.func = threadFunc;
	mCFunc.user = userData;
	return start(mCFunc);
}

} // al::

#endif


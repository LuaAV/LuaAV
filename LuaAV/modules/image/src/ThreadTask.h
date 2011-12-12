#ifndef LUA_IMAGE_THREAD_TASK_H
#define LUA_IMAGE_THREAD_TASK_H 1

#include <pthread.h>
#include <semaphore.h>

template<class T>
class ThreadTask {
public:
	ThreadTask()
	:	active(false),
		blocking(true)
	{
		pthread_mutex_init(&mMutex, NULL); 
	}
	
	~ThreadTask() {
		stop();
		pthread_mutex_destroy(&mMutex); 
	}
	
	// possible not getting destroyed if in process of task and ~Image() gets called??
	static void * thread(void * ptr) {
		T *data	= (T *)ptr;
		if(data && data->o) {
			data->o->task();
		}
		data->active = false;
		// pthread_exit
		return 0;
	}
	
	void start(T *data) { 
		if(active){ 
			return; 
		} 

		// have to put this here because the thread can be running 
		// before the call to create it returns 
		active = true; 
		pthread_create(&mThread, NULL, thread, (void *)data); 
	}
	
	void clear() {
		active = false;
		mThread = 0;
	}
	
	void stop() { 
		if(active) {
			pthread_detach(mThread); 
			active = false; 
		}
	}

	// returns false if it can't lock
	bool lock() { 
		if(blocking) { 
			pthread_mutex_lock(&mMutex); 
		}
		else { 
			int value = pthread_mutex_trylock(&mMutex); 
			if(value == 0) {} 
			else { 
				return false; 
			} 
		} 
		return true; 
	} 

	bool unlock() { 
		pthread_mutex_unlock(&mMutex);
		return true; 
	}

protected:
	pthread_t        mThread;
	pthread_mutex_t  mMutex;
	
	bool active;
	bool blocking;
};

#endif	// LUA_IMAGE_THREAD_TASK_H
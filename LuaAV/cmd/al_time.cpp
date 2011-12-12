#include "al_time.h"

#define al_nsec2sec(ns)		(((al_sec)(ns)) * al_time_ns2s)
#define al_sec2nsec(s)		((al_nsec)(s * al_time_s2ns))

/* Windows */
#ifdef AL_WIN32
	#include <windows.h>

	/* singleton object to force init/quit of timing */
	namespace allo{
		namespace{
			struct TimeSingleton{
				TimeSingleton(){ timeBeginPeriod(1); }
				~TimeSingleton(){ timeEndPeriod(1); }
			};		
			static TimeSingleton timeSingleton;
		}
	}

	al_sec al_time() {
		return timeGetTime();
	}
	
	al_nsec al_time_nsec() {
		return al_sec2nsec(timeGetTime());
	}
	
	void al_sleep(al_sec v) {
		Sleep((DWORD)(v * 1.0e3));
	}
	
	void al_sleep_nsec(al_nsec v) {
		Sleep((DWORD)(v / (al_nsec)1e6));
	}
	
/* Posix (Mac, Linux) */
#else
	#include <sys/time.h>
	#include <time.h>
	
	al_sec al_time() {
		timeval t;
		gettimeofday(&t, NULL);	
		return (al_sec)t.tv_sec + (((al_sec)t.tv_usec) * 1.0e-6);
	}
	
	al_nsec al_time_nsec() {
		timeval t;
		gettimeofday(&t, NULL);	
		return al_sec2nsec(t.tv_sec) + (al_nsec)(t.tv_usec * 1000);
	}
	
	void al_sleep(al_sec v) {
		time_t sec = (time_t)v;
		al_nsec nsec = al_time_s2ns * (v - (al_sec)sec);
		timespec tspec = { sec, nsec }; 
		while (nanosleep(&tspec, &tspec) == -1)
			continue;
	}
	
	void al_sleep_nsec(al_nsec v) {
		al_sleep((al_sec)v * al_time_ns2s);
	}
	
#endif /* platform specific */

void al_sleep_until(al_sec target) {
	al_sec dt = target - al_time();
	if (dt > 0) al_sleep(dt);
}

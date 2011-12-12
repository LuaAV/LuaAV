#include "allocore/system/al_Time.hpp"

#define al_nsec2sec(ns)		(((al_sec)(ns)) * al_time_ns2s)
#define al_sec2nsec(s)		((al_nsec)(s * al_time_s2ns))

#define AL_TIME_USE_APR 1
#ifdef AL_TIME_USE_APR

#ifdef AL_LINUX
	#include "apr-1.0/apr_time.h"
#else
	#include "apr-1/apr_time.h"
#endif
	/*
	APR API documentation
	--------------------------------------------------------------------------------
	typedef apr_int64_t apr_time_t
	number of microseconds since 00:00:00 january 1, 1970 UTC
	--------------------------------------------------------------------------------
	void apr_sleep	(	apr_interval_time_t 	t	 )
	Sleep for the specified number of micro-seconds.

	Parameters:
	t 	desired amount of time to sleep.
	Warning:
	May sleep for longer than the specified time.
	--------------------------------------------------------------------------------
	*/

	al_sec al_time() {
		const apr_time_t t = apr_time_now();	// microseconds
		return ((al_sec)t) * 1.0e-6;			// 1 s / 1e6 us
	}

	al_nsec al_time_nsec() {
		const apr_time_t t = apr_time_now();	// microseconds
		return ((al_nsec)t) * 1e3;				// 1000 ns / 1 us
	}

	void al_sleep(al_sec v) {
		apr_interval_time_t t = (apr_interval_time_t)(v * 1.0e6);
		apr_sleep(t);
	}

	void al_sleep_nsec(al_nsec v) {
		apr_interval_time_t t = v * 1e-3;
		apr_sleep(t);
	}


#elif defined(AL_WIN32)
	/* Windows */
	#include <windows.h>

	/* singleton object to force init/quit of timing */
	namespace al{
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

#else
	/* Posix (Mac, Linux) */
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






namespace al {

void DelayLockedLoop :: setBandwidth(double bandwidth) {
	double F = 1./tperiod;		// step rate
	double omega = M_PI * 2.8 * bandwidth/F;
	mB = omega * sqrt(2.);	// 1st-order weight
	mC = omega * omega;		// 2nd-order weight
}

void DelayLockedLoop :: step(al_sec realtime) {
	if (mReset) {
		// The first iteration sets initial conditions.

		// init loop
		t2 = tperiod;
		t0 = realtime;
		t1 = t0 + t2;	// t1 is ideally the timestamp of the next block start

		// subsequent iterations use the other branch:
		mReset = false;
	} else {
		// read timer and calculate loop error
		// e.g. if t1 underestimated, terr will be
		al_sec terr = realtime - t1;
		// update loop
		t0 = t1;				// 0th-order (distance)
		t1 += mB * terr + t2;	// integration of 1st-order (velocity)
		t2 += mC * terr;		// integration of 2nd-order (acceleration)
	}

//		// now t0 is the current system time, and t1 is the estimated system time at the next step
//		//
//		al_sec tper_estimate = t1-t0;	// estimated real duration between this step & the next one
//		double factor = tperiod/tper_estimate;	// <1 if we are too slow, >1 if we are too fast
//		double real_rate = 1./tper_estimate;
//		al_sec tper_estimate2 = t2;	// estimated real duration between this step & the next one
//		double factor2 = 1./t2;	// <1 if we are too slow, >1 if we are too fast
//		printf("factor %f %f rate %f\n", factor, factor2, real_rate);
}

} // al::

#ifndef INCLUDE_AL_TIME_HPP
#define INCLUDE_AL_TIME_HPP

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

/*
	C++ helper wrappers for al_time
*/

#include "allocore/system/al_Time.h"

namespace al{

/// Sleep for an interval of seconds
inline void wait(al_sec dt){ al_sleep(dt); }

/// Get current wall time in seconds
inline al_sec walltime(){ return al_time(); }

/// Timer with stopwatch-like functionality for benchmarking, etc.
class Timer {
public:
	Timer(): mStart(0), mStop(0){}

	al_nsec elapsed(){ return mStop - mStart; }					///< Returns nsec between start() and stop() calls
	al_sec elapsedSec(){ return al_time_ns2s * elapsed(); }		///< Returns  sec between start() and stop() calls
	void start(){ mStart=al_time_nsec(); }							///< Set start time as current time
	void stop(){ mStop=al_time_nsec(); }								///< Set stop time as current time

private:
	al_nsec mStart, mStop;	// start and stop times
};



/* 
	Helper object for events intended to run at a particular rate/period,
		but which might be subject to drift, latency and jitter.
	A curve mapping logical to real time can be drawn over an event period 
		by interpolating between t0 and t1
	@see http://www.kokkinizita.net/papers/usingdll.pdf
*/
class DelayLockedLoop {
public:
	
	DelayLockedLoop(al_sec step_period, double bandwidth = 0.5) {
		tperiod = step_period;
		setBandwidth(bandwidth);
		mReset = true;
	}
	
	// degree of smoothing
	void setBandwidth(double bandwidth);
	
	// call this after an xrun: will reset the timing adjustments
	void reset() { mReset = true; }
	
	// trigger this from the periodic event
	// realtime is the source time that we are trying to smoothly follow
	void step(al_sec realtime);
	
	// retrieve the current period/rate estimation (smoothed) 
	al_sec period_smoothed() const { return t2; }
	al_sec rate_smoothed() const { return 1./t2; }
	// retrieve the ideal period/rate
	al_sec period_ideal() const { return tperiod; }
	al_sec rate_ideal() const { return 1./tperiod; }
	
	// returns an estimate of corresponding real-time between current event & next
	// by linear interpolation of current event timestamp & projected next event timestamp
	al_sec realtime_interp(double alpha) const { return t0 + alpha*(t1-t0); }

protected:
	al_sec tperiod;	// event period in seconds
	al_sec t0;		// 0th-order component: timestamp of the current event
	al_sec t1;		// 1st-order component: ideally the timestamp of the next event
	al_sec t2;		// 2nd-order component (akin to acceleration, or smoothed event period)
	double mB, mC;	// 1st & 2nd order weights	
	bool mReset;
};





} // al::

#endif /* INCLUDE_AL_TIME_CPP_H */


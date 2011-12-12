#ifndef OF_CONSTANTS
#define OF_CONSTANTS



//-------------------------------
#define OF_VERSION			("OF_0.03")
//-------------------------------

#define OF_LOOP_NONE					0x01
#define OF_LOOP_PALINDROME				0x02
#define OF_LOOP_NORMAL					0x03


//-------------------------------
//  find the system type --------
//-------------------------------

// 		helpful:
// 		http://www.ogre3d.org/docs/api/html/OgrePlatform_8h-source.html

#if defined( __WIN32__ ) || defined( _WIN32 )
	#define TARGET_WIN32
#elif defined( __APPLE_CC__)
	#define TARGET_OSX
	#define TARGET_OS_MAC 1
#else
    #ifndef TARGET_LINUX
        #define TARGET_LINUX
    #endif
#endif
//-------------------------------


// then the the platform specific includes:
#ifdef TARGET_WIN32
	//this is for TryEnterCriticalSection
	//http://www.zeroc.com/forums/help-center/351-ice-1-2-tryentercriticalsection-problem.html
	#ifndef _WIN32_WINNT
		#   define _WIN32_WINNT 0x0600
	#endif
	#define __WINDOWS_DS__
	#define __WINDOWS_MM__
	#if (_MSC_VER)       // microsoft visual studio
		#pragma warning(disable : 4996)     // disable all deprecation warnings
		#pragma warning(disable : 4068)     // unknown pragmas
		#pragma warning(disable : 4101)     // unreferenced local variable
		#pragma	warning(disable : 4312)		// type cast conversion (in qt vp)
		#pragma warning(disable : 4311)		// type cast pointer truncation (qt vp)
		#pragma warning(disable : 4018)		// signed/unsigned mismatch (since vector.size() is a size_t)
	#endif

	#define TARGET_LITTLE_ENDIAN			// intel cpu

	// #define WIN32_HIGH_RES_TIMING

	// note: this is experimental!
	// uncomment to turn this on (only for windows machines)
	// if you want to try setting the timer to be high resolution
	// this could make camera grabbing and other low level
	// operations quicker, but you must quit the app normally,
	// ie, using "esc", rather than killing the process or closing
	// the console window in order to set the timer resolution back
	// to normal (since the high res timer might give the OS
	// problems)
	// info: http://www.geisswerks.com/ryan/FAQS/timing.html

#endif

#ifdef TARGET_OSX
	#ifndef __MACOSX_CORE__
		#define __MACOSX_CORE__
	#endif
	#include <unistd.h>

	#if defined(__LITTLE_ENDIAN__)
		#define TARGET_LITTLE_ENDIAN		// intel cpu
	#endif
#endif

#ifdef TARGET_LINUX
        #include <unistd.h>

	#if defined(__LITTLE_ENDIAN__)
		#define TARGET_LITTLE_ENDIAN		// intel cpu
	#endif

        // some things for serial compilation:
        #define B14400	14400
        #define B28800	28800


#endif

#ifndef __MWERKS__
#define OF_EXIT_APP(val)		exit(val);
#else
#define OF_EXIT_APP(val)		std::exit(val);
#endif




//------------------------------------------------ capture
// if are linux

#ifdef TARGET_LINUX

    #define OF_VIDEO_CAPTURE_V4L

#else

    // non - linux, pc or osx

    // comment out this following line, if you'd like to use the
    // quicktime capture interface on windows
    // if not, we default to videoInput library for
    // direct show capture...

    #define OF_SWITCH_TO_DSHOW_FOR_WIN_VIDCAP

    #ifdef OF_SWITCH_TO_DSHOW_FOR_WIN_VIDCAP
        #ifdef TARGET_OSX
            #define OF_VIDEO_CAPTURE_QUICKTIME
        #else
            #define OF_VIDEO_CAPTURE_DIRECTSHOW
        #endif
    #else
        // all quicktime, all the time
        #define OF_VIDEO_CAPTURE_QUICKTIME
    #endif
#endif


#ifdef TARGET_LINUX
    #ifndef OF_VIDEO_PLAYER_FOBS
        #define OF_VIDEO_PLAYER_FOBS
    #endif
#else
	#define OF_VIDEO_PLAYER_QUICKTIME
#endif



// core: ---------------------------
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include <string>
#include <sstream>  //for ostringsream
#include <iomanip>  //for setprecision
using namespace std;

#ifndef PI
	#define PI       3.14159265358979323846
#endif

#ifndef TWO_PI
	#define TWO_PI   6.28318530717958647693
#endif

#ifndef M_TWO_PI
	#define M_TWO_PI   6.28318530717958647693
#endif

#ifndef FOUR_PI
	#define FOUR_PI 12.56637061435917295385
#endif

#ifndef HALF_PI
	#define HALF_PI  1.57079632679489661923
#endif

#ifndef DEG_TO_RAD
	#define DEG_TO_RAD (PI/180.0)
#endif

#ifndef RAD_TO_DEG
	#define RAD_TO_DEG (180.0/PI)
#endif

#ifndef MIN
	#define MIN(x,y) (((x) < (y)) ? (x) : (y))
#endif

#ifndef MAX
	#define MAX(x,y) (((x) > (y)) ? (x) : (y))
#endif

#ifndef CLAMP
	#define CLAMP(val,min,max) (MAX(MIN(val,max),min))
#endif

#ifndef ABS
	#define ABS(x) (((x) < 0) ? -(x) : (x))
#endif

enum ofLogLevel{
	OF_LOG_VERBOSE,
	OF_LOG_NOTICE,
	OF_LOG_WARNING,
	OF_LOG_ERROR,
	OF_LOG_FATAL_ERROR,
	OF_LOG_SILENT	//this one is special and should always be last - set ofSetLogLevel to OF_SILENT to not recieve any messages
};

#define 	OF_FILLED				0x01
#define 	OF_OUTLINE				0x02
#define 	OF_WINDOW 				0
#define 	OF_FULLSCREEN 			1
#define 	OF_GAME_MODE			2

#define 	OF_RECTMODE_CORNER				0
#define 	OF_RECTMODE_CENTER				1


// these are straight out of glu, but renamed and included here
// for convenience
//
// we don't mean to wrap the whole glu library (or any other library for that matter)
// but these defines are useful to give people flexability over the polygonizer
//
// some info:
// http://glprogramming.com/red/images/Image128.gif
//
// also: http://glprogramming.com/red/chapter11.html
// (CSG ideas)

#define 	OF_POLY_WINDING_ODD 	          100130
#define 	OF_POLY_WINDING_NONZERO           100131
#define 	OF_POLY_WINDING_POSITIVE          100132
#define 	OF_POLY_WINDING_NEGATIVE          100133
#define		OF_POLY_WINDING_ABS_GEQ_TWO       100134

#define 	OF_CLOSE						  (true)


//--------------------------------------------
//
// 	Keyboard definitions
//
// 	ok -- why this?
// 	glut key commands have some annoying features,
// 	in that some normal keys have the same value as special keys,
// 	but we want ONE key routine, so we need to redefine several,
// 	so that we get some normalacy across keys routines
//
// 	(everything that comes through "glutSpecialKeyFunc" will get 256 added to it,
// 	to avoid conflicts, before, values like "left, right up down" (ie, 104, 105, 106) were conflicting with
// 	letters.. now they will be 256 + 104, 256 + 105....)


#define OF_KEY_MODIFIER 	0x0100

#define OF_KEY_BACKSPACE	8
#define OF_KEY_RETURN		13
#define OF_KEY_PRINTSCR		127

// zach - there are more of these keys, we can add them here...
// these are keys that are not coming through "special keys"
// via glut, but just other keys on your keyboard like
//

#define OF_KEY_F1			(GLUT_KEY_F1 | OF_KEY_MODIFIER)
#define OF_KEY_F2			(GLUT_KEY_F2 | OF_KEY_MODIFIER)
#define OF_KEY_F3			(GLUT_KEY_F3 | OF_KEY_MODIFIER)
#define OF_KEY_F4			(GLUT_KEY_F4 | OF_KEY_MODIFIER)
#define OF_KEY_F5			(GLUT_KEY_F5 | OF_KEY_MODIFIER)
#define OF_KEY_F6			(GLUT_KEY_F6 | OF_KEY_MODIFIER)
#define OF_KEY_F7			(GLUT_KEY_F7 | OF_KEY_MODIFIER)
#define OF_KEY_F8			(GLUT_KEY_F8 | OF_KEY_MODIFIER)
#define OF_KEY_F9			(GLUT_KEY_F9 | OF_KEY_MODIFIER)
#define OF_KEY_F10			(GLUT_KEY_F10 | OF_KEY_MODIFIER)
#define OF_KEY_F11			(GLUT_KEY_F11 | OF_KEY_MODIFIER)
#define OF_KEY_F12			(GLUT_KEY_F12 | OF_KEY_MODIFIER)

#define OF_KEY_LEFT			(GLUT_KEY_LEFT | OF_KEY_MODIFIER)
#define OF_KEY_UP			(GLUT_KEY_UP | OF_KEY_MODIFIER)
#define OF_KEY_RIGHT		(GLUT_KEY_RIGHT | OF_KEY_MODIFIER)
#define OF_KEY_DOWN			(GLUT_KEY_DOWN | OF_KEY_MODIFIER)
#define OF_KEY_PAGE_UP		(GLUT_KEY_PAGE_UP | OF_KEY_MODIFIER)
#define OF_KEY_PAGE_DOWN	(GLUT_KEY_PAGE_DOWN | OF_KEY_MODIFIER)
#define OF_KEY_HOME			(GLUT_KEY_HOME | OF_KEY_MODIFIER)
#define OF_KEY_END			(GLUT_KEY_END | OF_KEY_MODIFIER)
#define OF_KEY_INSERT		(GLUT_KEY_INSERT | OF_KEY_MODIFIER)


//--------------------------------------------



#endif

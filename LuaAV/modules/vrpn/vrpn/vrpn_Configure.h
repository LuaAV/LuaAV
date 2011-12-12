#ifndef	VRPN_CONFIGURE_H

//--------------------------------------------------------------
/* This file contains configuration options for VRPN.  The first
   section has definition lines that can be commented in or out
   at build time.  The second session has automaticly-generated
   directives and should not be edited. */
//--------------------------------------------------------------

//--------------------------------------------------------//
// EDIT BELOW THIS LINE FOR NORMAL CONFIGURATION SETTING. //
//--------------------------------------------------------//

//-----------------------
// Default port to listen on for a server.  It used to be 4500
// up through version 6.03, but then all sorts of VPNs started
// using this, as did Microsoft.  Port 3883 was assigned to VRPN
// by the Internet Assigned Numbers Authority (IANA) October, 2003.
// Change this to make a location-specific default if you like.
// The parentheses are to keep it from being expanded into something
// unexpected if the code has a dot after it.
#define	vrpn_DEFAULT_LISTEN_PORT_NO (3883)

//-----------------------
// Instructs VRPN to expose the vrpn_gettimeofday() function also
// as gettimeofday() so that external programs can use it.  This
// has no effect on any system that already has gettimeofday()
// defined, and is put here for Windows.  This function should
// not really be implemented within VRPN, but it was expedient to
// include it when porting applications to Windows.  Turn this
// off if you have another implementation, or if you want to call
// vrpn_gettimeofday() directly.
#define	VRPN_EXPORT_GETTIMEOFDAY

//-----------------------
// Tells VRPN to compile with support for the Message-Passing
// Interface (MPI) library.  There is a configuration section below
// that has a library path for the MPI library to link against.
// You will need to add the path to mpi.h and other needed files
// into your Visual Studio Tools/Options/Projects and Solutions/
// C++ Directories include path.  The original implementation is
// done with MPICH2, but an attempt has been made to use only
// MPI version 1 basic functions.
//#define	vrpn_USE_MPI

//-----------------------
// Instructs VRPN to use phantom library to construct a unified
// server, using phantom as a common device, and phantom
// configuration in .cfg file.
//#define	VRPN_USE_PHANTOM_SERVER

//------------------------
// Instructs vrpn to use SensAble's HDAPI rather than GHOST library.
// Only used in conjuntion with VRPN_USE_PHANTOM_SERVER.
// PLEASE SPECIFY PATH TO HDAPI IN NEXT SECTION IF YOU USE THIS.
// Also, you need to go to the vrpn_phantom and vrpn_server projects
// and remove the GHOST include directories from the include paths.
// Yes, HDAPI fails if it even has them in the path (as so many other
// things also fail).  At least we're rid of them now.  When you
// uncomment it (to use GHOST), add the following to the include
// directories for the vrpn_phantom project: $(SYSTEMDRIVE)\Program Files\SensAble\GHOST\v4.0\include,$(SYSTEMDRIVE)\Program Files\SensAble\GHOST\v4.0\external\stl,
#define VRPN_USE_HDAPI

//------------------------
// Instructs vrpn to use Ghost 3.1 instead of Ghost 3.4.
// Only used in conjuntion with VRPN_USE_PHANTOM_SERVER.
// PLEASE SPECIFY PATH TO GHOSTLIB IN NEXT SECTION IF YOU USE THIS
// (This is expected to be used on systems where Ghost 4.0 is not
// available, such as the SGI platform.  If you are using this on
// a Windows PC with Visual Studio, you will need to alter
// server_src/vrpn_phantom.dsp to reference the Ghost 3.1 include
// paths.)
// #define VRPN_USE_GHOST_31

//-----------------------
// Instructs VRPN to use the high-performance timer code on
// Windows, rather than the default clock which has an infrequent
// update.  At one point in the past, an implementation of this
// would only work correctly on some flavors of Windows and with
// some types of CPUs.
// There are actually two implementations
// of the faster windows clock.  The original one, made by Hans
// Weber, checks the clock rate to see how fast the performance
// clock runs (it takes a second to do this when the program
// first calls vrpn_gettimeofday()).  The second version by Haris
// Fretzagias relies on the timing supplied by Windows.  To use
// the second version, also define VRPN_WINDOWS_CLOCK_V2.
#define	VRPN_UNSAFE_WINDOWS_CLOCK
#define	VRPN_WINDOWS_CLOCK_V2

//-----------------------
// Instructs VRPN library and server to include code that uses
// the DirectX SDK.  If you set this, you may to edit the
// system configuration section below to point at the correct version
// of DirectX.  WARNING: With the August 2006 DirectX SDK, you
// cannot link against the debug library in Visual Studio 6.0,
// only the release.  Hopefully, Visual Studio.NET doesn't have
// this problem.
// IMPORTANT!  If you define this, you need to edit the Tools/Options
// menu:
//    For Visual studio 6, use the Directories tab, and add the
// include and lib paths to the TOP of the lists for all configurations.
//    For Visual studio .NET, add to the top of the Projects and Solutions/
//  VC++ Directories entry.
//    This will let the code find the right version when it compiles.
//#define	VRPN_USE_DIRECTINPUT

//-----------------------
// Instructs VRPN library and server to include code that uses
// the DirectShow SDK.  If you set this, you may to edit the
// system configuration section below to point at the correct version
// of the Platform SDK.  WARNING: With the August 2006 DirectX SDK, you
// cannot link against the debug library in Visual Studio 6.0,
// only the release.  Visual Studio.NET doesn't have this problem.
//#define	VRPN_USE_DIRECTSHOW

//-----------------------
// Instructs the VRPN server to create an entry for the Adrienne
// time-code generator.  This is a device that produces time values
// from an analog video stream so that events in the virtual world
// can be synchronized with events on a movie.  The Adrienne folder
// should be located at the same level as the VRPN folder for the
// code to find it.
//#define	VRPN_INCLUDE_TIMECODE_SERVER

//-----------------------
// Compiles the InterSense Tracker using the
// InterSense Interface Libraries SDK (tested for version
// 3.45) on windows.  This should work with all Intersense trackers,
// both the USB and the serial port versions.  The files isense.h,
// types.h and isense.c should be put in a directory called 'isense'
// at the same level as the vrpn directory.  The isense.dll should
// be put either in Windows/system32 or in the location where the
// executable lives or somewhere on the path.
//#define VRPN_INCLUDE_INTERSENSE

//-----------------------
// Instructs VRPN library and server to include code that uses
// the National Instruments Nidaq libary to control analog outputa.
// Later in this file, we also instruct the compiler to link with
// the National Instruments libraries if this is defined.  Either or
// both of these can be defined, depending on which library you
// need to use.
//#define	VRPN_USE_NATIONAL_INSTRUMENTS
//#define	VRPN_USE_NATIONAL_INSTRUMENTS_MX

//-----------------------
// Instructs VRPN library and server to include code that uses
// the US Digital SEI/A2 libary to control analog inputs from the
// A2 absolute encoder.
// Later in this file, we also instruct the compiler to link with
// the US Digital library if this is defined.  You also need to
// define VRPN_USE_NATIONAL_INSTRUMENTS_MX above if you want to
// use this.
//#define	VRPN_USE_USDIGITAL

//-----------------------
// Instructs VRPN to use the default room space transforms for
// the Desktop Phantom as used in the nanoManipulator application
// rather than the default world-origin with identity rotation.
// Please don't anyone new use the room space transforms built
// into VRPN -- they are a hack pulled forward from Trackerlib.
#define	DESKTOP_PHANTOM_DEFAULTS

//------------------------
// Instructs VRPN to use microscribe3D library to construct a unified
// server
//#define VRPN_USE_MICROSCRIBE

//------------------------
// Compiles the VRPN libary with the PhaseSpace Tracker using the 
// PhaseSpace OWL API on Linux and Windows.
//
// In Linux:
// The PhaseSpace header files (owl.h, etc) and libraries (libowlsock)
// should be placed in the phasespace directory at the same level as 
// the vrpn folder.  Also, PHASESPACE needs to be uncommented in the
// server_src/Makefile so that the libraries are properly linked.  
// libowlsock.so will need to be present in the directory of the
// final executable or in the default library path such as /usr/lib
//
// In Windows: 
// The PhaseSpace header files (owl.h, etc) should be placed in the 
// phasespace directory at the same level as the vrpn folder.
// libowlsock.lib will need to be located there as well.
// libowlsock.dll will need to be in the path or with the executable
// at run time.  Edit the path below to say where the .lib file
// can be found.
//
//#define VRPN_INCLUDE_PHASESPACE

//-----------------------
// Instructs VRPN to use a DLL interface on Windows systems.
// When using this, link with VRPNDLL.LIB (and VRPN.DLL) rather
// than VRPN.LIB in user code.  This is experimental and is
// under development to enable C# and other languages to pull in
// VRPN.  This is only needed when trying to link VRPN with
// languages other than C++ (and not even for Java).  If you don't
// have a good reason to, don't define it.
// Not implemented for .so-based Unix systems.
//#define VRPN_USE_SHARED_LIBRARY

//------------------------
// Instructs VRPN to use GPM Linux interface mouse interface.
// WARNING: If you define this, then you must also edit the server_src
// Makefile to include "-lgpm" into the SYSLIBS definition line for the
// architecture you use this on.  We had to change this because not all
// Linux releases included this library.
//#define VRPN_USE_GPM_MOUSE

//------------------------
// Instructs VRPN to use the GLI Interactive LLC MotionNode library to
// interface VRPN to their tracker.  If you do this, you must edit
// the include paths in the vrpn and vrpndll libraries to point to the
// correct locations and the lib path in vrpn_server and any other
// applications you build (including custom ones) to point to the right
// location.  You also have to have Boost (www.boost.org) installed and
// have pointed the vrpn and vrpndll project include paths to it and
// the vrpndll and vrpn_server lib paths to it.
// WARNING: This code does not compile under visual studio 6.0.
//#define VRPN_USE_MOTIONNODE

//------------------------------------------------------------------//
// SYSTEM CONFIGURATION SECTION                                     //
// EDIT THESE DEFINITIONS TO POINT TO OPTIONAL LIBRARIES.  THEY ARE //
// USED BELOW TO LOCATE LIBRARIES AND INCLUDE FILES.                //
//------------------------------------------------------------------//

#define VRPN_SYSTEMDRIVE "C:"

#define VRPN_PHASESPACE_LIB_PATH "../../phasespace/"

#define VRPN_HDAPI_PATH         VRPN_SYSTEMDRIVE "/Program Files/SensAble/3DTouch/lib/"
#define VRPN_HDAPI_UTIL_PATH    VRPN_SYSTEMDRIVE "/Program Files/SensAble/3DTouch/utilities/lib/"
#define VRPN_GHOST_31_PATH      VRPN_SYSTEMDRIVE "/Program Files/SensAble/GHOST/v3.1/lib/"
#define VRPN_GHOST_40_PATH      VRPN_SYSTEMDRIVE "/Program Files/SensAble/GHOST/v4.0/lib/"

#define VRPN_NIDAQ_PATH         VRPN_SYSTEMDRIVE "/Program Files/National Instruments/NI-DAQ/Lib/"
#define VRPN_NIDAQ_MX_PATH      VRPN_SYSTEMDRIVE "/Program Files/National Instruments/NI-DAQ/DAQmx ANSI C Dev/lib/msvc/"
#define VRPN_USDIGITAL_PATH     VRPN_SYSTEMDRIVE "/Program Files/SEI Explorer/"

#ifdef  vrpn_USE_MPI
#pragma comment (lib, VRPN_SYSTEMDRIVE "/Program Files/MPICH2/lib/mpi.lib")
#endif

// Load Adrienne libraries if we are using the timecode generator.
// If this doesn't match where you have installed these libraries,
// edit the following lines to point at the correct libraries.  Do
// this here rather than in the project settings so that it can be
// turned on and off using the definition above.
#ifdef	VRPN_INCLUDE_TIMECODE_SERVER
#pragma comment (lib, "../../Adrienne/AEC_DLL/AEC_NTTC.lib")
#endif

#ifdef VRPN_USE_MOTIONNODE
#pragma comment(lib, "libMotionNodeSDK.lib")
#endif

//---------------------------------------------------------------//
// DO NOT EDIT BELOW THIS LINE FOR NORMAL CONFIGURATION SETTING. //
//---------------------------------------------------------------//

// Load libowlsock.lib if we're using Phasespace.
#ifdef	VRPN_INCLUDE_PHASESPACE
#pragma comment (lib, VRPN_PHASESPACE_LIB_PATH "libowlsock.lib")
#endif

// Load VRPN Phantom library if we are using phantom server as unified server
// Load SensAble Technologies GHOST library to run the Phantom
// NOTE: The paths to these libraries are set in the Settings/Link tab of
// the various project files.  The paths to the include files are in the
// Settings/C++/preprocessor tab.
#ifdef VRPN_USE_PHANTOM_SERVER
  #ifdef VRPN_USE_HDAPI
    #pragma comment (lib, VRPN_HDAPI_PATH "hd.lib")
    #ifdef	_DEBUG
      #pragma comment (lib,VRPN_HDAPI_UTIL_PATH "hdud.lib")
    #else
      #pragma comment (lib,VRPN_HDAPI_UTIL_PATH "hdu.lib")
    #endif
    #pragma comment (lib, VRPN_HDAPI_PATH "hl.lib")
  #else
    #ifdef VRPN_USE_GHOST_31
      #pragma comment (lib,VRPN_GHOST_31_PATH "GHOST31.lib")
    #else
      #pragma comment (lib,VRPN_GHOST_40_PATH "GHOST40.lib")
    #endif
  #endif
#endif

// DirectInput include file and libraries.
// Load DirectX SDK libraries and tell which version we need if we are using it.
#ifdef	VRPN_USE_DIRECTINPUT
#define	DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#endif

#ifdef	VRPN_USE_DIRECTINPUT
#pragma comment (lib, "dxguid.lib")
#pragma comment (lib, "dxerr8.lib")
#pragma comment (lib, "dinput8.lib")
#endif

// Load National Instruments libraries if we are using them.
// If this doesn't match where you have installed these libraries,
// edit the following lines to point at the correct libraries.  Do
// this here rather than in the project settings so that it can be
// turned on and off using the definition above.
// NOTE: The paths to these libraries are set in the Settings/Link tab of
// the various project files.  The paths to the include files are in the
// Settings/C++/preprocessor tab.
#ifdef	VRPN_USE_NATIONAL_INSTRUMENTS
#pragma comment (lib, VRPN_NIDAQ_PATH "nidaq32.lib")
#pragma comment (lib, VRPN_NIDAQ_PATH "nidex32.lib")
#endif
#ifdef	VRPN_USE_NATIONAL_INSTRUMENTS_MX
#pragma comment (lib, VRPN_NIDAQ_MX_PATH "NIDAQmx.lib")
#endif

// Load US Digital libraries if we are using them.
// If this doesn't match where you have installed these libraries,
// edit the following lines to point at the correct libraries.  Do
// this here rather than in the project settings so that it can be
// turned on and off using the definition above.
// NOTE: The paths to these libraries are set in the Settings/Link tab of
// the various project files.  The paths to the include files are in the
// Settings/C++/preprocessor tab.
#ifdef  VRPN_USE_USDIGITAL
#pragma comment (lib, VRPN_USDIGITAL_PATH "SEIDrv32.lib")
#endif

// Load Microscribe-3D SDK libraries 
// If this doesn't match where you have installed these libraries,
// edit the following lines to point at the correct libraries.  Do
// this here rather than in the project settings so that it can be
// turned on and off using the definition above.
#ifdef        VRPN_USE_MICROSCRIBE
#pragma comment (lib, "armdll32.lib")
#endif

// This will be defined in the VRPN (non-DLL) project and nothing else
// Overrides USE_SHARED_LIBRARY to get rid of "inconsistent DLL linkage" warnings.
#ifdef VRPNDLL_NOEXPORTS
#undef VRPN_USE_SHARED_LIBRARY
#endif

// This will be defined in the VRPN (DLL) project and nothing else
// Forces "USE_SHARED_LIBRARY independent of definition above so that the
// DLL will build
#if defined(VRPNDLL_EXPORTS) && !defined(VRPN_USE_SHARED_LIBRARY)
#define VRPN_USE_SHARED_LIBRARY
#endif

// For client code, make sure we add the proper library dependency to the linker
#ifdef _WIN32
#pragma comment (lib, "wsock32.lib")  // VRPN requires the Windows Sockets library.
#ifdef VRPN_USE_SHARED_LIBRARY
#ifdef VRPNDLL_EXPORTS
#define  VRPN_API		 __declspec(dllexport) 
#else
#define  VRPN_API		 __declspec(dllimport)
#pragma comment (lib, "vrpndll.lib")
#endif
#else
#ifndef VRPNDLL_NOEXPORTS
#pragma comment (lib, "vrpn.lib")     // We'll need the VRPN library
#endif
#define  VRPN_API
#endif
#define	 VRPN_CALLBACK	 __stdcall
#else
// In the future, other architectures may need their own sections
#define  VRPN_API
#define  VRPN_CALLBACK
#endif

#define	VRPN_CONFIGURE_H
#endif


/* libinotifytools/src/inotifytools/inotify.h.  Generated from inotify.h.in by configure.  */
#ifndef _INOTIFYTOOLS_INOTIFY_H
#define _INOTIFYTOOLS_INOTIFY_H 1

/* this is defined to 1 if <sys/inotify.h> works. */
#define SYS_INOTIFY_H_EXISTS_AND_WORKS 1


#ifdef SYS_INOTIFY_H_EXISTS_AND_WORKS
#include <sys/inotify.h>
#else
#include "inotify-nosys.h"
#endif // SYS_INOTIFY_H_EXISTS_AND_WORKS


#endif


/* include/llvm/Config/llvm-config.h.  Generated from llvm-config.h.in by configure.  */
/*===-- llvm/config/llvm-config.h - llvm configure variable -------*- C -*-===*/
/*                                                                            */
/*                     The LLVM Compiler Infrastructure                       */
/*                                                                            */
/* This file is distributed under the University of Illinois Open Source      */
/* License. See LICENSE.TXT for details.                                      */
/*                                                                            */
/*===----------------------------------------------------------------------===*/

/* This file enumerates all of the llvm variables from configure so that
   they can be in exported headers and won't override package specific
   directives.  This is a C file so we can include it in the llvm-c headers.  */

/* To avoid multiple inclusions of these variables when we include the exported
   headers and config.h, conditionally include these.  */
/* TODO: This is a bit of a hack.  */
#ifndef CONFIG_H

/* Installation directory for binary executables */
#define LLVM_BINDIR "/home/nicholas/2.8-final/Phase2/Release/llvmCore-2.8.install/bin"

/* Time at which LLVM was configured */
#define LLVM_CONFIGTIME "Sat Oct  9 13:59:42 PDT 2010"

/* Installation directory for data files */
#define LLVM_DATADIR "/home/nicholas/2.8-final/Phase2/Release/llvmCore-2.8.install/share/llvm"

/* Installation directory for documentation */
#define LLVM_DOCSDIR "/home/nicholas/2.8-final/Phase2/Release/llvmCore-2.8.install/share/doc/llvm"

/* Installation directory for config files */
#define LLVM_ETCDIR "/home/nicholas/2.8-final/Phase2/Release/llvmCore-2.8.install/etc/llvm"

/* Host triple we were built on */
#define LLVM_HOSTTRIPLE "x86_64-unknown-linux-gnu"

/* Installation directory for include files */
#define LLVM_INCLUDEDIR "/home/nicholas/2.8-final/Phase2/Release/llvmCore-2.8.install/include"

/* Installation directory for .info files */
#define LLVM_INFODIR "/home/nicholas/2.8-final/Phase2/Release/llvmCore-2.8.install/info"

/* Installation directory for libraries */
#define LLVM_LIBDIR "/home/nicholas/2.8-final/Phase2/Release/llvmCore-2.8.install/lib"

/* Installation directory for man pages */
#define LLVM_MANDIR "/home/nicholas/2.8-final/Phase2/Release/llvmCore-2.8.install/man"

/* Build multithreading support into LLVM */
#define LLVM_MULTITHREADED 1

/* LLVM architecture name for the native architecture, if available */
#define LLVM_NATIVE_ARCH X86

/* LLVM name for the native Target init function, if available */
#define LLVM_NATIVE_TARGET LLVMInitializeX86Target

/* LLVM name for the native TargetInfo init function, if available */
#define LLVM_NATIVE_TARGETINFO LLVMInitializeX86TargetInfo

/* LLVM name for the native AsmPrinter init function, if available */
#define LLVM_NATIVE_ASMPRINTER LLVMInitializeX86AsmPrinter

/* Define if this is Unixish platform */
#define LLVM_ON_UNIX 1

/* Define if this is Win32ish platform */
/* #undef LLVM_ON_WIN32 */

/* Define to path to circo program if found or 'echo circo' otherwise */
/* #undef LLVM_PATH_CIRCO */

/* Define to path to dot program if found or 'echo dot' otherwise */
/* #undef LLVM_PATH_DOT */

/* Define to path to dotty program if found or 'echo dotty' otherwise */
/* #undef LLVM_PATH_DOTTY */

/* Define to path to fdp program if found or 'echo fdp' otherwise */
/* #undef LLVM_PATH_FDP */

/* Define to path to Graphviz program if found or 'echo Graphviz' otherwise */
/* #undef LLVM_PATH_GRAPHVIZ */

/* Define to path to gv program if found or 'echo gv' otherwise */
/* #undef LLVM_PATH_GV */

/* Define to path to neato program if found or 'echo neato' otherwise */
/* #undef LLVM_PATH_NEATO */

/* Define to path to twopi program if found or 'echo twopi' otherwise */
/* #undef LLVM_PATH_TWOPI */

/* Installation prefix directory */
#define LLVM_PREFIX "/home/nicholas/2.8-final/Phase2/Release/llvmCore-2.8.install"

#endif

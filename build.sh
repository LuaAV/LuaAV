#!/bin/bash   

# debugging: 
set -x

ROOT=`pwd`
PLATFORM=`uname`
ARCH=`uname -m`
echo Building for $PLATFORM $ARCH from $ROOT

pushd src

# create FFI header
luajit h2ffi.lua av.h av_ffi_header

#clean
rm -f av
rm -f *.o
rm -f *.d

CC='g++'

$CC -c -x c++ -arch $ARCH -O3 -Wall -I$ALLOCOREPATH/build/include -fno-stack-protector av.cpp

$CC *.o -w -rdynamic -pagezero_size 10000 -image_base 100000000 -keep_private_externs -lluajit-5.1 -framework Carbon -framework Cocoa -framework CoreAudio -framework GLUT -framework OpenGL -o av
#-force_load $ALLOCOREPATH/build/lib/liballocore.a -force_load $ALLOCOREPATH/build/lib/liballoutil.a 

popd

cp src/av .
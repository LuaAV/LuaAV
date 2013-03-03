#!/bin/bash   

# debugging: 
set -x

ROOT=`pwd`
PLATFORM=`uname`
ARCH=`uname -m`
PRODUCT_NAME="av"
echo Building $PRODUCT_NAME for $PLATFORM $ARCH from $ROOT

pushd src

# create FFI header
luajit h2ffi.lua av.h av_ffi_header

# clean
rm -f $PRODUCT_NAME 
rm -f *.o
rm -f *.d

if [[ $PLATFORM == 'Darwin' ]]; then
	
	CC='g++'
	$CC -c -x c++ -arch $ARCH -O3 -Wall -I$ALLOCOREPATH/build/include -fno-stack-protector av.cpp

	$CC *.o -w -rdynamic -pagezero_size 10000 -image_base 100000000 -keep_private_externs -lluajit-5.1 -framework Carbon -framework Cocoa -framework CoreAudio -framework GLUT -framework OpenGL -o $PRODUCT_NAME 
#-force_load $ALLOCOREPATH/build/lib/liballocore.a -force_load $ALLOCOREPATH/build/lib/liballoutil.a 

elif [[ $PLATFORM == 'Linux' ]]; then
	
	CC='g++'
	CFLAGS="-O3 -Wall -fPIC -ffast-math -Wno-unknown-pragmas -MMD"
	DEFINES="-D_GNU_SOURCE -DEV_MULTIPLICITY=1 -DHAVE_GETTIMEOFDAY -D__LINUX_ALSA__"
	INCLUDEPATHS="-I/usr/local/include/luajit-2.0 -I/usr/include/luajit-2.0"
	LDFLAGS="-w -rdynamic -Wl,-E "
	LINKERPATHS="-L/usr/local/lib -L/usr/lib"
	#LIBRARIES="-lluajit-5.1 -lfreeimage -lGLEW -lGLU -lGL -lglut -lasound ../externs/libuv/libuv.a -lrt -lpthread"
	LIBRARIES="-lluajit-5.1 -lGLEW -lGLU -lGL -lglut -lrt -lpthread"
	
	$CC -c $CFLAGS $DEFINES $INCLUDEPATHS av.cpp
	$CC $LDFLAGS $LINKERPATHS $LIBRARIES -Wl,-whole-archive *.o -Wl,-no-whole-archive $LIBRARIES -o $PRODUCT_NAME

	
else

	echo "unknown platform" $PLATFORM
	
fi

popd

cp src/av .


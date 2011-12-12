RM= rm -f
OBJECTS = luaFileSystem.o

# Convenience platforms targets.
# PLATS= aix ansi bsd freebsd generic linux macosx mingw posix solaris
PLATS= linux macosx
MYCFLAGS=
MYLDFLAGS=

none:
	@echo "Please choose a platform:"
	@echo "   $(PLATS)"

all:	lfs
		mv lfs.so ../modules

clean:
	$(RM) $(OBJECTS)

macosx:
	$(MAKE) all MYCFLAGS="-fmessage-length=0 -pipe -Wno-trigraphs -fpascal-strings -fasm-blocks -Os -I/usr/local/include" MYLDFLAGS="-bundle -undefined dynamic_lookup"

linux:
	$(MAKE) all MYCFLAGS="-I/usr/include/lua5.1 -fPIC" MYLDFLAGS="-llua5.1 -shared"


lfs:	$(OBJECTS)
		g++ $(OBJECTS) -o lfs.so $(MYLDFLAGS)

luaFileSystem.o:	luaFileSystem.c
		g++ -x c $(MYCFLAGS) -c luaFileSystem.c



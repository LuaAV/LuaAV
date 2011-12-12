my-dir = $(dir $(lastword $(MAKEFILE_LIST)))
OS_MAKEFILES = $(shell ls Makefile.OS.*)

include Makefile.sourcefiles

OBJS := $(foreach FILE,$(LOCAL_SRC_FILES),$(FILE:.cpp=.o))

SNDFILE_INCLUDE = `pkg-config --cflags sndfile`
SNDFILE_LIB = `pkg-config --libs sndfile`

# -g
CXXFLAGS = -O3 -fPIC -Wall -I$(my-dir) $(SNDFILE_INCLUDE)

# figure out what platform we're on

ifndef OS
	OS=$(shell ./platform)
endif

ifeq (,$(findstring Makefile.OS.$(OS),$(OS_MAKEFILES)))

### Help message
all %:
	@echo OS is set to [$(OS)]
	@echo Please set the OS environment variable to one of the following:
	@ls Makefile.OS.*
	@echo for example:
	@echo '`OS=Linux-i386 make`'
	@echo or else make a Makefile.$(OS)

else

include Makefile.OS.$(OS)

all: platform $(PLATFORM_TARGETS)
 
platform: Makefile.OS.$(OS)
	@echo "Building for the $(OS) platform"
	@mkdir -p ../libs/$(OS)

clean:
	rm -rf $(LOCAL_MODULE).so *.d *.o me/rjdj/zengarden/*.class ../test/me/rjdj/zengarden/*.class ../ZenGarden.jar ../libs/$(OS)/*

libzengarden-static: ../libs/$(OS)/libzengarden.a

../libs/$(OS)/libzengarden.a: $(OBJS)
	$(AR) rcs $@ $(OBJS)

libjnizengarden: ../libs/$(OS)/libjnizengarden.$(JNI_EXTENSION)

../libs/$(OS)/libjnizengarden.$(JNI_EXTENSION): ./me/rjdj/zengarden/jnizengarden.cpp $(OBJS)
	$(call MAKE_SO, $@, $<, $(OBJS))

libzengarden: ../libs/$(OS)/libzengarden.$(SO_EXTENSION)

../libs/$(OS)/libzengarden.$(SO_EXTENSION): $(OBJS)
	$(call MAKE_SO, $@, , $(OBJS))

java-jar: ../ZenGarden.jar

../ZenGarden.jar: me/rjdj/zengarden/*.java
	javac -classpath ../junit-4.8.2.jar ./me/rjdj/zengarden/*.java ../test/me/rjdj/zengarden/*.java
	jar cfm ../ZenGarden.jar me/rjdj/zengarden/manifest me/rjdj/zengarden/*.class
	cd ../test && jar uf ../ZenGarden.jar me/rjdj/zengarden/*.class

endif

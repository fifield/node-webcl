#
# This file is part of WebCL.
# Project home page: http://webcl.nokiaresearch.com/
#

include ../buildconfig.inc

all: build-dynamic build-static

help:
	$(info )
	$(info Rules: build-dynamic build-static clean ultra-clean)
	$(info )
	@true


CXX ?= g++
AR ?= ar

ifeq ($(ENABLE_DEBUG), 1)
# Build settings for debug buld
DEBUG_FLAGS = -DCL_WRAPPER_ENABLE_DEBUG -g
else
DEBUG_FLAGS =
endif

ifeq ($(ENABLE_LOG), 1)
# If LOG_LEVEL_DEFAULT is not set, then it's set here as a fallback
LOG_LEVEL_DEFAULT ?= 1
LOG_FLAGS = -DCL_WRAPPER_ENABLE_LOG -DCL_WRAPPER_LOG_LEVEL_DEFAULT=$(LOG_LEVEL_DEFAULT)

 ifdef LOG_TO_FILE
 LOG_FLAGS += -DCL_WRAPPER_LOG_TO_FILE=$(LOG_TO_FILE)
 endif

else
LOG_FLAGS =
endif

ifeq ($(ENABLE_TRACK_FUNCTIONS), 1)
DEFINES += -DCL_WRAPPER_TRACK_FUNCTIONS
endif
ifeq ($(ENABLE_TRACK_ALLOCS), 1)
DEFINES += -DCL_WRAPPER_TRACK_ALLOCS
endif


# OpenCL-OpenGL interoperability
ifeq ($(ENABLE_GL_INTEROP), 1)
CL_WRAPPER_ENABLE_OPENGL_SUPPORT = -DCL_WRAPPER_ENABLE_OPENGL_SUPPORT
else
CL_WRAPPER_ENABLE_OPENGL_SUPPORT =
endif


# Fallback: 1.1
CL_VERSION_SUPPORT ?= 110
# Set OpenCL version support
CL_WRAPPER_CL_VERSION_SUPPORT = $(CL_VERSION_SUPPORT)



OPENCL_DEFINES =
OPENCL_INCLUDES =
OPENCL_CXXFLAGS =
OPENCL_LDFLAGS =
ifneq ($(ENABLE_GL_INTEROP),)
OPENCL_LDFLAGS += -l$(OPENCL_LIB)
endif


TARGET_PREFIX = /home/jfifield/node-workspace/node-webcl/build/
DEPS_PREFIX = .deps/
BUILD_PREFIX = .build/
SOURCES = clwrappercommon.cpp commandqueuewrapper.cpp contextwrapper.cpp \
 devicewrapper.cpp eventwrapper.cpp kernelwrapper.cpp memoryobjectwrapper.cpp \
 platformwrapper.cpp programwrapper.cpp samplerwrapper.cpp
OBJECTS = $(SOURCES:%.cpp=$(BUILD_PREFIX)%.o)
TARGET_NAME = clwrapper


DEFINES += $(OPENCL_DEFINES)
INCLUDES += $(OPENCL_INCLUDES) -I../include
CXXFLAGS += -pipe -Wall $(OPENCL_CXXFLAGS) $(DEBUG_FLAGS) $(LOG_FLAGS) $(CL_WRAPPER_ENABLE_OPENGL_SUPPORT)
LDFLAGS += -pipe $(OPENCL_LDFLAGS)

DEP = $(CXX) -MM


# Dependency files
$(DEPS_PREFIX)%.dep: %.cpp
	@if ! test -z "$(DEPS_PREFIX)"; then mkdir -p $(DEPS_PREFIX) >/dev/null 2>&1 ; fi ; true
	$(DEP) $(DEFINES) $(INCLUDES) $(CXXFLAGS) $< > $@

# Dependency files are only generated if the goal does not include "clean" or "help"
xblockdeps=
ifneq ($(findstring clean, $(MAKECMDGOALS)),) # findstring evaluates to empty if no clean
xblockdeps=true
endif
ifneq ($(findstring help, $(MAKECMDGOALS)),)
xblockdeps=true
endif
ifeq ($(xblockdeps),)
# Include a dependency file for each source file.
-include $(SOURCES:%.cpp=$(DEPS_PREFIX)%.dep)
endif


# Build targets
$(BUILD_PREFIX)%.o: %.cpp
	@if ! test -z "$(BUILD_PREFIX)"; then mkdir -p $(BUILD_PREFIX) >/dev/null 2>&1 ; fi ; true
	$(CXX) $*.cpp $(DEFINES) $(INCLUDES) $(CXXFLAGS) -c -o $@

build: build-dynamic

build-dynamic: $(TARGET_PREFIX)lib$(TARGET_NAME).so

build-static: $(TARGET_PREFIX)lib$(TARGET_NAME).a

$(TARGET_PREFIX)lib$(TARGET_NAME).so: $(OBJECTS)
	$(CXX) $^ -shared -fPIC -Wl,-Bsymbolic -Wl,-z,defs $(LDFLAGS) -o $@
	chmod +x $@
	strip $@

$(TARGET_PREFIX)lib$(TARGET_NAME).a: $(OBJECTS)
	$(AR) rcs $@ $^


clean:
	@rm -f $(DEPS_PREFIX)*.dep 2>/dev/null ; true
	@rm -f $(BUILD_PREFIX)*.o 2>/dev/null ; true
	@rm -f $(TARGET_PREFIX)lib$(TARGET_NAME).so 2>/dev/null ; true
	@rm -f $(TARGET_PREFIX)lib$(TARGET_NAME).a 2>/dev/null ; true

ultra-clean: clean
	@rm -f ./*~ 2>/dev/null ; true
	@if ! test -z "$(DEPS_PREFIX)"; then rmdir "$(DEPS_PREFIX)" 2>/dev/null ; fi ; true
	@if ! test -z "$(BUILD_PREFIX)"; then rmdir "$(BUILD_PREFIX)" 2>/dev/null ; fi ; true


.PHONY: help build build-dynamic build-static clean ultra-clean

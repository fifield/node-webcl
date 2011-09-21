# This file contains the build settings for CLWrapper.

# Build tools
CXX ?= g++
AR ?= ar


# Enable debug build.
# Set value to 1 to enable. Comment out or set any other value to disable.
ENABLE_DEBUG ?= 0

# Enable logging.
# Set value to 1 to enable. Comment out or set any other value to disable.
ENABLE_LOG ?= 0

# Enable logging to file. Comment out to disable.
# Note: value must be given within escaped quotes, e.g. \"log_file.txt\" .
#LOG_TO_FILE = \"clwrapper.log\"

# Default logging level, higher levels include lower level messages.
#   0: none
#   1: errors
#   2: warnings
#   3: info
#   4: debug
LOG_LEVEL_DEFAULT ?= 2

# Track function calls to log using D_METHOD_START.
# Set value to 1 to enable. Comment out or set any other value to disable.
export ENABLE_TRACK_FUNCTIONS ?= 0

# Track memory allocations to log
# Set value to 1 to enable. Comment out or set any other value to disable.
export ENABLE_TRACK_ALLOCS ?= 0


# Enable GL interoperability.
# Set value to 1 to enable. Comment out or set any other value to disable.
export ENABLE_GL_INTEROP ?= 0


# Set maximum CL version supported
#  OpenCL 1.0 => 100
#  OpenCL 1.1 => 110
#  ...
# This is mainly to enable building against OpenCL implementations that do not
# yet support recent versions. CLWrapper API depending on more recent OpenCL API
# than indicated by this setting should not be called and its functionality
# remains undefined (though generally function calls just fail with error
# CL_INVALID_VALUE).
CL_VERSION_SUPPORT ?= 110


# Set the base library name of the OpenCL library used.
# E.g. "libOpenCL.so" => "OpenCL"
OPENCL_LIB = OpenCL

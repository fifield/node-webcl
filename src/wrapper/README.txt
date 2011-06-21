-----------------------------------------------------------------------

               CLWrapper - C++ OpenCL Wrapper for WebCL

-----------------------------------------------------------------------

CLWrapper library provides a C++ API wrapper for OpenCL [1] C API. The design
goal is to provide a layer that enables the Nokia WebCL to be implemented
for web browsers using C++ with relative ease. The library provides object
life cycle management and a type-based mechanism for retrieving info
parameters as well as setting OpenCL kernel arguments.

Project home page: http://webcl.nokiaresearch.com/



LICENSING
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

CLWrapper is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License version 2.1 as published
by the Free Software Foundation. See included COPYING file for full license.

Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).



ACKNOWLEDGEMENTS
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

Contact: Jari Nikara  jari.nikara@nokia.com

Developers:
    Janne Pietiäinen
    Jari Nikara
    Tomi Aarnio
    Eero Aho



BUILDING
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

As a prerequisite, an OpenCL SDK (i.e. library and the header files) is
required. An SDK is available from e.g. AMD, NVIDIA and INTEL for use with
their respective hardware. An LGPL licensed sample implementation, CLEP [2],
can be used as well. In addition, Doxygen [3] is needed for generating
documentation from sources.

Before building the library, build settings in buildconfig.inc should be
reviewed and modified where necessary. These settings include debug build,
logging, limiting OpenCL version support and the name of OpenCL library, which
needs to be modified especially for CLEP.

The CLWrapper library is built using GNU Make [4] or compatible. Provided
Make targets are:

  build or build-dynamic:       Build a shared (dynamic) library
                                (libclwrapper.so).

  build-static:                 Build a static library (libclwrapper.a).

  clean:                        Clean build results.

  ultra-clean:                  Same as clean but in addition removes any files
                                ending with ~-character (assumed editor backups)
                                and the directories created for build results
                                and generated build dependency listings.

  docs                          Generate source documentation using Doxygen.

In general, it is sufficient to use the default target, which builds both
shared and static libraries. This is achieved by simply running Make:
  $ make

The following environment variables are honoured:
  CXX           The C++ compiler (default: g++). This is expected to be
                compatible with g++.
  AR            The tool used for creating static library (default: ar).
  DEFINES       Include macro predefinitions in compilation in gcc format
                (-DFOO=bar)
  INCLUDES      Add extra include directories.
  CXXFLAGS      Add extra compiler flags.
  LDFLAGS       Add extra linker flags.

For example, in case the OpenCL library requires additional library path it
can be set as follows:
  $ LDFLAGS=-L/usr/local/FooCL/lib make



REFERENCES
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

[1] http://www.khronos.org/opencl/
[2] http://gitorious.org/clep
[3] http://www.doxygen.org/
[4] http://www.gnu.org/software/make/


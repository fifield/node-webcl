#
# This file is part of WebCL.
# Project home page: http://webcl.nokiaresearch.com/
#

# CLWrapper top level Makefile.

all:
	cd src && make $@

help:
	cd src && make $@

build:
	cd src && make $@

build-dynamic:
	cd src && make $@

build-static:
	cd src && make $@

docs: doxygen.conf
	doxygen $<

clean:
	@rm -rf docs/html 2>/dev/null ; true
	cd src && make $@

ultra-clean:
	@rm -rf docs/html 2>/dev/null ; true
	@rm -f ./*~ include/*~ 2>/dev/null ; true
	cd src && make $@

.PHONY: all help build build-dynamic build-static docs clean ultra-clean

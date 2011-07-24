# Makefile for Python XBMC library interface

export EXTRA_COMPILE=$(CXXFLAGS) $(DEFINES) $(INCLUDES)

.PHONY: all build install clean

all: install

CLEAN_FILES=
PYTHON=python

include ../../Makefile.include

clean:
	$(PYTHON) setup.py clean -a

src/scraper.cpp: src/scraper.cpp.in src/defs.mako
	src/generate.py $< > $@

build: src/scraper.cpp
	$(PYTHON) setup.py build

install: src/scraper.cpp
	$(PYTHON) setup.py install


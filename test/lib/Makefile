# Makefile for Python XBMC library interface

export EXTRA_COMPILE=$(CXXFLAGS) $(DEFINES) $(INCLUDES)

.PHONY: all build install

all: install

CLEAN_FILES=
PYTHON=python

include ../../Makefile.include

clean:
	$(PYTHON) setup.py clean -a

build:
	$(PYTHON) setup.py build

install:
	$(PYTHON) setup.py install


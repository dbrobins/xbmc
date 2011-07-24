#!/usr/bin/python2

# Python bindings generator

import os, sys
from mako.template import Template
from mako.lookup import TemplateLookup

if len(sys.argv) != 2:
  raise RuntimeError('syntax: ' + sys.argv[0] + ' <input file>')

lookup = TemplateLookup(os.getcwd())  # allow relative includes

print Template(filename = sys.argv[1], lookup = lookup).render(),

# NOTE: output file is provided to avoid Mako dependency.

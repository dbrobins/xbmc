import os, re
from distutils.core import setup, Extension

extra_cxx = os.getenv('EXTRA_COMPILE', [])  # set in the Makefile
if extra_cxx:
  extra_cxx = extra_cxx.split()
  extra_cxx = filter(lambda x: not re.match("-D'", x), extra_cxx)  # bad quoted define
extra_cxx.append('-std=c++0x')

xbmc_lib = Extension('_load',
       sources = ['src/lib.cpp'],
       libraries = ['xbmc'],
       library_dirs = ['/usr/lib64/xbmc'],
       runtime_library_dirs = ['/usr/lib64/xbmc'],
       extra_compile_args = extra_cxx,
       )

xbmc_scraper = Extension('scraper',
       sources = ['src/scraper.cpp'],
       libraries = ['xbmc'],
       library_dirs = ['/usr/lib64/xbmc'],
       runtime_library_dirs = ['/usr/lib64/xbmc'],
       extra_compile_args = extra_cxx,
       )

setup (name = 'XBMC Library',
       version = '1.0',
       description = 'XBMC shared library access',
       author = 'David Robins',
       packages = ['xbmc', 'xbmc.lib'],
       package_dir = {'xbmc': 'ext'},
       ext_package = 'xbmc.lib',
       ext_modules = [xbmc_lib, xbmc_scraper],
       )

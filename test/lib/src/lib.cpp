// This is the package module for xbmc.lib._load.
// Its purpose to initialize libxbmc.
// It is wrapped by xbmc.lib (__init__.py) which ensures RTLD_GLOBAL is set.

#include "Python.h"

#include "xbmc.h"

PyMODINIT_FUNC
init_load()
{
  Py_InitModule("_load", NULL);
  XBMC_Init(grfxaNil, "python");
}

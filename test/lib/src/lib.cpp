// This is the package module for xbmc.lib (equivalent to xbmc/lib/__init__.py).
// Its purpose to initialize libxbmc.

#include "Python.h"

#include "xbmc.h"

PyMODINIT_FUNC
initlib()
{
  Py_InitModule("__init__", NULL);
  XBMC_Init(grfxaNil, "python");
}

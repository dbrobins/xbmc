import sys, ctypes

fix = hasattr(sys, 'getdlopenflags')
if fix:
  old = sys.getdlopenflags()
  sys.setdlopenflags(old | ctypes.RTLD_GLOBAL)

# load the library and make its symbols available
import xbmc.lib._load

if fix:
  sys.setdlopenflags(old)


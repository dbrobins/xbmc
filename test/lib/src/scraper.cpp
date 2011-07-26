// Implements xbmc.lib.scraper
//
// *** GENERATED CODE DO NOT EDIT ***
//


#include "Python.h"
#include "structmember.h"

#include "xbmc.h"
#include "addons/Addon.h"
#include "addons/Scraper.h"
#include "addons/AddonManager.h"
#include "filesystem/FileCurl.h"
#include "video/VideoInfoTag.h"

using namespace std;
using namespace ADDON;


// Python type objects

static PyTypeObject ptoTyp = { PyObject_HEAD_INIT(NULL) };  // type constants
static PyTypeObject ptoMps = { PyObject_HEAD_INIT(NULL) };  // string map
static PyTypeObject ptoSet = { PyObject_HEAD_INIT(NULL) };  // settings
static PyTypeObject ptoDep = { PyObject_HEAD_INIT(NULL) };  // dependencies
static PyTypeObject ptoScr = { PyObject_HEAD_INIT(NULL) };  // scraper
static PyTypeObject ptoSce = { PyObject_HEAD_INIT(NULL) };  // exception


// Empty object
struct EmO
{
  PyObject_HEAD
};

// Python scraper object
struct ScrO : EmO
{
  ScraperPtr psc;
};

// Scraper exception
struct SceO : PyBaseExceptionObject
{
  ~SceO() { delete psce; }
  const CScraperError *psce;
};

// Base type for sub-objects with pointer to scraper
struct ScrB : EmO
{
  ~ScrB() { Py_XDECREF(pscro); }
  ScrO *pscro;
};

// Base type for map
template <class T>
struct MpsB : ScrB
{
  const T *pmp;
  typename T::const_iterator i;
};

// Python immutable string map
struct MpsO : MpsB<InfoMap>
{
};

// Python settings object
struct SetO : MpsB<SETTINGS>
{
};

// Python dependencies object
struct DepO : MpsB<ADDONDEPS>
{
};


// Utility functions

// passed-in object assumed to be zero-initialized already
static void InitPto(PyTypeObject &pto, const char *sName, size_t cb, const char *sDoc)
{
  pto.tp_name = sName;
  pto.tp_basicsize = cb;
  pto.tp_doc = sDoc;
  pto.tp_new = PyType_GenericNew;
  pto.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;
}

// raise Python exception for scraper error; conveniently returns NULL
PyObject *PyoRaiseSce(const CScraperError &sce)
{
  // copy the error; it's going away
  SceO *psceo = (SceO *)PyObject_CallFunctionObjArgs((PyObject *)&ptoSce, NULL);
  psceo->psce = new CScraperError(sce); /*sce.FAborted() ? new CScraperError()
                               : new CScraperError(sce.Title(), sce.Message());XXX*/
  PyErr_SetObject((PyObject *)&ptoSce, (PyObject *)psceo);
  return NULL;
}

static inline int FAnyArgs(PyObject *args, PyObject *kwds)
{
    if (PyTuple_GET_SIZE(args) || (kwds && PyDict_Check(kwds) && PyDict_Size(kwds)))
    {
      PyErr_SetString(PyExc_RuntimeError, "callable takes no parameters");
      return true;
    }
    return false;
}


// Scraper object implementation

static int scr_init(ScrO *pscro, PyObject *args, PyObject *kwds)
{
  // parse arguments: scraper type (Type.T) or name (e.g., "metadata.tvdb.com")
  if (kwds && PyDict_Size(kwds))
  {
    PyErr_SetString(PyExc_TypeError, "unexpected keyword argument");
    return -1;
  }

  TYPE typ = ADDON_UNKNOWN;
  const char *sName = NULL;
  if (!PyArg_ParseTuple(args, "i", &typ))
  {
    if (!PyArg_ParseTuple(args, "s", &sName))
    {
      PyErr_SetString(PyExc_TypeError, "scraper type or name required");
      return -1;
    }
    PyErr_Clear();
  }

  // fetch the addon from the addon manager
  AddonPtr pao;
  if (sName)
  {
    if (!CAddonMgr::Get().GetAddon(sName, pao))
    {
      PyErr_SetString(PyExc_RuntimeError, "scraper not found");
      return -1;
    }
  }
  else if (!CAddonMgr::Get().GetDefault(typ, pao))
  {
    PyErr_SetString(PyExc_RuntimeError, "default scraper unavailable");
    return -1;
  }

  // cast to scraper and store
  pscro->psc = boost::dynamic_pointer_cast<CScraper>(pao);
  return 0;
}

template <class T>
static void foo_dealloc(T *pt)
{
  pt->~T();
}
#define pfn_dealloc(T) (void (*)(T *))(foo_dealloc)

static PyObject *scr_nfo_url(ScrO *pscro, PyObject *ppyo)
{
  // verify and convert passed-in string
  const char *sIn;
  Py_ssize_t cchIn;
  if (PyString_AsStringAndSize(ppyo, (char **)&sIn, &cchIn) < 0)
    return NULL;
  CStdString s(sIn, cchIn);

  try
  {
    CScraperUrl scurl = pscro->psc->NfoUrl(s);
    if (scurl.m_url.empty())
      Py_RETURN_NONE;
    //XXX return a proper object!
    CStdString &sRet = scurl.m_url.begin()->m_url;
    return PyString_FromStringAndSize(sRet, sRet.size());
  }
  catch(const CScraperError &sce)
  {
    return PyoRaiseSce(sce);
  }
}

//XXX need a way to set fcurl (globally, or in the object?)...

static PyMethodDef pmdScr[] = {
  {"nfo_url", (PyCFunction)scr_nfo_url, METH_O,
    "Given the contents of a .nfo file, returns an URL (None if none found)" },

/*
  {"find_movie", (PyCFunction)scr_find_movie, METH_VARARGS,
    "Find a movie given the title; replace special chars if clean is True" },
  {"find_album", (PyCFunction)scr_find_album, METH_VARARGS,
    "Find an album given the title and optional artist" },
  {"find_artist", (PyCFunction)scr_find_artist, METH_O,
    "Find an artist given a name" },

 * XXX really these should be called on the sub-object returned from the above...
 * ... art_details should store the search string on return so it can be autopassed
 * ... they can also be given fcurl and scurl will be implicit from the object
  {"episodes", (PyCFunction)vid_episodes, METH_NOARGS,
    "Get episode list" },
  {"details", (PyCFunction)vid_details, METH_NOARGS,
    "Get video details" },
  {"details", (PyCFunction)alb_details, METH_NOARGS,
    "Get album details" },
  {"details", (PyCFunction)art_details, METH_NOARGS,
    "Get artist details" },
*/
#if 0
  // scraper media functions

  std::vector<CScraperUrl> FindMovie(XFILE::CFileCurl &fcurl,
    const CStdString &sMovie, bool fFirst);
  std::vector<MUSIC_GRABBER::CMusicAlbumInfo> FindAlbum(XFILE::CFileCurl &fcurl,
    const CStdString &sAlbum, const CStdString &sArtist = "");
  std::vector<MUSIC_GRABBER::CMusicArtistInfo> FindArtist(
    XFILE::CFileCurl &fcurl, const CStdString &sArtist);
  EPISODELIST GetEpisodeList(XFILE::CFileCurl &fcurl, const CScraperUrl &scurl);

  bool GetVideoDetails(XFILE::CFileCurl &fcurl, const CScraperUrl &scurl,
    bool fMovie/*else episode*/, CVideoInfoTag &video);
  bool GetAlbumDetails(XFILE::CFileCurl &fcurl, const CScraperUrl &scurl,
    CAlbum &album);
  bool GetArtistDetails(XFILE::CFileCurl &fcurl, const CScraperUrl &scurl,
    const CStdString &sSearch, CArtist &artist);
#endif
  {NULL}  /* Sentinel */
};






static PyObject *scr_get_i_type(ScrO *scro, void *)
{
  return PyInt_FromLong(scro->psc->Type());
}




static PyObject *scr_get_s_id(ScrO *scro, void *)
{
  CStdString s = scro->psc->ID();
  return PyString_FromStringAndSize(s, s.size());
}




static PyObject *scr_get_s_name(ScrO *scro, void *)
{
  CStdString s = scro->psc->Name();
  return PyString_FromStringAndSize(s, s.size());
}




static PyObject *scr_get_f_enabled(ScrO *scro, void *)
{
  return PyBool_FromLong(scro->psc->Enabled());
}




static PyObject *scr_get_f_active(ScrO *scro, void *)
{
  return PyBool_FromLong(scro->psc->IsInUse());
}




static PyObject *scr_get_ver_ver(ScrO *scro, void *)
{
  // not comparable (maybe later?); just a simple string
  return PyString_FromString(scro->psc->Version().c_str());
}




static PyObject *scr_get_ver_min_ver(ScrO *scro, void *)
{
  // not comparable (maybe later?); just a simple string
  return PyString_FromString(scro->psc->MinVersion().c_str());
}




static PyObject *scr_get_s_summary(ScrO *scro, void *)
{
  CStdString s = scro->psc->Summary();
  return PyString_FromStringAndSize(s, s.size());
}




static PyObject *scr_get_s_desc(ScrO *scro, void *)
{
  CStdString s = scro->psc->Description();
  return PyString_FromStringAndSize(s, s.size());
}




static PyObject *scr_get_s_path(ScrO *scro, void *)
{
  CStdString s = scro->psc->Path();
  return PyString_FromStringAndSize(s, s.size());
}




static PyObject *scr_get_s_profile(ScrO *scro, void *)
{
  CStdString s = scro->psc->Profile();
  return PyString_FromStringAndSize(s, s.size());
}




static PyObject *scr_get_s_lib_path(ScrO *scro, void *)
{
  CStdString s = scro->psc->LibPath();
  return PyString_FromStringAndSize(s, s.size());
}




static PyObject *scr_get_s_changelog(ScrO *scro, void *)
{
  CStdString s = scro->psc->ChangeLog();
  return PyString_FromStringAndSize(s, s.size());
}




static PyObject *scr_get_s_fanart(ScrO *scro, void *)
{
  CStdString s = scro->psc->FanArt();
  return PyString_FromStringAndSize(s, s.size());
}




static PyObject *scr_get_s_author(ScrO *scro, void *)
{
  CStdString s = scro->psc->Author();
  return PyString_FromStringAndSize(s, s.size());
}




static PyObject *scr_get_s_icon(ScrO *scro, void *)
{
  CStdString s = scro->psc->Icon();
  return PyString_FromStringAndSize(s, s.size());
}




static PyObject *scr_get_i_rating(ScrO *scro, void *)
{
  return PyInt_FromLong(scro->psc->Stars());
}




static PyObject *scr_get_s_disclaimer(ScrO *scro, void *)
{
  CStdString s = scro->psc->Disclaimer();
  return PyString_FromStringAndSize(s, s.size());
}




static PyObject *scr_get_s_lang(ScrO *scro, void *)
{
  CStdString s = scro->psc->Language();
  return PyString_FromStringAndSize(s, s.size());
}




static PyObject *scr_get_f_has_settings(ScrO *scro, void *)
{
  return PyBool_FromLong(scro->psc->HasSettings());
}



static PyObject *scr_get_mps_extra_info(ScrO *pscro, void *)
{
  return PyObject_CallFunctionObjArgs((PyObject *)&ptoMps, pscro, NULL);
}

static PyObject *scr_get_set_settings(ScrO *pscro, void *)
{
  return PyObject_CallFunctionObjArgs((PyObject *)&ptoSet, pscro, NULL);
}

static PyObject *scr_get_dep_deps(ScrO *pscro, void *)
{
  return PyObject_CallFunctionObjArgs((PyObject *)&ptoDep, pscro, NULL);
}

static PyGetSetDef pgsScr[] = {


  {(char *)"type", (getter)scr_get_i_type, NULL, (char *)"Scraper type, from Type enumeration", NULL},
  {(char *)"id", (getter)scr_get_s_id, NULL, (char *)"Scraper id (dotted pseudo-domain)", NULL},
  {(char *)"name", (getter)scr_get_s_name, NULL, (char *)"Scraper display name", NULL},
  {(char *)"enabled", (getter)scr_get_f_enabled, NULL, (char *)"True if scraper is currently enabled", NULL},
  {(char *)"active", (getter)scr_get_f_active, NULL, (char *)"True if scraper is currently running", NULL},
  {(char *)"ver", (getter)scr_get_ver_ver, NULL, (char *)"Scraper version (NOT COMPARABLE)", NULL},
  {(char *)"min_ver", (getter)scr_get_ver_min_ver, NULL, (char *)"Minimum compatible version", NULL},
  {(char *)"summary", (getter)scr_get_s_summary, NULL, (char *)"Scraper summary", NULL},
  {(char *)"desc", (getter)scr_get_s_desc, NULL, (char *)"Scraper description", NULL},
  {(char *)"path", (getter)scr_get_s_path, NULL, (char *)"Path to scraper files", NULL},
  {(char *)"profile", (getter)scr_get_s_profile, NULL, (char *)"Path to profile-local data", NULL},
  {(char *)"lib_path", (getter)scr_get_s_lib_path, NULL, (char *)"Path to addon library", NULL},
  {(char *)"changelog", (getter)scr_get_s_changelog, NULL, (char *)"Scraper change log", NULL},
  {(char *)"fanart", (getter)scr_get_s_fanart, NULL, (char *)"Fan art path", NULL},
  {(char *)"author", (getter)scr_get_s_author, NULL, (char *)"Scraper developer", NULL},
  {(char *)"icon", (getter)scr_get_s_icon, NULL, (char *)"Path to scraper icon", NULL},
  {(char *)"rating", (getter)scr_get_i_rating, NULL, (char *)"Rating (0-5 stars)", NULL},
  {(char *)"disclaimer", (getter)scr_get_s_disclaimer, NULL, (char *)"Disclaimer text", NULL},
  {(char *)"lang", (getter)scr_get_s_lang, NULL, (char *)"Scraper language", NULL},
  {(char *)"has_settings", (getter)scr_get_f_has_settings, NULL, (char *)"True if scraper has custom settings", NULL},


  {(char *)"extra_info", (getter)scr_get_mps_extra_info, NULL, (char *)"Extra info dictionary", NULL},
  {(char *)"settings", (getter)scr_get_set_settings, NULL, (char *)"Addon settings", NULL},
  {(char *)"deps", (getter)scr_get_dep_deps, NULL, (char *)"Addon dependencies", NULL},
  {NULL}  /* Sentinel */
};


// String map object implementation

static bool FInitScrb(ScrB *pscrb, PyObject *args, PyObject *kwds)
{
  if (kwds && PyDict_Size(kwds))
  {
    PyErr_SetString(PyExc_TypeError, "unexpected keyword argument");
    return false;
  }

  ScrO *pscro = NULL;
  if (!PyArg_ParseTuple(args, "O", &pscro))
  {
    PyErr_SetString(PyExc_TypeError, "scraper object required");
      return false;
  }

  Py_INCREF(pscro);
  pscrb->pscro = pscro;
  return true;
}

static int mps_init(MpsO *pmpso, PyObject *args, PyObject *kwds)
{
  if (!FInitScrb(pmpso, args, kwds))
    return -1;
  pmpso->pmp = &pmpso->pscro->psc->ExtraInfo();
  return 0;
}

static PyObject *mps_iter(MpsO *pmpso)
{
  pmpso->i = pmpso->pmp->begin();
  Py_INCREF(pmpso);
  return (PyObject *)pmpso;
}

static PyObject *mps_iternext(MpsO *pmpso)
{
  if (pmpso->i == pmpso->pmp->end())
    return NULL;
  const CStdString &sKey = pmpso->i->first;
  const CStdString &sVal = pmpso->i->second;
  ++pmpso->i;
  return PyTuple_Pack(2,
   PyString_FromStringAndSize(sKey, sKey.size()),
   PyString_FromStringAndSize(sVal, sVal.size()));
}

static Py_ssize_t mps_length(MpsO *pmpso)
{
  return pmpso->pmp->size();
}

static PyObject *mps_subscript(MpsO *pmpso, PyObject *key)
{
  const char *s = PyString_AsString(key);
  if (!s)
    return NULL;
  InfoMap::const_iterator i = pmpso->pmp->find(s);
  if (i == pmpso->pmp->end())
  {
    PyErr_SetObject(PyExc_KeyError, key);
    return NULL;
  }
  return PyString_FromStringAndSize(i->second, i->second.size());
}

static PyMappingMethods pmmMps =
{
  (lenfunc)mps_length,
  (binaryfunc)mps_subscript,
  NULL  // no assignment
};


// Settings object implementation

static int set_init(SetO *pseto, PyObject *args, PyObject *kwds)
{
  if (!FInitScrb(pseto, args, kwds))
    return -1;
  pseto->pmp = &pseto->pscro->psc->GetSettings();
  return 0;
}

static int set_assign(SetO *pseto, PyObject *key, PyObject *val)
{
  const char *sKey, *sVal;
  Py_ssize_t cchKey, cchVal;
  if (PyString_AsStringAndSize(key, (char **)&sKey, &cchKey) < 0 ||
    PyString_AsStringAndSize(val, (char **)&sVal, &cchVal) < 0)
  {
    return -1;
  }
  pseto->pscro->psc->UpdateSetting(CStdString(sKey, cchKey), CStdString(sVal, cchVal));
  return 0;
}

static PyObject *set_save(SetO *pseto, PyObject *ppyo)
{
  pseto->pscro->psc->SaveSettings();
  Py_RETURN_NONE;
}

static PyMappingMethods pmmSet =
{
  (lenfunc)mps_length,
  (binaryfunc)mps_subscript,
  (objobjargproc)set_assign
};

static PyMethodDef pmdSet[] = {
  {"save", (PyCFunction)set_save, METH_NOARGS, "Save settings" },
  {NULL}  /* Sentinel */
};


// Dependencies object implementation

static int dep_init(DepO *pdepo, PyObject *args, PyObject *kwds)
{
  if (!FInitScrb(pdepo, args, kwds))
    return -1;
  pdepo->pmp = &pdepo->pscro->psc->GetDeps();
  return 0;
}

static PyObject *dep_iter(DepO *pdepo)
{
  pdepo->i = pdepo->pmp->begin();
  Py_INCREF(pdepo);
  return (PyObject *)pdepo;
}

static PyObject *PyoDepVal(const ADDONREQ &areq)
{
  return PyTuple_Pack(2, 
   PyString_FromString(areq.first.c_str()), PyBool_FromLong(areq.second));
}

static PyObject *dep_iternext(DepO *pdepo)
{
  if (pdepo->i == pdepo->pmp->end())
    return NULL;
  const CStdString &sKey = pdepo->i->first;
  const ADDONREQ &areq = pdepo->i->second;
  ++pdepo->i;
  return PyTuple_Pack(2,
   PyString_FromStringAndSize(sKey, sKey.size()), PyoDepVal(areq));
}

static Py_ssize_t dep_length(DepO *pdepo)
{
  return pdepo->pmp->size();
}

static PyObject *dep_subscript(DepO *pdepo, PyObject *key)
{
  const char *s = PyString_AsString(key);
  if (!s)
    return NULL;
  ADDONDEPS::const_iterator i = pdepo->pmp->find(s);
  if (i == pdepo->pmp->end())
  {
    PyErr_SetObject(PyExc_KeyError, key);
    return NULL;
  }
  return PyoDepVal(i->second);
}

static PyMappingMethods pmmDep =
{
  (lenfunc)dep_length,
  (binaryfunc)dep_subscript,
  NULL  // no assignment
};


// Scraper exception object

static int sce_init(SceO *psceo, PyObject *args, PyObject *kwds)
{
  if (FAnyArgs(args, kwds))
    return -1;
  return Py_TYPE(psceo)->tp_base->tp_init((PyObject *)psceo, args, kwds);
}

static PyObject *sce_repr(SceO *psceo)
{
  if (psceo->psce->FAborted())
    return PyString_FromString("ScraperError: scraper aborted!");
  return PyString_FromFormat("ScraperError: %s, %s",
   psceo->psce->Title().c_str(), psceo->psce->Message().c_str());
}






static PyObject *sce_get_f_aborted(SceO *sceo, void *)
{
  return PyBool_FromLong(sceo->psce->FAborted());
}




static PyObject *sce_get_s_title(SceO *sceo, void *)
{
  CStdString s = sceo->psce->Title();
  return PyString_FromStringAndSize(s, s.size());
}




static PyObject *sce_get_s_message(SceO *sceo, void *)
{
  CStdString s = sceo->psce->Message();
  return PyString_FromStringAndSize(s, s.size());
}



static PyGetSetDef pgsSce[] = {


  {(char *)"aborted", (getter)sce_get_f_aborted, NULL, (char *)"Did the scraper abort?", NULL},
  {(char *)"title", (getter)sce_get_s_title, NULL, (char *)"Error dialog title", NULL},
  {(char *)"message", (getter)sce_get_s_message, NULL, (char *)"Error message", NULL},


  {NULL}  /* Sentinel */
};

// Module initialization

PyMODINIT_FUNC
initscraper()
{
  PyImport_ImportModule("xbmc.lib");
  // Unfortunately we have to leak this reference; there's no module cleanup
  // function, and Python APIs can't be called from Py_AtExit.

  InitPto(ptoTyp, "xbmc.scraper.Type", sizeof(EmO), "Scraper types");
  ptoTyp.tp_flags = Py_TPFLAGS_IS_ABSTRACT;

  InitPto(ptoMps, "xbmc.scraper.StringMap", sizeof(MpsO), "Immutable string map");
  ptoMps.tp_flags |= Py_TPFLAGS_HAVE_ITER;
  ptoMps.tp_init = (initproc)mps_init;
  ptoMps.tp_dealloc = (destructor)pfn_dealloc(MpsO);
  ptoMps.tp_as_mapping = &pmmMps;
  ptoMps.tp_iter = (getiterfunc)mps_iter;
  ptoMps.tp_iternext = (iternextfunc)mps_iternext;

  InitPto(ptoSet, "xbmc.scraper.Settings", sizeof(SetO), "Addon settings");
  ptoSet.tp_flags |= Py_TPFLAGS_HAVE_ITER;
  ptoSet.tp_init = (initproc)set_init;
  ptoSet.tp_dealloc = (destructor)pfn_dealloc(SetO);
  ptoSet.tp_as_mapping = &pmmSet;
  ptoSet.tp_iter = (getiterfunc)mps_iter;
  ptoSet.tp_iternext = (iternextfunc)mps_iternext;
  ptoSet.tp_methods = pmdSet;

  InitPto(ptoDep, "xbmc.scraper.Dependencies", sizeof(DepO), "Addon dependencies");
  ptoDep.tp_flags |= Py_TPFLAGS_HAVE_ITER;
  ptoDep.tp_init = (initproc)dep_init;
  ptoDep.tp_dealloc = (destructor)pfn_dealloc(DepO);
  ptoDep.tp_as_mapping = &pmmDep;
  ptoDep.tp_iter = (getiterfunc)dep_iter;
  ptoDep.tp_iternext = (iternextfunc)dep_iternext;

  InitPto(ptoScr, "xbmc.scraper.Scraper", sizeof(ScrO), "Scraper interface");
  ptoScr.tp_init = (initproc)scr_init;
  ptoScr.tp_dealloc = (destructor)pfn_dealloc(ScrO);
  ptoScr.tp_methods = pmdScr;
  ptoScr.tp_getset = pgsScr;

  InitPto(ptoSce, "xbmc.scraper.Error", sizeof(SceO), "Scraper error or abort");
  ptoSce.tp_base = (PyTypeObject *)PyExc_BaseException;
  ptoSce.tp_new = (newfunc)ptoSce.tp_base->tp_new;  // our fields are zero-inited
  ptoSce.tp_init = (initproc)sce_init;
  ptoSce.tp_dealloc = (destructor)pfn_dealloc(SceO);
  ptoSce.tp_repr = (reprfunc)sce_repr;
  ptoSce.tp_getset = pgsSce;

  if (PyType_Ready(&ptoTyp) < 0 || PyType_Ready(&ptoMps) < 0 || PyType_Ready(&ptoSet) ||
    PyType_Ready(&ptoDep) < 0 || PyType_Ready(&ptoScr) < 0 || PyType_Ready(&ptoSce) < 0)
  {
    return;
  }

  // Type.T scraper type enumeration
  PyDict_SetItemString(ptoTyp.tp_dict, "ALBUMS", PyInt_FromLong(ADDON_SCRAPER_ALBUMS));
  PyDict_SetItemString(ptoTyp.tp_dict, "ARTISTS", PyInt_FromLong(ADDON_SCRAPER_ARTISTS));
  PyDict_SetItemString(ptoTyp.tp_dict, "MOVIES", PyInt_FromLong(ADDON_SCRAPER_MOVIES));
  PyDict_SetItemString(ptoTyp.tp_dict, "MUSICVIDEOS", PyInt_FromLong(ADDON_SCRAPER_MUSICVIDEOS));
  PyDict_SetItemString(ptoTyp.tp_dict, "TVSHOWS", PyInt_FromLong(ADDON_SCRAPER_TVSHOWS));

  PyObject *mod = Py_InitModule("scraper", NULL);
  if (mod == NULL)
    return;

  Py_INCREF(&ptoScr);
  Py_INCREF(&ptoTyp);
  Py_INCREF(&ptoMps);
  Py_INCREF(&ptoSet);
  Py_INCREF(&ptoDep);
  Py_INCREF(&ptoSce);
  PyModule_AddObject(mod, "Scraper", (PyObject *)&ptoScr);
  PyModule_AddObject(mod, "Type", (PyObject *)&ptoTyp);
  PyModule_AddObject(mod, "StringMap", (PyObject *)&ptoMps);
  PyModule_AddObject(mod, "Settings", (PyObject *)&ptoSet);
  PyModule_AddObject(mod, "Dependencies", (PyObject *)&ptoDep);
  PyModule_AddObject(mod, "Error", (PyObject *)&ptoSce);
}


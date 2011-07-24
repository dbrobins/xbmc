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


// Empty object
// - used by type container (ptoTyp)
struct EmO
{
  PyObject_HEAD
};

// Python scraper object
struct ScrO
{
  PyObject_HEAD
  ScraperPtr psc;
};

// Python immutable string map
struct MpsO
{
  PyObject_HEAD
  ScrO *pscro;
  const InfoMap *pmp;
  InfoMap::const_iterator i;
};

// Python settings object
struct SetO
{
  PyObject_HEAD
  PyObject *pscro;
  SETTINGS *pset;
};

// Python dependencies object
struct DepO
{
  PyObject_HEAD
  PyObject *pscro;
  ADDONDEPS *pdep;
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

// raise Python exception for scraper error
PyObject *PyoRaiseSce(const CScraperError &sce)
{
  //XXX get our own exception
  if (sce.FAborted())
    PyErr_SetString(PyExc_RuntimeError, "scraper aborted");
  else
    PyErr_SetString(PyExc_RuntimeError, "CScraperError");
  return NULL;
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

static void scr_dealloc(ScrO *pscro)
{
  pscro->~ScrO();
}

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
//also need a scraper object
//and a scraper exception/abort (two separate Python exceptions?)

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

 * XXX really these should be called on the object returned from the above...
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




static PyObject *scr_get_i_type(ScrO *pscro, void *)
{
  return PyInt_FromLong(pscro->psc->Type());
}




static PyObject *scr_get_s_id(ScrO *pscro, void *)
{
  CStdString s = pscro->psc->ID();
  return PyString_FromStringAndSize(s, s.size());
}




static PyObject *scr_get_s_name(ScrO *pscro, void *)
{
  CStdString s = pscro->psc->Name();
  return PyString_FromStringAndSize(s, s.size());
}




static PyObject *scr_get_f_enabled(ScrO *pscro, void *)
{
  return PyBool_FromLong(pscro->psc->Enabled());
}




static PyObject *scr_get_f_active(ScrO *pscro, void *)
{
  return PyBool_FromLong(pscro->psc->IsInUse());
}




static PyObject *scr_get_ver_ver(ScrO *pscro, void *)
{
  // not comparable (maybe later?); just a simple string
  fprintf(stderr, "get version [%s]\n", pscro->psc->Version().c_str());
  return PyString_FromString(pscro->psc->Version().c_str());
}




static PyObject *scr_get_ver_min_ver(ScrO *pscro, void *)
{
  // not comparable (maybe later?); just a simple string
  fprintf(stderr, "get version [%s]\n", pscro->psc->MinVersion().c_str());
  return PyString_FromString(pscro->psc->MinVersion().c_str());
}




static PyObject *scr_get_s_summary(ScrO *pscro, void *)
{
  CStdString s = pscro->psc->Summary();
  return PyString_FromStringAndSize(s, s.size());
}




static PyObject *scr_get_s_desc(ScrO *pscro, void *)
{
  CStdString s = pscro->psc->Description();
  return PyString_FromStringAndSize(s, s.size());
}




static PyObject *scr_get_s_path(ScrO *pscro, void *)
{
  CStdString s = pscro->psc->Path();
  return PyString_FromStringAndSize(s, s.size());
}




static PyObject *scr_get_s_profile(ScrO *pscro, void *)
{
  CStdString s = pscro->psc->Profile();
  return PyString_FromStringAndSize(s, s.size());
}




static PyObject *scr_get_s_lib_path(ScrO *pscro, void *)
{
  CStdString s = pscro->psc->LibPath();
  return PyString_FromStringAndSize(s, s.size());
}




static PyObject *scr_get_s_changelog(ScrO *pscro, void *)
{
  CStdString s = pscro->psc->ChangeLog();
  return PyString_FromStringAndSize(s, s.size());
}




static PyObject *scr_get_s_fanart(ScrO *pscro, void *)
{
  CStdString s = pscro->psc->FanArt();
  return PyString_FromStringAndSize(s, s.size());
}




static PyObject *scr_get_s_author(ScrO *pscro, void *)
{
  CStdString s = pscro->psc->Author();
  return PyString_FromStringAndSize(s, s.size());
}




static PyObject *scr_get_s_icon(ScrO *pscro, void *)
{
  CStdString s = pscro->psc->Icon();
  return PyString_FromStringAndSize(s, s.size());
}




static PyObject *scr_get_i_rating(ScrO *pscro, void *)
{
  return PyInt_FromLong(pscro->psc->Stars());
}




static PyObject *scr_get_s_disclaimer(ScrO *pscro, void *)
{
  CStdString s = pscro->psc->Disclaimer();
  return PyString_FromStringAndSize(s, s.size());
}




static PyObject *scr_get_s_lang(ScrO *pscro, void *)
{
  CStdString s = pscro->psc->Language();
  return PyString_FromStringAndSize(s, s.size());
}




static PyObject *scr_get_f_has_settings(ScrO *pscro, void *)
{
  return PyBool_FromLong(pscro->psc->HasSettings());
}



static PyObject *scr_get_mps_extra_info(ScrO *pscro, void *)
{
  return (PyObject *)PyObject_CallFunctionObjArgs((PyObject *)&ptoMps, pscro, NULL);
}

static PyGetSetDef pgsScr[] = {

  {"type", (getter)scr_get_i_type, NULL, "Scraper type, from Type enumeration", NULL},
  {"id", (getter)scr_get_s_id, NULL, "Scraper id (dotted pseudo-domain)", NULL},
  {"name", (getter)scr_get_s_name, NULL, "Scraper display name", NULL},
  {"enabled", (getter)scr_get_f_enabled, NULL, "True if scraper is currently enabled", NULL},
  {"active", (getter)scr_get_f_active, NULL, "True if scraper is currently running", NULL},
  {"ver", (getter)scr_get_ver_ver, NULL, "Scraper version (NOT COMPARABLE)", NULL},
  {"min_ver", (getter)scr_get_ver_min_ver, NULL, "Minimum compatible version", NULL},
  {"summary", (getter)scr_get_s_summary, NULL, "Scraper summary", NULL},
  {"desc", (getter)scr_get_s_desc, NULL, "Scraper description", NULL},
  {"path", (getter)scr_get_s_path, NULL, "Path to scraper files", NULL},
  {"profile", (getter)scr_get_s_profile, NULL, "Path to profile-local data", NULL},
  {"lib_path", (getter)scr_get_s_lib_path, NULL, "Path to addon library", NULL},
  {"changelog", (getter)scr_get_s_changelog, NULL, "Scraper change log", NULL},
  {"fanart", (getter)scr_get_s_fanart, NULL, "Fan art path", NULL},
  {"author", (getter)scr_get_s_author, NULL, "Scraper developer", NULL},
  {"icon", (getter)scr_get_s_icon, NULL, "Path to scraper icon", NULL},
  {"rating", (getter)scr_get_i_rating, NULL, "Rating (0-5 stars)", NULL},
  {"disclaimer", (getter)scr_get_s_disclaimer, NULL, "Disclaimer text", NULL},
  {"lang", (getter)scr_get_s_lang, NULL, "Scraper language", NULL},
  {"has_settings", (getter)scr_get_f_has_settings, NULL, "True if scraper has custom settings", NULL},

  {"extra_info", (getter)scr_get_mps_extra_info, NULL, "Extra info dictionary", NULL},
  {NULL}  /* Sentinel */
};


// String map (std::map<CStdString, CStdString>) object implementation

static int mps_init(MpsO *pmpso, PyObject *args, PyObject *kwds)
{
  if (kwds && PyDict_Size(kwds))
  {
    PyErr_SetString(PyExc_TypeError, "unexpected keyword argument");
    return -1;
  }

  ScrO *pscro = NULL;
  if (!PyArg_ParseTuple(args, "O", &pscro))
  {
    PyErr_SetString(PyExc_TypeError, "scraper object required");
      return -1;
  }

  Py_INCREF(pscro);
  pmpso->pscro = pscro;
  pmpso->pmp = &pscro->psc->ExtraInfo();
  return 0;
}

static void mps_dealloc(MpsO *pmpso)
{
  Py_XDECREF(pmpso->pscro);
  pmpso->~MpsO();
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
  ++pmpso->i;
  return PyString_FromStringAndSize(sKey, sKey.size());
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
    Py_RETURN_NONE;
  return PyString_FromStringAndSize(i->second, i->second.size());
}

PyMappingMethods pmmMps =
{
  (lenfunc)mps_length,
  (binaryfunc)mps_subscript,
  NULL  // no assignment
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
  ptoMps.tp_dealloc = (destructor)mps_dealloc;
  ptoMps.tp_as_mapping = &pmmMps;
  ptoMps.tp_iter = (getiterfunc)mps_iter;
  ptoMps.tp_iternext = (iternextfunc)mps_iternext;

  InitPto(ptoSet, "xbmc.scraper.Settings", sizeof(SetO), "Addon settings");
  ptoSet.tp_flags = Py_TPFLAGS_IS_ABSTRACT;//XXX

  InitPto(ptoDep, "xbmc.scraper.Dependencies", sizeof(DepO), "Addon dependencies");
  ptoDep.tp_flags = Py_TPFLAGS_IS_ABSTRACT;//XXX

  InitPto(ptoScr, "xbmc.scraper.Scraper", sizeof(ScrO), "Scraper interface");
  ptoScr.tp_init = (initproc)scr_init;
  ptoScr.tp_dealloc = (destructor)scr_dealloc;
  ptoScr.tp_methods = pmdScr;
  ptoScr.tp_getset = pgsScr;

  if (PyType_Ready(&ptoTyp) < 0 || PyType_Ready(&ptoMps) < 0 || PyType_Ready(&ptoSet) ||
    PyType_Ready(&ptoDep) < 0 || PyType_Ready(&ptoScr) < 0)
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
  PyModule_AddObject(mod, "Scraper", (PyObject *)&ptoScr);
  PyModule_AddObject(mod, "Type", (PyObject *)&ptoTyp);
  PyModule_AddObject(mod, "StringMap", (PyObject *)&ptoMps);
  PyModule_AddObject(mod, "Settings", (PyObject *)&ptoSet);
  PyModule_AddObject(mod, "Dependencies", (PyObject *)&ptoDep);
}


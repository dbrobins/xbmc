// Implements xbmc.lib.scraper

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

//XXX sample code; delete me!
static void test()
{
  AddonPtr pao;
  if (!CAddonMgr::Get().GetDefault(ADDON_SCRAPER_MOVIES, pao))
  {
    fprintf(stderr, "Failed to get default movie scraper.\n");
    return;
  }
  ScraperPtr psc = boost::dynamic_pointer_cast<CScraper>(pao);

  XFILE::CFileCurl fcurl;
  vector<CScraperUrl> vcscurl = psc->FindMovie(fcurl, "Ben-Hur", true/*fFirst*/);
  printf("Results (%d):\n", (int)vcscurl.size());
  for (auto i = vcscurl.begin(); i != vcscurl.end(); ++i)
  {
    printf("\tTitle: %s (id: %s)\n", i->strTitle.c_str(), i->strId.c_str());
    if (i->m_url.size() != 1)
      printf("\t(%d items)\n", (int)i->m_url.size());
    else
      printf("\t%s\n", i->m_url[0].m_url.c_str());
  }

  // details for the first...
  CVideoInfoTag vidtag;
  if (vcscurl.empty() || !psc->GetVideoDetails(fcurl, vcscurl[0], true/*fMovie*/, vidtag))
    printf("GetVideoDetails failed or no matches\n");
  else
  {
    printf("Got details: title %s, genre %s; plot:\n%s\n",
      vidtag.m_strTitle.c_str(), vidtag.m_strGenre.c_str(), vidtag.m_strPlot.c_str());
  }
}

// Python scraper object
struct ScrO
{
  PyObject_HEAD
  ScraperPtr psc;
};

static int scr_init(ScrO *pscro, PyObject *args, PyObject *kwds)
{
  // XXX for now always return the default movie scraper and ignore args
  AddonPtr pao;
  if (!CAddonMgr::Get().GetDefault(ADDON_SCRAPER_MOVIES, pao))
  {
    PyErr_SetString(PyExc_RuntimeError, "default scraper unavailable");
    return -1;
  }
  pscro->psc = boost::dynamic_pointer_cast<CScraper>(pao);
  return 0;
}

static void scr_dealloc(ScrO *pscro)
{
  pscro->~ScrO();
}

static PyMethodDef pmdScr[] = {
/*XXX
  {"name", (PyCFunction)Noddy_name, METH_NOARGS,
    "Return the name, combining the first and last name" },
*/
  {NULL}  /* Sentinel */
};

static PyMemberDef pddScr[] = {
/*XXX
  {"number", T_INT, offsetof(Noddy, number), 0,
    "noddy number"},
*/
  {NULL}  /* Sentinel */
};

static PyTypeObject ptoScr =
{
  PyObject_HEAD_INIT(NULL)
  0,                         /*ob_size*/
  "xbmc.scraper.Scraper",    /*tp_name*/
  sizeof(ScrO),              /*tp_basicsize*/
  0,                         /*tp_itemsize*/
  (destructor)scr_dealloc,   /*tp_dealloc*/
  0,                         /*tp_print*/
  0,                         /*tp_getattr*/
  0,                         /*tp_setattr*/
  0,                         /*tp_compare*/
  0,                         /*tp_repr*/
  0,                         /*tp_as_number*/
  0,                         /*tp_as_sequence*/
  0,                         /*tp_as_mapping*/
  0,                         /*tp_hash */
  0,                         /*tp_call*/
  0,                         /*tp_str*/
  0,                         /*tp_getattro*/
  0,                         /*tp_setattro*/
  0,                         /*tp_as_buffer*/
  Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
  "XBMC scraper interface",  /* tp_doc */
  0,                         /* tp_traverse */
  0,                         /* tp_clear */
  0,                         /* tp_richcompare */
  0,                         /* tp_weaklistoffset */
  0,                         /* tp_iter */
  0,                         /* tp_iternext */
  pmdScr,                    /* tp_methods */
  pddScr,                    /* tp_members */
  0,                         /* tp_getset */
  0,                         /* tp_base */
  0,                         /* tp_dict */
  0,                         /* tp_descr_get */
  0,                         /* tp_descr_set */
  0,                         /* tp_dictoffset */
  (initproc)scr_init,        /* tp_init */
  0,                         /* tp_alloc */
  PyType_GenericNew          /* tp_new */
};

PyMODINIT_FUNC
initscraper()
{
  PyImport_ImportModule("xbmc.lib");
  // Unfortunately we have to leak this reference; there's no module cleanup
  // function, and Python APIs can't be called from Py_AtExit.

  if (PyType_Ready(&ptoScr) < 0)
    return;

  PyObject *mod = Py_InitModule("scraper", NULL);
  if (mod == NULL)
    return;

  Py_INCREF(&ptoScr);
  PyModule_AddObject(mod, "Scraper", (PyObject *)&ptoScr);
}


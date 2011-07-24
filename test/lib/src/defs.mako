## Extension module definition library

<%def name="get_f(name, cpp)">
static PyObject *${name}(ScrO *pscro, void *)
{
  return PyBool_FromLong(pscro->psc->${cpp}());
}
</%def>


<%def name="get_i(name, cpp)">
static PyObject *${name}(ScrO *pscro, void *)
{
  return PyInt_FromLong(pscro->psc->${cpp}());
}
</%def>

<%def name="get_s(name, cpp)">
static PyObject *${name}(ScrO *pscro, void *)
{
  CStdString s = pscro->psc->${cpp}();
  return PyString_FromStringAndSize(s, s.size());
}
</%def>

<%def name="get_ver(name, cpp)">
static PyObject *${name}(ScrO *pscro, void *)
{
  // not comparable (maybe later?); just a simple string
  fprintf(stderr, "get version [%s]\n", pscro->psc->${cpp}().c_str());
  return PyString_FromString(pscro->psc->${cpp}().c_str());
}
</%def>

<%!
mpgetfn = {'int': 'i', 'CStdString': 's', 'bool': 'f', 'AddonVersion': 'ver'}
getset = []
%>
<%def name="add_get(py, cpp, typ, doc)">
<%doc>
# py - python name
# cpp - C++ method name
# typ - C++ return type (or castable equivalent)
# doc - docstring
</%doc>
<%
  # determine getter function (die if no match)
  get = mpgetfn[typ]

  # create thunk function name
  fn = "scr_get_" + get + "_" + py

  # create the getset entry
  getset.append({'py': py, 'fn': fn, 'doc': doc})

  # dump the thunk
  getattr(self, 'get_' + get)(fn, cpp)
%>
</%def>

<%def name="getdefs()">
% for i in getset:
  {"${i['py']}", (getter)${i['fn']}, NULL, "${i['doc']}", NULL},
% endfor
</%def>

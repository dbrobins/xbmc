## Extension module definition library

<%!
# class: ScrO, etc.
# name: of pointer; pscro, etc.
# prefix: for function names, e.g. "scr"
# inner: inner object e.g. "psc->"
gen_ctx = {}
%>

<%def name="setcontext(cls, prefix, inner)">
<%
  global gen_ctx
  if getset:
    raise RuntimeError('new context set before render')
  gen_ctx = {'class': cls, 'name': cls.lower(), 'prefix': prefix, 'inner': inner}
%>
</%def>

<%def name="get_f(name, cpp)">
static PyObject *${name}(${gen_ctx['class']} *${gen_ctx['name']}, void *)
{
  return PyBool_FromLong(${gen_ctx['name']}->${gen_ctx['inner']}${cpp}());
}
</%def>

<%def name="get_i(name, cpp)">
static PyObject *${name}(${gen_ctx['class']} *${gen_ctx['name']}, void *)
{
  return PyInt_FromLong(${gen_ctx['name']}->${gen_ctx['inner']}${cpp}());
}
</%def>

<%def name="get_s(name, cpp)">
static PyObject *${name}(${gen_ctx['class']} *${gen_ctx['name']}, void *)
{
  CStdString s = ${gen_ctx['name']}->${gen_ctx['inner']}${cpp}();
  return PyString_FromStringAndSize(s, s.size());
}
</%def>

<%def name="get_ver(name, cpp)">
static PyObject *${name}(${gen_ctx['class']} *${gen_ctx['name']}, void *)
{
  // not comparable (maybe later?); just a simple string
  return PyString_FromString(${gen_ctx['name']}->${gen_ctx['inner']}${cpp}().c_str());
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
  fn = gen_ctx['prefix'] + "_get_" + get + "_" + py

  # create the getset entry
  getset.append({'py': py, 'fn': fn, 'doc': doc})

  # dump the thunk
  getattr(self, 'get_' + get)(fn, cpp)
%>
</%def>

<%def name="getdefs()">
<% global getset %>
% for i in getset:
  {(char *)"${i['py']}", (getter)${i['fn']}, NULL, (char *)"${i['doc']}", NULL},
% endfor
<% getset = [] %>
</%def>


import xbmc.lib
from xbmc.lib.scraper import Scraper, Type, Error

print("Scraper test loaded.")

# how to specify which to load?
# - one of the defaults, by the TYPE constants (export them)
# - by name e.g. "imdb.com" (with the full name in addons)
s = Scraper(Type.MOVIES)
s2 = Scraper("metadata.tvdb.com")

print("s has type {} id {} name {} ver {} desc:\n{}".
  format(s.type, s.id, s.name, s.ver, s.desc))
print("s2 has type {} id {} name {} ver {} desc:\n{}".
  format(s2.type, s2.id, s2.name, s2.ver, s2.desc))

print(dict(s.extra_info))

print(dict(s.settings))
print(s.settings['fanart'])

print(dict(s.deps))
print(s.deps['xbmc.metadata'][0])

# if program terminates (via std::terminate usually) on throwing ADDON::CScraperError,
# then libxbmc.so hasn't been loaded with symbols available; see ext/__init__.py
try:
  s.nfo_url("!") # will throw (abort)...
except Error as e:
  if e.aborted:
    print("scraper aborted!")
  else:
    print("scraper error: '{}', title '{}'".format(e.message, e.title))

# what operations are possible?
# - don't allow e.g. GetVideoDetails on Scraper objects;
# allow it on the items returned back... even just by access? (demand load attribute)
# - although really we should be explicit about such work - ?
# -- can still be a method that returns something and caches it? do we want to cache? nah.
# - so it becomes "GetDetails"... e.g.
#
# for m in s.FindMovies():
#   print(m.GetDetails())  # string form...
# [? should scraper have a string form too? sure... once we get methods]

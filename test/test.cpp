#include <cstdio>

#include "xbmc.h"
#include "addons/Addon.h"
#include "addons/Scraper.h"
#include "addons/AddonManager.h"
#include "filesystem/FileCurl.h"
#include "video/VideoInfoTag.h"

using namespace std;
using namespace ADDON;

int main()
{
  XBMC_Init(grfxaNil, "test");

  AddonPtr pao;
  if (!CAddonMgr::Get().GetDefault(ADDON_SCRAPER_MOVIES, pao))
  {
    fprintf(stderr, "Failed to get default movie scraper.\n");
    return 1;
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

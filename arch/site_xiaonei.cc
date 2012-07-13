#include "cwf/site.h"

#include "base/logging.h"
#include "base/globalinit.h"
#include "base/startuplist.h"

#include "cwf/stream.h"

#include "Account/account_adapter.h"
#include "user/usercache_adapter.h"
#include "user/userbase_adapter.h"

#include "OceCxxAdapter/src/TicketAdapter.h"
#include "OceCxxAdapter/src/UserBasicAdapter.h"

namespace xce {

cwf::User* Authorize(cwf::Request* q) {
#if 0
/*
GET /GetEntry.do?id=359632066&owner=262918183 HTTP/1.1
Host: blog.renren.com
User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.9.1.5) Gecko/20091102 Firefox/3.5.5 GTB5
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,* / *;q=0.8
Accept-Language: en-us,en;q=0.5
Accept-Encoding: gzip,deflate
Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7
Keep-Alive: 300
Connection: keep-alive
Referer: http://passport.renren.com/PLogin.do?domain=renren.com&origURL=http%3A%2F%2Fblog.renren.com%2FGetEntry.do%3Fid%3D359632066%26owner%3D262918183
Cookie: __utma=151146938.186591772.1251177831.1258717364.1258720586.33; __utmz=151146938.1258720586.33.19.utmccn=(referral)|utmcsr=home.renren.com|utmcct=/Home.do|utmcmd=referral; wpi_menu_app_tip=fix; wpi_menu_add_tip=fix; xn_rr_notice123183=0; _r01_=1; home_top_hints0925=0; h1103227366242=0; _de=959CFD5E13CA66FC17FDD995416F3B708ED172744450A224; ap=true; h1119227366242=0; Referer=http://login.renren.com/Login.do?rf=r&origURL=http%3A%2F%2Fblog.renren.com%2FGetEntry.do%3Fid%3D359632066%26owner%3D262918183; BeforeReferer=http://blog.renren.com/GetEntry.do?id=359632066&owner=262918183; _urm_227366242=44097; __utmc=151146938; kl=kl_227366242; xnsid=db84852d; id=227366242; societyguester=f211a9a43442e34511a673ff40c674672; t=f211a9a43442e34511a673ff40c674672; XNESSESSIONID=6151cf8a5931; xns=12599204100601; p=7ba659ef7d6b0a5fe40d38125496164e2

__utma=151146938.186591772.1251177831.1258717364.1258720586.33
__utmz=151146938.1258720586.33.19.utmccn=(referral)|utmcsr=home.renren.com|utmcct=/Home.do|utmcmd=referral
wpi_menu_app_tip=fix
wpi_menu_add_tip=fix
xn_rr_notice123183=0
_r01_=1
home_top_hints0925=0
h1103227366242=0
_de=959CFD5E13CA66FC17FDD995416F3B708ED172744450A224
ap=true
h1119227366242=0
Referer=http://login.renren.com/Login.do?rf=r&origURL=http%3A%2F%2Fblog.renren.com%2FGetEntry.do%3Fid%3D359632066%26owner%3D262918183
BeforeReferer=http://blog.renren.com/GetEntry.do?id=359632066&owner=262918183
_urm_227366242=44097
__utmc=151146938
kl=kl_227366242
xnsid=db84852d
id=227366242
societyguester=f211a9a43442e34511a673ff40c674672
t=f211a9a43442e34511a673ff40c674672
XNESSESSIONID=6151cf8a5931
xns=12599204100601
p=7ba659ef7d6b0a5fe40d38125496164e2
*/
#endif

  // get cookie
  // call TicketAdapter CachedTicketAdapter()
  using namespace cwf;

  const Cookie& c = q->cookie();
  std::string ticket = c.Get("t");
  if (ticket.size() != 33)
    return 0;

  int uid = -1;

  try {
    //passport::TicketManagerPrx ta = CachedTicketAdapter(ticket[32] - '0');
    //if (!ta)
    //  return 0;

    //uid = ta->verifyTicket(ticket, "");
		uid = passport::TicketAdapter::instance().verifyTicket(ticket, "");		//use ha adapter
  } catch(std::exception& e) {
    LOG(WARNING) << e.what();
  }

  if (-1 == uid)
    return 0;

  try {
    //xce::userbase::UserBasicManagerPrx prx = CachedUserBasicAdapter(uid);
    //if (prx) {
    //  xce::userbase::UserBasicDataPtr data = prx->getUserBasic(uid);
		xce::userbase::UserBasicDataPtr data = adapter::userbasic::UserBasicAdapter::instance().getUserBasic(uid);		//use ha adapter
    if (data)
      return new User(data->name, uid);
    //}

    return new User(uid);
  } catch(std::exception& e) {
    LOG(WARNING) << e.what();
  }
  return 0;
}

static void Init() {
#ifdef WIN32
  return;
#endif
  try {
    xce::Adapter::passport::ResetCached<passport::TicketManagerPrx>("M@Ticket", 10);
  } catch(std::exception & e) {
    LOG(INFO) << "passport prx reset error " << e.what();
  }
  // 不知何故，执行不完
  // xce::Adapter::userbase::ResetCached<userbase::UserBasicManagerPrx>("UBM@UserBase", 100);
}

GLOBAL_INIT(CWF_SITE, {
  xce::AddStartup(&Init);
});


}

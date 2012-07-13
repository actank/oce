#include "feed/feedview/modifytime.h"

#include <time.h>
#include <stdio.h>
#ifdef WIN32
#define snprintf _snprintf
#endif

#include "ctemplate/template.h"

#include "feed/base_feed/basictypes.h"
#include "feed/base_feed/logging.h"
#include "feed/base_feed/startuplist.h"
#include "feed/base_feed/globalinit.h"
#include "feed/base_feed/signals.h"
#include "IceLogger.h"

namespace xce {

// java version:
// http://svn.d.xiaonei.com/ugc/feed/src/main/java/com/xiaonei/platform/component/xfeed/display/template/directives/LongTimeFormartDirective.java

void FriendlyTimeModifier::Modify(const char* in, size_t inlen,
    const ctemplate::PerExpandData*,
    ctemplate::ExpandEmitter* out,
    const std::string& arg) const {
#if 0
      int min = (int) ( (d.getTime() - time) / (60 * 1000));
      if(min <= 0){
        out.write("刚刚更新");
        return ;
      }else if(min < 60){
        out.write(min+"分钟前");
        return ;
      }else if(min < 1440){
        int iHour = (int) ( (d.getTime() - time) / (60 * 60 * 1000));
        out.write(iHour+"小时前");
      }else if(min >=1440 && min < 4320){
        SimpleDateFormat format = new SimpleDateFormat("HH:mm");
        int iDay = (int) ( (d.getTime() - time) / (24 * 60 * 60 * 1000));
        d.setTime(time);
        out.write(iDay+"天前  " + format.format(d));
      }else{
        d.setTime(time);
        SimpleDateFormat format = new SimpleDateFormat("MM-dd HH:mm");
        String date = format.format(d);
      }
#endif

#ifdef WIN32
  #define atoll    _atoi64
#endif
  // 不需要复制 in，已经是一个c string

  // uint64 t = atoll(in);
  long long t = atoll(in);

  t = t / 1000; // java中的time是毫秒

  if (t < 10000)
    return;

  char buf[0x100];

  time_t now = time(0);
  int diff_minutes = (now - t) / 60;
  if (diff_minutes <= 0) {
    int sec = now - t;
    int len = 0;
    if(sec <= 0) {
      len = snprintf(buf, sizeof(buf), "刚刚更新");
    } 
    else {
      len = snprintf(buf, sizeof(buf), "%d秒钟前", sec);
    }
    out->Emit(buf, len);
  }
  else if (diff_minutes < 60) {
    int len = snprintf(buf, sizeof(buf), "%d分钟前", diff_minutes);
    out->Emit(buf, len);
  }
  else {
    struct tm timeinfo;
    struct tm nowtimeinfo;
#ifdef WIN32
    int err = localtime_s(&timeinfo, (const time_t *)&t);
    if (err)
      return;
    err = localtime_s(&nowtimeinfo, (const time_t *)&now);
    if (err)
      return;
#elif defined(POSIX)
    localtime_r((const time_t *)&t, &timeinfo); // TODO: 如何得知用户的时区
    localtime_r((const time_t *)&now, &nowtimeinfo);
#endif
    int len = 0;
    int year = nowtimeinfo.tm_year - timeinfo.tm_year;
    int day = nowtimeinfo.tm_yday - timeinfo.tm_yday;
    if(year >= 1) {
      len = strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M", &timeinfo);
    }
    else {
      if(day < 1) {
        len = strftime(buf, sizeof(buf), "今天 %H:%M", &timeinfo);
      }
      else if(day == 1) {
        len = strftime(buf, sizeof(buf), "昨天 %H:%M", &timeinfo);
      }
      else {
        len = strftime(buf, sizeof(buf), "%m-%d %H:%M", &timeinfo);
      }
    }
    out->Emit(buf, len);
  }
}

FriendlyTimeModifier friendly_time;

// TODO: ubb replace DoingUtil.getInstance().replace(s);

// 
static void SignalReload(int) {
  LOG(INFO) << "reload template";
  ctemplate::Template::ReloadAllIfChanged();
}

// 静态函数可能添加不上，需要main内部再显示添加一次
static void Init() {
  ctemplate::AddModifier("x-time", &friendly_time);

  // TODO: move to perfect place
  InstallSignal(XCE_SIGNAL_CWF_LOAD_TEMPLATE, &SignalReload);
}

GLOBAL_INIT(MODIFYTIME, {
  AddStartup(&Init);
});

}

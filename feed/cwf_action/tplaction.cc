#include "base/logging.h"
#include "base/startuplist.h"
#include "base/globalinit.h"
#include "base/signals.h"

#include "cwf/tplaction.h"

static void SignalReload(int) {
  LOG(INFO) << "reload template";
  ctemplate::Template::ReloadAllIfChanged();
}

// 静态函数可能添加不上，需要main内部再显示添加一次
static void Init() {
  xce::InstallSignal(XCE_SIGNAL_CWF_LOAD_TEMPLATE, &SignalReload);
}

GLOBAL_INIT(TPLACTION, {
  xce::AddStartup(&Init);
});

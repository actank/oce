#include "base/globalinit.h"
#include "base/startuplist.h"
#include "cwf/frame.h"
#include "action.h"
#include "unistd.h"

namespace xce {

static void Init() {

  cwf::FrameWork::RegisterAction(new xce::search::ResultAction);

}

GLOBAL_INIT(CWF_SDK_ACTION, {
  AddStartup(&Init);
});


}


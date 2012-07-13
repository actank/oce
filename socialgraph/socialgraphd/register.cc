#include "base/globalinit.h"
#include "base/startuplist.h"
#include "cwf/frame.h"

//#include "socialgraph/socialgraphd/layout.h"
//#include "socialgraph/socialgraphd/action.h"
#include "socialgraph/socialgraphd/action.h"

namespace xce {

static void Init() {

  cwf::FrameWork::RegisterAction(new xce::socialgraph::GetAction5);
  cwf::FrameWork::RegisterAction(new xce::socialgraph::RemoveAction5);

  cwf::FrameWork::RegisterAction(new xce::socialgraph::BlogAction);

  cwf::FrameWork::RegisterAction(new xce::socialgraph::VideoAction);

  cwf::FrameWork::RegisterAction(new xce::socialgraph::StatAction);

  cwf::FrameWork::RegisterAction(new xce::socialgraph::GetOtherAction5);
  cwf::FrameWork::RegisterAction(new xce::socialgraph::RemoveOtherAction5);
}

GLOBAL_INIT(CWF_SDK_ACTION, {
  AddStartup(&Init);
});

}

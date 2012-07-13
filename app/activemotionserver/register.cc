#include "base/globalinit.h"
#include "base/startuplist.h"
#include "cwf/frame.h"
#include "action.h"
#include "unistd.h"
//#include <locallogger.h>
#include "hadooplogger.h"
#include "socketlogger.h"

namespace xce {

static void Init() {

 try{
  /*
  std::ostringstream fcgi_log_path;
  fcgi_log_path << "/data/log_xce/log/fcgi/" << getpid() << "_log";
  FCGI_INIT( fcgi_log_path.str(), "INFO" );
  */

  std::ostringstream hadoop_log_path;
  hadoop_log_path << "/data/LogData/user_browse/" << getpid() << "_log";
  HADOOP_INIT( hadoop_log_path.str(), "INFO" );

  SOCKET_INIT("123.125.47.29",8082,"INFO");
 } catch(...){
  //TODO:
 }

  cwf::FrameWork::RegisterAction(new cwf::socialgraph::ClickAction);
  cwf::FrameWork::RegisterAction(new cwf::socialgraph::ScrollBottomAction);
  cwf::FrameWork::RegisterAction(new cwf::socialgraph::FocusAction);
  cwf::FrameWork::RegisterAction(new cwf::socialgraph::UnfocusAction);
  cwf::FrameWork::RegisterAction(new cwf::socialgraph::AppHoldAction);
  cwf::FrameWork::RegisterAction(new cwf::socialgraph::OtherAction);

}

GLOBAL_INIT(CWF_SDK_ACTION, {
  AddStartup(&Init);
});


}


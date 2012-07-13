/*
 * FeedStatLogI.cpp
 *
 *  Created on: 2011-2-17
 *      Author: ld
 */

//#include <sstream>
#include "FeedStatLogI.h"
#include "IceLogger.h"

using namespace xce::feed;
using namespace com::xiaonei::xce;


/*
bool MyCallback(const char *dump_path,
   const char *minidump_id,
   void *context,
   bool succeeded) {
  printf("got crash %s %s\n", dump_path, minidump_id);
  return true;
}*/
void MyUtil::initialize() {
  //static google_breakpad::ExceptionHandler eh(".", 0, MyCallback
   //                                    , 0, true);
	ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&FeedStatLogI::instance(), service.createIdentity("M", ""));


  //TaskManager::instance().scheduleRepeated(new StatTimer());
	//TaskManager::instance().scheduleRepeated(&ClearSaveDbVectorTimer::instance());
	//TaskManager::instance().scheduleRepeated(&PerformanceStat::instance());
	//TaskManager::instance().scheduleRepeated(&UpdateTimer::instance());
}




namespace xce {

namespace feed {


void FeedStatLogI::AddLog(const ::MyUtil::StrSeq& category, const string& log_str, const ::Ice::Current& cur){
  //MCE_INFO("FeedStatLogCacheI::AddLog -->" << category[0] << " " << category[1] << " " <<  log_str );
  
  try {
    logger_->addLog(category, log_str);
  } catch (...) {
    MCE_WARN("FeedStatLogI::add log exception -->" << log_str);
  }

}


}
}

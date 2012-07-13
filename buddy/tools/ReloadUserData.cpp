#include <OceCxxAdapter/src/UserPassportAdapter.h>
#include <OceCxxAdapter/src/UserNameAdapter.h>
#include <OceCxxAdapter/src/UserBornAdapter.h>
#include <OceCxxAdapter/src/UserDoingAdapter.h>
#include <OceCxxAdapter/src/UserRightAdapter.h>
#include <OceCxxAdapter/src/UserStageAdapter.h>
#include <OceCxxAdapter/src/UserStateAdapter.h>
#include <OceCxxAdapter/src/UserTimeAdapter.h>
#include <OceCxxAdapter/src/UserUrlAdapter.h>
#include <OceCxxAdapter/src/UserLoginCountAdapter.h>
#include <OceCxxAdapter/src/UserConfigAdapter.h>

using namespace std;
using namespace MyUtil;
using namespace xce::adapter;
using namespace xce::adapter::userpassport;
using namespace xce::adapter::username;
using namespace xce::adapter::userborn;
using namespace xce::adapter::userdoing;
using namespace xce::adapter::userright;
using namespace xce::adapter::userstage;
using namespace xce::adapter::userstate;
using namespace xce::adapter::usertime;
using namespace xce::adapter::userurl;
using namespace xce::adapter::userlogincount;

int main(int argc, char** argv){
  MCE_INIT("./reload.log", "INFO");
  if(argc < 2){
    MCE_INFO("./reload idfile");
    return 0;
  }
  string file;
  file = argv[1];
  ifstream in(file.c_str());
  for(string s;getline(in,s);){
  int id = atoi(s.c_str());
  try{
    MCE_INFO( "reload " << id);
    UserPassportAdapter::instance().reload(id);
    MCE_INFO( "reload passport done." );
    UserNameAdapter::instance().reload(id);
    MCE_INFO( "reload name done." );
    UserBornAdapter::instance().reload(id);
    MCE_INFO( "reload born done." );
    UserDoingAdapter::instance().reload(id);  
    MCE_INFO( "reload doing done." );
    UserRightAdapter::instance().reload(id);
    MCE_INFO( "reload right done." );
    UserStageAdapter::instance().reload(id);
    MCE_INFO( "reload stage done." );
    UserStateAdapter::instance().reload(id);
    MCE_INFO( "reload state done." );
    UserTimeAdapter::instance().reload(id);
    MCE_INFO( "reload time done." );
    UserUrlAdapter::instance().reload(id);
    MCE_INFO( "reload url done." );
    UserLoginCountAdapter::instance().reload(id);
    MCE_INFO( "reload logincount done." );
    UserConfigAdapter::instance().reload(id); 
    MCE_INFO( "reload config done." );
  }catch(Ice::Exception& e){
    MCE_WARN("ICEERROR " << id << e.what());
  }catch(...){
    MCE_WARN("UNKNOWNERROR " << id  );
  }
  }
  MCE_INFO( "done.");
}

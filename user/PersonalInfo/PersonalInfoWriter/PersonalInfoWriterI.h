#ifndef __XCE_PERSONALINFO_PERSONALINFOWRITERI_H__
#define __XCE_PERSONALINFO_PERSONALINFOWRITERI_H__

#include "PersonalInfo.h"
#include "../share/LinkViewDataI.cpp"
#include <Ice/Ice.h>
#include "TaskManager.h"
#include "TempMutex.h"
#include "ServiceI.h"

namespace xce {
namespace personalinfo {

//服务配置
const string SERVICE_IDENTITY = "PIW";
const string SERVER_CONFIG_PREFIX = "PersonalInfoWriter";
const string CONTROLLER_ENDPOINT = "ControllerPersonalInfoWriter";

//PersonalInfoWriterI
//--------------------------------------------------------------
class PersonalInfoWriterI: virtual public PersonalInfoWriter, public MyUtil::Singleton<PersonalInfoWriterI> {
public:
  PersonalInfoWriterI();

  void createPersonalInfo(const MyUtil::Str2StrMap& props, const Ice::Current& = Ice::Current());
  void setPersonalInfo(int userId, const MyUtil::Str2StrMap& props, const Ice::Current& = Ice::Current());

  void reloadLinkView(int userId, const Ice::Current& = Ice::Current());
  bool isValid(const Ice::Current& = Ice::Current());
  void setValid(bool newState, const Ice::Current& = Ice::Current());

private:
  xce::tempmutex::TempMutexManagerPtr _tempMutexManager;
  LinkViewDataFactoryIPtr _factory;
};

}
}
#endif

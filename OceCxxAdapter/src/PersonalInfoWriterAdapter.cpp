#include "PersonalInfoWriterAdapter.h"

using namespace xce::adapter::personalinfo;
using namespace xce::personalinfo;

PersonalInfoWriterAdapter::PersonalInfoWriterAdapter() :
    MyUtil::ReplicatedClusterAdapterI<PersonalInfoWriterPrx>
      ("ControllerPersonalInfoWriter", 120, 300) {
}

void PersonalInfoWriterAdapter::setPersonalInfo(int userId, MyUtil::Str2StrMap props) {
  if (userId <= 0) {
    return;
  }
  getProxy(userId)->setPersonalInfo(userId, props);
}

void PersonalInfoWriterAdapter::createPersonalInfo(int userId, MyUtil::Str2StrMap props) {
  if (userId <= 0) {
    return;
  }
  getProxy(userId)->createPersonalInfo(props);
}

void PersonalInfoWriterAdapter::reloadLinkView(int userId) {
  IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
  _idSet.insert(userId);
  _mutex.notify();
}

void PersonalInfoWriterAdapter::run() {
  MyUtil::IntSeq ids;
  {
    IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
    if(_idSet.empty()) {
      _mutex.wait();
    }
    ids.insert(ids.end(), _idSet.begin(), _idSet.end());
    _idSet.clear();
  }
  for(MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
    PersonalInfoWriterPrx prx = getProxy(*it);
    if(prx) {
      try {
        prx->reloadLinkView(*it);
      } catch (Ice::Exception& e) {
        MCE_WARN("PersonalInfoWriterAdapter::run reload Exception id=" << *it <<" " << e.what());
      }
    }
  }
}

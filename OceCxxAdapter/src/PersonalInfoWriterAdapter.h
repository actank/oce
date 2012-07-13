#ifndef __XCE_ADAPTER_PERSONALINFO_PERSONALINFOWRITER_ADAPTER_H__
#define __XCE_ADAPTER_PERSONALINFO_PERSONALINFOWRITER_ADAPTER_H__

#include "PersonalInfo.h"
#include "Singleton.h"
#include "AdapterI.h"
#include <ext/hash_set>
#include <IceUtil/IceUtil.h>

using namespace __gnu_cxx;

namespace xce {
namespace adapter {
namespace personalinfo {

class PersonalInfoWriterAdapter : public MyUtil::Singleton<PersonalInfoWriterAdapter>, IceUtil::Thread,
    public MyUtil::ReplicatedClusterAdapterI<xce::personalinfo::PersonalInfoWriterPrx> {
public:
  PersonalInfoWriterAdapter();
  void setPersonalInfo(int userId, MyUtil::Str2StrMap props);
  void createPersonalInfo(int userId, MyUtil::Str2StrMap props);
  void reloadLinkView(int userId);

protected:
  void run();

private:
  IceUtil::Monitor<IceUtil::Mutex> _mutex;
  hash_set<int> _idSet;
};

}
}
}
#endif

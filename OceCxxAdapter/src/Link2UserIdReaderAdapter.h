#ifndef __XCE_ADAPTER_PERSONALINFO_LINK2USERIDREADER_ADAPTER_H__
#define __XCE_ADAPTER_PERSONALINFO_LINK2USERIDREADER_ADAPTER_H__

#include "PersonalInfo.h"
#include "Singleton.h"
#include "AdapterI.h"

namespace xce {
namespace adapter {
namespace personalinfo {

class Link2UserIdReaderAdapter : public MyUtil::Singleton<Link2UserIdReaderAdapter>,
    public MyUtil::ReplicatedClusterAdapterI<xce::personalinfo::Link2UserIdReaderPrx> {
public:
  Link2UserIdReaderAdapter();

  //仅供中间层服务调用
  void add(const string& link, int userId);
  void remove(const string& link);
  void setData(int userId, const Ice::ObjectPtr& userData);
  void setData(const MyUtil::ObjectResultPtr& dataMap);
};

}
}
}
#endif

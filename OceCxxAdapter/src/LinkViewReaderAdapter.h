#ifndef __XCE_ADAPTER_PERSONALINFO_LINKVIEWREADER_ADAPTER_H__
#define __XCE_ADAPTER_PERSONALINFO_LINKVIEWREADER_ADAPTER_H__

#include "PersonalInfo.h"
#include "Singleton.h"
#include "AdapterI.h"

namespace xce {
namespace adapter {
namespace personalinfo {

class LinkViewReaderAdapter : public MyUtil::Singleton<LinkViewReaderAdapter>,
    public MyUtil::ReplicatedClusterAdapterI<xce::personalinfo::LinkViewReaderPrx> {
public:
  LinkViewReaderAdapter();
  MyUtil::Str2StrMap getLinkView(int userId);
  MyUtil::Str2StrMap getLinkViewSync(int userId);
  void setLinkView(int userId, const MyUtil::Str2StrMap& props);

  //仅供中间层服务调用
  void setData(int userId, const Ice::ObjectPtr& userData);
  void setData(const MyUtil::ObjectResultPtr& dataMap);
};

}
}
}
#endif

#ifndef HOTSTATESUBSCRIBEADAPTER_H__
#define HOTSTATESUBSCRIBEADAPTER_H__

#include "Singleton.h"
#include "AdapterI.h"

#include <HotStateSubscribe.h>

using namespace std;
using namespace com::xiaonei::search5::logicserver;
using namespace com::xiaonei::search::state;
using namespace MyUtil;

class HotStateSubscribeAdapter : public MyUtil::Singleton<HotStateSubscribeAdapter>, public MyUtil::ReplicatedClusterAdapterI<HotStateSubscribePrx>  {

  public:

    HotStateSubscribeAdapter();
    void update(IndexUpdateData info);
    bool test(const string & pattern, const string& inputState);
    vector<map<string, string> > getNewState(const string& type, int& limit);
    bool subscribe(const string& type, const string& pattern, int& maxSize, string& passwd);
    bool unSubscribe(const string& type);
    vector<map<string, string> > showTypeInfos();
    map<string, string>  showTypeInfo(const string& type);


    virtual size_t cluster() {
      return 1;
    } 
    
};
#endif

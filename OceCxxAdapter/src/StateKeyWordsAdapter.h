#ifndef STATEKEYWORDSADAPTER_H__
#define STATEKEYWORDSADAPTER_H__

#include "Singleton.h"
#include "AdapterI.h"

#include <ContentSearch.h>

using namespace std;
using namespace com::xiaonei::search5::logicserver;

class StateKeyWordsAdapter : public MyUtil::Singleton<StateKeyWordsAdapter>, public MyUtil::ReplicatedClusterAdapterI<IndexUpdateLogicPrx>  {

  public:

    StateKeyWordsAdapter();
    void update(IndexUpdateData info);
    void setValid(bool newState);
    void setRecovery(bool newState);
    bool isRecovery(); 
    virtual size_t cluster() {
      return 1;
    } 
  protected:
    
    bool isValid(const Ice::ObjectPrx& proxy);
};
#endif

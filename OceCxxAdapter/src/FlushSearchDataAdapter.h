#ifndef FLUSHSEARCHDATAADAPTER_H_
#define FLUSHSEARCHDATAADAPTER_H_

#include "AdapterI.h"
#include <ContentSearch.h>
#include <boost/shared_ptr.hpp>


using namespace com::xiaonei::search5::logicserver;
class FlushSearchDataAdapter :  public MyUtil::ReplicatedClusterAdapterI<IndexUpdateLogicPrx>  {

  public:
    virtual ~FlushSearchDataAdapter();
    bool update(IndexUpdateData info);
    void setValid(bool new_state);
    void setRecovery(bool new_state);
    bool isRecovery(); 
    FlushSearchDataAdapter(const string & business_type);
  protected:
    virtual size_t cluster()  {
      return cluster_;
    }
    bool isValid(const Ice::ObjectPrx& proxy);
  private:
    string business_type_;
    size_t cluster_;
};

class FlushDataAdapterFactory  {
  public:
  static void loadFlushSearchDataAdapter();
  static boost::shared_ptr<FlushSearchDataAdapter> getNamed(string name); 
  protected:
    static map<string, boost::shared_ptr<FlushSearchDataAdapter> > instances_;
};



#endif

#include "Singleton.h"
#include "AdapterI.h"

#include <ContentSearchRmi.h>

using namespace com::xiaonei::search5::logicserver;


class SubIndexAdapter : public MyUtil::ReplicatedAdapterI,
  public MyUtil::AdapterISingleton<MyUtil::SearchCacheChannel, SubIndexAdapter>
{

  public:
    virtual ~SubIndexAdapter();
    bool update(int mod, IndexUpdateDataSeq infos);
    
    void setName(const string& name)  {
      _name = name;
    }

    void setEndpoints(const string& endpoints)  {
      _endpoints = endpoints;
    }

    void setProxy()  {
      _proxy = locate<RmiLogicPrx>(_name, MyUtil::TWO_WAY);
    }
 
    RmiLogicPrx getProxy()  {
      return _proxy;
    }

  protected:

    
    virtual string name() {
      return _name; 
    }
    virtual string endpoints() {
      return _endpoints;
    }
    virtual size_t cluster() {
      return 1;
    }
    bool isValid(const Ice::ObjectPrx& proxy);
    
  private:
     
    RmiLogicPrx _proxy;
};

class SubIndexAdapterFactory   {
  public:
    static void loadSubIndexAdapter();
    
    static RmiLogicPrx getNamed(const string & name);

  private:
    static map<string, RmiLogicPrx > _instances;   

}; 


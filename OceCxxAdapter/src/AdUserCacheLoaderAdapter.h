#ifndef __ADUSERCACHELOADER_ADAPTER_H__
#define __ADUSERCACHELOADER_ADAPTER_H__

#include "AdapterI.h"
#include "AdUserCache.h"

namespace xce{
  namespace ad{
    using namespace MyUtil;
    class AdUserCacheLoaderAdapter : public AdapterI, public AdapterISingleton<MyUtil::McProxyChannel, AdUserCacheLoaderAdapter>{
      public:
        void LoadByIdSeq(const MyUtil::IntSeq& ids);
        void LoadById(int userid);
        void ReloadUserInfoByIdSeq(const MyUtil::IntSeq& ids);
        void ReloadUserInfoById(int userid);
        string GetById(int userid);
        int getCluster(){
          return _cluster;
        }
      protected:
        virtual string name(){
          return "AdUserCacheLoader";
        }
        virtual string endpoints(){
          return "@AdUserCacheLoader";
        }
        virtual size_t cluster(){
          return 10;
        }
        AdUserCacheLoaderPrx getManagerOneway(int id);
        AdUserCacheLoaderPrx getManager(int id);
      private:
        vector<AdUserCacheLoaderPrx> _managersOneway;
        vector<AdUserCacheLoaderPrx> _managers;
    };
  };
};
#endif

#ifndef _CHECKER_H_
#define _CHECKER_H_

#include "SearchCache.h"
#include "search/SearchCache2/src/Util/SearchMemCacheData.h"
#include "OceCxxAdapter/src/SearchMemCacheAdapter.h"
#include "search/SearchCache2/src/Util/DatabaseCacheFactory.h"
#include "search/SearchCache2/src/Util/IDbCache.h"
#include "ServiceI.h"
#include "IceUtil/IceUtil.h"
#include "QueryRunner.h"
using namespace MyUtil;
using namespace IceUtil;
using namespace com::xiaonei::xce;
using namespace xce::searchcache;
using std::cout;
using std::endl;

namespace xce {
  namespace searchcache {

    typedef boost::shared_ptr<IDbCache> IDbCachePtr;

    class LoadThread;

    class SearchCacheLoader {
     private:
       int __cluster;
       SearchCacheLoader(){};
       SearchCacheLoader& operator=(const SearchCacheLoader& rhs);
     public:

       bool Initialize();
       static SearchCacheLoader& instance() {
         if (!__instance) {
           IceUtil::Mutex::Lock lock(__mutex);
           if (!__instance) {
             __instance = new SearchCacheLoader();
           }
         }
         return *__instance;
       };

       void join();

       string getDSN(int mod) { 
         if (mod>__cluster) mod = 0;
         return __vecDSN[mod];
       }

     private:
       typedef vector<IceUtil::ThreadControl> LoadThreadVec;
       static SearchCacheLoader* __instance;
       static IceUtil::Mutex __mutex;
       LoadThreadVec __vecLoadThread;
       vector<string> __vecDSN;
    };

    class LoadThread: virtual public IceUtil::Thread {
     public:
       LoadThread(int mod) : mod_(mod) {

       };
       ~LoadThread() {};
     protected:
       virtual void run() {
         //while(true) {
         try {
           //检测isValid,如果valid==false， 开始填充容器
           //XceCacheAdapter::instance().SetValid(mod_, false);

           if (SearchMemCacheAdapter::instance().IsValid(mod_)) {
             return ;
           }

           string strDSN = SearchCacheLoader::instance().getDSN(mod_);
           IDbCachePtr db_cache = DatabaseCacheFactory::instance().getDbCache(strDSN);

           if (db_cache->Initialize() == false) {
             MCE_FATAL("Db" << mod_ << " initialize failed");
             goto __error;
           }     

           IDbResult* pDbResult = db_cache->Query();

           if (pDbResult == NULL) {
             MCE_WARN("don't iterator, because Query() return null pointer!");
             goto __error;
           }

           long int uid = 0;
           int idx = 0;
           map<int, int> id_2_view_count;
           while (BinStringPtr binObj = pDbResult->FetchObject(uid)) {
             if (0 == uid) continue;
             char* pBuf = NULL;
             int len = 0;

             binObj->getBuf(&pBuf, &len);

             if (pBuf==NULL || len <= 0) continue;

             Int2ByteSeq id_2_bytes_seq;
             id_2_bytes_seq[uid].assign(pBuf, pBuf+len);
             SearchMemCacheAdapter::instance().doUpdateMemCacheData(id_2_bytes_seq);
             uid = 0;
             if (0 == ++idx % 1000) {
               MCE_INFO("mod " << mod_ << " have load " << idx << " user id");
             }
           }
           SearchMemCacheAdapter::instance().SetValid(mod_, true);
           db_cache->Close();
         } catch(IceUtil::NullHandleException& e) {
           MCE_FATAL("IceUtil" << e.what());
           goto __error;
         } catch(std::exception& e) {
           MCE_FATAL("std::exception" << e.what());
           goto __error;
         }
         catch(...) {
           MCE_FATAL("don't catch exception");
           goto __error;
         }
__error:
         //sleep(2);
         //}
         MCE_INFO("Task done for mod " << mod_);
         std::cout << "Task done for mod " <<mod_ << std::endl;
       }
     private:
       int mod_; 
    };

  }
}
#endif

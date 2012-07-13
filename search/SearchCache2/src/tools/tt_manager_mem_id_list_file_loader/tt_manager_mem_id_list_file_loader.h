#ifndef _CHECKER_H_
#define _CHECKER_H_

#include <time.h>
#include <fstream>
#include <IceUtil/IceUtil.h>
#include <SearchCache.h>
#include <ServiceI.h>
#include "search/SearchCache2/src/Util/IDbCache.h"
#include "search/SearchCache2/src/tt_manager/bit_vector.h"
#include "search/SearchCache2/src/Util/DatabaseCacheFactory.h"

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

class TTManagerMemIdListFileLoader {
	private:
		int __cluster;
		TTManagerMemIdListFileLoader() { 
      pbit_vector_ = new CBitVector(~(1<<31));
    };
    TTManagerMemIdListFileLoader& operator=(const TTManagerMemIdListFileLoader& rhs);
	public:

		bool Initialize();
    ~TTManagerMemIdListFileLoader() { 
      if (pbit_vector_) {
        delete pbit_vector_; 
        pbit_vector_ = NULL;
      }   
    };
    static TTManagerMemIdListFileLoader& instance() {
      if (!__instance) {
        IceUtil::Mutex::Lock lock(__mutex);
                if (!__instance) {
                    __instance = new TTManagerMemIdListFileLoader();
                }
            }
            return *__instance;
		};

		void join();

		string getDSN(int mod) { 
			if (mod>__cluster) mod = 0;
			return __vecDSN[mod];
		}

    inline CBitVector& get_bit_vector_() {
      return *pbit_vector_;
    }   
  private:
    typedef vector<IceUtil::ThreadControl> LoadThreadVec;
    static TTManagerMemIdListFileLoader* __instance;
    static IceUtil::Mutex __mutex;
    LoadThreadVec __vecLoadThread;
    vector<string> __vecDSN;
    CBitVector* pbit_vector_;
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
						string strDSN = TTManagerMemIdListFileLoader::instance().getDSN(mod_);
            //stringstream file_name ;
            //file_name << "./" << _mod << ".txt";
            //ofstream file((file_name.str()).c_str());
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

						long uid = 0;
            int idx = 0;
						while (BinStringPtr binObj = pDbResult->FetchObject(uid)) {
              if (uid < 0) {
                cout << "FATAL! uid=" << uid <<" < 0" << endl;
              }
              TTManagerMemIdListFileLoader::instance().get_bit_vector_().Set(uid);

              time_t calender_time = time(NULL);
              struct tm*  broken_time;

              if (0 == ++idx % 1000) {
                calender_time = time(NULL);
                broken_time = localtime(&calender_time);
                cout <<"[" << broken_time->tm_hour << ":" << broken_time->tm_min << ":" << broken_time->tm_sec << "] "<<idx<<" user ids have been processed"<<endl;
               /// if (0 == idx % 10000) {
               ///   TTManagerMemIdListFileLoader::instance().get_bit_vector_().Save("./user_id_list_file.bin");
               ///   cout <<"[" << broken_time->tm_hour << ":" << broken_time->tm_min << ":" << broken_time->tm_sec << "]"<<idx<<" Save File!"<<endl;
               /// }
                if (0 == idx % 1000000) {
                  TTManagerMemIdListFileLoader::instance().get_bit_vector_().Save("./user_id_list_file.bin");
                  cout <<"[" << broken_time->tm_hour << ":" << broken_time->tm_min << ":" << broken_time->tm_sec << "]"<<idx<<" Save File!"<<endl;
                }
              }
            }
            TTManagerMemIdListFileLoader::instance().get_bit_vector_().Save("./user_id_list_file.bin");
						db_cache->Close();
        }
				catch(IceUtil::NullHandleException e) {
					MCE_FATAL("IceUtil" << e.what());
					goto __error;
				}
				catch(std::exception e) {
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
        TTManagerMemIdListFileLoader::instance().get_bit_vector_().Save("./user_id_list_file.bin");
        std::cout<<"Task done, Save File Done, mod=" << mod_<< std::endl;
		}
	private:
		int mod_; 
};

}
}
#endif

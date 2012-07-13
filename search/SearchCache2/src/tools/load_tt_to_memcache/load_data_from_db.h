/*
 * =====================================================================================
 *
 *       Filename:  load_data_from_db.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2010年10月08日 20时35分40秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lijie (), lijie@opi-corp.com
 *        Company:  Opi-Corp
 *
 * =====================================================================================
 */
#include "QueryRunner.h"
#include "ConnectionHolder.h"
#include "TaskManager.h"

#include <IceUtil/IceUtil.h>
#include <ServiceI.h>
#include "../../Util/DatabaseCacheFactory.h"
#include "search/SearchCache2/src//Util/IDbCache.h"
namespace xce {
  namespace searchcache{
  const string USER_STATUS_FIELD = " id, status ";
  const string DB_SEARCH2_USERBASIC_STATUS = "search_user_passport";
  const int IDS_BATCH_SIZE = 1000;
  const int kInvokeLoadThreadNum = 10;
  const string DSN = "tokyotryant://10.3.20.153:1981";


  class BasicStatusResultHandlerI: public com::xiaonei::xce::ResultHandler {
    public:
      BasicStatusResultHandlerI(Ice::Int& beginId, std::vector<int>& thread);
      virtual bool handle(mysqlpp::Row& row) const;
    private:
      Ice::Int& beginId_;
      std::vector<int>& thread_;
  };
/*
  class LoadData2CacheTask: virtual public MyUtil::Task  {
    private:
       MyUtil::IntSeq uids_;
    public:
       LoadData2CacheTask(MyUtil::IntSeq uids);
       virtual void handle();
  };
*/
  class InvokeLoad: virtual public IceUtil::Thread {
    public:
	  InvokeLoad(boost::shared_ptr<IDbCache> db_cache):_db_cache(db_cache),_shouldStop(false){
	  }
      void invoke(const MyUtil::IntSeq& userIds) {
        if (userIds.empty()) {
          return;
        }
        IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
        _cache.insert(_cache.end(), userIds.begin(), userIds.end());

        if (_cache.size() >= IDS_BATCH_SIZE) {
          _mutex.notify();
        }
      }
	  void insert(const MyUtil::IntSeq& userIds){
		if(userIds.empty())
			return;
		int count=0;
		int total=0;
		while(total < userIds.size()){
			vector<int>::const_iterator itA = userIds.begin()+total;
			vector<int>::const_iterator itB = total+IDS_BATCH_SIZE > userIds.size() ? userIds.end():itA+IDS_BATCH_SIZE;
			total += (itB - itA);
			vector<int> ids(itA,itB);
			std::map<int, ByteSeq> data_map;
      		_db_cache->Query(ids, data_map);
			//insert into objectacache
	  	}
	  }
	  void stop(){
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
		_shouldStop = true;
	  }
    protected:
      virtual void run() {
        while (true) {
          try {
            MyUtil::IntSeq batch;
            {
              IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
			  if(_shouldStop && _cache.empty())
				break;
              if (_cache.size() < IDS_BATCH_SIZE) {
                _mutex.timedWait(IceUtil::Time::seconds(1));
              }
              batch.swap(_cache);
            }
			insert(batch);	
            //MyUtil::TaskManager::instance().execute(new LoadData2CacheTask(batch));
          } catch (...) {
            MCE_WARN("InvokeReload main loop exception");
          }
        }
		MCE_WARN("Thread Load  Done");
      }
    private:
      MyUtil::IntSeq _cache;
	  bool _shouldStop;
	  boost::shared_ptr<IDbCache> _db_cache;
      IceUtil::Monitor<IceUtil::Mutex> _mutex;
  };

  };
};

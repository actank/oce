/*
 * =====================================================================================
 *
 *       Filename:  load_data_from_db.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2010年10月08日 20时38分55秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lijie (), lijie@opi-corp.com
 *        Company:  Opi-Corp
 *
 * =====================================================================================
 */
#include "load_data_from_db.h"

using namespace MyUtil;
using namespace IceUtil;
using namespace com::xiaonei::xce;
using namespace xce::searchcache;
   int cluster, mod;
   void usage()  {
     std::cout<<"load_data_from_db begin limit"<<std::endl;
   } 
    
   bool belongsMe(int id){
       if (cluster==0) return true;
         return (mod == (id % cluster));
   }
  
   void initialize(int& cluster, int& mod)  {
     cluster =4;
     mod = 0;
     //TaskManager::instance().config(xce::searchcache::TASK_LEVEL_FILL, ThreadPoolConfig(1, 1));
     //TaskManager::instance().config(xce::searchcache::TASK_LEVEL_FILL_BASIC, ThreadPoolConfig(1, 64));
     //TaskManager::instance().config(xce::searchcache::TASK_LEVEL_FILL_CONFIG, ThreadPoolConfig(1, 8)); 
   }
    
    
    BasicStatusResultHandlerI::BasicStatusResultHandlerI(Ice::Int& beginId, std::vector<int>& thread) :
      beginId_(beginId), thread_(thread) {
      }

    bool BasicStatusResultHandlerI::handle(mysqlpp::Row& row) const {
      int id = (int) row["id"];
      int status = (int) row["status"];
      if (status < 3 && belongsMe(id)) {
        thread_.push_back(id);
      }
      return true;
    }
/*
    LoadData2CacheTask::LoadData2CacheTask(MyUtil::IntSeq uids) :
        MyUtil::Task(TASK_LEVEL_LOAD_CACHE), uids_(uids) {
        }

    void LoadData2CacheTask::handle() {
      if (uids_.empty()) return;

      MCE_INFO("SearchCacheManagerI::load " << uids_.size() << " belongs to me");

      boost::shared_ptr<IDbCache> db_cache = DatabaseCacheFactory::instance().getDbCache(DSN);

      if (db_cache->Initialize() == false) {
        MCE_WARN("Db initialize failed");
        return;
      }
      std::map<int, ByteSeq> data_map;
      db_cache->Query(uids_, data_map);
      //ObjectCacheHelper::instance().removeObject(uids)/暂先注掉
      //ObjectCacheHelper::instance().addObject(uids_,data_map);//暂先注掉
      /*for (MyUtil::IntSeq::const_iterator it = uids_.begin(); it != uids_.end(); ++it) {
        if(!SearchCacheManagerI::instance().belongsMe(*it)) continue;
        db_cache->Query(id_sets, data_map);
        std::cout<<"map size is "<<data_map.size()<<std::endl;
        SearchMemCacheDataPtr obj = new SearchMemCacheData;
        if (db_cache->Query(*it, obj.get())) {
          int id = *it;
          ObjectCacheHelper::instance().removeObject(id);
          ObjectCacheHelper::instance().addObject(id, obj->Serialize());
        } else {
          ObjectCacheHelper::instance().removeObject(*it);
          if (*it != 0) {
            MCE_WARN("cached Id = " << (*it) << " is failed" << " load from database");
          }
        }
      }
      db_cache->Close();
    }
*/
    int main(int argc, char** argv)  {
      if(argc ==3 )  {  
      int count, cycle;
      initialize(cluster, mod);
      int begin = atoi(argv[1]);
      int limit = atoi(argv[2]);
      std::vector<InvokeLoad *>  _vecInvokeLoad; 
      for (int i=0; i<kInvokeLoadThreadNum; ++i) {
		boost::shared_ptr<IDbCache> db_cache = DatabaseCacheFactory::instance().getDbCache(DSN);
		if(db_cache->Initialize())
        	_vecInvokeLoad.push_back(new InvokeLoad(db_cache));
      }

	  if(_vecInvokeLoad.empty())
			return 0;
      for (std::vector<InvokeLoad *>::const_iterator iter=_vecInvokeLoad.begin(); 
          iter!=_vecInvokeLoad.end(); ++iter) {
        (*iter)->start(128 * 1024).detach();
      }
      count = begin;
      cycle = 0;
      while(count < limit)  {
        std::vector<int> ids;
        Statement status_sql;
        status_sql << "SELECT " << USER_STATUS_FIELD << " FROM user_passport WHERE id > "
          << begin << " LIMIT " << IDS_BATCH_SIZE;
        BasicStatusResultHandlerI status_handler(begin, ids);
        QueryRunner(DB_SEARCH2_USERBASIC_STATUS, CDbRServer).query(status_sql, status_handler);
        count += ids.size();
        _vecInvokeLoad.at(cycle%_vecInvokeLoad.size())->invoke(ids);
        cycle++;
      }  
      }else  {
        usage();
        return 0;
      }
      } 



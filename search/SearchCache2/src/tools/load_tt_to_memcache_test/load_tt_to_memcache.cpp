/*
 * =====================================================================================
 *
 *       Filename:  load_tt_to_mamcache.cpp
 *
 *    Description:  Test program for loading data from tt
 *
 *        Version:  1.0
 *        Created:  2010年09月30日 16时12分32秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lijie (), lijie@opi-corp.com
 *        Company:  Opi-Corp
 *
 * =====================================================================================
 */

#include "load_tt_to_memcache.h"
using namespace MyUtil;
using namespace IceUtil;
using namespace com::xiaonei::xce;
using namespace xce::searchcache;

bool belongsMe(int id){
  if (CLUSTER==0) return true;
  return (MOD == (id % CLUSTER));
}


void FillFromDbCacheTask(std::vector<int>& id_sets, int beginId, int limit) {
  MyUtil::init_logger("Mce", "tt_loader.log", "INFO");
  try{
    boost::shared_ptr<IDbCache> db_cache = DatabaseCacheFactory::instance().getDbCache(DSN);

    if (db_cache->Initialize() == false) {
      MCE_WARN("Db initialize failed");
      return;
    }
    
    struct timeval start0, end0, start1, end1;
    
    gettimeofday(&start0, NULL);

    IDbResult* pDbResult = db_cache->Query();

    if (pDbResult == NULL) {
      MCE_INFO("don't iterator, because Query() return null pointer!");
      return;
    }
    gettimeofday(&end0, NULL);
    int duration = ((end0.tv_sec*1000000 + end0.tv_usec) - (start0.tv_sec*1000000 + start0.tv_usec))/1000;
    std::cout<<" NULL Query cost "<<duration<<" ms"<<std::endl;
    long int uid = beginId;//0;
    int count = 0;
    while (BinStringPtr binObj = pDbResult->FetchObject(uid) ) {
      if(!belongsMe(uid) || uid == 0) continue;
      if(count >= limit) break;
      id_sets.push_back(uid);
      count++;
      MCE_INFO("userid is:"<<uid);
      char* pBuf = NULL;
      int len = 0;

      binObj->getBuf(&pBuf, &len);

      if (pBuf==NULL || len <= 0) continue;

      /*  SearchMemCacheDataPtr cachePtr = new SearchMemCacheData();
      //反序列化成内存cache
      if (!cachePtr->Unserialize(pBuf, len)) {
        continue;
      }

      //序列化成内存buf
      BinStringPtr binObj1 = cachePtr->Serialize();
      pBuf = NULL;
      len = 0;
      binObj1->getBuf(&pBuf, &len);
      if (pBuf==NULL || len <= 0) continue;
      //重新生成BinString的目的是压缩多分配的空间
      BinStringPtr binObj2 = new BinString(pBuf, len);
      MCE_DEBUG("loading userid=" << uid << "\t" << binObj->getBufferLen() << "\t" << binObj->getBufferCapacity() << "\t" << binObj2->getBufferLen() << "\t" << binObj2-   >getBufferCapacity());

     // ObjectCacheHelper::instance().addObject(uid, binObj2);
      uid = 0;
    }*/
    }
    gettimeofday(&end1, NULL);
    int duration1 = ((end1.tv_sec*1000000 + end1.tv_usec) - (end0.tv_sec*1000000 + end0.tv_usec))/1000;
    std::cout<<"iterator cost "<<duration1<<" ms"<<std::endl;

    db_cache->Close();
  }catch(...){
    MCE_WARN("FillTask done with exception");
  }
  MCE_INFO("FillTask done.");
}





BasicStatusResultHandlerI::BasicStatusResultHandlerI(Ice::Int& beginId, std::vector<int>& valid_ids_set) :
  _beginId(beginId), valid_ids_set_(valid_ids_set) {
  }

bool BasicStatusResultHandlerI::handle(mysqlpp::Row& row) const {
  int id = (int) row["id"];
  int status = (int) row["status"];
  if (status < 3 && belongsMe(id)) { 
    valid_ids_set_.push_back(id);
  }
  return true;
}






void usage()   {
  std::cout<<"load_tt_to_memcache traversal_way|batch_way begin limit"<<std::endl;
}






void initialize(int beginId, int limit, std::vector<int> & ids)  {
  int count = beginId;
  while(count < limit)  {
    Statement status_sql;
    status_sql << "SELECT " << USER_STATUS_FIELD << " FROM user_passport WHERE id > "
      << beginId << " LIMIT " << IDS_BATCH_SIZE;
    BasicStatusResultHandlerI status_handler(beginId, ids);
    QueryRunner(DB_SEARCH2_USERBASIC_STATUS, CDbRServer).query(status_sql, status_handler);
    count = ids.size();
  }
}



int main(int argc, char ** argv)  {
  MyUtil::init_logger("Mce", "tt_loader.log", "INFO");
  if(argc <= 3)  {
    usage();
    return 0;
  }  else  {
    int begin, limit;
    std::map<int, ByteSeq> data_map;
    std::vector<int> id_sets;
    struct timeval total_start, total_end, total_start1, total_end1;
    if(strcmp(argv[1],"traversal_way") == 0)  {
      if(argc == 4)  {
        begin = atoi(argv[2]);
        limit = atoi(argv[3]);
        std::vector<int> id_sets;
        TimeCost tc = TimeCost::create("traversal_way");
        gettimeofday(&total_start, NULL);
        FillFromDbCacheTask(id_sets, begin, limit);
        gettimeofday(&total_end, NULL);
        int duration = ((total_end.tv_sec*1000000 + total_end.tv_usec) - (total_start.tv_sec*1000000 + total_start.tv_usec))/1000;
        std::cout<<"traversal cost "<<duration<<" ms"<<std::endl;
        tc.step("traversal end!");
        boost::shared_ptr<IDbCache> db_cache = DatabaseCacheFactory::instance().getDbCache(DSN);

        if (db_cache->Initialize() == false) {
          MCE_INFO("Db initialize failed");
          std::cout<<"Db initialize failed"<<std::endl;
          return 0;
        }
        gettimeofday(&total_start1, NULL);
        std::cout<<"vec size is "<<id_sets.size()<<std::endl;
        db_cache->Query(id_sets, data_map);
        std::cout<<"map size is "<<data_map.size()<<std::endl;
        gettimeofday(&total_end1, NULL);
        int duration1 = ((total_end1.tv_sec*1000000 + total_end1.tv_usec) - (total_start1.tv_sec*1000000 + total_start1.tv_usec))/1000;
        std::cout<<"batch cost "<<duration1<<" ms"<<std::endl;

      } else  {
        usage();
        return 0;
      }  
    }  else if(strcmp(argv[1], "batch_way")  == 0)  {
      std::map<int, ByteSeq> data_map;
      struct timeval total_start0, total_start1, total_end;
      if(argc == 4)  {
        begin = atoi(argv[2]);
        limit = atoi(argv[3]);
        TimeCost tc = TimeCost::create("batch_way");
        gettimeofday(&total_start0, NULL);
        initialize(begin, limit, id_sets);
        gettimeofday(&total_start1, NULL);
        TokyoTyrantCache* t = new TokyoTyrantCache(DSN);
        t->Query(id_sets, data_map);
        gettimeofday(&total_end, NULL);
        int duration0 = ((total_end.tv_sec*1000000 + total_end.tv_usec) - (total_start0.tv_sec*1000000 + total_start0.tv_usec))/1000;
        int duration1 = ((total_end.tv_sec*1000000 + total_end.tv_usec) - (total_start1.tv_sec*1000000 + total_start1.tv_usec))/1000;
        std::cout<<"don't read database cost "<<duration0<<std::endl;
        std::cout<<"total cost "<<duration1<<std::endl;
        tc.step("batch_way end!");        
      } else {
        usage();
        return 0;
      }
    }  else  {
      usage();
      return 0;
    }
  }

}

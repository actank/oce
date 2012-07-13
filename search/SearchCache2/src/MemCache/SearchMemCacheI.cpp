#include <execinfo.h>
#include "QueryRunner.h"
#include "../Util/DatabaseCacheFactory.h"
#include "SearchMemCacheI.h"

//***************************************************************************

//初始化
ObjectCacheHelper* ObjectCacheHelper::__instance = 0;
IceUtil::Mutex ObjectCacheHelper::__mutex;


void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();

	int cluster = service.getCommunicator()->getProperties()->getPropertyAsInt("SearchCacheN.Cluster");
	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("SearchCacheN.Mod");
    //获得cached的存储位置
    //cached=0:内存cache,从mysql数据库加载数据到DBCache
    //cached=1:从DbCached加载数据，对外提供服务
	int cached = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("SearchCacheN.FromCache", 0);
    //
	string dsn  = service.getCommunicator()->getProperties()->getProperty("SearchCacheN.Dsn");

	//string dsn  = service.getCommunicator()->getProperties()->getPropertyWithDefault("SearchCache.Dsn", TOKYOTYRANT_DSN_PREFIX"127.0.0.1:1982");//线上1981
    ObjectCacheHelper::instance().bootstrap(BUCKET, cluster);
    ObjectCacheHelper::instance().registerObjectCache(cached);

    //service.registerObjectCache(SEARCH_CACHE, "SC", new SearchCacheFromDBFactoryI(cached));

	TaskManager::instance().config(TASK_LEVEL_FILL, ThreadPoolConfig(1, 1));
	TaskManager::instance().config(TASK_LEVEL_UPDATE_DBCACHE, ThreadPoolConfig(16, 32));

	service.getAdapter()->add(&SearchCacheManagerI::instance(),service.createIdentity(SEARCH_CACHE_MANAGER, ""));

	SearchCacheManagerI::instance().bootstrap(cluster, mod, cached, dsn);

	service.registerXceObserver(new ServiceSchemaI);
}

//***************************************************************************

BasicStatusResultHandlerI::BasicStatusResultHandlerI(Ice::Int& beginId, std::vector<int>& thread) :
  beginId_(beginId), thread_(thread) {
  }

bool BasicStatusResultHandlerI::handle(mysqlpp::Row& row) const {
  int id = (int) row["id"];
  int status = (int) row["status"];
  if (status < 3 && SearchCacheManagerI::instance().belongsMe(id)) {
    thread_.push_back(id);
  }
  if(id>beginId_)
    beginId_=id;
  return true;
}



SearchCacheManagerI::SearchCacheManagerI() : _valid(false) {
	_invokeLoad.start(128 * 1024).detach();
}

void SearchCacheManagerI::bootstrap(int cluster, int mod, int cached, string& dsn) {
	_cluster = cluster;
	_mod = mod;
	_cached = cached;
    _dsn = dsn;

    TaskManager::instance().execute(new FillFromDbCacheTask());

	MCE_INFO("bootstrap cluster="<<_cluster << " mod="<<_mod);
}

SearchCacheResult SearchCacheManagerI::getSearchCache(::Ice::Int userId,const Ice::Current&) {
	MCE_DEBUG("Get SearchCache");
	return getSearchCacheData(userId)->getProperties();
}

Int2ByteSeq SearchCacheManagerI::getSearchCacheList2(const ::MyUtil::IntSeq& userIds, const Ice::Current& ic) {

    struct timeval tvStart;
    struct timeval tvStart1;
    struct timeval tvEnd;
	MyUtil::LongSeq uids;

    gettimeofday(&tvStart, NULL);

	Int2ByteSeq result;
	for (MyUtil::IntSeq::const_iterator it = userIds.begin(); it!= userIds.end(); ++it) {
        BinStringPtr binStrPtr = ObjectCacheHelper::instance().findObject<BinStringPtr> (*it);
		if (binStrPtr.get() != NULL) {
            char* pBuf = NULL;
            int len = 0;
            binStrPtr->getBuf(&pBuf, &len);
            if (pBuf == NULL || len==0) continue;
            result[*it].assign(pBuf, pBuf+len);
		} else {
            //scq
            if (*it != 0) {
                uids.push_back(*it);
            }
		}
	}

    gettimeofday(&tvStart1, NULL);

    if (!uids.empty())
        _invokeLoad.invoke(uids);

    gettimeofday(&tvEnd, NULL);
    int start = tvStart.tv_sec*1000000 + tvStart.tv_usec;
    int start1 = tvStart1.tv_sec*1000000 + tvStart1.tv_usec;
    int end = tvEnd.tv_sec*1000000 + tvEnd.tv_usec;
    if ((end - start) > 5000) {
        const Ice::ConnectionPtr con = ic.con;
        if (con.get() != NULL) {
            MCE_INFO(con->toString() << " Get SearchCache List2: time=" << end-start << "us " << 
                     "invokeLoad: time=" << end - start1 << "us total=" << userIds.size());
        }
    }
	return result;
}

Int2SearchCacheResultMap SearchCacheManagerI::getSearchCacheList(const ::MyUtil::IntSeq& userIds, const Ice::Current& ic) {

    struct timeval tvStart;
    struct timeval tvStart1;
    struct timeval tvEnd;
	MyUtil::LongSeq uids;

    gettimeofday(&tvStart, NULL);

	Int2SearchCacheResultMap result;
	for (MyUtil::IntSeq::const_iterator it = userIds.begin(); it!= userIds.end(); ++it) {
        BinStringPtr binStrPtr = ObjectCacheHelper::instance().findObject<BinStringPtr> (*it);
		if (binStrPtr.get() != NULL) {
            SearchMemCacheDataPtr pSCData = new SearchMemCacheData();
            char* pBuf = NULL;
            int len = 0;
            binStrPtr->getBuf(&pBuf, &len);

            if (pBuf==NULL || len <= 0) continue;

            if (pSCData->Unserialize(pBuf, len)) {
                result[*it] = pSCData->getProperties();
            }
		} else {
            //scq
            if (*it != 0) {
                uids.push_back(*it);
            }
		}
	}

    gettimeofday(&tvStart1, NULL);

    if (!uids.empty())
        _invokeLoad.invoke(uids);

    gettimeofday(&tvEnd, NULL);
    int start = tvStart.tv_sec*1000000 + tvStart.tv_usec;
    int start1 = tvStart1.tv_sec*1000000 + tvStart1.tv_usec;
    int end = tvEnd.tv_sec*1000000 + tvEnd.tv_usec;
    if ((end - start) > 5000) {
        const Ice::ConnectionPtr con = ic.con;
        if (con.get() != NULL) {
            MCE_INFO(con->toString() << " Get SearchCache List: time=" << end-start << "us " << 
                     "invokeLoad: time=" << end - start1 << "us total=" << userIds.size());
        }
    }
	return result;
}

void SearchCacheManagerI::load(const ::MyUtil::IntSeq& ids, const Ice::Current&) {
    //if (!_valid) return;
	MyUtil::LongSeq uids;
	for (MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
		if (belongsMe(*it)) {
			uids.push_back(*it);
			//MCE_DEBUG("SearchCacheManagerI::load IS " << *it);
		} else {
			//MCE_DEBUG("SearchCacheManagerI::load NOT " << *it);
		}
	}

    if (!uids.empty()) {
        std::sort(uids.begin(),uids.end()); 
        MyUtil::LongSeq::iterator end = std::unique(uids.begin(),uids.end()); 
        uids.resize(end - uids.begin());
    }


    _invokeLoad.invoke(uids);
}
void SearchCacheManagerI::update(const UpdateUnitSeq& updateUnits, const Ice::Current&) {
    MCE_INFO("Entering SearchCacheManagerI::update, updateUnits.size() is "<<updateUnits.size());
    return ;
}


bool SearchCacheManagerI::isValid(const Ice::Current&) {
	RWRecMutex::RLock lock(_validMutex);
	return _valid;
}

void SearchCacheManagerI::setValid(bool valid, const Ice::Current&) {
	RWRecMutex::WLock lock(_validMutex);
	_valid = valid;
}

SearchMemCacheDataPtr SearchCacheManagerI::getSearchCacheData(int userId) {
    SearchMemCacheDataPtr result = new SearchMemCacheData();
    return result;
}

bool SearchCacheManagerI::puts(const Int2ByteSeq& id2BytesMap, const Ice::Current&) {
  MCE_INFO("SearchCacheManagerI::puts id2BytesMap.size()=" << id2BytesMap.size());
    Int2ByteSeq::const_iterator mapIter = id2BytesMap.begin();
    for(; mapIter!=id2BytesMap.end(); ++mapIter)
    {
            int userId = mapIter->first;
            if(!SearchCacheManagerI::instance().belongsMe(userId)) continue;
            SearchMemCacheDataPtr pSCData = new SearchMemCacheData();
            if (pSCData->Unserialize((const char*)(*mapIter).second.data(), (*mapIter).second.size())) {
                //序列化成内存buf
                BinStringPtr binObj1 = pSCData->Serialize();
                char* pBuf = NULL;
                int len = 0;
                binObj1->getBuf(&pBuf, &len);
                if (pBuf==NULL || len <= 0) continue;
                //重新生成BinString的目的是压缩多分配的空间
                BinStringPtr binObj2 = new BinString(pBuf, len);
                ObjectCacheHelper::instance().removeObject(userId);
                ObjectCacheHelper::instance().addObject(userId, binObj2);
            }

    }

    return true;
}

bool SearchCacheManagerI::removes(const MyUtil::LongSeq& userIds, const Ice::Current&) {
  MCE_INFO("SearchCacheManagerI::removes userIds.size()=" << userIds.size());
    for(MyUtil::LongSeq::const_iterator iter=userIds.begin(); iter!=userIds.end(); ++iter)
    {
        int userId = *iter;
        if(!SearchCacheManagerI::instance().belongsMe(userId)) continue;
        ObjectCacheHelper::instance().removeObject(userId);
    }
    return true;
 }


//***************************************************************************
LoadUpdateDataCacheTask::LoadUpdateDataCacheTask(MyUtil::LongSeq uids) : 
	MyUtil::Task(TASK_LEVEL_UPDATE_DBCACHE), uids_(uids) {
}

//TODO:每次都要重连TT，不好，应该只初始化一次即可
void LoadUpdateDataCacheTask::handle() {
    if (uids_.empty()) return;

	MCE_INFO("SearchCacheManagerI::load " << uids_.size() << " belongs to me");

    boost::shared_ptr<IDbCache> db_cache = DatabaseCacheFactory::instance().getDbCache(SearchCacheManagerI::instance().getDSN());

    if (db_cache->Initialize() == false) {
        MCE_WARN("Db initialize failed");
		return;
    }

    for (MyUtil::LongSeq::const_iterator it = uids_.begin(); it != uids_.end(); ++it) {
        if(!SearchCacheManagerI::instance().belongsMe(*it)) continue;

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
//***************************************************************************

FillFromDbCacheTask::FillFromDbCacheTask() :
	MyUtil::Task(TASK_LEVEL_FILL) {
}

void FillFromDbCacheTask::handle() {
  try{
    int count, cycle, begin = 0,  maxid = -1;
    std::vector<LoadCache *>  _vecInvokeLoad; 
    std::vector<IceUtil::ThreadControl> thread_controls;
    std::vector<LoadCache *> threads; 
    for (int i=0; i<kInvokeLoadThreadNum; ++i) {
      boost::shared_ptr<IDbCache> db_cache = DatabaseCacheFactory::instance().getDbCache(SearchCacheManagerI::instance().getDSN());
      if(db_cache->Initialize()){
        LoadCache * loader = new LoadCache(db_cache);
        threads.push_back(loader);
        thread_controls.push_back(loader->start());
      }
      else  {
        MCE_WARN("Db initialize failed");
        return; 
      } 
    }
    //MCE_INFO("VEC thread size is "<<threads.size());
    if(threads.empty())
      return;


    count = begin;
    cycle = 0;
    MCE_INFO("start reading db...");
    TimeCost tc = TimeCost::create("load_data",1);
    while(count < LIMIT)  {
      std::vector<int> ids;
      Statement status_sql;
      if(maxid == begin)  {
        break;
      }
      status_sql << "SELECT " << USER_STATUS_FIELD << " FROM user_passport WHERE id > "
        << begin << " LIMIT " << IDS_BATCH_SIZE;
      maxid = begin;
      BasicStatusResultHandlerI status_handler(begin, ids);
      QueryRunner(DB_SEARCH2_USERBASIC_STATUS, CDbRServer).query(status_sql, status_handler);
      count += ids.size();
     
      //MCE_INFO("READ AFTER DB ids size is "<<ids.size());
      threads.at(cycle%threads.size())->invoke(ids);
      cycle++;

      MCE_INFO("cycle is "<< cycle<<" total : "<<count);
    }
    tc.step("Db load done!");
    for(vector<LoadCache *>::iterator it=threads.begin();it!=threads.end();++it){
      (*it)->stop();
    }
    for (vector<IceUtil::ThreadControl>::iterator i=thread_controls.begin(); i!=thread_controls.end(); ++i) {
      i->join();
    }
    tc.step("TT load done!");

        /*IDbResult* pDbResult = db_cache->Query();

        if (pDbResult == NULL) {
            MCE_WARN("don't iterator, because Query() return null pointer!");
            return;
        }

        long int uid = 0;
        
        while (BinStringPtr binObj = pDbResult->FetchObject(uid)) {
            if(!SearchCacheManagerI::instance().belongsMe(uid) || uid == 0) continue;
           
            MCE_DEBUG("userid is:"<<uid); 
            char* pBuf = NULL;
            int len = 0;

            binObj->getBuf(&pBuf, &len);

            if (pBuf==NULL || len <= 0) continue;

            SearchMemCacheDataPtr cachePtr = new SearchMemCacheData();
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


            MCE_DEBUG("loading userid=" << uid << "\t" << binObj->getBufferLen() << "\t" << binObj->getBufferCapacity() << "\t" << binObj2->getBufferLen() << "\t" << binObj2->getBufferCapacity());

            ObjectCacheHelper::instance().addObject(uid, binObj2);
            uid = 0;
        }
       */ 
        //db_cache->Close();
	}catch(...){
		MCE_WARN("FillTask done with exception");
	}
	SearchCacheManagerI::instance().setValid(true);
	MCE_INFO("FillTask done.");
}
//***************************************************************************

SearchCacheFromDBFactoryI::SearchCacheFromDBFactoryI(int cached) : _cached(cached) {
}

Ice::ObjectPtr SearchCacheFromDBFactoryI::create(Ice::Long id) {
	MCE_DEBUG("Loading " << id);

	SearchMemCacheDataPtr result = new SearchMemCacheData;

	if(!SearchCacheManagerI::instance().belongsMe(id)) return NULL;

	result->_id = id;

	//userid=0是没用的数据,直接过滤掉
	if (id == 0) {
		return NULL;
	}


    boost::shared_ptr<IDbCache> db_cache = DatabaseCacheFactory::instance().getDbCache(SearchCacheManagerI::instance().getDSN());
    if (db_cache->Initialize() == false)
    {
        MCE_WARN("Db initialize failed");
        return NULL;
    }

    if (!db_cache->Query(id, result.get())) {
        //scq
        if (id != 0) {
            MCE_WARN("XXX cached Id = " << id << " is failed" << " load from database");
            return NULL;
        }
    }
    db_cache->Close();

	return result->Serialize();
}

MyUtil::ObjectResultPtr SearchCacheFromDBFactoryI::create(const MyUtil::LongSeq& ids) {
	MCE_DEBUG("Loading " << ids.size() << " objects");
	ObjectResultPtr results = new ObjectResult;
	if (ids.empty()) {
		return results;
	}

    /*
     */
    boost::shared_ptr<IDbCache> db_cache = DatabaseCacheFactory::instance().getDbCache(SearchCacheManagerI::instance().getDSN());

    if (db_cache->Initialize() == false)
    {
        MCE_WARN("Db initialize failed");
        return results;
    }


    for (MyUtil::LongSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {

        if(!SearchCacheManagerI::instance().belongsMe(*it)) continue;

//        MyUtil::ObjectMap::iterator itResult = results->data.find(*it);
//      if (itResult == results->data.end()) {
            SearchMemCacheDataPtr obj = new SearchMemCacheData;
            if (db_cache->Query(*it, obj.get())) {
                results->data.insert(make_pair(*it, obj->Serialize()));
            } else {
                //scq
                if (*it != 0) {
                    MCE_WARN("cached Id = " << (*it) << " is failed" << " load from database");
                }
            }
//       } else {
//           //scq
//           if (*it != 0) {
//               MCE_WARN("unreachable code segment " << __FILE__ << " " << __LINE__);
//           }
//       }
    }
    db_cache->Close();

	return results;
}
//***************************************************************************

void ServiceSchemaI::configure(const Ice::PropertiesPtr& props) {
	TaskManager::instance().config(props, "Fill", TASK_LEVEL_FILL, ThreadPoolConfig(1, 1));
}



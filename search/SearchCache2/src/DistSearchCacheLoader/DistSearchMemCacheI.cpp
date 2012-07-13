#include <execinfo.h>
#include "QueryRunner.h"
#include "../Util/DatabaseCacheFactory.h"
#include "DistSearchMemCacheI.h"

//***************************************************************************


void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();

	int cluster = service.getCommunicator()->getProperties()->getPropertyAsInt("DistSearchCachePreloader.Cluster");

  vector<string> tt_addresses;

  for(int i = 0; i < cluster; i++)  {
    char address_char[15]; 
    SearchCacheManagerI::instance().itoa(i, address_char);
    string address = (string)address_char;
    address = service.getCommunicator()->getProperties()->getProperty("DistSearchCachePreloader.TT"+address);
    tt_addresses.push_back(address);
  }

	TaskManager::instance().config(TASK_LEVEL_FILL, ThreadPoolConfig(1, 1));
	TaskManager::instance().config(TASK_LEVEL_UPDATE_DBCACHE, ThreadPoolConfig(16, 32));

	service.getAdapter()->add(&SearchCacheManagerI::instance(),service.createIdentity(SEARCH_CACHE_MANAGER, ""));

	SearchCacheManagerI::instance().bootstrap(cluster, tt_addresses);

	service.registerXceObserver(new ServiceSchemaI);
}

//***************************************************************************

BasicStatusResultHandlerI::BasicStatusResultHandlerI(Ice::Int& beginId, std::vector<std::vector<int> >& thread, int cluster) :
  beginId_(beginId), thread_(thread), cluster_(cluster) {
  }

bool BasicStatusResultHandlerI::handle(mysqlpp::Row& row) const {
  int id = (int) row["id"];
  int status = (int) row["status"];
  if (status < 3 /*&& SearchCacheManagerI::instance().belongsMe(id)*/) {
    thread_.at(id % cluster_).push_back(id);
  }
  if(id>beginId_)
    beginId_=id;
  return true;
}



SearchCacheManagerI::SearchCacheManagerI() : _valid(false) {
	//_invokeLoad.start(128 * 1024).detach();
}

int SearchCacheManagerI::itoa(int i, char* string)  {
    int power,j; j=i;
    int count = 0;
    for(power=1;j>=10;j/=10) power*=10;
    for(;power>0;power/=10) {
      *string++ ='0' + i/power;
      i%=power;
      count ++;
    }
    *string='\0';
    return count;
}

void SearchCacheManagerI::bootstrap(int cluster, vector<string>& dsns) {
	_cluster = cluster;
  _dsns = dsns;
  DistSearchCacheAdapter::instance().initialize();
    TaskManager::instance().execute(new FillFromDbCacheTask());

	MCE_INFO("bootstrap cluster="<<_cluster);
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
        BinStringPtr binStrPtr = NULL;//ObjectCacheHelper::instance().findObject<BinStringPtr> (*it);
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
        BinStringPtr binStrPtr = NULL;//ObjectCacheHelper::instance().findObject<BinStringPtr> (*it);
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
		if (true/*belongsMe(*it)*/) {
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
            //if(!SearchCacheManagerI::instance().belongsMe(userId)) continue;
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
                //ObjectCacheHelper::instance().removeObject(userId);
                //ObjectCacheHelper::instance().addObject(userId, binObj2);
            }

    }

    return true;
}

bool SearchCacheManagerI::removes(const MyUtil::LongSeq& userIds, const Ice::Current&) {
  MCE_INFO("SearchCacheManagerI::removes userIds.size()=" << userIds.size());
    for(MyUtil::LongSeq::const_iterator iter=userIds.begin(); iter!=userIds.end(); ++iter)
    {
        int userId = *iter;
        //if(!SearchCacheManagerI::instance().belongsMe(userId)) continue;
        //ObjectCacheHelper::instance().removeObject(userId);
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

    boost::shared_ptr<IDbCache> db_cache = DatabaseCacheFactory::instance().getDbCache(SearchCacheManagerI::instance().getDSN(0));

    if (db_cache->Initialize() == false) {
        MCE_WARN("Db initialize failed");
		return;
    }

    for (MyUtil::LongSeq::const_iterator it = uids_.begin(); it != uids_.end(); ++it) {
       // if(!SearchCacheManagerI::instance().belongsMe(*it)) continue;

		SearchMemCacheDataPtr obj = new SearchMemCacheData;
		if (db_cache->Query(*it, obj.get())) {
			int id = *it;
			//ObjectCacheHelper::instance().removeObject(id);
			//ObjectCacheHelper::instance().addObject(id, obj->Serialize());
		} else {
			//ObjectCacheHelper::instance().removeObject(*it);
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
      if(SearchCacheManagerI::instance().getDSN(i % (kInvokeLoadThreadNum/16)) == "wrong TT")  {
        MCE_WARN("Mod = "<<i<<" TT wrong!");
        continue;
      }
      MCE_INFO(SearchCacheManagerI::instance().getDSN(i));
      boost::shared_ptr<IDbCache> db_cache = DatabaseCacheFactory::instance().getDbCache(SearchCacheManagerI::instance().getDSN(i % (kInvokeLoadThreadNum/16)));
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
    while(count < LIMIT)  {
      std::vector<std::vector<int> > ids;
      for(int i = 0; i < kInvokeLoadThreadNum; ++i)  {
        std::vector<int> tmp;
        ids.push_back(tmp);
      }
      Statement status_sql;
      if(maxid == begin)  {
        break;
      }
      status_sql << "SELECT " << USER_STATUS_FIELD << " FROM user_passport WHERE id > "
        << begin << " LIMIT " << IDS_BATCH_SIZE;
      maxid = begin;
      BasicStatusResultHandlerI status_handler(begin, ids,kInvokeLoadThreadNum);
      QueryRunner(DB_SEARCH2_USERBASIC_STATUS, CDbRServer).query(status_sql, status_handler);
      //count += ids.size();
     
      //MCE_INFO("READ AFTER DB ids size is "<<ids.size());
      for(int i = 0; i < kInvokeLoadThreadNum; ++i)  {
        threads.at(i)->invoke(ids.at(i));
        count += ids.at(i).size();
      }
      //threads.at(cycle%threads.size())->invoke(ids);
      cycle++;

      MCE_INFO("cycle is "<< cycle<<" total : "<<count);
    }
    for(vector<LoadCache *>::iterator it=threads.begin();it!=threads.end();++it){
      (*it)->stop();
    }
    for (vector<IceUtil::ThreadControl>::iterator i=thread_controls.begin(); i!=thread_controls.end(); ++i) {
      i->join();
    }
	}catch(...){
		MCE_WARN("FillTask done with exception");
	}
	SearchCacheManagerI::instance().setValid(true);
	MCE_INFO("FillTask done.");
}

//***************************************************************************

void ServiceSchemaI::configure(const Ice::PropertiesPtr& props) {
	TaskManager::instance().config(props, "Fill", TASK_LEVEL_FILL, ThreadPoolConfig(1, 1));
}


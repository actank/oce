#ifndef __SEARCHCACHEI_H__ 
#define __SEARCHCACHEI_H__ 
#include <SearchCache.h> 
#include "../Util/SearchMemCacheData.h" 
#include "../Util/BinString.h"
#include <ServiceI.h> 
#include "util/cpp/TimeCost.h"
#include <IceUtil/IceUtil.h> 
#include <QueryRunner.h> 
#include "../DistSearchCacheLogic/DbOperation.h"
#include "DistSearchCacheAdapter.h"
#include "../Util/SearchCacheData.h"


using namespace MyUtil; 
using namespace IceUtil; 
using namespace com::xiaonei::xce; 
using namespace xce::searchcache; 
using namespace xce::searchcache::dboperation;
using namespace xce::dist::searchcache;
namespace xce { 
  namespace searchcache {

const Ice::Int SEARCH_CACHE = 0;
const Ice::Int BUCKET = 10;

const std::string SEARCH_CACHE_MANAGER = "M";

const int TASK_LEVEL_FILL = 1;
const int TASK_LEVEL_UPDATE_DBCACHE = 9;

const string USER_STATUS_FIELD = " id, status ";
const string DB_SEARCH2_USERBASIC_STATUS = "search_user_passport";
const int IDS_BATCH_SIZE = 15*2000;
const int kInvokeLoadThreadNum = 64;
const int NULL_NUM = 5; 
const int LIMIT = 200000000;
//***************************************************************************

class BasicStatusResultHandlerI: public com::xiaonei::xce::ResultHandler {
  public:
    BasicStatusResultHandlerI(Ice::Int& beginId, std::vector<std::vector<int> >& thread, int cluster);
    virtual bool handle(mysqlpp::Row& row) const;
  private:
    Ice::Int& beginId_;
    std::vector<std::vector<int> >& thread_;
    int cluster_;
};

class LoadCache: virtual public IceUtil::Thread {
  public:
    LoadCache(boost::shared_ptr<IDbCache> db_cache):_db_cache(db_cache),_shouldStop(0){
    }
    void invoke(const MyUtil::IntSeq& userIds) {
      MCE_INFO("invoke... userIds.size is "<< userIds.size());
      if (userIds.empty()) {
        return;
      }
      IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
      _cache.insert(_cache.end(), userIds.begin(), userIds.end());
      
      //_safe_value.insert(_safe_value.end(), safeValues.begin(), safeValues.end());
      if ((int)_cache.size() >= IDS_BATCH_SIZE) {
        _mutex.notify();
      }
    }
    void insert(const MyUtil::IntSeq& userIds){
      if(userIds.empty())
        return;
      int total=0;
      while(total < userIds.size()){
        vector<int>::const_iterator itA = userIds.begin()+total;
        vector<int>::const_iterator itB = total+IDS_BATCH_SIZE > userIds.size() ? userIds.end():itA+IDS_BATCH_SIZE;
        total += (itB - itA);
        vector<int> ids(itA,itB);
        std::map<int, ByteSeq> data_map;
        std::vector<SearchCacheDataPtr> * data_array_ptr = new std::vector<SearchCacheDataPtr>();
        _db_cache->Query(ids, data_array_ptr);
        
        std::map<long, SearchCacheMemDataPtr> sc_map;
        for(std::vector<SearchCacheDataPtr>::iterator iter = data_array_ptr->begin(); iter!= data_array_ptr->end(); iter++)  {
           SearchCacheMemDataPtr tmp = new SearchCacheMemData;
           
           TransformData((*iter), tmp);
          
           MCE_INFO("loading userid=" << (*iter)->_id);
           sc_map[(long)((*iter)->_id)] = tmp;
        }
        DistSearchCacheAdapter::instance().setData(sc_map);
        sc_map.clear();
        data_array_ptr->clear(); 
        delete data_array_ptr;
        MCE_INFO("ids size is "<<ids.size()<<" DATA_MAP SIZE IS "<<data_map.size());
        //insert into objectacache
      }
    }

    void TransformData(SearchCacheDataPtr source, SearchCacheMemDataPtr dest)  {
      dest->set_status(source->_status);
      dest->set_browseconfig(source->_browse_config);
      dest->set_statusconfig(source->_status_config);
      dest->set_basicconfig(source->_basic_config);
      dest->set_profileprivacy(source->_profile_privacy);
      dest->set_headurl(source->_headurl);
      dest->set_tinyurl(source->_tinyurl);
      dest->set_state(source->_state);
      dest->set_stage(source->_stage);
      dest->set_name(source->_name);
      dest->set_gender(source->_gender);
      for(std::vector<Network>::iterator iter = source->_networks.begin(); iter != source->_networks.end(); iter++)  {
        NetWorkInfo * tmp = dest->add_networks();
        tmp->set_id(iter->id);
        tmp->set_stage(iter->stage);
        tmp->set_name(iter->name);
      }
      
      for(std::vector<ElementarySchool>::iterator iter = source->_elementarySchools.begin(); iter != source->_elementarySchools.end(); iter++)  {
        ElementarySchoolInfo * tmp = dest->add_elementaryschools();
        tmp->set_id(iter->id);
        tmp->set_year(iter->year);
        tmp->set_name(iter->name);
      }
      for(std::vector<JuniorSchool>::iterator iter = source->_juniorSchools.begin(); iter != source->_juniorSchools.end(); iter++)  {
        JuniorSchoolInfo * tmp = dest->add_juniorschools();
        tmp->set_id(iter->id);
        tmp->set_year(iter->year);
        tmp->set_name(iter->name);
      }
      for(std::vector<HighSchool>::iterator iter = source->_highSchools.begin(); iter != source->_highSchools.end(); iter++)  {
        HighSchoolInfo * tmp = dest->add_highschools();
        tmp->set_id(iter->id);
        tmp->set_name(iter->name);
        tmp->set_year(iter->year);
        tmp->set_class1(iter->class1);
        tmp->set_class2(iter->class2);
        tmp->set_class3(iter->class3);
      }

      for(std::vector<College>::iterator iter = source->_colleges.begin(); iter != source->_colleges.end(); iter++)  {
        CollegeInfo * tmp = dest->add_collegeschools();
        tmp->set_id(iter->id);
        tmp->set_name(iter->name);
        tmp->set_year(iter->year);
        tmp->set_department(iter->department);
      }

      for(std::vector<University>::iterator iter = source->_universities.begin(); iter != source->_universities.end(); iter++)  {
        UniversityInfo * tmp = dest->add_universityschools();
        tmp->set_id(iter->id);
        tmp->set_name(iter->name);
        tmp->set_year(iter->year);
        tmp->set_department(iter->department);
        tmp->set_dorm(iter->dorm);
      }

      for(std::vector<Workplace>::iterator iter = source->_workplaces.begin(); iter != source->_workplaces.end(); iter++)  {
        WorkplaceInfo * tmp = dest->add_workplaces();
        tmp->set_id(iter->id);
        tmp->set_name(iter->name);
      }

      for(std::vector<Region>::iterator iter = source->_regions.begin(); iter != source->_regions.end(); iter++)  {
        RegionInfo * tmp = dest->add_regions();
        tmp->set_id(iter->id);
        tmp->set_city(iter->city);
        tmp->set_province(iter->province);
      }
    }

    void stop(){
      IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
      _shouldStop = 1;
    }

  protected:
    virtual void run() {
      while (true) {
        try {
          //MCE_INFO("THREAD RUN...");
          MyUtil::IntSeq batch;
          {
            IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
            if(_cache.empty()){
              if(_shouldStop == 1){
                //MCE_INFO("STOP AND EMPTY");
                break;
              }
            }
            if (_cache.size() < IDS_BATCH_SIZE) {
              _mutex.timedWait(IceUtil::Time::seconds(1));
            }
            batch.swap(_cache);
          }
          insert(batch);
        } catch (...) {
          MCE_WARN("InvokeReload main loop exception");
        }
      }
    }

  private:
    MyUtil::IntSeq _cache;
    boost::shared_ptr<IDbCache> _db_cache;
    int _shouldStop;
    IceUtil::Monitor<IceUtil::Mutex> _mutex;
};
class LoadUpdateDataCacheTask: virtual public MyUtil::Task {
private:
    MyUtil::LongSeq uids_;
public:
	LoadUpdateDataCacheTask(MyUtil::LongSeq uids);
	virtual void handle();
};

class SearchCacheManagerI: virtual public SearchCacheManager,
		public MyUtil::Singleton<SearchCacheManagerI> {
public:
	SearchCacheManagerI();
	virtual SearchCacheResult getSearchCache(::Ice::Int, const Ice::Current&);

	virtual Int2SearchCacheResultMap getSearchCacheList(const ::MyUtil::IntSeq&,
			const Ice::Current&);
    virtual Int2ByteSeq getSearchCacheList2(const ::MyUtil::IntSeq&, 
            const ::Ice::Current&);

	virtual void load(const ::MyUtil::IntSeq&, const Ice::Current&);
    void update(const UpdateUnitSeq& updateUnits, const Ice::Current&);
    bool puts(const Int2ByteSeq& id2Map, const Ice::Current&);
    bool removes(const MyUtil::LongSeq& userIds, const Ice::Current&);
	virtual bool isValid(const Ice::Current& = Ice::Current());
	virtual void setValid(bool valid, const Ice::Current& = Ice::Current());
  int itoa(int i,char* string);

	void bootstrap(int cluster, vector<string>& dsns);
	

    string getDSN(int mod) { 
      if(mod < 0 || mod >= _cluster) {
        return "wrong TT";
      }  else {
        return _dsns.at(mod);
      }
    }

private:
	class InvokeLoad: virtual public IceUtil::Thread {
		public:
			void invoke(const MyUtil::LongSeq& userIds) {
				if (userIds.empty()) {
					return;
				}
				IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
				_cache.insert(_cache.end(), userIds.begin(), userIds.end());

				if (_cache.size() >= 200) {
					_mutex.notify();
				}
			}
		protected:
			virtual void run() {
				while (true) {
					try {
						MyUtil::LongSeq batch;
						{
							IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
							if (_cache.size() < 200) {
								_mutex.timedWait(IceUtil::Time::seconds(1));
							}
							batch.swap(_cache);
						}

                        ::MyUtil::TaskManager::instance().execute(new LoadUpdateDataCacheTask(batch));
					} catch (...) {
						MCE_WARN("InvokeReload main loop exception");
					}
				}
			}
		private:
			MyUtil::LongSeq _cache;
			IceUtil::Monitor<IceUtil::Mutex> _mutex;
		};

	InvokeLoad _invokeLoad;

	SearchMemCacheDataPtr getSearchCacheData(int userId);

	bool _valid;
	IceUtil::RWRecMutex _validMutex;

	int _cluster;
  vector<string> _dsns;
};

class FillFromDbCacheTask: virtual public MyUtil::Task {
public:
	FillFromDbCacheTask();
	virtual void handle();
};

//***************************************************************************

class ServiceSchemaI: public MyUtil::Descriptor {
public:
	virtual void configure(const Ice::PropertiesPtr& props);
};

//***************************************************************************

}
}
#endif

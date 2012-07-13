#ifndef __FOOTPRINTNEW_I_H__
#define __FOOTPRINTNEW_I_H__

#include <limits>
#include "HomeFootprintReplicaAdapter.h"
#include <IceUtil/IceUtil.h>
#include <IceUtil/Mutex.h>
#include "FootprintUnite.h"
#include <vector>
#include <algorithm>
#include "QueryRunner.h"
#include "ServiceI.h"
#include <boost/circular_buffer.hpp>

namespace xce{
namespace footprint{

using namespace MyUtil;
using namespace xce::serverstate;

const int VALID = 0;
const int VISITSHORT = 1;
const int REACHLIMIT = 2;

const Ice::Int FOOTPRINT_SET = 0;
const Ice::Int FOOTPRINT_BUF_SET = 1;
const Ice::Int USER_VISIT_COUNT = 2;

const Ice::Int NOVISITCOUNTINTERVAL = 60*10;
const Ice::Int VIEW_COUNT_LIMIT = 500;

const static int TASK_LEVEL_BUILD = 1;
const static int TASK_LEVEL_SAVE = 2;
const static int TASK_LEVEL_ADD = 3;
const static int TASK_LEVEL_REMOVE = 4;

const static int TABLE_HOME_FOOTPRINT_HEAD_MOD = 100;
const static std::string COLUMN_VISITOR = "visitors";
const static std::string TABLE_HOME_FOOTPRINT_HEAD = "home_footprint_head";
const static std::string TABLE_HOME_FOOTPRINT_BODY = "home_footprint_body";
const static int CACHE_SIZE=9;
const static int CACHE_BUF_SIZE=39;
const static int MIN_LIMIT = 200000000;         // 分钟和秒计时的一个界限

const static std::string COLUMN_ID = "id";
const static std::string COLUMN_VIEWCOUNT = "viewcount";

const std::string COLUMN_OWNER = "owner";
const static std::string DESC_FOOTPRINT = "footprint";
const static std::string DESC_HOME_FOOTPRINT_HEAD = "home_footprint_head";
const static std::string DESC_HOME_FOOTPRINT_BODY = "home_footprint_body";
const static std::string DESC_VIEW_COUNT = "user_view_count";
const static std::string TABLE_VIEW_COUNT = "view_count";

const int PRODUCE_BATCH_SIZE = 500;
const int CONSUME_BATCH_SIZE = 500;
const int CONSUME_BATCH_THREAD_SIZE = 5;
const int WRITE_BATCH_SIZE = 1000;
const int PRX_DATAQUEUE_MAX_SIZE = 50000;

class IntSeqSerialize {
public:
   // 去掉对旧数据的serialize和unserialize 
   static std::string serialize(const BaseStepSeq& seq) {
          string result;
          vector<int> byteSeq;
          for ( size_t i = 0; i < seq.size(); ++i ) {
                  // 从旧的分钟计时到新的秒计时的转换判断
                  if ( seq[i]->timestamp < MIN_LIMIT ) {
                          seq[i]->timestamp *= 60;
                  }
                  byteSeq.push_back(seq[i]->visitor);
                  byteSeq.push_back(seq[i]->timestamp);
          }

          result.resize(seq.size() * 2 * sizeof(int));
          memcpy((void*)(result.data()), &byteSeq[0], seq.size()*2*sizeof(int));
          return result;
   };
   static std::string serialize(const HomeStepSeq& seq) {
          string result;
          vector<int> byteSeq;
          for ( size_t i = 0; i < seq.size(); ++i ) {
                  // 从旧的分钟计时到新的秒计时的转换判断
                  if ( seq[i]->timestamp < MIN_LIMIT ) {
                          seq[i]->timestamp *= 60;
                  }
                  byteSeq.push_back(seq[i]->visitor);
		  byteSeq.push_back(int(seq[i]->gid>>32L));
		  byteSeq.push_back(int(seq[i]->gid&0x7FFFFFFF));
                  byteSeq.push_back(seq[i]->timestamp);
          }

          result.resize(seq.size() * 4 * sizeof(int));
          memcpy((void*)(result.data()), &byteSeq[0], seq.size()*4*sizeof(int));
          return result;
   };
};

template<class T>
class FootprintEqualComparator : public std::unary_function<T, bool> {
public:
        FootprintEqualComparator(long visitor) :
                visitor_(visitor) {
        };

        bool operator()(const T& p) const {
            return (visitor_ == p->visitor);
        }
private:
        long visitor_;
};

class SortFootprint {
public:
        static bool OrderByTimeDesc(const BaseStepPtr& p1, const BaseStepPtr& p2) {
                return p1->timestamp > p2->timestamp;
        }
};

template<class T, class Manager>
class FootprintSet : virtual public Ice::Object,
		virtual public IceUtil::Mutex {
public:
	typedef typename boost::circular_buffer<T> __circular_buffer;
	typedef typename __circular_buffer::iterator __circular_buffer_iterator;

	FootprintSet(size_t num){
		footprints_=boost::circular_buffer<T>(num);
	};
	Ice::Int getSize() {
		IceUtil::Mutex::Lock lock(*this);
        	return footprints_.size();
	};


	std::vector<T> getAll(){
		IceUtil::Mutex::Lock lock(*this);
		std::vector<T> result;
    for (__circular_buffer_iterator it = footprints_.begin(); it != footprints_.end(); ++it) {
      result.push_back(*it);
    }
		return result;
	};
	std::vector<T> get(Ice::Int begin, Ice::Int limit, Ice::Int time=0){
		IceUtil::Mutex::Lock lock(*this);

		std::vector<T> result;

		if (begin < 0 || (size_t)begin >= footprints_.size() || limit < 0) {
      return result;
    }
		limit = min(limit, (int)(footprints_.size() - begin));

		/*T* p=footprints_.linearize();

		while(p!=NULL){
			if(result.size()>=(size_t)limit||(time>0&&(*p)->timestamp<time))
				break;
			result.push_back(*p++);
		}*/

		for (__circular_buffer_iterator it=(footprints_.begin() + begin); it != footprints_.end(); ++it) {
			if (result.size() >= (size_t)limit || (time > 0 && (*it)->timestamp < time))
				break;
			result.push_back(*it);
		}
		return result;
	};
	bool push_back(const T& p){
		IceUtil::Mutex::Lock lock(*this);
		if(!footprints_.full()){
			footprints_.push_back(p);
			return true;
		}
		return false;
			
	}
	T getTop(){
		T t = NULL;
		IceUtil::Mutex::Lock lock(*this);
		if(!footprints_.empty()){
			t = footprints_.front();
		}
		return t;
	}

        bool remove(const T& p) {
                IceUtil::Mutex::Lock lock(*this);
                
                __circular_buffer_iterator pos = find_if(footprints_.begin(), footprints_.end(), FootprintEqualComparator<T>(p->visitor));
                if (pos != footprints_.end()) {
                        footprints_.erase(pos);
                        return true;
                }else{
                        return false;
                }
        };
        bool add(const T& p) {
                IceUtil::Mutex::Lock lock(*this);

                __circular_buffer_iterator pos = find_if(footprints_.begin(), footprints_.end(), FootprintEqualComparator<T>(p->visitor));
                if (pos != footprints_.end()) {
                        footprints_.erase(pos);
                }

                p->timestamp = time(NULL);
                footprints_.push_front(p);
		return true;
        };

/*
	virtual bool modify(const FootprintInfoPtr& fpt, Manager* manager,bool isAdd,bool isSec=false) {
		IceUtil::Mutex::Lock lock(*this);

		__vector_iterator pos = std::find_if(footprints_.begin(), footprints_.end(), FootprintEqualComparator<T>(fpt->visitor));
		if (pos != footprints_.end()) {
			footprints_.erase(pos);
		}else if(!isAdd){
			return false;
		}
		if(isAdd)
			footprints_.insert(footprints_.begin(), fpt);
		for(size_t i=0;i<footprints_.size();++i){
			MCE_INFO("footprintset "<<footprints_[i]);
		}
		//manager->evictFootprint(fpt, footprints_, isSec);
		return true;
	};

	void merge(__vector& fpts) {
		IceUtil::Mutex::Lock lock(*this);

		for(__vector_iterator it=fpts.begin();
				it != fpts.end(); ++it){
			__vector_iterator pos = find_if(footprints_.begin(), footprints_.end(),
					FootprintEqualComparator<T>((*it)->visitor));
			if (pos == footprints_.end()) {
				footprints_.push_back(*it);
			}
		}
	};
	virtual	std::vector<T>& getAll() {
	        IceUtil::Mutex::Lock lock(*this);
        	return footprints_;
	};
	void push_back(vector<T>& seq){
		IceUtil::Mutex::Lock lock(*this);
		footprints_.insert(footprints_.end()-1,seq.begin(),seq.end());
	};
	vector<T> delTop(size_t num){
		IceUtil::Mutex::Lock lock(*this);
		num = num > footprints_.size()? footprints_.size():num;
		vector<T> res(num);
		res.assign(footprints_.begin(),footprints_.begin()+num);
		for(size_t i=num;i<footprints_.size();++i){
			footprints_[i-num] = footprints_[i];	
		}
		footprints_.resize(footprints_.size()-num);
		return res;
	};
*/
	void clear() {
		IceUtil::Mutex::Lock lock(*this);
		footprints_.clear();
	};

/*
	vector<T> evictFootprint(size_t maxSize){
		IceUtil::Mutex::Lock lock(*this);

		vector<T> res;
		if(footprints_.size() <= maxSize)	
			return res;
		while (footprints_.size() > (size_t)maxSize) {
			res.push_back(footprints_.back());	
			footprints_.pop_back();
		}
		return res;
	};
*/
	bool set(const mysqlpp::Row::value_type & str){
		vector<int> tmp;
		tmp.resize(str.size()/sizeof(int));       // 之所以要先/3再*3,因为3个int成员才能组成一个footprint
		memcpy((void*)(&tmp[0]), str.data(), tmp.size()*sizeof(int));

		IceUtil::Mutex::Lock lock(*this);
		for ( size_t i = 0; i < (tmp.size()>>1); ++i ) {
			BaseStepPtr fptn = new BaseStep;
			fptn->visitor = tmp[i<<1];
			fptn->timestamp = tmp[(i<<1)+1];
			if ( (fptn->visitor > 0) && (fptn->timestamp > 0)) {
				footprints_.push_back(fptn);
			}
		}
		return true;

	};
	
protected:
	 boost::circular_buffer<T> footprints_;
};

template<typename T>
class FootprintSetHandler : public com::xiaonei::xce::ResultHandler{
public:
        FootprintSetHandler(T& fsp):fsp_(fsp){};
        virtual bool handle(mysqlpp::Row& row) const {
		fsp_->set(row[COLUMN_VISITOR.c_str()]);
		return true;
	};
private:
        T& fsp_;
};

class HomeFootprintSet;
typedef IceUtil::Handle<HomeFootprintSet> HomeFootprintSetPtr;

class HomeFootprintCacheI;

class HomeFootprintSet : public FootprintSet<HomeStepPtr,HomeFootprintCacheI>{

public:
        HomeFootprintSet(size_t num) : 
		FootprintSet<HomeStepPtr,HomeFootprintCacheI> (num),
                viewCountLimitTime_((time(NULL)/(24*60*60))*(24*60*60)),
                viewCountLimtCount_(0){
	};

  HomeFootprintSet(const HomeVisitHistoryPtr& ptr, size_t num) : FootprintSet<HomeStepPtr,HomeFootprintCacheI>(num),
                                                     viewCountLimitTime_((time(NULL)/(24*60*60))*(24*60*60)),
                                                     viewCountLimtCount_(0) {
    this->ownerId_ = ptr->owner;
    for (vector<HomeStepPtr>::const_iterator it = ptr->footprints.begin(); it != ptr->footprints.end(); ++it) {
      HomeStepPtr hPtr = new HomeStep;
      hPtr->gid = (*it)->gid;
      hPtr->timestamp = (*it)->timestamp;
      hPtr->visitor = (*it)->visitor;
      this->footprints_.push_back(hPtr);
    }
  }
  static MyUtil::ObjectResultPtr changeToHomeVisitHistoryMap(const vector<HomeFootprintSetPtr>& ptrs) {
    MyUtil::ObjectResultPtr resultPtr = new MyUtil::ObjectResult;
    for (vector<HomeFootprintSetPtr>::const_iterator it = ptrs.begin(); it != ptrs.end(); ++it) {
      resultPtr->data.insert(make_pair<long, HomeVisitHistoryPtr>((*it)->getOwnerId(), (*it)->parseTo()));
    }
    return resultPtr;
  }

  static MyUtil::ObjectResultPtr changeToHomeVisitHistoryMap(const MyUtil::ObjectResultPtr& ptrs) {
    MyUtil::ObjectResultPtr resultPtr = new MyUtil::ObjectResult;
    for (map<long, Ice::ObjectPtr>::const_iterator it = ptrs->data.begin(); it != ptrs->data.end(); ++it) {
      resultPtr->data.insert(make_pair<long, HomeVisitHistoryPtr>(it->first, (HomeFootprintSetPtr::dynamicCast(it->second))->parseTo()));
    }
    return resultPtr;
  }

  static MyUtil::ObjectResultPtr changeFromHomeVisitHistoryMap(const MyUtil::ObjectResultPtr& ptr, size_t num) {
    MyUtil::ObjectResultPtr resultPtr = new MyUtil::ObjectResult;
    for (map<long, Ice::ObjectPtr>::const_iterator it = ptr->data.begin(); it != ptr->data.end(); ++it) {
      resultPtr->data.insert(make_pair<long, HomeFootprintSetPtr>(it->first, new HomeFootprintSet(HomeVisitHistoryPtr::dynamicCast(it->second), num)));
    }
    return resultPtr;
  }

  static int getCategory(string tableName) {
    if (tableName.find(TABLE_HOME_FOOTPRINT_HEAD) != string::npos) {
      return FOOTPRINT_SET;
    } else if (tableName.find(TABLE_HOME_FOOTPRINT_BODY) != string::npos) {
      return FOOTPRINT_BUF_SET;
    } else {
      return -1;
    }
  }
  HomeVisitHistoryPtr parseTo() {
    HomeVisitHistoryPtr ptr = new HomeVisitHistory;
    IceUtil::Mutex::Lock lock(*this);
    ptr->owner = this->ownerId_;
    for (boost::circular_buffer<HomeStepPtr>::const_iterator it = this->footprints_.begin(); it != this->footprints_.end(); ++it) {
      HomeStepPtr hPtr = new HomeStep;
      hPtr->gid = (*it)->gid;
      hPtr->visitor = (*it)->visitor;
      hPtr->timestamp = (*it)->timestamp;
      ptr->footprints.push_back(hPtr);
    }
    return ptr;
  }
/*
	void add(const vector<HomeStepPtr>& fpts, bool ignoreLimit=false){
		for(vector<HomeStepPtr>::const_iterator it=fpts.begin();it!=fpts.end();++it)
			add(*it,ignoreLimit);
	}

	void remove(const vector<HomeStepPtr>& fpts){
		for(vector<HomeStepPtr>::const_iterator it=fpts.begin();it!=fpts.end();++it)
			remove(*it);
	}
*/
        pair<int, HomeStepPtr> add(const HomeStepPtr& fpt, bool ignoreLimit = false) {

		pair<bool, HomeStepPtr> pair=make_pair<bool, HomeStepPtr>(VALID,NULL);	
		int now = time(NULL);
		pair.first = VALID;
                IceUtil::Mutex::Lock lock(*this);
                boost::circular_buffer<HomeStepPtr>::iterator pos = find_if(footprints_.begin(), footprints_.end(), FootprintEqualComparator<HomeStepPtr>(fpt->visitor));
                if (pos != footprints_.end()) {
                        if ( now - (*pos)->timestamp < NOVISITCOUNTINTERVAL ) { 
                                pair.first = VISITSHORT;
                        }
                        footprints_.erase(pos);
		} else if(footprints_.full()) {
			pair.second = footprints_.back();			
		}

//		fpt->timestamp = now;
		footprints_.push_front(fpt);

		//第二级索引是不涉及到人气限制相关的,或者两次访问间隔太短
		if(ignoreLimit || pair.first)
			return pair;

                // 只有脚印有效的时候，才进行以下判断人气
		pair.first = isReachViewCountLimit();			
		return pair;
        };

	void setOld(int owner,const mysqlpp::Row::value_type & str){
                vector<int> tmp;
                tmp.resize(str.size()/sizeof(int)); 
                memcpy((void*)(&tmp[0]), str.data(), tmp.size()*sizeof(int));

		ostringstream oss;
                IceUtil::Mutex::Lock lock(*this);
                for ( size_t i = 0; i < tmp.size()/2; ++i ) {
                        HomeStepPtr fptn = new HomeStep;
                        fptn->visitor = tmp[2*i];
			fptn->gid = owner; 
                        fptn->timestamp = tmp[2*i+1];
		//	oss<<fptn->visitor<<" ";
                        if ( (fptn->visitor > 0) && (fptn->timestamp >0 && (fptn->gid > 0L)) ) {
                                footprints_.push_back(fptn);
                        }
                }
	}
	virtual	void set(const mysqlpp::Row::value_type & str){
                vector<int> tmp;
                tmp.resize(str.size()/sizeof(int)); 
                memcpy((void*)(&tmp[0]), str.data(), tmp.size()*sizeof(int));

		ostringstream oss;
                IceUtil::Mutex::Lock lock(*this);
                for ( size_t i = 0; i < tmp.size()/4; ++i ) {
                        HomeStepPtr fptn = new HomeStep;
                        fptn->visitor = tmp[4*i];
                        fptn->gid = ((long)tmp[4*i+1]<<32L) + tmp[4*i+2];
                        fptn->timestamp = tmp[4*i+3];
		//	oss<<fptn->visitor<<" ";
                        if ( (fptn->visitor > 0) && (fptn->timestamp >0 && (fptn->gid > 0L)) ) {
                                footprints_.push_back(fptn);
                        }
                }
//		MCE_INFO("sequence db :"<<oss.str());
        };
  void setOwnerId(long ownerId) {
    IceUtil::Mutex::Lock lock(*this);
    ownerId_ = ownerId;
  }
  long getOwnerId() const {
    IceUtil::Mutex::Lock lock(*this);
    return ownerId_;
  }
private:
        int isReachViewCountLimit() {
                if ( viewCountLimtCount_++ < VIEW_COUNT_LIMIT ) {
                        return VALID;
                }
                if ( time(NULL) - viewCountLimitTime_ > (24*60*60) ) {
                        //int viewCountLimitTimeLast = viewCountLimitTime_;
                        viewCountLimitTime_ = (time(NULL)/(24*60*60)) * 24*60*60;
                        viewCountLimtCount_ = 0;
                        return VALID;
                }
                return REACHLIMIT;
        };
        long ownerId_;
        time_t viewCountLimitTime_;
        int viewCountLimtCount_;
};

class HomeFootprintSetSeqHandler : public com::xiaonei::xce::ResultHandler {
public:
  HomeFootprintSetSeqHandler(vector<HomeFootprintSetPtr>& pFootprintSetSeq, size_t num) : footprintSetSeq_(pFootprintSetSeq), num_(num) {}
  virtual bool handle(mysqlpp::Row& row) const {
    HomeFootprintSetPtr t = new HomeFootprintSet(num_);
    t->set(row[COLUMN_VISITOR.c_str()]);
    t->setOwnerId((long)row[COLUMN_OWNER.c_str()]);
    footprintSetSeq_.push_back(t);
    return true;
  }
private:
  vector<HomeFootprintSetPtr>& footprintSetSeq_;
  size_t num_;
};

template<class,class> class SaveThread;


template<class T,class Manager>
class SaveBucket {
public:
	//typedef typename std::vector<T> __vector;
	//typedef typename std::vector<T>::iterator __vector_iterator;
	typedef IceUtil::Handle<SaveThread<T,Manager> > SaveThreadPtr;
	SaveBucket(){};
	SaveBucket(std::string table, size_t size, size_t batchSize,size_t batchTime, size_t begin,size_t limit, Manager* manager):
		table_(table),size_(size){
			for(size_t i=0;i<size;++i){
				SaveThreadPtr sp = new SaveThread<T,Manager>(getTableName(i),begin,limit,batchSize,batchTime,manager);
				save_threads_.push_back(sp);
				save_threads_[i]->start();
			}
		};
	void put(long owner) {
		save_threads_[owner%size_]->put(owner);
	};
	std::string getTableName(long owner){
		if(size_==1)
			return table_;
		ostringstream oss;
		oss<< table_ <<"_"<< owner%size_;
		return oss.str();
	};
private:
        vector<SaveThreadPtr> save_threads_;
	std::string table_;
	size_t size_;
};

/*
template<class T,class Manager>
class AddTask : public MyUtil::Task {
	public:
		AddTask(const FootprintInfoPtr& footprint, SaveBucket<T,Manager>* bucket, Manager* manager):
				owner_(footprint->owner),bucket_(bucket),manager_(manager){		
		};
		virtual void handle(){
		//	if(bucket_->put(owner_)){
		//		MyUtil::TaskManager::instance().execute(new SaveTask<T, Manager>(owner_, bucket_, manager_));
		//	}
		};
private:
        long owner_;
        SaveBucket<T,Manager>* bucket_;
	Manager* manager_;
};
*/

template<class T,class Manager>
class SaveThread : public IceUtil::Thread {
public:
	//typedef typename std::vector<T> __vector;
	//typedef typename std::map<long,__vector>::iterator __map_iterator;

        SaveThread(const string& table,int begin,int limit, size_t batchSize,size_t batchTime,Manager* manager)
			:table_(table),begin_(begin),limit_(limit),batchSize_(batchSize),batchTime_(batchTime),manager_(manager){
	};
	void put(long owner){
                IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
		fptnSet_.insert(owner);
		if(fptnSet_.size() > batchSize_)
			mutex_.notify();
	};
	virtual void run(){
		
		while(true){
			try{
				std::set<long> fptnSet;
				{
					IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
					if (fptnSet_.size() < batchSize_) {
						mutex_.timedWait(IceUtil::Time::seconds(batchTime_));
					}
					fptnSet_.swap(fptnSet);
				}
				if(!fptnSet.empty()){
					com::xiaonei::xce::Statement sql;
					MCE_INFO("SaveTask::handle table:" << table_<<" saveSize:"<<fptnSet.size());
					sql << "INSERT INTO " << table_ << " (owner, visitors) values";
					for (std::set<long>::iterator it = fptnSet.begin(); it != fptnSet.end(); ++it){

						if ( it != fptnSet.begin() ) {
							sql << ", ";
						}

						std::vector<T> ftQueue = manager_->getAll(*it,begin_,limit_)->steps;
						MCE_DEBUG("save steps size:"<<ftQueue.size());
						sql << "(" << (*it) << ", "  << mysqlpp::quote << IntSeqSerialize::serialize(ftQueue) << ")";
					}

					sql << " ON DUPLICATE KEY UPDATE " << "visitors = values(visitors)";
					com::xiaonei::xce::QueryRunner("footprint", com::xiaonei::xce::CDbWServer, table_).schedule(sql);
          //设置HomeFootprintReplica中的值
          int category = HomeFootprintSet::getCategory(table_);
          MyUtil::ObjectResultPtr ptr = ServiceI::instance().getObjectCache()->locateObjects(category, 
              MyUtil::LongSeq(fptnSet.begin(), fptnSet.end()));
          HomeFootprintReplicaAdapter::instance().setFootprint(HomeFootprintSet::changeToHomeVisitHistoryMap(ptr), category);
				}
			}catch(...){
				MCE_WARN("SaveThread has Exception!!!!");
			}
		}

	};
private:
	IceUtil::Monitor<IceUtil::Mutex> mutex_;
        std::string table_;
        std::set<long> fptnSet_;
	size_t begin_;
	size_t limit_;
	size_t batchSize_;
	size_t batchTime_;
	Manager* manager_;
};

class UserVisitCountInfo;

typedef IceUtil::Handle<UserVisitCountInfo> UserVisitCountInfoPtr;

class UserVisitCountInfo : virtual public Ice::Object, virtual public IceUtil::Mutex {
public:
  UserVisitCountInfo(int userId, int count) : _userId(userId), _count(count) {
  }

  UserVisitCountInfo() : _userId(-1), _count(0) {
  }

  UserVisitCountInfo(const UserVisitCountPtr& ptr) : _userId(ptr->userId), _count(ptr->count) {
  }

  static MyUtil::ObjectResultPtr changeToUserVisitCountPtrMap(const vector<UserVisitCountInfoPtr>& ptrs) {
    MyUtil::ObjectResultPtr resultPtr = new MyUtil::ObjectResult;
    for (vector<UserVisitCountInfoPtr>::const_iterator it = ptrs.begin(); it != ptrs.end(); ++it) {
      resultPtr->data.insert(make_pair<long, UserVisitCountPtr>((*it)->getUserId(), (*it)->parseToUserVisitCountPtr()));
    }
    return resultPtr;
  }

  static MyUtil::ObjectResultPtr changeFromUserVisitCountPtrMap(const MyUtil::ObjectResultPtr& ptrs) {
    MyUtil::ObjectResultPtr resultPtr = new MyUtil::ObjectResult;
    if (ptrs == 0) {
      return resultPtr;
    }
    for (map<long, Ice::ObjectPtr>::const_iterator it = ptrs->data.begin(); it != ptrs->data.end(); ++it) {
      resultPtr->data.insert(make_pair<long, UserVisitCountInfoPtr>(it->first, new UserVisitCountInfo(UserVisitCountPtr::dynamicCast(it->second))));
    }
    return resultPtr;
  }

  void incCount(Ice::Int step) {
    IceUtil::Mutex::Lock lock(*this);
    _count += step;
  }

  int getUserId() const {
    IceUtil::Mutex::Lock lock(*this);
    return _userId;
  }

  int getCount() const {
    IceUtil::Mutex::Lock lock(*this);
    return _count;
  }

  void setCount(Ice::Int count) {
    IceUtil::Mutex::Lock lock(*this);
    _count = count;
  }

  UserVisitCountPtr parseToUserVisitCountPtr() {
    return new UserVisitCount(_userId, _count);
  }

private:
  int _userId;
  int _count;
};

class UserVisitCountInfoResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
  UserVisitCountInfoResultHandlerI(vector<UserVisitCountInfoPtr>& results) : _results(results) {}
  virtual bool handle(mysqlpp::Row& row) const {
    int count = (int) row[COLUMN_VIEWCOUNT.c_str()];
    int userId = (int) row[COLUMN_ID.c_str()];
    UserVisitCountInfoPtr ptr = new UserVisitCountInfo(userId, count);
    _results.push_back(ptr);
    return true;
  }
private:
  vector<UserVisitCountInfoPtr>& _results;
};

};
};
#endif


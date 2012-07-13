#ifndef __HOME_FOOTPRINT_CACHE_I_H__
#define __HOME_FOOTPRINT_CACHE_I_H__

#include <FootprintUnite.h>
#include <TaskManager.h>
#include "app/FootprintUnite/share/HomeFootprintBufReaderI.h"
#include "app/FootprintUnite/share/HomeFootprintReaderI.h"
#include "app/FootprintUnite/share/HomeUserVisitCountReaderI.h"

namespace xce
{
namespace footprint
{

const int TASK_LEVEL_LOADBUFSET = 10;

const uint VISITCOUNT_BATCH_SIZE = 1000;
const int VISITCOUNT_BATCH_TIME = 1;


class Evictor;

class OldFootprintFactoryI : public MyUtil::ServantFactory {
public:
        virtual Ice::ObjectPtr create(long id);
};


class OldFootprintSetHandler : public com::xiaonei::xce::ResultHandler{
public:
        OldFootprintSetHandler(HomeFootprintSetPtr& fsp):fsp_(fsp){};
        virtual bool handle(mysqlpp::Row& row) const;
private:
        HomeFootprintSetPtr fsp_;
};

class FootprintFactoryI : public MyUtil::ServantFactory {
public:
        virtual Ice::ObjectPtr create(long id);
};

class FootprintBufFactoryI : public MyUtil::ServantFactory {
public:
        virtual Ice::ObjectPtr create(long id);
};

class UserVisitCountFactoryI : public MyUtil::ServantFactory {
public:
        Ice::ObjectPtr create(long id);
};

class SetVisitCountThread : public IceUtil::Thread {
public:
	SetVisitCountThread() {};
	void push(long owner) {
		MCE_INFO("SetVisitCountThread::push, owner: " << owner);
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
		owners_.insert(owner);
		if (owners_.size() > VISITCOUNT_BATCH_SIZE) {
			mutex_.notify();
		}
	}

	virtual void run();
private:
	IceUtil::Monitor<IceUtil::Mutex> mutex_;
	std::set<long> owners_;
};
typedef IceUtil::Handle<SetVisitCountThread> SetVisitCountThreadPtr;

class HomeFootprintCacheI : virtual public HomeFootprintCache,
	public MyUtil::Singleton<HomeFootprintCacheI>{
public:
    HomeFootprintCacheI();
    virtual ~HomeFootprintCacheI();
    virtual HomeVisitHistoryPtr getHomeFootprintObj(long id, int category, const Ice::Current& crt = Ice::Current());
    virtual UserVisitCountPtr getUserVisitCountObj(int id, const Ice::Current& crt = Ice::Current());

    virtual void visit(const FootprintInfoPtr&,
                       const Ice::Current& crt = Ice::Current());

    virtual void visitBatch(const xce::footprint::FootprintInfoSeq&,
                            const Ice::Current&);

    virtual void remove(const xce::footprint::FootprintInfoPtr&,
                        const Ice::Current&);

    virtual ::Ice::Int getSize(Ice::Long,
                               const Ice::Current&);

    virtual HomeInfoPtr getAll(Ice::Long,Ice::Int,Ice::Int,Ice::Int,
		    const Ice::Current& crt=Ice::Current());

    virtual HomeInfoPtr visitAndGet(const FootprintInfoPtr&,
		Ice::Int, Ice::Int, Ice::Int,const Ice::Current& crt=Ice::Current());

    virtual ::Ice::Int getUserVisitCount(Ice::Long,
		    const Ice::Current& crt=Ice::Current());

    virtual void setUserVisitCount(Ice::Long, Ice::Int,
		    const Ice::Current&);

    virtual void incUserVisitCount(Ice::Long,
		    const Ice::Current& crt=Ice::Current());

    virtual void incUserVisitCountBatch(const ::MyUtil::LongSeq&,
		    Ice::Int,	const Ice::Current&);


    virtual bool isValid(const Ice::Current&);

    virtual void setValid(bool,
                          const Ice::Current& = Ice::Current());

    virtual void contral(const ::MyUtil::Str2StrMap&,
                         const Ice::Current&);
    std::string getTableName(long owner);
    std::string getTableBufName(long owner);

    HomeFootprintSetPtr getFootprintSet(long id);
    HomeFootprintSetPtr getFootprintBufSet(long id);

    HomeFootprintSetPtr findFootprintSet(long id);
    HomeFootprintSetPtr findFootprintBufSet(long id);

//    void evictFootprint(const FootprintInfoPtr& fpt, HomeStepSeq& seq, bool isSec);
//    void fillFootprint(const FootprintInfoPtr& fpt, HomeStepSeq& seq);
 //   void evictBufFootprint(const FootprintInfoPtr& fpt, HomeStepSeq& seq, bool isAdd = false);

    UserVisitCountInfoPtr locateUserVisitCount(long userId);
    UserVisitCountInfoPtr findUserVisitCount(long userId);

    SaveBucket<HomeStepPtr,HomeFootprintCacheI>*  saveBucket_;
    SaveBucket<HomeStepPtr,HomeFootprintCacheI>*  saveBufBucket_;

    Evictor* evictor_;
	FootprintBufFactoryI* buf_factory_;
	SetVisitCountThreadPtr setVisitCountThread_;
  static void setUserVisitCountToReplica(int userId, int count);
  static void setUserVisitCountToReplica(const map<int, int>& userToCount);
private:
    bool isValid_;
    IceUtil::RWRecMutex validMutex_;

    size_t dataSize_;
    size_t tableCount_; 
    size_t saveBatchSize_;
    size_t saveBufBatchSize_;
    size_t saveBatchTime_;
    size_t saveBufBatchTime_;
    std::string tableName_;
    std::string tableBufName_;
};


class UserVisitCountResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
        UserVisitCountResultHandlerI(UserVisitCountPtr& result);
        virtual bool handle(mysqlpp::Row&) const;
private:
        UserVisitCountPtr& _result;
};


class EvictorThread: virtual public IceUtil::Thread {
public:
	EvictorThread(size_t batchSize):batchSize_(batchSize){
	};
	void add(int owner,const HomeStepPtr& step, int state){
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
		to_be_evictor_[owner].push_back(make_pair<HomeStepPtr,int>(step, state));

		if(to_be_evictor_.size() >= batchSize_){
			mutex_.notify();
		}
	};

	virtual void run(){
		while(true){
			try{
				map<int, vector<pair<HomeStepPtr,int> > > to_be_evictor;
				{
					IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
					if(to_be_evictor_.size()<batchSize_)
						mutex_.timedWait(IceUtil::Time::seconds(10));;
					to_be_evictor.swap(to_be_evictor_);
				}
				if(!to_be_evictor.empty()){
					MCE_INFO("EvictorThread run : evictor size "<<to_be_evictor.size());	
					for(map<int,vector<pair<HomeStepPtr,int> > >::iterator it=to_be_evictor.begin();it!=to_be_evictor.end();++it){
						HomeFootprintSetPtr p2 = HomeFootprintCacheI::instance().getFootprintBufSet(it->first);
						if(!p2)
							continue;
						for(vector<pair<HomeStepPtr,int> >::iterator iter=it->second.begin();iter!=it->second.end();++iter){
							switch(iter->second){
								case 1:
									p2->add(iter->first, true);break;
								case 2:
									p2->remove(iter->first);break;
								case 3:
									{
										HomeFootprintSetPtr p1 = HomeFootprintCacheI::instance().getFootprintSet(it->first);
										HomeStepPtr t = p2->getTop();
										if(t && p1->push_back(t)){
											p2->remove(t);
											HomeFootprintCacheI::instance().saveBucket_->put(it->first);
										}

										break;
									}
								default:;
							}

						}
						HomeFootprintCacheI::instance().saveBufBucket_->put(it->first);
					}
				}
			}catch(...){
				MCE_WARN("EvictorThread Exception!!!");
			}
		}
	};
private:
	IceUtil::Monitor<IceUtil::Mutex> mutex_;
	map<int, vector<pair<HomeStepPtr,int> > > to_be_evictor_;
	size_t size_;
	size_t batchSize_;
};
typedef IceUtil::Handle<EvictorThread> EvictorThreadPtr;

class Evictor : public IceUtil::Shared {
public:
        Evictor(int size, int batchSize):size_(size){
		for(int i=0;i<size;++i){
			evictor_threads_.push_back(new EvictorThread(batchSize));
			evictor_threads_[i]->start();
		}
	};
	void add(int owner,const HomeStepPtr& step){
		evictor_threads_[owner%size_]->add(owner,step,1);
	};
	void remove(int owner,const HomeStepPtr& step){
		evictor_threads_[owner%size_]->add(owner,step,2);
	};
	void fill(int owner,const HomeStepPtr& step){
		evictor_threads_[owner%size_]->add(owner,step,3);
	};
private:
	vector<EvictorThreadPtr> evictor_threads_;
	int size_;
};

class LoadBufSetTask : public MyUtil::Task {
public:

	LoadBufSetTask(int userId, FootprintBufFactoryI* factory) :
		MyUtil::Task(TASK_LEVEL_LOADBUFSET), userId_(userId), factory_(factory) {}
	virtual void handle();
private:
	int userId_;
	FootprintBufFactoryI* factory_;
};

class HomeFootprintCacheReaderI : public HomeFootprintReaderI, public Singleton<HomeFootprintCacheReaderI> {
public:
  virtual void setServerStatus() {
    HomeFootprintCacheI::instance().setValid(false);
  }
  virtual string getIdentifier() {
    return "HFR";
  }
};

class HomeFootprintBufCacheReaderI : public HomeFootprintBufReaderI, public Singleton<HomeFootprintBufCacheReaderI> {
public:
  virtual void setServerStatus() {
    HomeFootprintCacheI::instance().setValid(false);
  }
  virtual string getIdentifier() {
    return "HFBR";
  }
};

class HomeUserVisitCountCacheReaderI : public HomeUserVisitCountReaderI, public Singleton<HomeUserVisitCountCacheReaderI> {
public:
  virtual void setServerStatus() {
    HomeFootprintCacheI::instance().setValid(false);
  }
  virtual string getIdentifier() {
    return "HUVCR";
  }
};
}

}

#endif

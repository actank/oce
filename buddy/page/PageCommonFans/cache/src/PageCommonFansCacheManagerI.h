#ifndef _XCE_PAGE_COMMON_FANS_CACHE_MANAGER_H_
#define _XCE_PAGE_COMMON_FANS_CACHE_MANAGER_H_
#include "PageCommonFans.h"
#include <map>
#include <set>
#include <boost/thread/shared_mutex.hpp>
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include "IceExt/src/ObjectCacheI.h"
#include "IceExt/src/ServiceI.h"
#include <ConnectionPoolManager.h>
#include <QueryRunner.h>
#include "UtilCxx/src/Singleton.h"
#include "base/threadpool.h"
#include "base/rwlock.h"
#include "FriendRank.h"
#include "pcfdata.pb.h"

namespace xce {
namespace buddy {

const int kPageCommonFansCacheData = 0;

struct PageCommonFansData : virtual public Ice::Object {
	PageCommonFansData() { }
	virtual ~PageCommonFansData() { }
  std::string data_;
	IceUtil::Mutex mutex_;
};

typedef IceUtil::Handle<PageCommonFansData> PageCommonFansDataPtr;

int CompareFriendRank(const xce::socialgraph::RankData& rd1, const xce::socialgraph::RankData& rd2);

class ComputeMessage : public xce::Message {
public:
  ComputeMessage(int userId, int pageId) : userId_(userId), pageId_(pageId) {
	}
  ComputeMessage(int userId, int pageId, std::string data) : userId_(userId), pageId_(pageId), data_(data) { 
	}
  virtual void Run();
  virtual ~ComputeMessage() { }
  std::string data() {
    return data_;
  }
private:
  int userId_;
  int pageId_;
  std::string data_;
};

class PageFansDAO {
public:
  PageFansDAO() {}
  virtual ~PageFansDAO() {}
  bool FetchPageFans(int pageId, MyUtil::IntSeq& targetIdSeq, MyUtil::IntSeq& resultIdSeq);
};

class FetchFansResultHandler : public com::xiaonei::xce::ResultHandler {
public:
	FetchFansResultHandler(MyUtil::IntSeq& resultIntSeq) : resultIntSeq_(resultIntSeq) {

	}
  virtual bool handle(mysqlpp::Row& row) const;
  virtual ~FetchFansResultHandler() {}
private:
  MyUtil::IntSeq& resultIntSeq_;
};



//  manager
class PageCommonFansCacheManagerI : virtual public PageCommonFansCacheManager, 
                                    virtual public MyUtil::Singleton<PageCommonFansCacheManagerI> {
public:
	virtual ~PageCommonFansCacheManagerI() {}

	virtual MyUtil::IntSeq GetCommon(Ice::Int uid, 
                                   Ice::Int pid, 
                                   Ice::Int begin = 0,  
                                   Ice::Int limit = 0, 
                                   const Ice::Current& current = Ice::Current());

  virtual MyUtil::IntSeq GetUnCommon(Ice::Int uid, 
                                     Ice::Int pid, 
                                     Ice::Int begin = 0, 
                                     Ice::Int limit = 0, 
                                     const Ice::Current& current = Ice::Current());
  virtual bool SetData(const xce::buddy::PageCommonFans&, const Ice::Current& current = Ice::Current());

	virtual CommonFans GetCommonAndUnCommon(const xce::buddy::CommonFansParam&, const Ice::Current& current = Ice::Current());

	virtual void AddPages(const MyUtil::IntSeq& pageIdSeq, const Ice::Current& current = Ice::Current());

	virtual MyUtil::Int2IntSeqMap GetFull(int uid, const Ice::Current& current = Ice::Current());

private:
	friend class MyUtil::Singleton<PageCommonFansCacheManagerI>;
  PageCommonFansCacheManagerI() : threadPool_(8, 40) { 

  }

	bool CheckParam(const xce::buddy::CommonFansParam& param) {
    if (CheckParam(param.cbegin, param.climit) && CheckParam(param.ubegin, param.ulimit)) {
		  return true;
   	}
	  return false;
	}

	bool CheckParam(Ice::Int begin, Ice::Int limit) {
    if (begin < 0)
      return false;
    if (limit < -1 || limit == 0)
      return false;
    return true;
	}

private:
  xce::ThreadPool threadPool_;
	std::set<int> pageSet_;
  xce::ReadWriteLock rwLock_;
};


class ServiceSchemaI: public MyUtil::Descriptor {
public:
  virtual void configure(const Ice::PropertiesPtr& props);
};

class PageCommonFansDataFactory : virtual public MyUtil::ServantFactory {
public:
	virtual Ice::ObjectPtr create(Ice::Int id) {
		PageCommonFansDataPtr object = new PageCommonFansData();
    return object;
	}
};


}
}
#endif



#ifndef __TALK_CACHE_LOADER_I_H__
#define __TALK_CACHE_LOADER_I_H__

#include "ServiceI.h"
#include "TalkCacheLoader.h"
#include "FeedMemcProxy/client/talk_cache_client.h"
namespace com {
namespace xiaonei {
namespace talk {
namespace cache {
namespace loader{
using namespace MyUtil;
//using namespace xce::talk;
	
const string DB_INSTANCE = "biz_doing_info";
const int MAX_LENGTH =  50;	//TODO:

class StatLoader
{
public:
    StatLoader(){
		;
	}
	static bool loadStat(	TalkUserMap& result, const MyUtil::IntSeq& ids);
	static bool loadStat( TalkUserPtr& ui );
private:
	static string getTableName(int id);
	static string getDbInstance();
	static string getTableExpression(int id);

};
	
const int poolsize = 1009;

class CacheLoaderManagerI : public CacheLoaderManager, public Singleton<CacheLoaderManagerI>{
public:
	virtual void updateUser(int userid, const MyUtil::Str2StrMap& props, const Ice::Current& = Ice::Current());
	virtual TalkUserPtr LoadById(int userid, const Ice::Current& = Ice::Current());
	virtual TalkUserMap LoadBySeq(const MyUtil::IntSeq& userids, const Ice::Current& = Ice::Current());
	TalkUserPtr LocateUser(int userid);
	TalkUserMap LocateUser(const MyUtil::IntSeq& userids);
private:
	IceUtil::Mutex _mutex[poolsize];
};



class LoadTask: virtual public MyUtil::Task {
public:
  LoadTask(int uid) : _userid(uid){
  }
  virtual void handle();
private:
  int _userid;
};

}
;
}
;
}
;
}
;
}
;

#endif


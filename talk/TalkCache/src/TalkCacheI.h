#ifndef __TALK_CACHE_I_H__
#define __TALK_CACHE_I_H__

#include "ServiceI.h"
#include "TalkCache.h"

namespace com {
namespace xiaonei {
namespace talk {
namespace cache {
using namespace MyUtil;

	
const string DB_INSTANCE = "biz_doing_info";
const int MAX_LENGTH =  50;	//TODO:
class UserFactoryI : public ServantFactory {
public:
	virtual Ice::ObjectPtr create(Ice::Int id);
	virtual ObjectResultPtr create(const MyUtil::IntSeq& ids);
private:
//	void loadStat(	ObjectResultPtr& result, const MyUtil::IntSeq& ids);
//	void loadStat( UserPtr& ui );
};

class StatLoader
{
public:
    StatLoader(){
		;
	}
	static void loadStat(	ObjectResultPtr& result, const MyUtil::IntSeq& ids);
	static void loadStat( UserPtr& ui );
private:
	static string getTableName(int id);
	static string geDbInstance();
	static string getTableExpression(int id);

};
	
class CacheManagerI : public CacheManager
{
public:
	virtual void updateUser(Ice::Int id, const MyUtil::Str2StrMap& props, const Ice::Current& = Ice::Current());
};


}
;
}
;
}
;
}
;

#endif


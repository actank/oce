#ifndef __SCORE_LOADER_N_I_H__
#define __SCORE_LOADER_N_I_H__

#include <IceUtil/IceUtil.h>
#include <QueryRunner.h>
#include <ServiceI.h>
#include "ReplicaCluster/CacheLoader/src/CacheLoader.h"
#include "../share/ScoreCacheData.h"
#include "../share/ScoreCacheDataFactory.h"

namespace xce {
namespace scorecache {

const int TASK_LEVEL_LOGINDAYS = 1;

const int TASK_LEVEL_SET_SCORE = 2;
const int TASK_LEVEL_UPDATE_SCORE = 3;

const int TASK_LEVEL_SET_AWARDCOUNT = 4;
const int TASK_LEVEL_UPDATE_AWARDCOUNT = 5;

const int TASK_LEVEL_UPDATE_SIGN = 6;
const int TASK_LEVEL_UPDATE_VIP = 7;

const int TASK_LEVEL_RELOAD = 8;
const int TASK_LEVEL_RELOAD_BATCH = 9;
const int TASK_LEVEL_SET_MASK = 10;

const int TASK_LEVEL_BAD_DATA = 11;

const int TASK_LEVEL_UPDATE_DB_LOGIN = 12;
const int TASK_LEVEL_UPDATE_DB_SCORE = 13;
const int TASK_LEVEL_UPDATE_DB_AWARD = 14;
const int TASK_LEVEL_UPDATE_DB_SIGN = 15;

const int TASK_LEVEL_SEND_EVENT = 16;

class ScoreLoaderNI : virtual public ScoreLoaderN,
        virtual public MyUtil::Singleton<ScoreLoaderNI> {
public:
        virtual void setLoginDays( Ice::Int userId, Ice::Int historyLoginDays, Ice::Int continueLoginDays, Ice::Long lastLoginTime, Ice::Int loginType, const Ice::Current& );
        virtual void updateLoginType(Ice::Int userId, Ice::Int loginType,const Ice::Current&);

	virtual xce::scorecache::ScoreDataN setLoginDaysWithRes( Ice::Int userId, Ice::Int historyLoginDays, Ice::Int continueLoginDays, Ice::Long lastLoginTime, Ice::Int loginType, const Ice::Current& );
	virtual xce::scorecache::ScoreDataN updateLoginTypeWithRes(Ice::Int userId, Ice::Int loginType,const Ice::Current&);

        virtual void setScore(Ice::Int userId, Ice::Int score,const Ice::Current& );
        virtual void incScore(Ice::Int userId, Ice::Int inc,const Ice::Current& );
        virtual void decScore(Ice::Int userId, Ice::Int dec,const Ice::Current& );

        virtual void setAwardCount(Ice::Int userId, Ice::Int awardCount,const Ice::Current& );
        virtual void incAwardCount(Ice::Int userId, Ice::Int inc,const Ice::Current& );
        virtual void decAwardCount(Ice::Int userId, Ice::Int dec,const Ice::Current& );

        virtual void setOn(Ice::Int userId, Ice::Int pos,const Ice::Current&);
        virtual void setOff(Ice::Int userId, Ice::Int pos,const Ice::Current&);

        virtual void updateVipState(Ice::Int userId, Ice::Int state,const Ice::Current&);
        virtual void updateVipLevel(Ice::Int userId, Ice::Int level,const Ice::Current&);

	virtual void reload(::Ice::Int, const Ice::Current&);
	virtual void reloadBatch(const MyUtil::IntSeq&, const Ice::Current&);

	virtual void setMask(::Ice::Long, const Ice::Current&);
private:
        friend class MyUtil::Singleton<ScoreLoaderNI>;
};

class SetLoginDaysTask : virtual public MyUtil::Task {
public:
	SetLoginDaysTask( Ice::Int userId, Ice::Int historyLoginDays, Ice::Int continueLoginDays, Ice::Long lastLoginTime, Ice::Int loginType, ::Ice::Int level=TASK_LEVEL_LOGINDAYS ) :
		Task(level),_userId(userId),_historyLoginDays(historyLoginDays),_continueLoginDays(continueLoginDays),_lastLoginTime(lastLoginTime),_loginType(loginType){
	}
protected:
        virtual void handle();
private:
	::Ice::Int _userId;
	::Ice::Int _historyLoginDays;
	::Ice::Int _continueLoginDays;
	::Ice::Long _lastLoginTime;
	::Ice::Int _loginType;
};

class UpdateLoginTypeTask : virtual public MyUtil::Task {
public:
	UpdateLoginTypeTask( Ice::Int userId, Ice::Int loginType, ::Ice::Int level=TASK_LEVEL_LOGINDAYS ) :
		Task(level),_userId(userId),_loginType(loginType){
	}
protected:
        virtual void handle();
private:
	::Ice::Int _userId;
	::Ice::Int _loginType;
};

class SetScoreTask : virtual public MyUtil::Task {
public:
	SetScoreTask( Ice::Int userId, Ice::Int score, ::Ice::Int level=TASK_LEVEL_SET_SCORE ) :
		Task(level),_userId(userId),_score(score){
	}
protected:
        virtual void handle();
private:
	::Ice::Int _userId;
	::Ice::Int _score;
};

class UpdateScoreTask : virtual public MyUtil::Task {
public:
	UpdateScoreTask( Ice::Int userId, Ice::Int inc, ::Ice::Int level=TASK_LEVEL_UPDATE_SCORE ) :
		Task(level),_userId(userId),_inc(inc){
	}
protected:
        virtual void handle();
private:
	::Ice::Int _userId;
	::Ice::Int _inc;
};

class SetAwardCountTask : virtual public MyUtil::Task {
public:
	SetAwardCountTask( Ice::Int userId, Ice::Int awardCount, ::Ice::Int level=TASK_LEVEL_SET_AWARDCOUNT ) :
		Task(level),_userId(userId),_awardCount(awardCount){
	}
protected:
        virtual void handle();
private:
	::Ice::Int _userId;
	::Ice::Int _awardCount;
};

class UpdateAwardCountTask : virtual public MyUtil::Task {
public:
	UpdateAwardCountTask( Ice::Int userId, Ice::Int inc, ::Ice::Int level=TASK_LEVEL_UPDATE_AWARDCOUNT ) :
		Task(level),_userId(userId),_inc(inc){
	}
protected:
        virtual void handle();
private:
	::Ice::Int _userId;
	::Ice::Int _inc;
};

class UpdateSignTask : virtual public MyUtil::Task {
public:
	UpdateSignTask( Ice::Int userId, Ice::Int pos, bool on, ::Ice::Int level=TASK_LEVEL_UPDATE_SIGN ) :
		Task(level),_userId(userId),_pos(pos),_on(on){
	}
protected:
        virtual void handle();
private:
	::Ice::Int _userId;
	::Ice::Int _pos;
	bool _on;
};

class UpdateVipStateTask : virtual public MyUtil::Task {
public:
	UpdateVipStateTask( Ice::Int userId, Ice::Int state, ::Ice::Int level=TASK_LEVEL_UPDATE_VIP ) :
		Task(level),_userId(userId),_state(state){
	}
protected:
        virtual void handle();
private:
	::Ice::Int _userId;
	::Ice::Int _state;
};

class UpdateVipLevelTask : virtual public MyUtil::Task {
public:
	UpdateVipLevelTask( Ice::Int userId, Ice::Int vip_level, ::Ice::Int level=TASK_LEVEL_UPDATE_VIP ) :
		Task(level),_userId(userId),_level(vip_level){
	}
protected:
        virtual void handle();
private:
	::Ice::Int _userId;
	::Ice::Int _level;
};

class SendEventTask: virtual public MyUtil::Task {
public:
	SendEventTask( const std::string& log, const MyUtil::Str2StrMap& event, ::Ice::Int level=TASK_LEVEL_SEND_EVENT ) :
		Task(level),_log(log),_event(event){
	}
protected:
        virtual void handle();
private:
	std::string _log;
	MyUtil::Str2StrMap _event;
};

class ReloadTask : virtual public MyUtil::Task {
public:
	ReloadTask( ::Ice::Int userId, ::Ice::Int level=TASK_LEVEL_RELOAD ) :
		Task(level),_userId(userId){
	}
protected:
        virtual void handle();
private:
	::Ice::Int _userId;
};

class ReloadBatchTask : virtual public MyUtil::Task {
public:
	ReloadBatchTask( const MyUtil::IntSeq& userIds, ::Ice::Int level=TASK_LEVEL_RELOAD_BATCH ) :
		Task(level),_userIds(userIds){
	}
protected:
        virtual void handle();
private:
	MyUtil::IntSeq _userIds;
};

class SetMaskTask : virtual public MyUtil::Task {
public:
	SetMaskTask( ::Ice::Long mask, ::Ice::Int level=TASK_LEVEL_SET_MASK ) :
		Task(level),_mask(mask){
	}
protected:
        virtual void handle();
private:
	::Ice::Int _mask;
};

class FoundBadNewDataTask : virtual public MyUtil::Task {
public:
	FoundBadNewDataTask( Ice::Int userId, const std::string& reson, xce::scorecache::ScoreCacheDataNIPtr old_datan, xce::scorecache::ScoreCacheDataNIPtr new_datan, ::Ice::Int level=TASK_LEVEL_BAD_DATA) :
		Task(level),_userId(userId),_reson(reson),_old_datan(old_datan),_new_datan(new_datan){
	}
protected:
        virtual void handle();
private:
	::Ice::Int _userId;
	std::string _reson;
	xce::scorecache::ScoreCacheDataNIPtr _old_datan;
	xce::scorecache::ScoreCacheDataNIPtr _new_datan;
};

class UpdateDbLoginTask : virtual public MyUtil::Task {
public:
	UpdateDbLoginTask( const std::string& db, const std::string& pattern, const std::string& sql, ::Ice::Int level=TASK_LEVEL_UPDATE_DB_LOGIN ):
		Task(level),_db(db),_pattern(pattern),_sql(sql){
	}
protected:
        virtual void handle();
private:
	std::string _db;
	std::string _pattern;
	std::string _sql;
};
class UpdateDbScoreTask : virtual public MyUtil::Task {
public:
	UpdateDbScoreTask( const std::string& db, const std::string& pattern, const std::string& sql, const std::string& log, ::Ice::Int level=TASK_LEVEL_UPDATE_DB_SCORE ):
		Task(level),_db(db),_pattern(pattern),_sql(sql),_log(log){
	}
protected:
        virtual void handle();
private:
	std::string _db;
	std::string _pattern;
	std::string _sql;
	std::string _log;
};
class UpdateDbAwardTask : virtual public MyUtil::Task {
public:
	UpdateDbAwardTask( const std::string& db, const std::string& pattern, const std::string& sql, ::Ice::Int level=TASK_LEVEL_UPDATE_DB_AWARD ):
		Task(level),_db(db),_pattern(pattern),_sql(sql){
	}
protected:
        virtual void handle();
private:
	std::string _db;
	std::string _pattern;
	std::string _sql;
};
class UpdateDbSignTask : virtual public MyUtil::Task {
public:
	UpdateDbSignTask( const std::string& db, const std::string& pattern, const std::string& sql, ::Ice::Int level=TASK_LEVEL_UPDATE_DB_SIGN ):
		Task(level),_db(db),_pattern(pattern),_sql(sql){
	}
protected:
        virtual void handle();
private:
	std::string _db;
	std::string _pattern;
	std::string _sql;
};

//***************************************************************************

class SetDataHelper: public MyUtil::Singleton<SetDataHelper> {
public:
        void initialize();
        void invokeDelay( Ice::Int, const Ice::ObjectPtr& );
        void invokeNow( Ice::Int, const Ice::ObjectPtr& );
        void swap(std::vector< pair<long,Ice::ObjectPtr> >& empty_vector);
private:
        IceUtil::Monitor<IceUtil::Mutex> mutex_;
        std::vector< pair<long,Ice::ObjectPtr> > datas_;

        class SetDataThread: virtual public IceUtil::Thread {
        public:
                SetDataThread(SetDataHelper& helper) :
                        helper_(helper) {
                        }
                virtual void run();
        private:
                SetDataHelper& helper_;
        };
        typedef IceUtil::Handle<SetDataThread> SetDataThreadPtr;

        SetDataThreadPtr setter_;
};

//***************************************************************************

class ServiceSchemaI: public MyUtil::Descriptor {
public:
        virtual void configure(const Ice::PropertiesPtr& props);
};

}
}

#endif

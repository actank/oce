#include "ScoreLoaderNI.h"
#include "ScoreChecker.h"
#include "UserScoreEventAdapter.h"
#include "ScoreCacheAdapter.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

using namespace std;
using namespace xce::scorecache;
using namespace xce::scorecache::adapter;
using namespace wservice::userscore::adapter;
using namespace MyUtil;
using namespace mysqlpp;
using namespace com::xiaonei::xce;
using namespace xce::clusterstate;
using namespace xce::serverstate;
using namespace xce::cacheloader;

void MyUtil::initialize(){
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&ScoreLoaderNI::instance(), service.createIdentity("L", ""));

	TaskManager::instance().config(TASK_LEVEL_LOGINDAYS, ThreadPoolConfig(1, 1));
	TaskManager::instance().config(TASK_LEVEL_SET_SCORE, ThreadPoolConfig(1, 1));
	TaskManager::instance().config(TASK_LEVEL_UPDATE_SCORE, ThreadPoolConfig(1, 100));
	TaskManager::instance().config(TASK_LEVEL_SET_AWARDCOUNT, ThreadPoolConfig(1, 1));
	TaskManager::instance().config(TASK_LEVEL_UPDATE_AWARDCOUNT, ThreadPoolConfig(1, 10));
	TaskManager::instance().config(TASK_LEVEL_UPDATE_SIGN, ThreadPoolConfig(1, 1));
	TaskManager::instance().config(TASK_LEVEL_UPDATE_VIP, ThreadPoolConfig(1, 1));
	TaskManager::instance().config(TASK_LEVEL_RELOAD, ThreadPoolConfig(1, 10));
	TaskManager::instance().config(TASK_LEVEL_RELOAD_BATCH, ThreadPoolConfig(1, 10));
	TaskManager::instance().config(TASK_LEVEL_SET_MASK, ThreadPoolConfig(1, 1));
	TaskManager::instance().config(TASK_LEVEL_BAD_DATA, ThreadPoolConfig(1, 1));

	TaskManager::instance().config(TASK_LEVEL_UPDATE_DB_LOGIN, ThreadPoolConfig(1, 1));
	TaskManager::instance().config(TASK_LEVEL_UPDATE_DB_SCORE, ThreadPoolConfig(1, 10));
	TaskManager::instance().config(TASK_LEVEL_UPDATE_DB_AWARD, ThreadPoolConfig(1, 1));
	TaskManager::instance().config(TASK_LEVEL_UPDATE_DB_SIGN, ThreadPoolConfig(1, 1));

	TaskManager::instance().config(TASK_LEVEL_SEND_EVENT, ThreadPoolConfig(1, 10));

	service.registerXceObserver(new ServiceSchemaI);

	//SetDataHelper::instance().initialize();
	CheckerHelper::instance().initialize();

	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("ScoreLoaderN.Mod");
	int sInterval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("ScoreLoaderN.Interval", 5);
	// 向Controller注册
	ServerStateSubscriber::instance().initialize("ControllerScoreLoaderN",
			&ScoreLoaderNI::instance(), mod, sInterval);

}

/*****************************************************************************/

void ScoreLoaderNI::setLoginDays( Ice::Int userId, Ice::Int historyLoginDays, Ice::Int continueLoginDays, Ice::Long lastLoginTime, Ice::Int loginType,
		const Ice::Current& ){
	MCE_INFO("[ScoreLoaderNI::setLoginDays] userId=" << userId << " hisday=" <<  historyLoginDays << " conday=" << continueLoginDays << " time=" << lastLoginTime << " type=" << loginType); 
	TaskManager::instance().execute(new SetLoginDaysTask(userId, historyLoginDays, continueLoginDays, lastLoginTime, loginType));
}
void ScoreLoaderNI::updateLoginType(Ice::Int userId, Ice::Int loginType,const Ice::Current&){
	MCE_INFO("[ScoreLoaderNI::updateLoginType] userId=" << userId << " loginType=" << loginType );
	TaskManager::instance().execute(new UpdateLoginTypeTask(userId,loginType));
}

ScoreDataN ScoreLoaderNI::setLoginDaysWithRes( Ice::Int userId, Ice::Int historyLoginDays, Ice::Int continueLoginDays, Ice::Long lastLoginTime, Ice::Int loginType,
		const Ice::Current& ){
	MCE_INFO("[ScoreLoaderNI::setLoginDaysWithRes] userId=" << userId << " hisday=" <<  historyLoginDays << " conday=" << continueLoginDays << " time=" << lastLoginTime << " type=" << loginType); 

	bool succ=true;
	Ice::ObjectSeq objsn = ScoreCacheNAdapter::instance().setLoginDays(userId,historyLoginDays,continueLoginDays,lastLoginTime,loginType,succ);

	ScoreCacheDataNIPtr new_datan;
	if( succ && objsn.size() != 2 ){
		MCE_INFO("[setLoginDaysWithRes::CREATE] userId=" << userId
				<< " historyLoginDays=" << historyLoginDays
				<< " continueLoginDays=" << continueLoginDays
				<< " lastLoginTime=" << lastLoginTime
				<< " loginType=" << loginType );

		ScoreCacheDataNIPtr new_datan = new ScoreCacheDataNI( ScoreCacheDataNPtr::dynamicCast( ScoreCacheDataNFactory::instance().create(userId) ) );
		new_datan->setLogin( historyLoginDays,continueLoginDays,lastLoginTime,loginType);

		ScoreCacheDataPtr obj = new ScoreCacheData;
		obj->historyLoginDays = new_datan->historyLoginDays;
		obj->continueLoginDays = new_datan->continueLoginDays;
		//obj->activeDays = new_datan->activeDays;
		obj->lastLoginTime = new_datan->lastLoginTime;
		obj->loginType = new_datan->loginType;
		obj->score = new_datan->score;
		obj->awardCount = new_datan->awardCount;
		obj->vip = new_datan->vip;
		obj->sign = new_datan->sign;

		ScoreCacheNAdapter::instance().load(userId,new_datan->getScoreCacheDataN());
		return new_datan->getScoreDataN(userId,0);
	}
	if( !succ ){
		if( objsn.size() == 0 ){
			MCE_WARN("[setLoginDaysWithRes::FAILED] userId=" << userId
					<< " historyLoginDays=" << historyLoginDays
					<< " continueLoginDays=" << continueLoginDays
					<< " lastLoginTime=" << lastLoginTime
					<< " loginType=" << loginType );
			return ScoreDataN();
		} else {
			MCE_WARN("[setLoginDaysWithRes::EXP] userId=" << userId
					<< " historyLoginDays=" << historyLoginDays
					<< " continueLoginDays=" << continueLoginDays
					<< " lastLoginTime=" << lastLoginTime
					<< " loginType=" << loginType );
			CheckerHelper::instance().invokeDelay( userId );
		}
	}

	new_datan = new ScoreCacheDataNI( ScoreCacheDataNPtr::dynamicCast( objsn[0] ) );
	ScoreCacheDataNIPtr old_datan = new ScoreCacheDataNI( ScoreCacheDataNPtr::dynamicCast( objsn[1] ) );

	ostringstream pattern;
	pattern << "user_score_" << (userId%100);
	ostringstream sql;
	sql	<< "INSERT INTO score_login_" << userId%100 << " (user_id,history_days,continue_days,last_time,login_type,active_days)"
		<< " VALUES(" << userId << "," << new_datan->historyLoginDays << "," << new_datan->continueLoginDays << ",FROM_UNIXTIME(" << new_datan->lastLoginTime/1000 << ")," << new_datan->loginType << "," << new_datan->activeDays << ")"
		<< " ON DUPLICATE KEY UPDATE history_days=" << new_datan->historyLoginDays << ",continue_days=" << new_datan->continueLoginDays << ",last_time=FROM_UNIXTIME(" << new_datan->lastLoginTime/1000 << "),login_type=" << new_datan->loginType << ",active_days=" << new_datan->activeDays;
	TaskManager::instance().execute(new UpdateDbLoginTask("user_score",pattern.str(),sql.str()));

	ScoreDataN res = new_datan->getScoreDataN(userId,0);

	//TODO:
        MyUtil::Str2StrMap event;
        event["EVENT_TYPE"] = "102";
	event["USER_ID"] = boost::lexical_cast<string>(userId);
	event["USER_SCORE"] = boost::lexical_cast<string>(new_datan->score);
	event["LEVEL"] = boost::lexical_cast<string>(res.level);
	event["CONTINUE_DAY"] = boost::lexical_cast<string>(new_datan->continueLoginDays);
	event["HISTORY_DAY"] = boost::lexical_cast<string>(new_datan->historyLoginDays);
	event["LAST_LOGIN_TIME"] = boost::lexical_cast<string>(new_datan->lastLoginTime);
	event["LOGIN_TYPE"] = boost::lexical_cast<string>(new_datan->loginType);
	event["CONTINUE_DAY_OLD"] = boost::lexical_cast<string>(old_datan->continueLoginDays);
	event["HISTORY_DAY_OLD"] = boost::lexical_cast<string>(old_datan->historyLoginDays);
	event["LAST_LOGIN_TIME_OLD"] = boost::lexical_cast<string>(old_datan->lastLoginTime);
	event["LOGIN_TYPE_OLD"] = boost::lexical_cast<string>(old_datan->loginType);
	event["SIGN"] = boost::lexical_cast<string>(new_datan->sign);
	event["AWARD_COUNT"] = boost::lexical_cast<string>(new_datan->awardCount);
	ostringstream log;
	log << "[setLoginDaysWithRes::send(event)] userId=" << userId
				<< " hisday(" << old_datan->historyLoginDays << "->" << new_datan->historyLoginDays << ")"
				<< " conday(" << old_datan->continueLoginDays << "->" << new_datan->continueLoginDays << ")"
				<< " time(" << old_datan->lastLoginTime << "->" << new_datan->lastLoginTime << ")"
				<< " type(" << old_datan->loginType << "->" << new_datan->loginType << ")";
	TaskManager::instance().execute(new SendEventTask(log.str(),event));
	return res;
}

ScoreDataN ScoreLoaderNI::updateLoginTypeWithRes(Ice::Int userId, Ice::Int loginType,const Ice::Current&){
	MCE_INFO("[ScoreLoaderNI::updateLoginTypeWithRes] userId=" << userId << " loginType=" << loginType );

///
	bool succ=true;
	Ice::ObjectSeq objsn = ScoreCacheNAdapter::instance().updateLoginType(userId,loginType,succ);

	if( succ && objsn.size() != 2 ){
		MCE_INFO("[updateLoginTypeWithRes::CREATE] userId=" << userId
				<< " loginType=" << loginType );
		TaskManager::instance().execute(new ReloadTask(userId));

                ScoreDataN res;
                res.id = -1;
                res.historyLoginDays = -1;
                res.continueLoginDays = -1;
                res.activeDays = -1;
                res.lastLoginTime = -1;
                res.loginType = -1;
                res.score = -1;
                res.level = -1;
                res.nextLevelScore = -1;
                res.awardCount = -1;
                res.vip = -1;
                res.sign = -1;
                return res;
	}
	if( !succ ){
		if( objsn.size() == 0 ){
			MCE_WARN("[updateLoginTypeWithRes::FAILED] userId=" << userId
					<< " loginType=" << loginType );
			return ScoreDataN();
		} else {
			MCE_WARN("[updateLoginTypeWithRes::EXP] userId=" << userId
					<< " loginType=" << loginType );
			CheckerHelper::instance().invokeDelay( userId );
		}
	}

	ScoreCacheDataNIPtr new_datan = new ScoreCacheDataNI( ScoreCacheDataNPtr::dynamicCast( objsn[0] ) );
	ScoreCacheDataNIPtr old_datan = new ScoreCacheDataNI( ScoreCacheDataNPtr::dynamicCast( objsn[1] ) );

	ostringstream pattern;
	pattern << "user_score_" << (userId%100);
	ostringstream sql;
	sql	<< "INSERT INTO score_login_" << userId%100 << " (user_id,history_days,continue_days,last_time,login_type,active_days)"
		<< " VALUES(" << userId << "," << new_datan->historyLoginDays << "," << new_datan->continueLoginDays << ",FROM_UNIXTIME(" << new_datan->lastLoginTime/1000 << ")," << new_datan->loginType << "," << new_datan->activeDays << ")"
		<< " ON DUPLICATE KEY UPDATE login_type=" << new_datan->loginType;
	TaskManager::instance().execute(new UpdateDbLoginTask("user_score",pattern.str(),sql.str()));

	//TODO:

	return new_datan->getScoreDataN(userId,0);
}

void ScoreLoaderNI::setScore(Ice::Int userId, Ice::Int score,const Ice::Current& ){
	MCE_INFO("[ScoreLoaderNI::setScore] userId=" << userId << " score=" << score );
	TaskManager::instance().execute(new SetScoreTask(userId,score));
}
void ScoreLoaderNI::incScore(Ice::Int userId, Ice::Int inc,const Ice::Current& ){
	TaskManager::instance().execute(new UpdateScoreTask(userId,inc));
}
void ScoreLoaderNI::decScore(Ice::Int userId, Ice::Int dec,const Ice::Current& ){
	MCE_INFO("[ScoreLoaderNI::decScore] userId=" << userId << " dec=" << dec );
	TaskManager::instance().execute(new UpdateScoreTask(userId,-dec));
}

void ScoreLoaderNI::setAwardCount(Ice::Int userId, Ice::Int awardCount,const Ice::Current& ){
	MCE_INFO("[ScoreLoaderNI::setAwardCount] userId=" << userId << " awardCount=" << awardCount );
	TaskManager::instance().execute(new SetAwardCountTask(userId,awardCount));
}
void ScoreLoaderNI::incAwardCount(Ice::Int userId, Ice::Int inc,const Ice::Current& ){
	MCE_INFO("[ScoreLoaderNI::incAwardCount] userId=" << userId << " inc=" << inc );
	TaskManager::instance().execute(new UpdateAwardCountTask(userId,inc));
}
void ScoreLoaderNI::decAwardCount(Ice::Int userId, Ice::Int dec,const Ice::Current& ){
	MCE_INFO("[ScoreLoaderNI::decAwardCount] userId=" << userId << " dec=" << dec );
	TaskManager::instance().execute(new UpdateAwardCountTask(userId,-dec));
}

void ScoreLoaderNI::setOn(Ice::Int userId, Ice::Int pos,const Ice::Current&){
	MCE_INFO("[ScoreLoaderNI::setOn] userId=" << userId << " pos=" << pos );
	TaskManager::instance().execute(new UpdateSignTask(userId,pos,true));
}
void ScoreLoaderNI::setOff(Ice::Int userId, Ice::Int pos,const Ice::Current&){
	MCE_INFO("[ScoreLoaderNI::setOff] userId=" << userId << " pos=" << pos );
	TaskManager::instance().execute(new UpdateSignTask(userId,pos,false));
}

void ScoreLoaderNI::updateVipState(Ice::Int userId, Ice::Int state, const Ice::Current&){
	MCE_INFO("[ScoreLoaderNI::updateVipState] userId=" << userId << " state=" << state );
	TaskManager::instance().execute(new UpdateVipStateTask(userId,state));
}

void ScoreLoaderNI::updateVipLevel(Ice::Int userId, Ice::Int level, const Ice::Current&){
	MCE_INFO("[ScoreLoaderNI::updateVipLevel] userId=" << userId << " level=" << level );
	TaskManager::instance().execute(new UpdateVipLevelTask(userId,level));
}

void ScoreLoaderNI::reload(::Ice::Int userId,
		const Ice::Current&){
	MCE_INFO("[ScoreLoaderNI::reload] userId=" << userId );
	TaskManager::instance().execute(new ReloadTask(userId));
}

void ScoreLoaderNI::reloadBatch(const MyUtil::IntSeq& userIds,
		const Ice::Current&){
	MCE_INFO("[ScoreLoaderNI::reloadBatch] userId.size()=" << userIds.size() );
	TaskManager::instance().execute(new ReloadBatchTask(userIds));
}

void ScoreLoaderNI::setMask(::Ice::Long mask,
		const Ice::Current&){
	MCE_INFO("[ScoreLoaderNI::setMask] mask=" << mask );
	TaskManager::instance().execute(new SetMaskTask(mask));
}

/*****************************************************************************/

void SetLoginDaysTask::handle(){
	bool succ=true;
	Ice::ObjectSeq objsn = ScoreCacheNAdapter::instance().setLoginDays(_userId,_historyLoginDays,_continueLoginDays,_lastLoginTime,_loginType,succ);

	ScoreCacheDataNIPtr new_datan;
	if( succ && objsn.size() != 2 ){
		MCE_INFO("[SetLoginDaysTask::CREATE] userId=" << _userId
				<< " historyLoginDays=" << _historyLoginDays
				<< " continueLoginDays=" << _continueLoginDays
				<< " lastLoginTime=" << _lastLoginTime
				<< " loginType=" << _loginType );

		ScoreCacheDataNIPtr new_datan = new ScoreCacheDataNI( ScoreCacheDataNPtr::dynamicCast( ScoreCacheDataNFactory::instance().create(_userId) ) );
		new_datan->setLogin( _historyLoginDays,_continueLoginDays,_lastLoginTime,_loginType);

		ScoreCacheDataPtr obj = new ScoreCacheData;
		obj->historyLoginDays = new_datan->historyLoginDays;
		obj->continueLoginDays = new_datan->continueLoginDays;
		//obj->activeDays = new_datan->activeDays;
		obj->lastLoginTime = new_datan->lastLoginTime;
		obj->loginType = new_datan->loginType;
		obj->score = new_datan->score;
		obj->awardCount = new_datan->awardCount;
		obj->vip = new_datan->vip;
		obj->sign = new_datan->sign;

		ScoreCacheNAdapter::instance().load(_userId,new_datan->getScoreCacheDataN());
		return;
	}
	if( !succ ){
		if( objsn.size() == 0 ){
			MCE_WARN("[SetLoginDaysTask::FAILED] userId=" << _userId
					<< " historyLoginDays=" << _historyLoginDays
					<< " continueLoginDays=" << _continueLoginDays
					<< " lastLoginTime=" << _lastLoginTime
					<< " loginType=" << _loginType );
			return;
		} else {
			MCE_WARN("[SetLoginDaysTask::EXP] userId=" << _userId
					<< " historyLoginDays=" << _historyLoginDays
					<< " continueLoginDays=" << _continueLoginDays
					<< " lastLoginTime=" << _lastLoginTime
					<< " loginType=" << _loginType );
			CheckerHelper::instance().invokeDelay( _userId );
		}
	}

	new_datan = new ScoreCacheDataNI( ScoreCacheDataNPtr::dynamicCast( objsn[0] ) );
	ScoreCacheDataNIPtr old_datan = new ScoreCacheDataNI( ScoreCacheDataNPtr::dynamicCast( objsn[1] ) );

	ostringstream pattern;
	pattern << "user_score_" << (_userId%100);
	ostringstream sql;
	sql	<< "INSERT INTO score_login_" << _userId%100 << " (user_id,history_days,continue_days,last_time,login_type,active_days)"
		<< " VALUES(" << _userId << "," << new_datan->historyLoginDays << "," << new_datan->continueLoginDays << ",FROM_UNIXTIME(" << new_datan->lastLoginTime/1000 << ")," << new_datan->loginType << "," << new_datan->activeDays << ")"
		<< " ON DUPLICATE KEY UPDATE history_days=" << new_datan->historyLoginDays << ",continue_days=" << new_datan->continueLoginDays << ",last_time=FROM_UNIXTIME(" << new_datan->lastLoginTime/1000 << "),login_type=" << new_datan->loginType << ",active_days=" << new_datan->activeDays;
	TaskManager::instance().execute(new UpdateDbLoginTask("user_score",pattern.str(),sql.str()));

	//TODO:
        MyUtil::Str2StrMap event;
        event["EVENT_TYPE"] = "102";
	event["USER_ID"] = boost::lexical_cast<string>(_userId);
	event["USER_SCORE"] = boost::lexical_cast<string>(new_datan->score);
	event["LEVEL"] = boost::lexical_cast<string>(new_datan->getScoreDataN(_userId,0).level);
	event["CONTINUE_DAY"] = boost::lexical_cast<string>(new_datan->continueLoginDays);
	event["HISTORY_DAY"] = boost::lexical_cast<string>(new_datan->historyLoginDays);
	event["LAST_LOGIN_TIME"] = boost::lexical_cast<string>(new_datan->lastLoginTime);
	event["LOGIN_TYPE"] = boost::lexical_cast<string>(new_datan->loginType);
	event["CONTINUE_DAY_OLD"] = boost::lexical_cast<string>(old_datan->continueLoginDays);
	event["HISTORY_DAY_OLD"] = boost::lexical_cast<string>(old_datan->historyLoginDays);
	event["LAST_LOGIN_TIME_OLD"] = boost::lexical_cast<string>(old_datan->lastLoginTime);
	event["LOGIN_TYPE_OLD"] = boost::lexical_cast<string>(old_datan->loginType);
	event["SIGN"] = boost::lexical_cast<string>(new_datan->sign);
	event["AWARD_COUNT"] = boost::lexical_cast<string>(new_datan->awardCount);
	ostringstream log;
	log << "[SetLoginDaysTask::send(event)] userId=" << _userId
				<< " hisday(" << old_datan->historyLoginDays << "->" << new_datan->historyLoginDays << ")"
				<< " conday(" << old_datan->continueLoginDays << "->" << new_datan->continueLoginDays << ")"
				<< " time(" << old_datan->lastLoginTime << "->" << new_datan->lastLoginTime << ")"
				<< " type(" << old_datan->loginType << "->" << new_datan->loginType << ")";
	TaskManager::instance().execute(new SendEventTask(log.str(),event));
}

void UpdateLoginTypeTask::handle(){
	bool succ=true;
	Ice::ObjectSeq objsn = ScoreCacheNAdapter::instance().updateLoginType(_userId,_loginType,succ);

	if( succ && objsn.size() != 2 ){
		MCE_INFO("[UpdateLoginTypeTask::RELOAD] userId=" << _userId
				<< " loginType=" << _loginType );
		TaskManager::instance().execute(new ReloadTask(_userId));
		return;
	}
	if( !succ ){
		if( objsn.size() == 0 ){
			MCE_WARN("[UpdateLoginTypeTask::FAILED] userId=" << _userId
					<< " loginType=" << _loginType );
			return;
		} else {
			MCE_WARN("[UpdateLoginTypeTask::EXP] userId=" << _userId
					<< " loginType=" << _loginType );
			CheckerHelper::instance().invokeDelay( _userId );
		}
	}

	ScoreCacheDataNIPtr new_datan = new ScoreCacheDataNI( ScoreCacheDataNPtr::dynamicCast( objsn[0] ) );
	ScoreCacheDataNIPtr old_datan = new ScoreCacheDataNI( ScoreCacheDataNPtr::dynamicCast( objsn[1] ) );

	ostringstream pattern;
	pattern << "user_score_" << (_userId%100);
	ostringstream sql;
	sql	<< "INSERT INTO score_login_" << _userId%100 << " (user_id,history_days,continue_days,last_time,login_type,active_days)"
		<< " VALUES(" << _userId << "," << new_datan->historyLoginDays << "," << new_datan->continueLoginDays << ",FROM_UNIXTIME(" << new_datan->lastLoginTime/1000 << ")," << new_datan->loginType << "," << new_datan->activeDays << ")"
		<< " ON DUPLICATE KEY UPDATE login_type=" << new_datan->loginType;
	TaskManager::instance().execute(new UpdateDbLoginTask("user_score",pattern.str(),sql.str()));

	//TODO:

}
void SetScoreTask::handle(){
	bool succ=true;
	Ice::ObjectSeq objsn = ScoreCacheNAdapter::instance().setScore(_userId,_score,succ);

	if( succ && objsn.size() != 2 ){
		MCE_INFO("[SetScoreTask::RELOAD] userId=" << _userId
				<< " score=" << _score );
		TaskManager::instance().execute(new ReloadTask(_userId));
		return;
	}
	if( !succ ){
		if( objsn.size() == 0 ){
			MCE_WARN("[SetScoreTask::FAILED] userId=" << _userId
					<< " score=" << _score );
			return;
		} else {
			MCE_WARN("[SetScoreTask::EXP] userId=" << _userId
					<< " score=" << _score );
			CheckerHelper::instance().invokeDelay( _userId );
		}
	}

	ScoreCacheDataNIPtr new_datan = new ScoreCacheDataNI( ScoreCacheDataNPtr::dynamicCast( objsn[0] ) );
	ScoreCacheDataNIPtr old_datan = new ScoreCacheDataNI( ScoreCacheDataNPtr::dynamicCast( objsn[1] ) );

	ostringstream pattern;
	pattern << "user_score_" << (_userId%100);
	ostringstream sql;
	sql	<< "INSERT INTO score_" << _userId%100 << " (user_id,score)"
		<< " VALUES(" << _userId << "," << new_datan->score << ")"
		<< " ON DUPLICATE KEY UPDATE score=" << new_datan->score;

	ostringstream log;
	log << "[SetScoreTask] userId=" << _userId << " score=" << new_datan->score;
	TaskManager::instance().execute(new UpdateDbScoreTask("user_score",pattern.str(),sql.str(),log.str()));

	//TODO:
}
void UpdateScoreTask::handle(){
	Ice::ObjectSeq objsn;
	if( _inc >= 0){
		bool succ=true;
		objsn = ScoreCacheNAdapter::instance().incScore(_userId,_inc,succ);
		if( succ && objsn.size() != 2 ){
			MCE_INFO("[UpdateScoreTask::RELOAD] userId=" << _userId
					<< " inc=" << _inc );
			TaskManager::instance().execute(new ReloadTask(_userId));
			return;
		}
		if( !succ ){
			if( objsn.size() == 0 ){
				MCE_WARN("[UpdateScoreTask::FAILED] userId=" << _userId
						<< " inc=" << _inc );
				return;
			} else {
				MCE_WARN("[UpdateScoreTask::EXP] userId=" << _userId
						<< " inc=" << _inc );
				CheckerHelper::instance().invokeDelay( _userId );
			}
		}
	} else{
		bool succ=true;
		objsn = ScoreCacheNAdapter::instance().decScore(_userId,-_inc,succ);
		if( succ && objsn.size() != 2 ){
			MCE_INFO("[UpdateScoreTask::RELOAD] userId=" << _userId
					<< " dec=" << -_inc );
			TaskManager::instance().execute(new ReloadTask(_userId));
			return;
		}
		if( !succ ){
			if( objsn.size() == 0 ){
				MCE_WARN("[UpdateScoreTask::FAILED] userId=" << _userId
						<< " dec=" << -_inc );
				return;
			} else {
				MCE_WARN("[UpdateScoreTask::EXP] userId=" << _userId
						<< " dec=" << -_inc );
				CheckerHelper::instance().invokeDelay( _userId );
			}
		}
	}

	ScoreCacheDataNIPtr new_datan = new ScoreCacheDataNI( ScoreCacheDataNPtr::dynamicCast( objsn[0] ) );
	ScoreCacheDataNIPtr old_datan = new ScoreCacheDataNI( ScoreCacheDataNPtr::dynamicCast( objsn[1] ) );

	ostringstream log;

	ostringstream pattern;
	pattern << "user_score_" << (_userId%100);
	ostringstream sql;
	sql	<< "INSERT INTO score_" << _userId%100 << " (user_id,score)"
		<< " VALUES(" << _userId << "," << new_datan->score << ")";
	if( new_datan->score == 0 ){
		sql << " ON DUPLICATE KEY UPDATE score=" << new_datan->score;
		log << "[UpdateScoreTask] userId=" << _userId << " score=" << new_datan->score;
	} else if( _inc < 0 ){
		sql << " ON DUPLICATE KEY UPDATE score=score-" << -_inc;
		log << "[UpdateScoreTask] userId=" << _userId << " dec=" << -_inc;
	} else{
		sql << " ON DUPLICATE KEY UPDATE score=score+" << _inc;
		log << "[UpdateScoreTask] userId=" << _userId << " inc=" << _inc;
	}
	TaskManager::instance().execute(new UpdateDbScoreTask("user_score",pattern.str(),sql.str(),log.str()));

	//TODO:
	int new_level = new_datan->getScoreDataN(_userId,0).level;
	int old_level = old_datan->getScoreDataN(_userId,0).level;
	if( new_level > old_level ){
		MyUtil::Str2StrMap event;
		event["EVENT_TYPE"] = "101";
		event["USER_ID"] = boost::lexical_cast<string>(_userId);
		event["USER_SCORE"] = boost::lexical_cast<string>(new_datan->score);
		event["USER_SCORE_OLD"] = boost::lexical_cast<string>(old_datan->score);
		event["LEVEL"] = boost::lexical_cast<string>(new_level);
		event["LEVEL_OLD"] = boost::lexical_cast<string>(old_level);
		event["CONTINUE_DAY"] = boost::lexical_cast<string>(new_datan->continueLoginDays);
		event["HISTORY_DAY"] = boost::lexical_cast<string>(new_datan->historyLoginDays);
		event["LAST_LOGIN_TIME"] = boost::lexical_cast<string>(new_datan->lastLoginTime);
		event["LOGIN_TYPE"] = boost::lexical_cast<string>(new_datan->loginType);
		event["SIGN"] = boost::lexical_cast<string>(new_datan->sign);
		event["AWARD_COUNT"] = boost::lexical_cast<string>(new_datan->awardCount);
		ostringstream log;
		log << "[UpdateScoreTask::send(event)] userId=" << _userId
					<< " score(" << old_datan->score << "->" << new_datan->score << ")";
		TaskManager::instance().execute(new SendEventTask(log.str(),event));
	}

}
void SetAwardCountTask::handle(){
	bool succ=true;
	Ice::ObjectSeq objsn = ScoreCacheNAdapter::instance().setAwardCount(_userId,_awardCount,succ);

	if( succ && objsn.size() != 2 ){
		MCE_INFO("[SetAwardCountTask::RELOAD] userId=" << _userId
				<< " awardCount=" << _awardCount );
		TaskManager::instance().execute(new ReloadTask(_userId));
		return;
	}
	if( !succ ){
		if( objsn.size() == 0 ){
			MCE_WARN("[SetAwardCountTask::FAILED] userId=" << _userId
					<< " awardCount=" << _awardCount );
			return;
		} else {
			MCE_WARN("[SetAwardCountTask::EXP] userId=" << _userId
					<< " awardCount=" << _awardCount );
			CheckerHelper::instance().invokeDelay( _userId );
		}
	}

	ScoreCacheDataNIPtr new_datan = new ScoreCacheDataNI( ScoreCacheDataNPtr::dynamicCast( objsn[0] ) );
	ScoreCacheDataNIPtr old_datan = new ScoreCacheDataNI( ScoreCacheDataNPtr::dynamicCast( objsn[1] ) );

	ostringstream pattern;
	pattern << "user_score_" << (_userId%100);
	ostringstream sql;
	sql	<< "INSERT INTO score_award_count_" << _userId%100 << " (user_id,award_count)"
		<< " VALUES(" << _userId << "," << new_datan->awardCount << ")"
		<< " ON DUPLICATE KEY UPDATE award_count=" << new_datan->awardCount;
	TaskManager::instance().execute(new UpdateDbAwardTask("user_score",pattern.str(),sql.str()));

	//TODO:
}
void UpdateAwardCountTask::handle(){
	Ice::ObjectSeq objsn;
	if( _inc >= 0){
		bool succ=true;
		objsn = ScoreCacheNAdapter::instance().incAwardCount(_userId,_inc,succ);
		if( succ && objsn.size() != 2 ){
			MCE_INFO("[UpdateAwardCountTask::RELOAD] userId=" << _userId
					<< " inc=" << _inc );
			TaskManager::instance().execute(new ReloadTask(_userId));
			return;
		}
		if( !succ ){
			if( objsn.size() == 0 ){
				MCE_WARN("[UpdateAwardCountTask::FAILED] userId=" << _userId
						<< " inc=" << _inc );
				return;
			} else {
				MCE_WARN("[UpdateAwardCountTask::EXP] userId=" << _userId
						<< " inc=" << _inc );
				CheckerHelper::instance().invokeDelay( _userId );
			}
		}
	} else{
		bool succ=true;
		objsn = ScoreCacheNAdapter::instance().decAwardCount(_userId,-_inc,succ);
		if( succ && objsn.size() != 2 ){
			MCE_INFO("[UpdateAwardCountTask::RELOAD] userId=" << _userId
					<< " dec=" << -_inc );
			TaskManager::instance().execute(new ReloadTask(_userId));
			return;
		}
		if( !succ ){
			if( objsn.size() == 0 ){
				MCE_WARN("[UpdateAwardCountTask::FAILED] userId=" << _userId
						<< " dec=" << -_inc );
				return;
			} else {
				MCE_WARN("[UpdateAwardCountTask::EXP] userId=" << _userId
						<< " dec=" << -_inc );
				CheckerHelper::instance().invokeDelay( _userId );
			}
		}
	}

	ScoreCacheDataNIPtr new_datan = new ScoreCacheDataNI( ScoreCacheDataNPtr::dynamicCast( objsn[0] ) );
	ScoreCacheDataNIPtr old_datan = new ScoreCacheDataNI( ScoreCacheDataNPtr::dynamicCast( objsn[1] ) );

	ostringstream pattern;
	pattern << "user_score_" << (_userId%100);
	ostringstream sql;
	sql	<< "INSERT INTO score_award_count_" << _userId%100 << " (user_id,award_count)"
		<< " VALUES(" << _userId << "," << new_datan->awardCount << ")";
	if( new_datan->awardCount == 0 ){
		sql << " ON DUPLICATE KEY UPDATE award_count=" << new_datan->awardCount;
	} else if( _inc < 0 ){
		sql << " ON DUPLICATE KEY UPDATE award_count=award_count-" << -_inc;
	} else{
		sql << " ON DUPLICATE KEY UPDATE award_count=award_count+" << _inc;
	}
	TaskManager::instance().execute(new UpdateDbAwardTask("user_score",pattern.str(),sql.str()));

	//TODO:
}
void UpdateSignTask::handle(){
	Ice::ObjectSeq objsn;
	if( _on ){
		bool succ=true;
		objsn = ScoreCacheNAdapter::instance().setOn(_userId,_pos,succ);
		if( succ && objsn.size() != 2 ){
			MCE_INFO("[UpdateSignTask::RELOAD] userId=" << _userId
					<< " pos=" << _pos );
			TaskManager::instance().execute(new ReloadTask(_userId));
			return;
		}
		if( !succ ){
			if( objsn.size() == 0 ){
				MCE_WARN("[UpdateSignTask::FAILED] userId=" << _userId
						<< " pos=" << _pos );
				return;
			} else {
				MCE_WARN("[UpdateSignTask::EXP] userId=" << _userId
						<< " pos=" << _pos );
				CheckerHelper::instance().invokeDelay( _userId );
			}
		}
	} else{
		bool succ=true;
		objsn = ScoreCacheNAdapter::instance().setOff(_userId,_pos,succ);
		if( succ && objsn.size() != 2 ){
			MCE_INFO("[UpdateSignTask::RELOAD] userId=" << _userId
					<< " pos=" << _pos );
			TaskManager::instance().execute(new ReloadTask(_userId));
			return;
		}
		if( !succ ){
			if( objsn.size() == 0 ){
				MCE_WARN("[UpdateSignTask::FAILED] userId=" << _userId
						<< " pos=" << _pos );
				return;
			} else {
				MCE_WARN("[UpdateSignTask::EXP] userId=" << _userId
						<< " pos=" << _pos );
				CheckerHelper::instance().invokeDelay( _userId );
			}
		}
	}

	ScoreCacheDataNIPtr new_datan = new ScoreCacheDataNI( ScoreCacheDataNPtr::dynamicCast( objsn[0] ) );
	ScoreCacheDataNIPtr old_datan = new ScoreCacheDataNI( ScoreCacheDataNPtr::dynamicCast( objsn[1] ) );

	ostringstream pattern;
	pattern << "user_score_" << (_userId%100);
	ostringstream sql;
	sql	<< "INSERT INTO score_sign_" << _userId%100 << " (user_id,sign)"
		<< " VALUES(" << _userId << "," << new_datan->sign << ")"
		<< " ON DUPLICATE KEY UPDATE sign=" << new_datan->sign;
	TaskManager::instance().execute(new UpdateDbSignTask("user_score",pattern.str(),sql.str()));

	//TODO:
}
void UpdateVipStateTask::handle(){
	Ice::ObjectSeq objsn;
	bool succ=true;
	if( _state==0 || _state==2 ){
		objsn = ScoreCacheNAdapter::instance().updateVipState(_userId,1,succ);
	} else if( _state==1 ){
		objsn = ScoreCacheNAdapter::instance().updateVipState(_userId,2,succ);
	} else{
		objsn = ScoreCacheNAdapter::instance().updateVipState(_userId,_state,succ);
	}
	if( succ && objsn.size() != 2 ){
		MCE_INFO("[UpdateVipStateTask::RELOAD] userId=" << _userId
				<< " state=" << _state );
		TaskManager::instance().execute(new ReloadTask(_userId));
		return;
	}
	if( !succ ){
		if( objsn.size() == 0 ){
			MCE_WARN("[UpdateVipStateTask::FAILED] userId=" << _userId
					<< " state=" << _state );
			return;
		} else {
			MCE_WARN("[UpdateVipStateTask::EXP] userId=" << _userId
					<< " state=" << _state );
			CheckerHelper::instance().invokeDelay( _userId );
		}
	}

	ScoreCacheDataNIPtr new_datan = new ScoreCacheDataNI( ScoreCacheDataNPtr::dynamicCast( objsn[0] ) );
	ScoreCacheDataNIPtr old_datan = new ScoreCacheDataNI( ScoreCacheDataNPtr::dynamicCast( objsn[1] ) );

	if( new_datan->vip != old_datan->vip ){
		MyUtil::Str2StrMap event;
		event["EVENT_TYPE"] = "103";
		event["USER_ID"] = boost::lexical_cast<string>(_userId);
		event["USER_SCORE"] = boost::lexical_cast<string>(new_datan->score);
		event["LEVEL"] = boost::lexical_cast<string>(new_datan->getScoreDataN(_userId,0).level);
		event["SIGN"] = boost::lexical_cast<string>(new_datan->sign);
		event["AWARD_COUNT"] = boost::lexical_cast<string>(new_datan->awardCount);
		event["VIP_LEVEL"] = boost::lexical_cast<string>(abs(new_datan->vip));
		event["VIP_LEVEL_OLD"] = boost::lexical_cast<string>(abs(old_datan->vip));
		event["VIP_STATE"] = boost::lexical_cast<string>(new_datan->vip);
		event["VIP_STATE_OLD"] = boost::lexical_cast<string>(old_datan->vip);
		ostringstream log;
		log << "[UpdateVipStateTask::send(event)] userId=" << _userId
			<< " vip(" << old_datan->vip << "->" << new_datan->vip << ")";
		TaskManager::instance().execute(new SendEventTask(log.str(),event));
	}
}
void UpdateVipLevelTask::handle(){
	bool succ=true;
	Ice::ObjectSeq objsn = ScoreCacheNAdapter::instance().updateVipLevel(_userId,_level,succ);
	if( succ && objsn.size() != 2 ){
		MCE_INFO("[UpdateVipLevelTask::RELOAD] userId=" << _userId
				<< " level=" << _level );
		TaskManager::instance().execute(new ReloadTask(_userId));
		return;
	}
	if( !succ ){
		if( objsn.size() == 0 ){
			MCE_WARN("[UpdateVipLevelTask::FAILED] userId=" << _userId
					<< " level=" << _level );
			return;
		} else {
			MCE_WARN("[UpdateVipLevelTask::EXP] userId=" << _userId
					<< " level=" << _level );
			CheckerHelper::instance().invokeDelay( _userId );
		}
	}

	ScoreCacheDataNIPtr new_datan = new ScoreCacheDataNI( ScoreCacheDataNPtr::dynamicCast( objsn[0] ) );
	ScoreCacheDataNIPtr old_datan = new ScoreCacheDataNI( ScoreCacheDataNPtr::dynamicCast( objsn[1] ) );

	if( new_datan->vip != old_datan->vip ){
		MyUtil::Str2StrMap event;
		event["EVENT_TYPE"] = "103";
		event["USER_ID"] = boost::lexical_cast<string>(_userId);
		event["USER_SCORE"] = boost::lexical_cast<string>(new_datan->score);
		event["LEVEL"] = boost::lexical_cast<string>(new_datan->getScoreDataN(_userId,0).level);
		event["SIGN"] = boost::lexical_cast<string>(new_datan->sign);
		event["AWARD_COUNT"] = boost::lexical_cast<string>(new_datan->awardCount);
		event["VIP_LEVEL"] = boost::lexical_cast<string>(abs(new_datan->vip));
		event["VIP_LEVEL_OLD"] = boost::lexical_cast<string>(abs(old_datan->vip));
		event["VIP_STATE"] = boost::lexical_cast<string>(new_datan->vip);
		event["VIP_STATE_OLD"] = boost::lexical_cast<string>(old_datan->vip);
		ostringstream log;
		log << "[UpdateVipLevelTask::send(event)] userId=" << _userId
			<< " vip(" << old_datan->vip << "->" << new_datan->vip << ")";
		TaskManager::instance().execute(new SendEventTask(log.str(),event));
	}
}

void ReloadTask::handle(){
	ScoreCacheDataNPtr objn=0;
	try {
		objn = ScoreCacheDataNPtr::dynamicCast( ScoreCacheDataNFactory::instance().create(_userId) );
	} catch (Ice::Exception& e) {
		MCE_WARN("[ReloadTask] Ice::Exception (create:" << _userId << ") : " << e.what());
		return;
	} catch (mysqlpp::Exception& e) {
		MCE_WARN("[ReloadTask] mysqlpp::Exception (create:" << _userId << ") : " << e.what());
		return;
	} catch (std::exception& e) {
		MCE_WARN("[ReloadTask] std::excetpion (create:" << _userId << ") : " << ": " << e.what());
		return;
	} catch (...) {
		MCE_WARN("[ReloadTask] unknown exception (create:" << _userId << ") ." );
		return;
	}

	if( objn == 0 ){
		return;
	}

	ScoreCacheDataPtr obj = new ScoreCacheData;
	obj->historyLoginDays = objn->historyLoginDays;
	obj->continueLoginDays = objn->continueLoginDays;
	//obj->activeDays = objn->activeDays;
	obj->lastLoginTime = objn->lastLoginTime;
	obj->loginType = objn->loginType;
	obj->score = objn->score;
	obj->awardCount = objn->awardCount;
	obj->vip = objn->vip;
	obj->sign = objn->sign;

	ScoreCacheNAdapter::instance().load(_userId,objn);
}

void ReloadBatchTask::handle(){
	MyUtil::ObjectResultPtr objns;
	try {
		objns = ScoreCacheDataNFactory::instance().create(_userIds);
	} catch (Ice::Exception& e) {
		MCE_WARN("[ReloadBatchTask] Ice::Exception (create:" << _userIds.size() << " objs) : " << e.what());
		return;
	} catch (mysqlpp::Exception& e) {
		MCE_WARN("[ReloadBatchTask] mysqlpp::Exception (create:" << _userIds.size() << " objs) : " << e.what());
		return;
	} catch (std::exception& e) {
		MCE_WARN("[ReloadBatchTask] std::excetpion (create:" << _userIds.size() << " objs) : " << ": " << e.what());
		return;
	} catch (...) {
		MCE_WARN("[ReloadBatchTask] unknown exception (create:" << _userIds.size() << " objs) ." );
		return;
	}

	for( MyUtil::ObjectMap::iterator it = objns->data.begin(); it != objns->data.end(); ++it ){
		ScoreCacheDataNPtr objn = ScoreCacheDataNPtr::dynamicCast( it->second );
		if( objn == 0 ){
			return;
		}

		ScoreCacheDataPtr obj = new ScoreCacheData;
		obj->historyLoginDays = objn->historyLoginDays;
		obj->continueLoginDays = objn->continueLoginDays;
		//obj->activeDays = objn->activeDays;
		obj->lastLoginTime = objn->lastLoginTime;
		obj->loginType = objn->loginType;
		obj->score = objn->score;
		obj->awardCount = objn->awardCount;
		obj->vip = objn->vip;
		obj->sign = objn->sign;

		ScoreCacheNAdapter::instance().load(it->first,objn);
	}
	MCE_INFO("[ReloadBatchTask] reload:" << _userIds.size() << " objs done." );
}

void UpdateDbLoginTask::handle(){
	Statement sql;
	sql << _sql;
	QueryRunner(_db,CDbWServer, _pattern).execute(sql);
}
void UpdateDbScoreTask::handle(){
	Statement sql;
	sql << _sql;
	QueryRunner(_db,CDbWServer, _pattern).execute(sql,_log);
}
void UpdateDbAwardTask::handle(){
	Statement sql;
	sql << _sql;
	QueryRunner(_db,CDbWServer, _pattern).execute(sql);
}
void UpdateDbSignTask::handle(){
	Statement sql;
	sql << _sql;
	QueryRunner(_db,CDbWServer, _pattern).execute(sql);
}


void SetMaskTask::handle(){
	ScoreCacheNAdapter::instance().setMask(_mask);
}

void FoundBadNewDataTask::handle(){
	MCE_WARN("[FoundBadNewData] " << _reson << " : " << _userId << " (hisdays,condays,time,type) : ("
			<< _old_datan->historyLoginDays << ","
			<< _old_datan->continueLoginDays << ","
			<< _old_datan->lastLoginTime << ","
			<< _old_datan->loginType << ")"
			<< "=>("
			<< _new_datan->historyLoginDays << ","
			<< _new_datan->continueLoginDays << ","
			<< _new_datan->lastLoginTime << ","
			<< _new_datan->loginType << ")"
		);
}

/*****************************************************************************/

void SetDataHelper::initialize(){
	setter_ = new SetDataThread(*this);
	setter_->start(1024*1024).detach();
}

void SetDataHelper::invokeDelay( Ice::Int id, const Ice::ObjectPtr& o ){
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock( mutex_ );
	datas_.push_back( make_pair<long,Ice::ObjectPtr>(id,o) );
	if (datas_.size() >= 1000) mutex_.notify();
}
void SetDataHelper::invokeNow( Ice::Int id, const Ice::ObjectPtr& o ){
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock( mutex_ );
	datas_.push_back( make_pair<long,Ice::ObjectPtr>(id,o) );
	mutex_.notify();
}
void SetDataHelper::swap(vector< pair<long,Ice::ObjectPtr> >& empty_vector){
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
	if (datas_.size() < 1000) mutex_.timedWait(IceUtil::Time::seconds(5));
	datas_.swap(empty_vector);
}

void SetDataHelper::SetDataThread::run() {
	while (true){
		std::vector< pair<long,Ice::ObjectPtr> > to_be_set_;
		helper_.swap( to_be_set_ );
		if( to_be_set_.empty() ){
			MCE_INFO("[SetDataHelper::SetDataThread::run] setData: 0 objs" );
			continue;
		}
		ostringstream idstr;
		MyUtil::ObjectResultPtr objs = new MyUtil::ObjectResult();
		for ( std::vector< pair<long, Ice::ObjectPtr> >::iterator it = to_be_set_.begin();
				it != to_be_set_.end(); ++it ){
			objs->data.insert(*it);
			idstr << " " << it->first;
		}
		MCE_INFO("[SetDataHelper::SetDataThread::run] setData:" << idstr.str() );
		ScoreCacheNAdapter::instance().setData(objs);
	}
}

/*****************************************************************************/

void SendEventTask::handle(){
	try {
		UserScoreEventAdapter::instance().send( _event );
		MCE_INFO(_log);
	} catch( Ice::Exception e ){
		MCE_WARN( _log << " found Ice::Exception : " << e.what() );
	} catch( ... ){
		MCE_WARN( _log << " found unknown exception" );
	}
}

/*****************************************************************************/

void ServiceSchemaI::configure(const Ice::PropertiesPtr& props) {
	TaskManager::instance().config(props, "SetLoginDays", TASK_LEVEL_LOGINDAYS, ThreadPoolConfig(1,1));
	TaskManager::instance().config(props, "SetScore", TASK_LEVEL_SET_SCORE, ThreadPoolConfig(1,1));
	TaskManager::instance().config(props, "UpdateScore", TASK_LEVEL_UPDATE_SCORE, ThreadPoolConfig(1,100));
	TaskManager::instance().config(props, "SetAwardCount", TASK_LEVEL_SET_AWARDCOUNT, ThreadPoolConfig(1,1));
	TaskManager::instance().config(props, "UpdateAwardCount", TASK_LEVEL_UPDATE_AWARDCOUNT, ThreadPoolConfig(1,10));
	TaskManager::instance().config(props, "UpdateSign", TASK_LEVEL_UPDATE_SIGN, ThreadPoolConfig(1,1));
	TaskManager::instance().config(props, "UpdateVip", TASK_LEVEL_UPDATE_VIP, ThreadPoolConfig(1,1));
	TaskManager::instance().config(props, "Reload", TASK_LEVEL_RELOAD, ThreadPoolConfig(1,10));
	TaskManager::instance().config(props, "ReloadBatch", TASK_LEVEL_RELOAD_BATCH, ThreadPoolConfig(1,10));
	TaskManager::instance().config(props, "SetMask", TASK_LEVEL_SET_MASK, ThreadPoolConfig(1,1));
	TaskManager::instance().config(props, "FoundBadNewData", TASK_LEVEL_BAD_DATA, ThreadPoolConfig(1,1));
	TaskManager::instance().config(props, "UpdateDbLogin", TASK_LEVEL_UPDATE_DB_LOGIN, ThreadPoolConfig(1,1));
	TaskManager::instance().config(props, "UpdateDbScore", TASK_LEVEL_UPDATE_DB_SCORE, ThreadPoolConfig(1,10));
	TaskManager::instance().config(props, "UpdateDbAward", TASK_LEVEL_UPDATE_DB_AWARD, ThreadPoolConfig(1,1));
	TaskManager::instance().config(props, "UpdateDbSign", TASK_LEVEL_UPDATE_DB_SIGN, ThreadPoolConfig(1,1));
	TaskManager::instance().config(props, "SendEvent", TASK_LEVEL_SEND_EVENT, ThreadPoolConfig(1,10));
}


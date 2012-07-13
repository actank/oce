#include "UserScoreLogI.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

using namespace std;
using namespace MyUtil;
using namespace xce::userscore;
using namespace com::xiaonei::xce;
using namespace xce::clusterstate;
using namespace xce::serverstate;

void MyUtil::initialize(){
        ServiceI& service = ServiceI::instance();
        service.registerObjectCache(LOGNOTE_DATA, "LN", new UserScoreLogFactory);
        service.getAdapter()->add(&UserScoreLogManagerI::instance(),
                        service.createIdentity("M", ""));


	TaskManager::instance().config(TASK_LEVEL_ADDLOG, ThreadPoolConfig(1, 100));
//	TaskManager::instance().config(TASK_LEVEL_RECORD_SINGLE, ThreadPoolConfig(1, 100));
	TaskManager::instance().config(TASK_LEVEL_RECORD_BATCH, ThreadPoolConfig(1, 100));

        service.registerXceObserver(new ServiceSchemaI);

	RecordHelper::instance().initialize();

        int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("UserScoreLog.Mod");
        int sInterval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserScoreLog.Interval", 5);
        // 向Controller注册
        ServerStateSubscriber::instance().initialize("ControllerUserScoreLog",
                        &UserScoreLogManagerI::instance(), mod, sInterval);

}

/*****************************************************************************/

void LogNote::set(int type,int score){
        IceUtil::Mutex::Lock lock(*this);
	MyUtil::Int2IntMap::iterator it = _logs.find( type );
	if( it != _logs.end() ){
		it->second = score;
	} else{
		_logs[ type ] = score;
	}
}

void LogNote::addLog(int userId,int type,int scoreAdd){
	//bool recordNow = false;
	bool recordDelay = false;
        IceUtil::Mutex::Lock lock(*this);
	if( _logs.empty() ){
		recordDelay = true;
	}
	MyUtil::Int2IntMap::iterator it = _logs.find( type );
	if( it != _logs.end() ){
		it->second += scoreAdd;
	} else{
		_logs[ type ] = scoreAdd;
/*		if( (int)_logs.size() >= LIMIT_TYPE ){
			recordNow = true;
		}
*/
	}
/*	if( recordNow ){
		MCE_INFO( "[LogNote] userId=" << userId << " type.size()=" << _logs.size() << "(LIMIT_TYPE=" << LIMIT_TYPE << ") should record now." );
        	TaskManager::instance().execute( new RecordSingleTask( userId, _logs ) );
		_logs.clear();
	} else */ if( recordDelay ){
		RecordHelper::instance().invokeDelay( userId, this );
	}
}

MyUtil::Int2IntMap LogNote::getLogs(){
        IceUtil::Mutex::Lock lock(*this);
	return _logs;
}

MyUtil::Int2IntMap LogNote::popLogs(){
        IceUtil::Mutex::Lock lock(*this);
	MyUtil::Int2IntMap res = _logs;
	_logs.clear();
	return res;
}

/*****************************************************************************/

Ice::ObjectPtr UserScoreLogFactory::create(Ice::Int id) {
        LogNotePtr obj = new LogNote;
	 Statement sqlw;
	sqlw << "select date, id, log from user_score_log where date="<< mysqlpp::quote<<RecordHelper::instance().getDate()<<"and id="<<id;
	//QueryRunner("user_score_log",CDbRServer).query(sqlw, UserScoreResultHandler(obj));
	return obj;
}
bool UserScoreResultHandler::handle(mysqlpp::Row& res) const {
	string _log=res["log"].data();
	const char *p =_log.c_str();
	if (_log.length()>1) {
	int key;
	char* token=NULL;
	char* temp=NULL;
 	//vector<string> res;
	token=strtok((char*)p, ",");
	temp=strtok(token,":");
	key=atoi(temp);
	temp=strtok(NULL, ":");
	_result->set(key,atoi(temp));
	//res.push_back(token);

	while(token!=NULL) {
	token=strtok(NULL, ",");
	//res.push_back(token);
	temp=strtok(token,":");
        key=atoi(temp);
        temp=strtok(NULL, ":");
	_result->set(key,atoi(temp));
	}
	return true;
	 } else {
	return false;
	}

}
//obj->map[] = sx;

/*****************************************************************************/

void UserScoreLogManagerI::addLog(::Ice::Int userId,::Ice::Int type,::Ice::Int scoreAdd, const Ice::Current& current){
	MCE_INFO( "[UserScoreLogManagerI] addLog (" << userId << "," << type << "," << scoreAdd << ")"
		<< " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
        TaskManager::instance().execute( new AddLogTask( userId,type,scoreAdd ) );
}

/*****************************************************************************/

void AddLogTask::handle(){
        LogNotePtr obj = ServiceI::instance().locateObject<LogNotePtr> (
                        LOGNOTE_DATA, _userId);
	obj->addLog(_userId,_type,_scoreAdd);
}

/*****************************************************************************/

void RecordHelper::initialize(){
        recorder_ = new RecordThread(*this);
        recorder_->start(1024*1024).detach();
	MCE_INFO("[RecordHelper] initializing ..." );
}

string RecordHelper::getDate(){
	return recorder_->getDate();
}
void RecordHelper::invokeDelay( Ice::Int id, const Ice::ObjectPtr& o ){
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock( mutex_ );
	datas_.push_back( make_pair<long,Ice::ObjectPtr>(id,o) );
	mutex_.notify();
}
void RecordHelper::invokeNow( Ice::Int id, const Ice::ObjectPtr& o ){
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock( mutex_ );
	datas_.push_back( make_pair<long,Ice::ObjectPtr>(id,o) );
	mutex_.notify();
}
void RecordHelper::swap(vector< pair<long,Ice::ObjectPtr> >& empty_vector){
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
	//if ( (int)datas_.size() < LIMIT_QUEUE) mutex_.timedWait(IceUtil::Time::seconds(LIMIT_TIME));
	datas_.swap(empty_vector);
}


RecordHelper::RecordThread::RecordThread(RecordHelper& helper) :
                        helper_(helper) {
	MCE_INFO("[TimeThread] initializing ..." );
        tzset();
        time_t time_ = time(NULL);
	struct tm *local;
        local = localtime(&time_);

        local->tm_hour = 0;
        local->tm_min = 0;
        local->tm_sec = 0;

        _today = timelocal(local);

	char year[10];
        char month[10];
        char day[10];
        strftime(year, sizeof(year), "%Y", localtime(&time_));
        strftime(month, sizeof(month), "%m", localtime(&time_));
        strftime(day, sizeof(day), "%d", localtime(&time_));
        string _today_str = string(year) + "-" + string(month) + "-" + string(day); 
}


string RecordHelper::RecordThread::getDate() {
return _today_str;
}
void RecordHelper::RecordThread::run() {
	while (true){

		std::vector< pair<long,Ice::ObjectPtr> > to_be_record_;
		helper_.swap( to_be_record_ );
		if( to_be_record_.empty() ){
			MCE_INFO("[RecordThread] recordBatch: 0 users." );
		}
		std::map<Ice::Int,MyUtil::Int2IntMap> logMap;
		if (time(NULL)-_today>=(24*3600)){
			for ( std::vector< pair<long, Ice::ObjectPtr> >::iterator itObj = to_be_record_.begin();
					itObj != to_be_record_.end(); ++itObj ){
				MyUtil::Int2IntMap logs = LogNotePtr::dynamicCast( itObj->second )->popLogs();
				if( !logs.empty() ){
					logMap.insert( make_pair<Ice::Int,MyUtil::Int2IntMap>(itObj->first,logs) );
				}
			}

			MCE_INFO("[RecordThread] recordBatch: " << logMap.size() << " users." );
			tzset();
                        time_t time_ = time(NULL);

                        char year[10];
                        char month[10];
                        char day[10];
                        strftime(year, sizeof(year), "%Y", localtime(&time_));
                        strftime(month, sizeof(month), "%m", localtime(&time_));
                        strftime(day, sizeof(day), "%d", localtime(&time_));
                        _today_str = string(year) + "-" + string(month) + "-" + string(day);

			_today+=24*3600;

			for ( std::vector< pair<long, Ice::ObjectPtr> >::iterator itObj = to_be_record_.begin();
				itObj != to_be_record_.end(); ++itObj) 
			TaskManager::instance().execute( new RecordBatchTask( logMap, _today_str ) );
		} else {

			for ( std::vector< pair<long, Ice::ObjectPtr> >::iterator itObj = to_be_record_.begin();
					itObj != to_be_record_.end(); ++itObj ){
				MyUtil::Int2IntMap logs = LogNotePtr::dynamicCast( itObj->second )->getLogs();
				if( !logs.empty() ){
					logMap.insert( make_pair<Ice::Int,MyUtil::Int2IntMap>(itObj->first,logs) );
				}
			}
			MCE_INFO("[RecordThread] recordBatch: " << logMap.size() << " users." );

			for ( std::vector< pair<long, Ice::ObjectPtr> >::iterator itObj = to_be_record_.begin();

			itObj != to_be_record_.end(); ++itObj )
			TaskManager::instance().execute( new RecordBatchTask( logMap, _today_str ) );
		}   	
	//	sleep(3600);
		sleep(60);
	}
}

/*****************************************************************************/

/*
void RecordSingleTask::handle(){
        ostringstream scoreStr;
	for( MyUtil::Int2IntMap::iterator itMap = _logs.begin(); itMap != _logs.end(); ++itMap ){
		scoreStr << itMap->first << ":" << itMap->second << ",";
	}


        Statement sqlw;
        sqlw    << "INSERT INTO user_score_log (user_id,log)"
                << " VALUES (" << _userId << "," << mysqlpp::quote << scoreStr.str() << ")";
        QueryRunner("user_score_log", CDbWServer).execute(sqlw);
	
	MCE_INFO("[RecordSingleTask] recording : " << _userId << "(" << scoreStr.str() << ")" );
}
*/

void RecordBatchTask::handle(){
        Statement sqlw;
	std::map<Ice::Int,MyUtil::Int2IntMap>::iterator it = _logMap.begin(); 
	if( it == _logMap.end() ){
		return;
	}
ostringstream log;
	sqlw    << "INSERT INTO user_score_log (date,user_id,log)"
		<< " VALUES (" << _date<<","<<it->first << ",";
log << it->first << "(";
	ostringstream scoreStr;
	for( MyUtil::Int2IntMap::iterator itMap = it->second.begin(); itMap != it->second.end(); ++itMap ){
		scoreStr << itMap->first << ":" << itMap->second << ",";
	}
	sqlw << mysqlpp::quote << scoreStr.str() << ")";
log << scoreStr.str() << ") ";
	for( ++it; it != _logMap.end(); ++it ){
		sqlw << ",(" << it->first << ",";
log << it->first << "(";
        	ostringstream scoreItem;
		for( MyUtil::Int2IntMap::iterator itMap = it->second.begin(); itMap != it->second.end(); ++itMap ){
			scoreItem << itMap->first << ":" << itMap->second << ",";
		}
		sqlw << mysqlpp::quote << scoreItem.str() << ")";
log << scoreItem.str() << ") ";
	}

/*
        QueryRunner("user_score_log", CDbWServer).execute(sqlw);
*/
	MCE_INFO("[RecordBatchTask] recording : " << log.str() );
}

/*****************************************************************************/

void ServiceSchemaI::configure(const Ice::PropertiesPtr& props) {
        TaskManager::instance().config(props, "AddLog", TASK_LEVEL_ADDLOG, ThreadPoolConfig(1,100));
        //TaskManager::instance().config(props, "RecordSingle", TASK_LEVEL_RECORD_SINGLE, ThreadPoolConfig(1,100));
        TaskManager::instance().config(props, "RecordBatch", TASK_LEVEL_RECORD_BATCH, ThreadPoolConfig(1,100));
}


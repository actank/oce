#include "ServiceI.h"
#include "IPRecordI.h"

#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

#include "../../../util/cpp/IntSeqSerialize.h"

using namespace xce::iprecord;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace std;
using namespace xce::serverstate;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.registerObjectCache(IPRECORD_DATA, "IR",
			new IPRecordDataFactory);
	service.getAdapter()->add(&IPRecordManagerI::instance(), service.createIdentity(
			"M", ""));

        int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("IPRecord.Mod");
        string endpoint = service.getCommunicator()->getProperties()->getProperty("IPRecord.Endpoint");
        int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("IPRecord.Interval", 5);

        // 向Controller注册
        ServerStateSubscriber::instance().initialize("ControllerIPRecord",
                        &IPRecordManagerI::instance(), mod, interval);

        TaskManager::instance().config(TASK_LEVEL_UPDATE, ThreadPoolConfig(1,1));
        service.registerXceObserver(new ServiceSchemaI);
}

MyUtil::IntSeq IPRecordData::get(int limit){
	IceUtil::Mutex::Lock lock(*this);
	if( limit < 0){
		return _users;
	}
	MyUtil::IntSeq result;
	for( MyUtil::IntSeq::reverse_iterator it = _users.rbegin(); it != _users.rend(); ++it ) {
		if (--limit < 0 ) {
			break;
		}
		result.push_back(*it);
	}
	return result;
}

MyUtil::IntSeq IPRecordData::getWithMask(int limit,::MyUtil::IntSeq mask){
	IceUtil::Mutex::Lock lock(*this);
	MyUtil::IntSeq result;
	if( limit == 0){
		return result;
	}
	std::sort( mask.begin(), mask.end(), std::less<int>() );
	for( MyUtil::IntSeq::reverse_iterator it = _users.rbegin(); it != _users.rend(); ++it ) {
		MyUtil::IntSeq::iterator itMask = lower_bound(mask.begin(),mask.end(),*it);
		if( itMask!=mask.end() && *itMask==*it){
			continue;
		}
		result.push_back(*it);
		if (--limit==0) {
			break;
		}
	}
	return result;
}

int IPRecordData::size(){
	IceUtil::Mutex::Lock lock(*this);
	return _users.size();
}

void IPRecordData::add(::Ice::Long ip, ::Ice::Int userId){
	IceUtil::Mutex::Lock lock(*this);
	vector<int>::iterator it = find(_users.begin(), _users.end(), userId);
	if(it==_users.end()){
		if((int)_users.size()>=IPRECORD_LIMIT){
			_users.erase(_users.begin());
			_users.push_back(userId);
		}else{
			_users.push_back(userId);
		}
	}else{
		_users.erase(it);
		_users.push_back(userId);
	}
	MCE_INFO("[IPRecordData::add] add " << userId << " to " << ip << ", UpdateToDB!" );
	TaskManager::instance().execute( new UpdateToDBTask( ip, _users ) );
}

void IPRecordData::remove(::Ice::Long ip, ::Ice::Int userId){
	IceUtil::Mutex::Lock lock(*this);
	vector<int>::iterator it = find(_users.begin(), _users.end(), userId);
	if(it!=_users.end()){
		_users.erase(it);
		MCE_INFO("[IPRecordData::remove] remove " << userId << " from " << ip << ", UpdateToDB!" );
		TaskManager::instance().execute( new UpdateToDBTask( ip, _users ) );
	}
}

Ice::ObjectPtr IPRecordDataFactory::create(::Ice::Long ip) {
	MCE_INFO("IPRecordDataFactory::create(" << ip << ")" );
	IPRecordDataPtr ipdata = new IPRecordData();

	Statement sql;
	sql	<< "SELECT users FROM ip_users_record_" << ip%100
			<< " WHERE ip =" << ip;
        QueryRunner("ip_users_record", CDbRServer).query(
                        sql, IPRecordDataResultHandler(ipdata));

        return ipdata;
}

bool IPRecordDataResultHandler::handle(mysqlpp::Row& row) const
{
        const int num = row["users"].length() / sizeof(int);
        const char* p = row["users"].c_str();
        //MCE_INFO("#######################" << num);
        for (int i=0; i<num; ++i)
        {
                int userId = *(int*)p;  p += sizeof(int);
                _result->_users.push_back(userId);
        }
        //MCE_INFO("#######################" << num);
        return true;
}

IntSeq IPRecordManagerI::getUsers(::Ice::Long ip,::Ice::Int limit,
			const Ice::Current& current){
	IPRecordDataPtr obj = locateIPRecordData(ip);
	MCE_INFO( "[IPRecordManagerI::getUsers] ip=" << ip << " size=" << obj->size() << " limit=" << limit
                << " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
	return obj->get(limit);
}

IntSeq IPRecordManagerI::getUsersWithMask(::Ice::Long ip,::Ice::Int limit,const MyUtil::IntSeq& mask,
			const Ice::Current& current){
	IPRecordDataPtr obj = locateIPRecordData(ip);
	IntSeq res = obj->getWithMask(limit,mask);
	MCE_INFO( "[IPRecordManagerI::getUsersWithMask] ip=" << ip << " size=" << obj->size() << " limit=" << limit << " mask.size=" << mask.size() << " res.size()=" << res.size() 
                << " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
	return res;
}

Ice::Int IPRecordManagerI::getCount(::Ice::Long ip,
			const Ice::Current& current){
	Ice::Int size = locateIPRecordData(ip)->size();
	MCE_INFO( "[IPRecordManagerI::getCount] ip=" << ip << " size=" << size
                << " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
	return size;
}

void IPRecordManagerI::addUser(::Ice::Long ip,::Ice::Int userId,
			const Ice::Current& current){
	MCE_INFO( "[IPRecordManagerI::addUser] ip=" << ip << " id=" << userId
                << " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
	locateIPRecordData(ip)->add(ip,userId);
}

void IPRecordManagerI::removeUser(::Ice::Long ip,::Ice::Int userId,
			const Ice::Current& current){
	MCE_INFO( "[IPRecordManagerI::removeUser] ip=" << ip << " id=" << userId
                << " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
	locateIPRecordData(ip)->remove(ip,userId);
}

void IPRecordManagerI::reload(::Ice::Long ip,
			const Ice::Current& current){
	ServiceI::instance().reloadObject(IPRECORD_DATA, ip);
}

void IPRecordManagerI::updateToDB(::Ice::Long ip,
			const Ice::Current& current){
	IPRecordDataPtr obj = ServiceI::instance().findObject<IPRecordDataPtr> (
			IPRECORD_DATA, ip);
	if( !obj ){
		return;
	}
	TaskManager::instance().execute( new UpdateToDBTask( ip, obj->_users ) );
}

IPRecordDataPtr IPRecordManagerI::locateIPRecordData(::Ice::Long ip) {
	return ServiceI::instance().locateObject<IPRecordDataPtr>(IPRECORD_DATA, ip);
}

void UpdateToDBTask::handle()
{
	Statement sqlw;
	sqlw	<< "INSERT INTO ip_users_record_" << _ip%100 << " (ip,users)"
		<< " VALUES (" << _ip << "," << mysqlpp::quote << MyUtil::IntSeqSerialize::serialize(_users) << ")"
                << " ON DUPLICATE KEY UPDATE users=VALUES(users)";

	std::ostringstream log;
	log	<< "INSERT INTO ip_users_record_" << _ip%100 << " (ip,users)"
		<< " VALUES (" << _ip << ",";

	for( MyUtil::IntSeq::iterator it = _users.begin(); it != _users.end(); ++it ) {
		log << *it << "|";
	}

	log	<< ") ON DUPLICATE KEY UPDATE users=VALUES(users)";
	QueryRunner("ip_users_record", CDbWServer).execute(sqlw,log.str());
}

void ServiceSchemaI::configure(const Ice::PropertiesPtr& props) {
        TaskManager::instance().config(props, "UpdateToDB", TASK_LEVEL_UPDATE, ThreadPoolConfig(1,1));
}



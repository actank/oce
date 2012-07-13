#include "ServiceI.h"
#include "FriendFinderByIPI.h"
#include <boost/lexical_cast.hpp>
#include <IPRecordReplicaAdapter.h>
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace xce::friendfinder;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace std;
using namespace xce::serverstate;
using namespace xce::iprecord::adapter;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.registerObjectCache(FRIENDFINDER_BYIP_DATA, "FBI", new FriendFinderByIPDataFactory);
	service.getAdapter()->add(&FriendFinderByIPManagerI::instance(),
		service.createIdentity("M", ""));

        int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("FriendFinderByIP.Mod");
        string endpoint = service.getCommunicator()->getProperties()->getProperty("FriendFinderByIP.Endpoint");
        int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("FriendFinderByIP.Interval", 5);

        // 向Controller注册
        ServerStateSubscriber::instance().initialize("ControllerFriendFinderByIP",
                        &FriendFinderByIPManagerI::instance(), mod, interval);

        TaskManager::instance().config(TASK_LEVEL_ACCESS, ThreadPoolConfig(1,100));
        TaskManager::instance().config(TASK_LEVEL_UPDATE, ThreadPoolConfig(1,1));
        service.registerXceObserver(new ServiceSchemaI);

}

////***************************************************************************

int FriendFinderByIPData::getIPCount(){
	IceUtil::RWRecMutex::RLock lock(_mutex);
	return _freqips.size();
}
IPNoteSeq FriendFinderByIPData::getFreqIP(int limit){
	IPNoteISeq tmp;
	{
		IceUtil::RWRecMutex::RLock lock(_mutex);
		tmp = _freqips;
	}
	IPNoteSeq res;
	for(IPNoteISeq::iterator it = tmp.begin(); it
			!= tmp.end(); ++it) {
		IPNote note;
		note.ip = (long)it->_ip;
		note.onlineCount = it->_online_count;
		note.awayCount = it->_away_count;
		note.lastLoginTime = (int)( time(0) - it->_last_login_time );
		res.push_back(note);
		if( --limit == 0 ){
			break;
		}
	}
	return res;
}
IPNoteSeq FriendFinderByIPData::getRecIP(int limit){
	IPNoteISeq tmp;
	{
		IceUtil::RWRecMutex::RLock lock(_mutex);
		tmp = _freqips;
	}
	std::sort(tmp.begin(),tmp.end(),greater_lastlogintime() );
	IPNoteSeq res;
	for(IPNoteISeq::iterator it = tmp.begin(); it
			!= tmp.end(); ++it) {
		IPNote note;
		note.ip = (long)it->_ip;
		note.onlineCount = it->_online_count;
		note.awayCount = it->_away_count;
		note.lastLoginTime = (int)( time(0) - it->_last_login_time );
		res.push_back(note);
		if( --limit == 0 ){
			break;
		}
	}
	return res;
}
bool FriendFinderByIPData::access(const MyUtil::LongSeq& ips, Ice::Int userId ){
	//MCE_DEBUG("[FriendFinderByIPData::access] userId=" << userId << " ips.size=" << ips.size() );

	std::map<long,bool> ipmap;
	for( MyUtil::LongSeq::const_iterator it = ips.begin(); it != ips.end(); ++it ){
		ipmap[*it] = true;
	}
	time_t time_now = time(0);
	bool overtime = time_now-_lastRecordTime>12*3600;
	bool changed = false;

	IceUtil::RWRecMutex::WLock lock(_mutex);
	for( IPNoteISeq::iterator itNote = _freqips.begin(); itNote != _freqips.end(); ++itNote ){
		std::map<long,bool>::iterator it = ipmap.find( (long)(itNote->_ip) );
		if( it != ipmap.end() ){	//this ip is used again.
			it->second = false;
			if( overtime ){							//overtime, record _online_count.
				itNote->_online_count++;
				MCE_INFO("[FriendFinderByIPData::access] userId=" << userId << " (hit/overtime) add _online_count to " << itNote->_online_count );
				changed = true; 
				itNote->_last_login_time = time_now;
			} else if( itNote->_last_login_time < _lastRecordTime ){	//not overtime, but it was wrong.
				itNote->_away_count = itNote->_away_count>1 ? itNote->_away_count-2 : 0;
				MCE_INFO("[FriendFinderByIPData::access] userId=" << userId << " (hit/not overtime) found _last_login_time error, dec _away_count to " << itNote->_away_count );
				changed = true; 
			}
			MCE_DEBUG("[FriendFinderByIPData::access] userId=" << userId << " (hit/not overtime) do nothing." );
		} else{				//this ip is not used in this call.
			if( overtime ){			//overtime, record _away_count.
				itNote->_away_count+=2;
				MCE_INFO("[FriendFinderByIPData::access] userId=" << userId << " (not hit/overtime) dec _away_count to " << itNote->_away_count );
				changed = true; 
			}
			MCE_DEBUG("[FriendFinderByIPData::access] userId=" << userId << " (not hit/not overtime) do nothing" );
		}
	}

	if( overtime ){
		_lastRecordTime = time_now;
		MCE_INFO("[FriendFinderByIPData::access] userId=" << userId << " set _lastRecordtime to " << _lastRecordTime );
	}

	for( std::map<long,bool>::iterator it = ipmap.begin(); it != ipmap.end(); ++it ){
		if( it->second ){
			_freqips.push_back( IPNoteI(it->first,_lastRecordTime) );
			MCE_INFO("[FriendFinderByIPData::access] userId=" << userId << " new IPNoteI: ip=" << it->first << ", _lastRecordTime=" << _lastRecordTime );
			changed = true; 
		}
	}

	if( changed ){
		unsigned int old_first = _freqips.begin()->_ip;
		std::sort(_freqips.begin(),_freqips.end(),greater_freq());
		MCE_INFO("[FriendFinderByIPData::access] userId=" << userId << " sorting. begin()->_ip=" << _freqips.begin()->_ip << " size=" << _freqips.size() );
		unsigned int new_first = _freqips.begin()->_ip;
		if( new_first != old_first ){
			IPRecordAdapter::instance().removeUser( (long)old_first, userId );
			MCE_INFO("[FriendFinderByIPData::access] IPRecordAdapter::removeUser(" << old_first << "," << userId << ")");
		}
		MCE_INFO("[FriendFinderByIPData::access] IPRecordAdapter::addUser(" << new_first << "," << userId << ")");
		IPRecordAdapter::instance().addUser( (long)new_first, userId );
		if( _freqips.size() > IP_LIMIT ){
			IPNoteISeq(_freqips.begin(),_freqips.begin()+IP_LIMIT).swap( _freqips );
		}
	}

	return changed;
}

std::string FriendFinderByIPData::serialize(std::ostringstream& log){
	std::string result;
	IceUtil::RWRecMutex::RLock lock(_mutex);
	result.resize( sizeof(time_t) + ( sizeof(unsigned int)+sizeof(int)+sizeof(int)+sizeof(time_t) ) * _freqips.size() );
	char* p  = (char*)result.data();
	*(time_t*)p = _lastRecordTime;		p += sizeof(time_t);
	for( IPNoteISeq::iterator itNote = _freqips.begin(); itNote != _freqips.end(); ++itNote ){
		*(unsigned int*)p = itNote->_ip;	p += sizeof(unsigned int);
		*(int*)p = itNote->_online_count;	p += sizeof(int);
		*(int*)p = itNote->_away_count;		p += sizeof(int);
		*(time_t*)p = itNote->_last_login_time;	p += sizeof(time_t);
		log << itNote->_ip << "[" << itNote->_online_count << "/" << itNote->_away_count << "]";
	}
	return result;
}

void FriendFinderByIPData::setLastRecordTime( time_t lastRecordTime ){
	IceUtil::RWRecMutex::WLock lock(_mutex);
	_lastRecordTime = lastRecordTime;
}

void FriendFinderByIPData::addIPNoteI( const IPNoteI& note ){
	IceUtil::RWRecMutex::WLock lock(_mutex);
	_freqips.push_back(note);
}

////***************************************************************************

Ice::ObjectPtr FriendFinderByIPDataFactory::create(::Ice::Int id) {
        MCE_INFO( "FriendFinderByIPDataFactory::create(" << id << ")" );
	FriendFinderByIPDataPtr iplog = new FriendFinderByIPData();
        Statement sql;
        sql     << "SELECT ips FROM user_ips_record_" << id%100
                        << " WHERE id =" << id;
        QueryRunner("user_ips_record", CDbRServer).query(
                        sql, FriendFinderByIPDataResultHandler(iplog));
	return iplog;
}

bool FriendFinderByIPDataResultHandler::handle(mysqlpp::Row& row) const
{
        const int num = ( row["ips"].length() - sizeof(unsigned int) ) / ( sizeof(unsigned int)+sizeof(int)+sizeof(int)+sizeof(time_t) );
	if( num <= 0 ){
		return true;
	}

	const char* p = row["ips"].c_str();

	unsigned int lastRecordTime = *(unsigned int*)p;  p += sizeof(unsigned int);
	_result->setLastRecordTime( lastRecordTime );

	//MCE_INFO("#######################" << num);
	for (int i=0; i<num; ++i)
	{
		unsigned int ip = *(unsigned int*)p;	p += sizeof(unsigned int);
		int online_count = *(int*)p;		p += sizeof(int);
		int away_count = *(int*)p;		p += sizeof(int);
		time_t last_login_time = *(time_t*)p;	p += sizeof(time_t);

		IPNoteI note(ip,last_login_time); 
		note._online_count = online_count;
		note._away_count = away_count;

		_result->addIPNoteI(note);
	}
	//MCE_INFO("#######################" << num);
	return true;
}

////***************************************************************************

IPNoteSeq FriendFinderByIPManagerI::getFreqIP(::Ice::Int userId,::Ice::Int limit,
		const Ice::Current& current){
	FriendFinderByIPDataPtr obj = locateFriendFinderByIPData(userId);
	MCE_INFO("[FriendFinderByIPManagerI::getFreqIP] userId=" << userId << " ips.size=" << obj->getIPCount() << " limit=" << limit
                << " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
	return obj->getFreqIP(limit);
}
IPNoteSeq FriendFinderByIPManagerI::getRecIP(::Ice::Int userId,::Ice::Int limit,
		const Ice::Current& current){
	FriendFinderByIPDataPtr obj = locateFriendFinderByIPData(userId);
	MCE_INFO("[FriendFinderByIPManagerI::getRecIP] userId=" << userId << " ips.size=" << obj->getIPCount() << " limit=" << limit
                << " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
	return obj->getRecIP(limit);
}

void FriendFinderByIPManagerI::accessIP(::Ice::Int userId,::Ice::Long ip,
		const Ice::Current& current){
	MCE_INFO("[FriendFinderByIPManagerI::accessIP] userId=" << userId << " ip=" << ip
                << " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
	if( ip < 0 || ip == 2130706433 ){
		return;
	}
	MyUtil::LongSeq ips_tmp;
	ips_tmp.push_back( ip );
	TaskManager::instance().execute( new AccessTask( userId, ips_tmp ) );
}
void FriendFinderByIPManagerI::accessIPStr(::Ice::Int userId,const ::std::string& ipStr,
		const Ice::Current& current){
	//long ip = (long)inet_addr(ipStr.c_str());
        int ipSlice[4];
        sscanf(ipStr.c_str(),"%d.%d.%d.%d",&ipSlice[0],&ipSlice[1],&ipSlice[2],&ipSlice[3]);
        long ip = ((long)((((((ipSlice[0]<<8)+ipSlice[1])<<8)+ipSlice[2])<<8)+ipSlice[3])) & 0x00000000ffffffff;
	MCE_INFO("[FriendFinderByIPManagerI::accessIPStr] userId=" << userId << " ip=" << ipStr << "(" << ip << ")"
                << " " << current.con->toString().substr(current.con->toString().find("remote", 25)));

	if( ip < 0 || ip == 2130706433 ){
		return;
	}
	MyUtil::LongSeq ips_tmp;
	ips_tmp.push_back( ip );
	TaskManager::instance().execute( new AccessTask( userId, ips_tmp ) );
}

void FriendFinderByIPManagerI::accessIPs(::Ice::Int userId,const ::MyUtil::LongSeq& ips,
		const Ice::Current& current){
	MCE_INFO("[FriendFinderByIPManagerI::accessIPs] userId=" << userId << " ips.size=" << ips.size()
                << " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
	MyUtil::LongSeq ips_tmp;
	for( ::MyUtil::LongSeq::const_iterator it=ips.begin(); it!=ips.end(); ++it ){
		if( *it < 0 || *it == 2130706433 ){
			continue;
		}
		ips_tmp.push_back( *it );
	}
	TaskManager::instance().execute( new AccessTask( userId, ips_tmp ) );
}
void FriendFinderByIPManagerI::accessIPsStr(::Ice::Int userId,const ::MyUtil::StrSeq& ips,
		const Ice::Current& current){
	MCE_INFO("[FriendFinderByIPManagerI::accessIPsStr] userId=" << userId << " ips.size=" << ips.size()
                << " " << current.con->toString().substr(current.con->toString().find("remote", 25)));
	MyUtil::LongSeq ips_tmp;
	for( ::MyUtil::StrSeq::const_iterator it=ips.begin(); it!=ips.end(); ++it ){
		//long ip = (long) inet_addr((*it).c_str());
		int ipSlice[4];
		sscanf((*it).c_str(),"%d.%d.%d.%d",&ipSlice[0],&ipSlice[1],&ipSlice[2],&ipSlice[3]);
		long ip = ((long)((((((ipSlice[0]<<8)+ipSlice[1])<<8)+ipSlice[2])<<8)+ipSlice[3])) & 0x00000000ffffffff;

		if( ip < 0 || ip == 2130706433 ){
			continue;
		}
		ips_tmp.push_back( ip );
	}
	TaskManager::instance().execute( new AccessTask( userId, ips_tmp ) );
}

FriendFinderByIPDataPtr FriendFinderByIPManagerI::locateFriendFinderByIPData(::Ice::Int id) {
	return ServiceI::instance().locateObject<FriendFinderByIPDataPtr> (FRIENDFINDER_BYIP_DATA, id);
}

void FriendFinderByIPManagerI::reload(::Ice::Int userId,
		const Ice::Current& current){
        ServiceI::instance().reloadObject(FRIENDFINDER_BYIP_DATA, userId);
}
////***************************************************************************

void AccessTask::handle() {
	FriendFinderByIPDataPtr obj = ServiceI::instance().locateObject<FriendFinderByIPDataPtr>(FRIENDFINDER_BYIP_DATA, _userId);
	if( obj->access(_ips,_userId) ){
		TaskManager::instance().execute( new UpdateToDBTask( _userId, obj ) );
	}
}

void UpdateToDBTask::handle() {
        ostringstream log;
	log << "INSERT INTO user_ips_record_" << _userId%100 << " (id,ips)"
                << " VALUES (" << _userId << ",";

        Statement sqlw;
        sqlw    << "INSERT INTO user_ips_record_" << _userId%100 << " (id,ips)"
                << " VALUES (" << _userId << "," << _obj->serialize(log) << ")"
                << " ON DUPLICATE KEY UPDATE ips=VALUES(ips)";

	log << ") ON DUPLICATE KEY UPDATE ips=VALUES(ips)";

        QueryRunner("user_ips_record", CDbWServer).execute(sqlw,log.str());
}

////***************************************************************************

void ServiceSchemaI::configure(const Ice::PropertiesPtr& props) {
        TaskManager::instance().config(props, "Access", TASK_LEVEL_ACCESS, ThreadPoolConfig(1,100));
        TaskManager::instance().config(props, "UpdateToDB", TASK_LEVEL_UPDATE, ThreadPoolConfig(1,1));
}



#include "friendfindernewcomerworker.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "newcomercreator.h"
#include "facadestatefilter.h"

using namespace std;
using namespace MyUtil;
using namespace com::xiaonei::xce;
//using namespace xce::socialgraph;
using namespace xce::socialgraph::friendfindernewcomer;
using namespace xce::serverstate;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();

	service.getAdapter()->add(&FriendFinderNewComerWorkerI::instance(), service.createIdentity(
				"W", ""));

	TaskManager::instance().config( TASK_LEVEL_REPORT, ThreadPoolConfig(1, 100) );
	TaskManager::instance().config( TASK_LEVEL_REBUILD, ThreadPoolConfig(1, 10) );
	TaskManager::instance().config( TASK_LEVEL_PRELOAD, ThreadPoolConfig(1, 1) );

	TaskManager::instance().config( TASK_LEVEL_APPLY, ThreadPoolConfig(1, 1) );
	TaskManager::instance().config( TASK_LEVEL_ACCEPT, ThreadPoolConfig(1, 1) );
	TaskManager::instance().config( TASK_LEVEL_DENY, ThreadPoolConfig(1, 1) );
	TaskManager::instance().config( TASK_LEVEL_IGNORE, ThreadPoolConfig(1, 1) );

	TaskManager::instance().config( TASK_LEVEL_ADD, ThreadPoolConfig(1, 10) );
	TaskManager::instance().config( TASK_LEVEL_SHOOT, ThreadPoolConfig(1, 1) );
	TaskManager::instance().config( TASK_LEVEL_UPDATE_INFO, ThreadPoolConfig(1, 1) );
        service.registerXceObserver(new ServiceSchemaI);

	FacadeStateFilterInitThreadPtr init = new FacadeStateFilterInitThread;
	init->start(1024*1024).detach();

	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("Controller.Mod");
	int sInterval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Controller.Interval", 5);
	string ctlEndpoint = service.getCommunicator()->getProperties()->getPropertyWithDefault("Controller.ctlEndpoint","ControllerFriendFinderNewComerWorker");

	ServerStateSubscriber::instance().initialize(ctlEndpoint,
			&FriendFinderNewComerWorkerI::instance(), mod, sInterval);

        FriendFinderNewComerWorkerI::instance().Register("FFNCW", "ControllerFriendFinderNewComerWorker");
}

//***************************************************************************

void FriendFinderNewComerWorkerI::Report(const Info& info,
		const Ice::Current& current){
	//MCE_INFO("[Report] " << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "] userId=" << info.hostId);
	switch( info.operation ){
		case GET:
			TaskManager::instance().execute(new ReportTask(info));
			MCE_DEBUG("[GET] " << " userId=" << info.hostId);
			break;
		case DELETE:
			{
				int hostId = info.hostId;
				int guestId = info.friendIds.empty() ? 0 : info.friendIds[0];
				MCE_INFO("[DENY] hostId=" << hostId << " guestId=" << guestId);
				TaskManager::instance().execute(new DenyTask(hostId,guestId));
				break;
			}
			break;
		case UNDO:
			break;
		case CREATE:
			break;
		case APPLY:
			{
				int applicant = info.hostId;
				int acceptor = info.friendIds.empty() ? 0 : info.friendIds[0];
				MCE_INFO("[APPLY] applicant=" << applicant << " acceptor=" << acceptor);
				TaskManager::instance().execute(new ApplyTask(applicant,acceptor));
				break;
			}
		case ACCEPT:
			{
				int acceptor = info.hostId;
				int applicant = info.friendIds.empty() ? 0 : info.friendIds[0];
				MCE_INFO("[ACCEPT] acceptor=" << acceptor << " applicant=" << applicant);
				TaskManager::instance().execute(new AcceptTask(acceptor,applicant));
				break;
			}
		case DENY:
			{
				int hostId = info.hostId;
				int guestId = info.friendIds.empty() ? 0 : info.friendIds[0];
				MCE_INFO("[DENY] hostId=" << hostId << " guestId=" << guestId);
				TaskManager::instance().execute(new DenyTask(hostId,guestId));
				break;
			}
		case IGNORE:
			{
				MCE_INFO("[IGNORE] userId=" << info.hostId << " ids.size()=" << info.friendIds.size());
				TaskManager::instance().execute(new IgnoreTask(info.hostId,info.friendIds));
				break;
			}
		case UPDATE:
			{
				MCE_INFO("[UPDATE] userId=" << info.hostId );
				TaskManager::instance().execute(new UpdateInfoTask(info.hostId));
				break;
			}
		case ADD:
			{
				int registerTime = time(NULL);
				if( info.friendIds.empty() ){
					Statement sql;
					sql << "SELECT UNIX_TIMESTAMP(registertime) AS r_time FROM user_time WHERE id=" << info.hostId;
					QueryRunner("user_time_preloader",CDbRServer ).query(sql, RegisterTimeHandler(registerTime));
				} else{
					registerTime = info.friendIds[0];
				}
				MCE_INFO("[ADD] userId=" << info.hostId << " time=" << registerTime);
				TaskManager::instance().execute(new AddNewComerTask(info.hostId,registerTime));
				break;
			}
		default:
			break;
	}
}

void FriendFinderNewComerWorkerI::Rebuild(::Ice::Int userId,
		const Ice::Current& current){
	MCE_INFO("[Rebuild] " << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "] userId=" << userId );
	TaskManager::instance().execute(new RebuildNewComerTask(userId));
}

void FriendFinderNewComerWorkerI::PreLoad(int userId, bool signal,
		const Ice::Current& current){
	MCE_INFO("[Preload] " << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "] signal=" << signal );
	TaskManager::instance().execute(new PreloadTask(signal));
}

void FriendFinderNewComerWorkerI::apply(::Ice::Int applicant, ::Ice::Int acceptor){
        //MCE_DEBUG("[apply] " << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "] applicant=" << applicant
		//<< " acceptor=" << acceptor );
	TaskManager::instance().execute(new ApplyTask(applicant,acceptor));
}

void FriendFinderNewComerWorkerI::accept(::Ice::Int acceptor, ::Ice::Int applicant){
        //MCE_DEBUG("[accept] " << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "] acceptor=" << acceptor
		//<< " applicant=" << applicant );
	TaskManager::instance().execute(new AcceptTask(acceptor,applicant));
}

void FriendFinderNewComerWorkerI::deny(::Ice::Int hostId, ::Ice::Int guestId){
        //MCE_DEBUG("[deny] " << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "] hostId=" << hostId
		//<< " guestId=" << guestId );
	TaskManager::instance().execute(new DenyTask(hostId,guestId));
}

void FriendFinderNewComerWorkerI::ignore(::Ice::Int userId , const MyUtil::IntSeq& ids){
        //MCE_DEBUG("[ignore] " << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "] userId=" << userId
		//<< " ids.size()=" << ids.size() );
	if( ids.size() > 0 ){
		TaskManager::instance().execute(new IgnoreTask(userId,ids));
	}
}

void FriendFinderNewComerWorkerI::addNewComer(::Ice::Int userId){
	int registerTime = time(NULL);
	{
                Statement sql;
                sql << "SELECT UNIX_TIMESTAMP(registertime) AS r_time FROM user_time WHERE id=" << userId;
                QueryRunner("user_time_preloader",CDbRServer ).query(sql, RegisterTimeHandler(registerTime));
	}
	//MCE_INFO("[addNewComer] " << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "] userId=" << userId << " registerTime=" << registerTime );
	TaskManager::instance().execute(new AddNewComerTask(userId,registerTime));
}

void FriendFinderNewComerWorkerI::addNewComerWithTime(::Ice::Int userId,::Ice::Int registerTime){
	//MCE_INFO("[addNewComerWithTime] " << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "] userId=" << userId << " registerTime=" << registerTime );
	TaskManager::instance().execute(new AddNewComerTask(userId,registerTime));
}

void FriendFinderNewComerWorkerI::updateInfo(::Ice::Int userId){
	//MCE_INFO("[updateInfo] " << current.con->toString().substr(current.con->toString().find("remote",25)+17) << "] userId=" << userId );
	TaskManager::instance().execute(new UpdateInfoTask(userId));
}

//**************************************************************************************************************

void ReportTask::handle() {
/*
	vector<unsigned char> mc_objs(information.data.begin(), information.data.end());
	NewComerCacheDataIPtr obj = new NewComerCacheDataI( mc_objs );

	ostringstream log;
	log << "[ReportTask] hostId=" << _info.userId
		<< " hit=" << _info.hit
		<< " inDays=" << _info.inDays
		<< " limit=" << _info.limit
		<< " type=" << _info.type
		<< " res=" << _info.newsRes << "/" << _info.newsCount;

	struct tm *tm_local_now;
	time_t now = time(NULL);
	tm_local_now = localtime(&now);
	tm_local_now->tm_hour = 0;
	tm_local_now->tm_min = 0;
	tm_local_now->tm_sec = 0;
	time_t time_zero = timelocal(tm_local_now);
	int time_deadline = time_zero-DAY_LEFT*86400;

	NewComerDataMap newMap;
	bool changed = false;
	for( NewComerDataMap::iterator itMap = _info.res.begin(); itMap != _info.res.end(); ++itMap ){
		for( NewComerDataSeq::iterator itSeq = itMap->second.begin(); itSeq != itMap->second.end(); ++itSeq ){
			if( time_deadline > itSeq->time ){
				//at this moment, we should delete from db of (itMap->first + itSeq)
				Statement sql;
				sql << "delete from new_comer where userid=" << itMap->first << " and new_comer_id = " << itSeq->userId + ";";
				QueryRunner("new_comer", CDbRServer).execute(sql);

				changed = true;
			}
			if( time_deadline < itSeq->time ){
				newMap[ itMap->first ] = NewComerDataSeq( itSeq, itMap->second.end() );
				break;
			}
		}
	}
	if( changed ){
		NewComerCacheDataPtr obj = new NewComerCacheData;
		obj->news = newMap;
		FriendFinderNewComerCacheAdapter::instance().load(_info.userId,obj);
	}
	MCE_INFO( log.str() );
*/
}

void RebuildNewComerTask::handle(){
	NewComerCacheDataIPtr obj=0;
	try {
		NewComerDbDataMap db_objs = FriendFinderNewComerWorkerI::instance().readDb(_userId);
		obj = new NewComerCacheDataI( db_objs );
	} catch (Ice::Exception& e) {
		MCE_WARN("[RebuildNewComerTask] Ice::Exception (create:" << _userId << ") : " << e.what());
		return;
	} catch (mysqlpp::Exception& e) {
		MCE_WARN("[RebuildNewComerTask] mysqlpp::Exception (create:" << _userId << ") : " << e.what());
		return;
	} catch (std::exception& e) {
		MCE_WARN("[RebuildNewComerTask] std::excetpion (create:" << _userId << ") : " << ": " << e.what());
		return;
	} catch (...) {
		MCE_WARN("[RebuildNewComerTask] unknown exception (create:" << _userId << ") ." );
		return;
	}

	ostringstream tr;
	tr << _userId << "[" << obj->getIds().size();
	MyUtil::Int2IntSeqMap to_remove;
	vector<unsigned char> binary_data = obj->getBinData(_userId,to_remove);
	if( !binary_data.empty() ){
		FriendFinderNewComerWorkerI::instance().writeMc( _userId,binary_data );
	}
	if( to_remove.size()!=0 ){
		FriendFinderNewComerWorkerI::instance().removeDb( _userId,to_remove );
		tr << "-" << to_remove.size() << "]";
	} else{
		tr << "-0]";
	}
}

void PreloadTask::handle(){
}

//**************************************************************************************************************

void ApplyTask::handle() {
	RmInfo ret = FriendFinderNewComerWorkerI::instance().remove(_applicant,_acceptor);
        ostringstream log;
	log << "[ApplyTask] applicant/acceptor=" << _applicant << "/" << ret.guestId
		<< " hit/found=" << ret.hit << "/" << ret.found;
	MCE_INFO( log.str() );
}

void AcceptTask::handle() {
	RmInfo ret = FriendFinderNewComerWorkerI::instance().remove(_acceptor,_applicant);
        ostringstream log;
	log << "[AcceptTask] acceptor/applicant=" << _acceptor << "/" << ret.guestId
		<< " hit/found=" << ret.hit << "/" << ret.found;
	MCE_INFO( log.str() );
}

void DenyTask::handle() {
	RmInfo ret = FriendFinderNewComerWorkerI::instance().remove(_hostId,_guestId);
        ostringstream log;
	log << "[DenyTask] hostId/guestId=" << _hostId << "/" << ret.guestId
		<< " hit/found=" << ret.hit << "/" << ret.found;
	MCE_INFO( log.str() );
}

void IgnoreTask::handle() {
	for( MyUtil::IntSeq::iterator it=_ids.begin(); it!=_ids.end(); ++it ){
		RmInfo ret = FriendFinderNewComerWorkerI::instance().remove(_userId,*it);
		ostringstream log;
		log << "[IgnoreTask] hostId/guestId=" << _userId << "/" << ret.guestId
			<< " hit/found=" << ret.hit << "/" << ret.found;
		MCE_INFO( log.str() );
	}
}

void AddNewComerTask::handle() {
	ostringstream createLog;
	createLog << "[AddNewComerTask] adding " << _userId;

        struct timeval tvStart, tvFilt,
                tvApply, tvFriend, tvGuest, tvHosts,
                tvEnd;

        gettimeofday( &tvStart, NULL );
        double linStart = ((double)tvStart.tv_sec*1000000 + (double)tvStart.tv_usec);

	if( !FacadeStateFilter::instance().isValid() ){
		gettimeofday( &tvEnd, NULL );
		double linEnd = ((double)tvEnd.tv_sec*1000000 + (double)tvEnd.tv_usec);
		double linFiltValid = (linEnd - linStart)/1000000;
		double linTime = (linEnd - linStart)/1000000;
		createLog       << " filt(" << linFiltValid << "s)";
		createLog       << " total(" << linTime << "s)";
		MCE_INFO( createLog.str() << " failed (filt_not_ready)" );
		return;
	}
	if( FacadeStateFilter::instance().isFacadeStateTrue(_userId) ){
		gettimeofday( &tvEnd, NULL );
		double linEnd = ((double)tvEnd.tv_sec*1000000 + (double)tvEnd.tv_usec);
		double linFiltDup = (linEnd - linStart)/1000000;
		double linTime = (linEnd - linStart)/1000000;
		createLog       << " filt(" << linFiltDup << "s)";
		createLog       << " total(" << linTime << "s)";
		MCE_INFO( createLog.str() << " failed (already_done)" );
		return;
	}
        gettimeofday( &tvFilt, NULL );
        double linFiltEnd = ((double)tvFilt.tv_sec*1000000 + (double)tvFilt.tv_usec);
        double linFiltTime = (linFiltEnd - linStart)/1000000;
        createLog       << " filt(" << linFiltTime << "s)";

	IntSeq applyList = NewComerUtil::getApplySeq(_userId);
        gettimeofday( &tvApply, NULL );
        double linApplyEnd = ((double)tvApply.tv_sec*1000000 + (double)tvApply.tv_usec);
        double linApplyTime = (linApplyEnd - linFiltEnd)/1000000;
        createLog       << " apply(" << applyList.size() << ")(" << linApplyTime << "s)";
/*
	if( applyList.size() >= 100){
		MCE_INFO("[AddNewComerTask] userId=" << _userId << " has " << applyList.size() << " apply!" );
		return;
	}
*/

        IntSeq friendList = NewComerUtil::getFriendSeq(_userId);
        gettimeofday( &tvFriend, NULL );
        double linFriendEnd = ((double)tvFriend.tv_sec*1000000 + (double)tvFriend.tv_usec);
        double linFriendTime = (linFriendEnd - linApplyEnd)/1000000;
        createLog       << " friends(" << friendList.size() << ")(" << linFriendTime << "s)";
/*
        if( friendList.size() >= 20){
		MCE_INFO("[AddNewComerTask] userId=" << _userId << " has " << friendList.size() << " friends!" );
                return;
        }
*/

	ostringstream createGuestLog;
	createGuestLog << "";
	NewComerDbData obj = NewComerCreator::instance().createGuest(_userId,_registerTime,createGuestLog);
	if( obj.userId != _userId ){
		gettimeofday( &tvEnd, NULL );
		double linEnd = ((double)tvEnd.tv_sec*1000000 + (double)tvEnd.tv_usec);
		double linGuestTime = (linEnd - linFriendEnd)/1000000;
		createLog       << " guest(" << linGuestTime << "s)";
		double linTime = (linEnd - linStart)/1000000;
		createLog       << " total(" << linTime << "s)";
		MCE_INFO( createLog.str() << " failed (" << createGuestLog.str() << ")" );
		return;
	}
	FacadeStateFilter::instance().set( _userId );
        gettimeofday( &tvGuest, NULL );
        double linGuestEnd = ((double)tvGuest.tv_sec*1000000 + (double)tvGuest.tv_usec);
        double linGuestTime = (linGuestEnd - linFriendEnd)/1000000;
        createLog       << " guest(" << linGuestTime << "s)";

	Int2IntSeqMap hosts = NewComerCreator::instance().findHosts(_userId,friendList);
        gettimeofday( &tvHosts, NULL );
        double linHostsEnd = ((double)tvHosts.tv_sec*1000000 + (double)tvHosts.tv_usec);
        double linHostsTime = (linHostsEnd - linGuestEnd)/1000000;
        createLog       << " hosts(" << hosts.size() << ")(" << linHostsTime << "s)";

	TaskManager::instance().execute(new ShootNewComerTask(obj,hosts));
        gettimeofday( &tvEnd, NULL );
        double linEnd = ((double)tvEnd.tv_sec*1000000 + (double)tvEnd.tv_usec);
        double linShootTime = (linEnd - linHostsEnd)/1000000;
        createLog       << " shoot(" << linShootTime << "s)";

        double linTime = (linEnd - linStart)/1000000;
        createLog       << " total(" << linTime << "s)";

        MCE_INFO( createLog.str() << " success" );
}

void ShootNewComerTask::handle(){
	for( Int2IntSeqMap::iterator itMap = _hosts.begin(); itMap != _hosts.end(); ++itMap ){
		struct timeval tvStart, tvEnd;
		gettimeofday( &tvStart, NULL );
		double linStart = ((double)tvStart.tv_sec*1000000 + (double)tvStart.tv_usec);

		ostringstream shootLog;
		shootLog << "[ShootNewComerTask] shooting " << _obj.userId << "(" << _obj.time << ") to (type:" << itMap->first << ")";

		int facadeCount = 0;
		for( IntSeq::iterator itSeq = itMap->second.begin(); itSeq != itMap->second.end(); ++itSeq ){
			try {
				ostringstream timeLog;
				struct timeval tvHostStart, tvLocate, tvAdd, tvAddDb, tvWriteMc, tvRemoveDb;
				gettimeofday( &tvHostStart, NULL );
				double linHostStart = ((double)tvHostStart.tv_sec*1000000 + (double)tvHostStart.tv_usec);
				timeLog	<< "[";
				shootLog << " " << *itSeq << "[";

				NewComerCacheDataIPtr objs = FriendFinderNewComerWorkerI::instance().locateNewComerCacheDataI( *itSeq );
				gettimeofday( &tvLocate, NULL );
				double linLocateEnd = ((double)tvLocate.tv_sec*1000000 + (double)tvLocate.tv_usec);
				double linLocateTime = (linLocateEnd - linHostStart)/1000000;
				timeLog	<< linLocateTime;
				shootLog << objs->getIds().size();

				MyUtil::Int2IntSeqMap to_remove;
				vector<unsigned char> binary_data = objs->add( _obj.userId,itMap->first,_obj,to_remove );
				++facadeCount;
				gettimeofday( &tvAdd, NULL );
				double linAddEnd = ((double)tvAdd.tv_sec*1000000 + (double)tvAdd.tv_usec);
				double linAddTime = (linAddEnd - linLocateEnd)/1000000;
				timeLog	<< "+" << linAddTime;

				double linWriteMcEnd;
				if( !binary_data.empty() ){
					FriendFinderNewComerWorkerI::instance().addDb( *itSeq,itMap->first,_obj );
					gettimeofday( &tvAddDb, NULL );
					double linAddDbEnd = ((double)tvAddDb.tv_sec*1000000 + (double)tvAddDb.tv_usec);
					double linAddDbTime = (linAddDbEnd - linAddEnd)/1000000;
					timeLog	<< "+" << linAddDbTime;

					FriendFinderNewComerWorkerI::instance().writeMc( *itSeq,binary_data );
					gettimeofday( &tvWriteMc, NULL );
					linWriteMcEnd = ((double)tvWriteMc.tv_sec*1000000 + (double)tvWriteMc.tv_usec);
					double linWriteMcTime = (linWriteMcEnd - linAddDbEnd)/1000000;
					timeLog	<< "+" << linWriteMcTime;
					shootLog << "+1";
				} else{
					timeLog	<< "+0+0";
					shootLog << "+0";
					gettimeofday( &tvWriteMc, NULL );
					linWriteMcEnd = ((double)tvWriteMc.tv_sec*1000000 + (double)tvWriteMc.tv_usec);
				}
				if( to_remove.size()!=0 ){
					FriendFinderNewComerWorkerI::instance().removeDb( *itSeq,to_remove );
					gettimeofday( &tvRemoveDb, NULL );
					double linRemoveDbEnd = ((double)tvRemoveDb.tv_sec*1000000 + (double)tvRemoveDb.tv_usec);
					double linRemoveDbTime = (linRemoveDbEnd - linWriteMcEnd)/1000000;
					timeLog	<< "+" << linRemoveDbTime << "]";
					shootLog << "-" << to_remove.size() << "]";
				} else{
					timeLog	<< "+0]";
					shootLog << "-0]";
				}
				shootLog << timeLog.str();

			} catch (mysqlpp::Exception& e) {
				MCE_WARN("[AddNewComerTask] mysqlpp::Exception shoot " << _obj.userId << "(" << _obj.time << ") to (type:"
					<< itMap->first << ")" << *itSeq << e.what() );
			} catch (std::exception& e) {
				MCE_WARN("[AddNewComerTask] std::exception shoot " << _obj.userId << "(" << _obj.time << ") to (type:"
					<< itMap->first << ")" << *itSeq << e.what() );
			} catch (...) {
				MCE_WARN("[AddNewComerTask] unknown exception shoot " << _obj.userId << "(" << _obj.time << ") to (type:"
					<< itMap->first << ")" << *itSeq );
			}
		}

		gettimeofday( &tvEnd, NULL );
		double linEnd = ((double)tvEnd.tv_sec*1000000 + (double)tvEnd.tv_usec);
		double linTime = (linEnd - linStart)/1000000;
		shootLog	<< " total(" << facadeCount << ")(" << linTime << "s)";

		MCE_INFO( shootLog.str() );
	}
}

void UpdateInfoTask::handle(){
	if( FacadeStateFilter::instance().isValid() ){
		FacadeStateFilter::instance().reset( _userId );
	}
}

/*****************************************************************************/

RmInfo FriendFinderNewComerWorkerI::remove(::Ice::Int hostId, ::Ice::Int guestId){
	RmInfo info;
	info.hostId = hostId;
	info.guestId = guestId;
	info.hit = false;
	info.found = false;

	NewComerCacheDataIPtr obj = locateNewComerCacheDataI(hostId);
	vector<unsigned char> binary_data = obj->remove(info,guestId);
	if (info.found) {
		MyUtil::Int2IntSeqMap to_remove;
		for( MyUtil::IntSeq::iterator it=info.types.begin(); it!=info.types.end(); ++it ){
			to_remove[*it].push_back( guestId );
		}
		FriendFinderNewComerWorkerI::instance().removeDb( hostId,to_remove );
		FriendFinderNewComerWorkerI::instance().writeMc( hostId,binary_data );
	}
	return info;
}

void FriendFinderNewComerWorkerI::addDb(int userId,int type,const NewComerDbData& obj){
	Statement sql;
	sql << "insert ignore into new_comer(user_id,type,new_comer_id,register_time)"
		<< " values(" << userId << "," << type << "," << obj.userId << "," << obj.time << ")";
	QueryRunner("new_comer", CDbWServer ).execute(sql);
}

void FriendFinderNewComerWorkerI::removeDb(int userId,const MyUtil::Int2IntSeqMap& ids){
	bool begin=true;
	ostringstream cons;

	for( MyUtil::Int2IntSeqMap::const_iterator itMap = ids.begin(); itMap!=ids.end(); ++itMap ){
		for( MyUtil::IntSeq::const_iterator itSeq = itMap->second.begin(); itSeq != itMap->second.end(); ++itSeq ){
			if( begin ){
				cons << " (user_id=" << userId << " and new_comer_id=" << *itSeq << " and type=" << itMap->first << ")";
				begin=false;
			} else{
				cons << " or (user_id=" << userId << " and new_comer_id=" << *itSeq << " and type=" << itMap->first << ")";
			}
		}
	}

	Statement sql;
	sql << "delete from new_comer where " << cons.str();
	QueryRunner("new_comer", CDbWServer ).execute(sql);
}

void FriendFinderNewComerWorkerI::writeMc(int userId,const vector<unsigned char>& binary_data){
        ostringstream key;
        key << "FFNCW_" << userId;
        MemcachedClient::GetInstance().SetByDistCache(key.str(), binary_data);
}

NewComerCacheDataIPtr FriendFinderNewComerWorkerI::locateNewComerCacheDataI(int userId){
	ostringstream key;
	key << "FFNCW_" << userId;
	vector<unsigned char> mc_objs;
	MemcachedClient::GetInstance().GetByDistCache(key.str(), mc_objs);
	if( !mc_objs.empty() ){
		return new NewComerCacheDataI( mc_objs );
	} else {
		NewComerDbDataMap db_objs = readDb(userId);
		return new NewComerCacheDataI( db_objs );
	}
}

NewComerDbDataMap FriendFinderNewComerWorkerI::readDb(int userId){
        NewComerDbDataMap res;
        NewComerCacheIResultHandler handler( res );

        Statement sql;
        sql << "SELECT type, new_comer_id, register_time from new_comer where user_id = " << userId;
        QueryRunner("new_comer", CDbRServer ).query(sql, handler);

        return res;
}

/*****************************************************************************/

bool RegisterTimeHandler::handle(mysqlpp::Row& row) const {
        _registerTime = (int)row["r_time"];
        return true;
}

bool NewComerCacheIResultHandler::handle(mysqlpp::Row& row) const {
	int type = (int) row["type"];

	NewComerDbData dataptr;
	dataptr.userId = (int) row["new_comer_id"];
	dataptr.time = (int) row["register_time"];

	_res[type].push_back(dataptr);

	return true;
}

/*****************************************************************************/

void ServiceSchemaI::configure(const Ice::PropertiesPtr& props) {
        TaskManager::instance().config(props, "Report", TASK_LEVEL_REPORT, ThreadPoolConfig(1,100));
        TaskManager::instance().config(props, "Rebuild", TASK_LEVEL_RELOAD, ThreadPoolConfig(1,10));
        TaskManager::instance().config(props, "Preload", TASK_LEVEL_PRELOAD, ThreadPoolConfig(1,1));

        TaskManager::instance().config(props, "Apply", TASK_LEVEL_APPLY, ThreadPoolConfig(1,1));
        TaskManager::instance().config(props, "Accept", TASK_LEVEL_ACCEPT, ThreadPoolConfig(1,1));
        TaskManager::instance().config(props, "Deny", TASK_LEVEL_DENY, ThreadPoolConfig(1,1));
        TaskManager::instance().config(props, "Ignore", TASK_LEVEL_IGNORE, ThreadPoolConfig(1,1));

        TaskManager::instance().config(props, "AddNewComer", TASK_LEVEL_ADD, ThreadPoolConfig(1,10));
        TaskManager::instance().config(props, "ShootNewComer", TASK_LEVEL_SHOOT, ThreadPoolConfig(1,1));
        TaskManager::instance().config(props, "UpdateInfo", TASK_LEVEL_UPDATE_INFO, ThreadPoolConfig(1,1));
}


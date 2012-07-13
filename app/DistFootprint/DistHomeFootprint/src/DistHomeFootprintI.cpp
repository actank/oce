/*
 * =====================================================================================
 *
 *       Filename:  DistHomeFootprintI.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年02月17日 15时28分09秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#include "Footprint.pb.h"
#include "DistHomeFootprintI.h"
#include "ActiveTrackAdapter.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "BatchUpdaterAdapter.h"
#include "ServiceI.h"

using namespace ::MyUtil;
using namespace com::xiaonei::xce;
using namespace xce::footprint;
using namespace xce::log;
using namespace xce::serverstate;
using namespace xce::distcache::footprint;


void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&HomeFootprintCacheI::instance(), service.createIdentity("HFC", ""));


	// 加入向Controller注册
	int mod = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Footprint.Mod", 10);
	int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Footprint.Interval", 5);
	string ctrEndpoints = service.getCommunicator()->getProperties()
		->getPropertyWithDefault("Footprint.ControllerEndpoint", "ControllerDistHomeFootprint");
	ServerStateSubscriber::instance().initialize(ctrEndpoints, &HomeFootprintCacheI::instance(), mod, interval);

	HomeFootprintCacheI::instance().initialize();
}
//---------------------------------------------数据/转换/逻辑--------------------------------------------------------
HomeFootprintDataI::HomeFootprintDataI(DataPtr data) {
	try {
		string data_str(data->begin(), data->end());
		HomeFootprintData* mem_data = new HomeFootprintData();
		mem_data->ParseFromString(data_str);
		
		//visit_count_ = mem_data->visit_count;
		//timestamp_ = mem_data->timestamp;
		/*
		for(int i = mem_data->footprints_size()-1 ; i <= 0; i--) {//倒排有利于插入
			HomeFootprintIPtr tmp = new HomeFootprintI(mem_data->footprints(i).visitor,
					mem_data->footprints(i).time, mem_data->footprints(i).gid);
			footprints_.push_back(tmp);
		}
		*/
	} catch(...) {
	}
}

HomeFootprintDataI::~HomeFootprintDataI() {
}

bool HomeFootprintDataI::addFootprint(const FootprintInfoPtr& fpt) {
	try {
		HomeFootprintIPtr tmp = new HomeFootprintI(fpt->visitor, fpt->timestamp, fpt->gid);
		footprints_.push_back(tmp);
		if (visitCountFilter(fpt)) {
			//grow_count_ ++;
			mop::hi::svc::adapter::BatchUpdaterAdapter::instance().incUserViewCount(fpt->owner);
		}
		return true;
	} catch (...) {
	}
	return false;
}

DataPtr HomeFootprintDataI::getData() {
	/*
	HomeFootprintData data;
	data.set_visit_count(visit_count_);
	data.set_timestamp(timestamp);
	for (vector<HomeFootprintIPtr>::iterator it = footprints_.rbegin();
			it != footprints_.rend(); ++it) {
		HomeFootprint fpt;
		fpt.set_visitor(it->visitor_);
		fpt.set_time(it->time_);
		fpt.set_gid(it->gid_);
		data.add_footprints(fpt);
	}
	
	string result_str;
	data->SerializeToString(&result_str);
	DataPtr result(new Data( result_str.begin(), result_str.end() ));
	return result;
	*/
	DataPtr result;
	return result;
}

DataPtr HomeFootprintDataI::removeFootprint(const FootprintInfoPtr& fpt) {
	/*
	HomeFootprintData data;
	data.set_visitCount(visit_count_);
	data.set_timestamp(timestamp);
	for (vector<HomeFootprintIPtr>::iterator it = footprints_.rbegin();
			it != footprints_.rend(); ++it) {
		if (it->visitor == fpt->visitor) {
			continue;
		}
		HomeFootprint fpt;
		fpt.set_visitor(it->visitor_);
		fpt.set_time(it->time_);
		fpt.set_gid(it->gid_);
		data.add_footprints(fpt);
	}
	
	string result_str;
	data->SerializeToString(&result_str);
	DataPtr result(new Data( result_str.begin(), result_str.end() ));
	return result;
	*/
	DataPtr result;
	return result;
}

int HomeFootprintDataI::getSize() {
	return footprints_.size();
}

void HomeFootprintDataI::addRow(const mysqlpp::Row::value_type& str) {
	vector<int> tmp;
	tmp.resize(str.size()/sizeof(int)); 
	memcpy((void*)(&tmp[0]), str.data(), tmp.size()*sizeof(int));

	ostringstream oss;
	for ( size_t i = 0; i < tmp.size()/4; ++i ) {
		int visitor = tmp[4*i];
		int gid = ((long)tmp[4*i+1]<<32L) + tmp[4*i+2];
		int time = tmp[4*i+3];

		HomeFootprintIPtr fptn = new HomeFootprintI(visitor, time, gid);
		//if ( (fptn->visitor > 0) && (fptn->time >0 && (fptn->gid > 0L)) ) {
			footprints_.push_back(fptn);
		//}
	}
}

string HomeFootprintDataI::getHeadSerialize() {
	string result;
	vector<int> byte_seq;
	int end = (footprints_.size() > FOOTPRINT_HEAD ? footprints_.size() - FOOTPRINT_HEAD : footprints_.size());
	for (int i = footprints_.size()-1; i > end ; i --) {
		/*
		byte_seq.push_back(footprints_[i]->visitor);
		byte_seq.push_back(int(footprints_[i]->gid >> 32L));
		byte_seq.push_back(int(footprints_[i]->gid&0x7FFFFFFF));
		byte_seq.push_back(footprints_[i]->time);
		*/
	}
	result.resize(footprints_.size() * 4 * sizeof(int));
	memcpy((void*)(result.data()), &byte_seq[0], footprints_.size()*4*sizeof(int));
	return result;
}

string HomeFootprintDataI::getBodySerialize() {
	string result = "";
	if (footprints_.size() <= FOOTPRINT_HEAD) {
		return result;
	}
	vector<int> byte_seq;
	int begin = (footprints_.size() > FOOTPRINT_HEAD ? footprints_.size() - FOOTPRINT_HEAD : footprints_.size());
	int end = (footprints_.size() > FOOTPRINT_HEAD + FOOTPRINT_BODY ? footprints_.size() - FOOTPRINT_HEAD - FOOTPRINT_BODY : footprints_.size());
	for (int i = begin; i > end ; i --) {
		/*
		byte_seq.push_back(footprints_.at[i]->visitor);
		byte_seq.push_back(int(footprints_.at[i]->gid >> 32L));
		byte_seq.push_back(int(footprints_.at[i]->gid&0x7FFFFFFF));
		byte_seq.push_back(footprints_.at[i]->time);
		*/
	}
	result.resize(footprints_.size() * 4 * sizeof(int));
	memcpy((void*)(result.data()), &byte_seq[0], footprints_.size()*4*sizeof(int));
	return result;
}
//---------------------------------------------HomeFootprintDataI私有逻辑------------------------------------------
bool HomeFootprintDataI::visitCountFilter(const FootprintInfoPtr& fpt) {
	vector<HomeFootprintIPtr>::iterator it = find_if(footprints_.begin(), footprints_.end(), 
			FootprintEqualComparator<HomeFootprintIPtr>(fpt->visitor));

	int now = time(NULL);
	if (it != footprints_.end()) {
		if (now - (*it)->get_time() < VISIT_INTERVAL) {
			footprints_.erase(it);
			return false;
		}
	}

	int tmp = 0;//grow_count_;
	if (++tmp < VISIT_LIMIT) {
		return true;
	}
	/*
	if (now - grow_timestamp_ > 24*60*60) {
		grow_timestamp_ = now;
		grow_count_ = 0;
		return true;
	}
	*/
	return false;
}

//---------------------------------------------外部接口实现--------------------------------------------------------
HomeFootprintCacheI::HomeFootprintCacheI() {
}

HomeFootprintCacheI::~HomeFootprintCacheI() {
}

void HomeFootprintCacheI::initialize() {
	try {
		while (true) {
			dist_cache_ = DistCacheAdapter::connect("Footprint");
			if (NULL != dist_cache_) {
				break;
			}
		}
		factory_ = new HomeFootprintFactoryI();
		db_helper_ = new WriteDBHelper(100, dist_cache_);
	} catch (...) {
	}
}

void HomeFootprintCacheI::visit(const ::FootprintInfoPtr& fpt, const Ice::Current& current) {
	std::string remoteaddr = current.con->toString().substr(current.con->toString().find("remote", 25));
	MCE_INFO("[HomeFootprintCacheI::visit]  owner: " << fpt->owner << " visitor: " << fpt->visitor << " remoteaddr: " << remoteaddr);
	
	try {
		DataPtr data_ = dist_cache_ -> get(fpt->owner);
		HomeFootprintDataIPtr home_footprint_data_ = getHomeFootprintDataI(fpt->owner, data_);
		if (home_footprint_data_ -> addFootprint(fpt)) {
			dist_cache_ -> set(fpt->owner, home_footprint_data_ -> getData());
		}
	} catch (...) {
	}
}

void HomeFootprintCacheI::visitBatch(const ::FootprintInfoSeq& fpts, const Ice::Current& current) {
	std::string remoteaddr = current.con->toString().substr(current.con->toString().find("remote", 25));
	MCE_INFO("[HomeFootprintCacheI::visitBatch] " << " remoteaddr: " << remoteaddr);

	for(FootprintInfoSeq::const_iterator it = fpts.begin(); it != fpts.end(); ++it){
		visit(*it);
	}
}

void HomeFootprintCacheI::remove(const FootprintInfoPtr& fpt, const Ice::Current& current) {
	std::string remoteaddr = current.con->toString().substr(current.con->toString().find("remote", 25));
	MCE_INFO("[HomeFootprintCacheI::remove]  owner: " << fpt->owner << " visitor: " << fpt->visitor << " remoteaddr: " << remoteaddr);

	try {
		DataPtr data_ = dist_cache_ -> get(fpt->owner);
		HomeFootprintDataIPtr home_footprint_data_ = getHomeFootprintDataI(fpt->owner, data_);
		dist_cache_ -> set(fpt->owner, home_footprint_data_ -> removeFootprint(fpt));
	} catch (...) {
	}

}

::Ice::Int HomeFootprintCacheI::getSize(::Ice::Long id, const Ice::Current& current) {
	std::string remoteaddr = current.con->toString().substr(current.con->toString().find("remote", 25));
	MCE_INFO("[HomeFootprintCacheI::getSize]  id: " << id << " remoteaddr: " << remoteaddr);

	try {
		DataPtr data_ = dist_cache_ -> get(id);
		HomeFootprintDataIPtr home_footprint_data_ = getHomeFootprintDataI(id, data_);
		return home_footprint_data_ -> getSize();
	} catch (...) {
	}
}

::HomeInfoPtr HomeFootprintCacheI::getAll(::Ice::Long userId, 
		::Ice::Int begin, ::Ice::Int nLimit, ::Ice::Int tLimit=0, const Ice::Current& current) {
	std::string remoteaddr = current.con->toString().substr(current.con->toString().find("remote", 25));
	MCE_INFO("[HomeFootprintCacheI::getAll]  owner: " << userId << " remoteaddr: " << remoteaddr);

	//已经转移到客户端调用DistCache.
}

//FootprintFacade调用接口
HomeInfoPtr HomeFootprintCacheI::visitAndGet(const ::FootprintInfoPtr& fpt, ::Ice::Int begin, ::Ice::Int nLimit,
		::Ice::Int tLimit, const Ice::Current& current) {

	std::string remoteaddr = current.con->toString().substr(current.con->toString().find("remote", 25));
	MCE_INFO("[HomeFootprintCacheI::visitAndGet]  owner: "<<fpt->owner<<" visitor: "<<fpt->visitor<<" remoteaddr: "<<remoteaddr);
	visit(fpt,current);
	return getAll(fpt->owner,begin,nLimit,tLimit);
}

Ice::Int HomeFootprintCacheI::getUserVisitCount(Ice::Long userId, const Ice::Current& current) {
	std::string remoteaddr = current.con->toString().substr(current.con->toString().find("remote", 25));
	MCE_INFO("[HomeFootprintCacheI::getUserVisitCount]  owner: " << userId << " remoteaddr: " << remoteaddr);
}

void HomeFootprintCacheI::setUserVisitCount(long userId, Ice::Int count, const Ice::Current& current) {
	std::string remoteaddr = current.con->toString().substr(current.con->toString().find("remote", 25));
	MCE_INFO("[HomeFootprintCacheI::setUserVisitCount]  owner: " << userId << " count: " << count << " remoteaddr: " << remoteaddr);
}

void HomeFootprintCacheI::incUserVisitCount(long userId, const Ice::Current& current) {
	std::string remoteaddr = current.con->toString().substr(current.con->toString().find("remote", 25));
	MCE_INFO("[HomeFootprintCacheI::incUserVisitCount]  owner: " << userId << " remoteaddr: " << remoteaddr);
}

void HomeFootprintCacheI::incUserVisitCountBatch(const MyUtil::LongSeq& userIds, Ice::Int count, const Ice::Current& current) {
	std::string remoteaddr = current.con->toString().substr(current.con->toString().find("remote", 25));
	MCE_INFO("[HomeFootprintCacheI::incUserVisitCountBatch]  owner: " << userIds.size() << " count: " << count << " remoteaddr: " << remoteaddr);
}

bool HomeFootprintCacheI::isValid(const Ice::Current& current) {
	return true;
}

void HomeFootprintCache::setValid(bool newState, const Ice::Current& current) {
	/*
	if (isValid_) {
		ServerStateSubscriber::instance().setStatus(1);
	} else {
		ServerStateSubscriber::instance().setStatus(0);
	}
	*/
}

void HomeFootprintCacheI::contral(const ::MyUtil::Str2StrMap& cmd, const Ice::Current& current) {
}
//-----------------------------------------私有方法----------------------------------------------------------

HomeFootprintDataIPtr HomeFootprintCacheI::getHomeFootprintDataI(long owner, DataPtr data) {
	
	try {
		if (0 == data->size()) {
			//return factory_ -> create(owner);
		} else {
			return new HomeFootprintDataI(data);
		}
	} catch(...) {
	}
}

//-----------------------------------------数据库读取---------------------------------------------------------

Ice::ObjectPtr HomeFootprintFactoryI::create(long id) {
	HomeFootprintDataIPtr result = new HomeFootprintDataI();
	try {
		Statement sql;
		sql << "SELECT " << COLUMN_VIEWCOUNT << " FROM " << TABLE_VIEW_COUNT << " WHERE id = '" << id << "'";
		VisitCountResultHandlerI handler(result);
		QueryRunner(DESC_VIEW_COUNT, CDbRServer).query(sql, handler);
	} catch (mysqlpp::Exception& e) {
	} catch (...) {
	}

	try {
		Statement sql;
	//	sql << "SELECT * FROM " << TABLE_FOOTPRINT_HEAD << " WHERE " << COLUMN_OWNER << "=" << id;
		HomeFootprintResultHandlerI handler(result);
	//	QueryRunner(DESC_FOOTPRINT, CDbRServer, table).query(sql, handler);
	} catch (mysqlpp::Exception& e) {
	} catch (...) {
	}

	try {
		Statement sql;
	//	sql << "SELECT * FROM " << TABLE_FOOTPRINT_BODY << " WHERE " << COLUMN_OWNER << "=" << id;
		HomeFootprintResultHandlerI handler(result);
	//	QueryRunner(DESC_FOOTPRINT, CDbRServer, table).query(sql, handler);
	} catch (mysqlpp::Exception& e) {
	} catch (...) {
	}
	return result;
}

bool VisitCountResultHandlerI::handle(mysqlpp::Row& row) const {
	int visit_count = (int) row[COLUMN_VIEWCOUNT.c_str()];
	//result_ ->setVisitCount(visit_count);
	return true;
}

bool HomeFootprintResultHandlerI::handle(mysqlpp::Row& row) const {
	result_ ->addRow(row[COLUMN_VISITOR.c_str()]);
	return true;
}

//------------------------------------------------写数据库------------------------------------------------
void WriteThread::push (long owner) {
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
	owners_.insert(owner);

	if(owners_.size() > WRITE_BATCH_SIZE) {
		mutex_.notify();
	}
}

void WriteThread::run () {
	try{
		std::set<long> owner_set;
		{
			IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
			if (owners_.size() < WRITE_BATCH_SIZE) {
				mutex_.timedWait(IceUtil::Time::seconds(WRITE_WAIT_TIME));
			}
			owners_.swap(owner_set);
		}
		if(!owner_set.empty()){
			com::xiaonei::xce::Statement head_sql;
			head_sql << "INSERT INTO " << getTableName(TABLE_HEAD_FOOTPRINT) << " (owner, visitors) values";

			com::xiaonei::xce::Statement body_sql;
			body_sql << "INSERT INTO " << getTableName(TABLE_BODY_FOOTPRINT) << " (owner, visitors) values";
			for (std::set<long>::iterator it = owner_set.begin(); it != owner_set.end(); ++it){
				if ( it != owner_set.begin() ) {
					head_sql << ", ";
					body_sql << ", ";
				}

				DataPtr mem_data = cache_->get(*it);
				HomeFootprintDataI data(mem_data);
				head_sql << "(" << (*it) << ", "  << mysqlpp::quote << data.getHeadSerialize() << ")";

				string body_value = data.getBodySerialize();
				if ("" != body_value) {
					body_sql << "(" << (*it) << ", "  << mysqlpp::quote << data.getBodySerialize() << ")";
				}
			}

			head_sql << " ON DUPLICATE KEY UPDATE " << "visitors = values(visitors)";
			body_sql << " ON DUPLICATE KEY UPDATE " << "visitors = values(visitors)";

			com::xiaonei::xce::QueryRunner("footprint", com::xiaonei::xce::CDbWServer, getTableName(TABLE_HEAD_FOOTPRINT)).schedule(head_sql);
			com::xiaonei::xce::QueryRunner("footprint", com::xiaonei::xce::CDbWServer, getTableName(TABLE_BODY_FOOTPRINT)).schedule(body_sql);
		}
	}catch(...){
	}
}

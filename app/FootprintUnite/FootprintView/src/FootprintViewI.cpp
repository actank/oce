/*
 * =====================================================================================
 *
 *       Filename:  FootprintViewI.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年07月04日 18时16分56秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#include "FootprintViewI.h"
#include "HomeFootprintAdapter.h"
#include "util/cpp/InvokeMonitor.h"
#include <Monitor/client/service/ServiceMonitor.h>
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

using namespace MyUtil;
using namespace xce::footprint;
using namespace xce::serverstate;
using namespace com::xiaonei::xce;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();

	service.registerObjectCache(VISIT_COUNT_HISTORY, "VCH", new SyncServantFactory(new VisitCountHistoryFactoryI));
	service.getAdapter()->add(&FootprintViewManagerI::instance(), service.createIdentity("FPVM", ""));


	int mod = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("FootprintView.Mod", 10);
	int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("FootprintView.Interval", 5);

	string ctrEndpoints = service.getCommunicator()->getProperties()
		->getPropertyWithDefault("Footprint.ControllerEndpoint", "ControllerFootprintView");

	ServerStateSubscriber::instance().initialize(ctrEndpoints,
			&FootprintViewManagerI::instance(), mod, interval);
}

FootprintViewManagerI::FootprintViewManagerI() {
	for(int i = 0; i < TABLE_VISIT_COUNT_HISTORY_SIZE; i++) {
		visit_count_threads[i] = new SetVisitCountHistoryThread(i);
		visit_count_threads[i]->start();
	}
}

FootprintViewManagerI::~FootprintViewManagerI() {
}

bool FootprintViewManagerI::verifyVisitCount(const Int2IntMap& counts, const Ice::Current& current) {
	ostringstream oss;
	oss<< " counts.size():" << counts.size();
	InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

	bool result = true;
	for(std::map<int, int>::const_iterator it = counts.begin(); it != counts.end(); ++it) {
		VisitCountInfoPtr info = new VisitCountInfo;
		info->timestamp = time(0);
		info->count = it->second;
		info->state = 0;

		VisitCountHistorySetPtr obj = locateVisitCountHistory(it->first);
		
		bool return_value = obj->add(info);
		if (!return_value) {
			result = false;
			int top_count = obj->getTop()->count;
			MCE_WARN("interface: reportVisitCount message: visitcount exception id: " << it->first << " count: " << it->second << " topCount: " << top_count);
			//用历史最大值修复，这里都是经过校验的结果，所以保证递增；考虑效率问题，没有读数据库值，因为从逻辑上看，历史记录肯定大于数据库人气值
			//校验一般为加一后才操作，所以用历史最大值＋1修复
			HomeFootprintAdapter::instance().setUserVisitCount(it->first, top_count + 1);
		}

		if (result) {
			visit_count_threads[it->first%TABLE_VISIT_COUNT_HISTORY_SIZE]->push(it->first);
		}
	}
	return result;
}

VisitCountInfoSeq FootprintViewManagerI::getVisitCountHistory(Ice::Int id, const Ice::Current& current) {
	ostringstream oss;
	oss<< " owner:" << id;
	InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

	VisitCountHistorySetPtr obj = locateVisitCountHistory(id);
	if (obj) {
		return obj->getAll();
	}
	return VisitCountInfoSeq();
}

VisitCountInfoPtr FootprintViewManagerI::getRecentVisitCount(Ice::Int id, const Ice::Current& current) {
	ostringstream oss;
	oss<< " owner:" << id;
	InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

	VisitCountHistorySetPtr obj = locateVisitCountHistory(id);
	if (obj) {
		return obj->getTop();
	}
	return new VisitCountInfo();
}

void FootprintViewManagerI::setVisitCountLimit(Ice::Int id, const Ice::Current& current) {
	ostringstream oss;
	oss<< " owner:" << id;
	InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

	setVisitCountState(id, STATELIMIT, current);
	return;
}

void FootprintViewManagerI::setVisitCountState(Ice::Int id, Ice::Int state, const Ice::Current& current) {
	ostringstream oss;
	oss<< " owner:" << id << " state: " << state;
	InvokeClient ic = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);

	VisitCountHistorySetPtr obj = locateVisitCountHistory(id);
	if (obj) {
		VisitCountInfoPtr top = obj->getTop();
		if (top) {
			top->timestamp = time(0);
			top->state = state;
		} else {
			VisitCountInfoPtr info = new VisitCountInfo;
			info->timestamp = time(0);
			info->count = 0;
			info->state = state;
			obj->add(info);
		}
	}
	visit_count_threads[id%TABLE_VISIT_COUNT_HISTORY_SIZE]->push(id);
	
	return;
}

VisitCountHistorySetPtr FootprintViewManagerI::locateVisitCountHistory(long id) {
	return ServiceI::instance().locateObject<VisitCountHistorySetPtr>(VISIT_COUNT_HISTORY ,id);
}

string FootprintViewManagerI::getVisitCountHistoryTable(long id) {
	ostringstream oss;
	oss << TABLE_VISIT_COUNT_HISTORY << "_" << id%100;
	return oss.str();
}

Ice::ObjectPtr VisitCountHistoryFactoryI::create(long id) {
	VisitCountHistorySetPtr obj = new VisitCountHistorySet();
	string table = FootprintViewManagerI::instance().getVisitCountHistoryTable(id);
	Statement sql;
	sql << "SELECT * FROM " << table << " WHERE " << VISIT_COLUMN_OWNER << "=" << id;
	try {
		QueryRunner(DESC_VISITCOUNT_HISTORY, CDbRServer, table).query(sql, VisitCountHistoryHandler(obj));
	} catch (mysqlpp::Exception& e) {
		MCE_WARN("VisitCountHistoryFactoryI::create mysqlpp::Exception id: " << id << " " << e.what());
		return NULL;
	} catch (...) {
		MCE_WARN("VisitCountHistoryFactoryI::create Exception id: " << id);
		return NULL;
	}
	MCE_INFO("Loaded visitcount_history for id: " << id);
	return obj;
}

bool VisitCountHistoryHandler::handle(mysqlpp::Row& row) const {
	vch_ -> unserialize(row[VISIT_COLUMN_HISTORY.c_str()]);
	return true;
}


void SetVisitCountHistoryThread::run() {
	while(true) {
		std::set<long> owners;
		try {
			IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
			if (owners_.size() < VISITCOUNT_HISTORY_BATCH_SIZE) {
				mutex_.timedWait(IceUtil::Time::seconds(10));
			}
			owners_.swap(owners);
			//MCE_DEBUG("SetVisitCountHistoryThread::run, owners.size() " << owners.size());
			if (!owners.empty()) {
				string table = FootprintViewManagerI::instance().getVisitCountHistoryTable(id_);
				com::xiaonei::xce::Statement sql;
				sql << "INSERT INTO " << table << " (" << VISIT_COLUMN_OWNER<< "," << VISIT_COLUMN_HISTORY << ") values ";

				for (std::set<long>::iterator it = owners.begin(); it != owners.end(); ++it) {
					if ( it != owners.begin() ) {
						sql << ", ";
					}
 					VisitCountHistorySetPtr obj = FootprintViewManagerI::instance().locateVisitCountHistory(*it);

					sql << "(" << (*it) << "," << mysqlpp::quote << obj->serialize() << ")";
				}

				sql << " ON DUPLICATE KEY UPDATE " << VISIT_COLUMN_HISTORY << "= values(" << VISIT_COLUMN_HISTORY << ")";
				com::xiaonei::xce::QueryRunner(DESC_VISITCOUNT_HISTORY, com::xiaonei::xce::CDbWServer, table).schedule(sql);
			}
		} catch(...) {
			std::ostringstream oss;
			for (std::set<long>::iterator it = owners.begin(); it != owners.end(); ++it) {
				oss << *it << " ";
			}
			MCE_WARN("SetVisitCountThread::run Exception, ids: " << oss.str());
		}
	}
}

#include "BackupManagerI.h"
#include "util/cpp/String.h"
#include "ServiceI.h"
#include "QueryRunner.h"
#include "TaskManager.h"
#include "boost/lexical_cast.hpp"
using namespace talk::occupants;
using namespace MyUtil;
using namespace com::xiaonei::xce;
//-----------------------------------

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.registerObjectCache(ROOM_INFO, "RI", new RoomInfoFactoryI);

	service.getAdapter()->add(&BackupManagerI::instance(), service.createIdentity("M", ""));
	TaskManager::instance().schedule(new SaveTimer());
}

//-----------------------------------

void RoomInfo::put(const OccupantPtr& o) {
	if (o) {

		_info[o->rJid->rid][o->fJid->index] = o;

	}
}
void RoomInfo::remove(const RoomIdPtr& rid, const JidPtr& jid) {
	if (rid && jid) {
		map<RoomIdPtr, map<Ice::Long,OccupantPtr> >::iterator it =
				_info.find(rid);
		if (it != _info.end()) {
			map<Ice::Long, OccupantPtr>::iterator jit =
					it->second.find(jid->index);
			if (jit != it->second.end()) {
				it->second.erase(jit);
				if (it->second.empty()) {
					_info.erase(it);
				}
			}
		}
	}

}
void RoomInfo::removeAll(const RoomIdPtr& rid) {

	if (rid) {
		map<RoomIdPtr, map<Ice::Long,OccupantPtr> >::iterator it =
				_info.find(rid);
		if (it != _info.end()) {
			_info.erase(it);
		}
	}
}
OccupantSeq RoomInfo::get(const RoomIdPtr& rid) {
	OccupantSeq seq;
	if (rid) {
		map<RoomIdPtr, map<Ice::Long,OccupantPtr> >::iterator it =
				_info.find(rid);
		if (it == _info.end()) {
			return seq;
		}

		for (map<Ice::Long,OccupantPtr>::iterator jit = it->second.begin(); jit
				!= it->second.end(); ++jit) {
			seq.push_back(jit->second);
		}
	}
	return seq;
}
//----------------------------

Ice::ObjectPtr RoomInfoFactoryI::create(Ice::Long hashcode) {
	Statement sql;
	sql << "select * from occupant_backup where hashcode = " << boost::lexical_cast<string>(hashcode).c_str();
	//MCE_DEBUG("RoomFactoryI::create --> " << sql);
	try {
#ifndef NEWARCH
		mysqlpp::Result res = QueryRunner("im_groupchat", CDbRServer).store(sql);
#else
		mysqlpp::StoreQueryResult res = QueryRunner("im_groupchat", CDbRServer).store(sql);
#endif
		MCE_DEBUG("RoomInfoFactoryI::create --> load occupant_backup from database, count=" << res.size());
		RoomInfoPtr ri = new RoomInfo();
		for (size_t i= 0; i<res.size(); ++i) {

			mysqlpp::Row row = res.at(i);

			OccupantPtr o = new Occupant();
#ifndef NEWARCH
			o->rJid = stringToRoomJid(row["rjid"].get_string());
#else
			o->rJid = stringToRoomJid(row["rjid"].c_str());
#endif
			o->fJid = new Jid();
			o->fJid->userId = (int)row["userid"];
#ifndef NEWARCH
			o->fJid->endpoint = row["endpoint"].get_string();
#else
			o->fJid->endpoint = row["endpoint"].c_str();
#endif
			o->fJid->index = (Ice::Long)row["jid_index"];

			o->aType = (Affiliation)(int)row["affiliation"];
			o->role = (RoomRole)(int)row["role"];

			ri->put(o);
		}
		return ri;
	} catch(Ice::Exception& e) {
		MCE_WARN(e << " at " << __FILE__ << ":" << __LINE__);
	} catch (std::exception& e) {
		MCE_WARN(e.what() << " at " << __FILE__ << ":" << __LINE__);
	} catch (...) {
		MCE_WARN("Unkown exception at " << __FILE__ << ":" << __LINE__);
	}

	return 0;
}

//-------------------------

void BackupManagerI::_addUpdateSqlObj(const OccupantPtr& o) {
	IceUtil::Mutex::Lock lock(_sqlMutex);
	if (o && o->rJid->rid) {
		SqlObjPtr s = new SqlObj();
		s->op = 0;
		s->o = o;
		_saveSeq[o->rJid->rid].push_back(s);
	}
}
void BackupManagerI::_addRemoveSqlObj(const RoomIdPtr& rid, const JidPtr& jid) {
	IceUtil::Mutex::Lock lock(_sqlMutex);
	if (rid && jid) {
		OccupantPtr o = new Occupant();
		o->rJid = new RoomJid;
		o->rJid->rid = rid;
		o->fJid = jid;
		SqlObjPtr s = new SqlObj();
		s->op = 1;
		s->o = o;

		_saveSeq[rid].push_back(s);

	}

}
void BackupManagerI::_addRemoveSqlObj(const RoomIdPtr& rid) {
	IceUtil::Mutex::Lock lock(_sqlMutex);
	if (rid) {
		OccupantPtr o = new Occupant();
		o->rJid = new RoomJid;
		o->rJid->rid = rid;

		SqlObjPtr s = new SqlObj();
		s->op = 1;
		s->o = o;
		_saveSeq[rid].push_back(s);
	}
}

void BackupManagerI::swapSqlObj(SqlMap& m) {
	IceUtil::Mutex::Lock lock(_sqlMutex);
	_saveSeq.swap(m);
}

void BackupManagerI::put(const OccupantPtr& o, const Ice::Current&) {
	MCE_DEBUG("BackupManagerI::put");
	if (o && o->rJid && o->rJid->rid) {
		MCE_DEBUG("BackupManagerI::put --> rjid:"<< roomJidToString(o->rJid));
		Ice::Long hashcode = StrUtil::hashcode(roomIdToString(o->rJid->rid));
		RoomInfoPtr ri = ServiceI::instance().locateObject<RoomInfoPtr>(ROOM_INFO, hashcode);
		MCE_DEBUG("BackupManagerI::put --> hashcode:"<< hashcode << " ri.size:"<<ri->get(o->rJid->rid).size());
		if (ri) {
			ri->put(o);
		}
		_addUpdateSqlObj(o);
	}
}
void BackupManagerI::puts(const OccupantSeq& oseq, const Ice::Current&) {
	for (size_t i = 0; i < oseq.size(); ++i) {
		put(oseq.at(i));
	}
}
void BackupManagerI::remove(const RoomIdPtr& rid, const JidPtr& jid,
		const Ice::Current&) {
	if (rid && jid) {
		Ice::Long hashcode = StrUtil::hashcode(roomIdToString(rid));
		RoomInfoPtr ri = ServiceI::instance().findObject<RoomInfoPtr>(ROOM_INFO, hashcode);
		if (ri) {
			ri->remove(rid, jid);
		}
		_addRemoveSqlObj(rid, jid);
	}

}
void BackupManagerI::removeAll(const RoomIdPtr& rid, const Ice::Current&) {
	if (rid) {
		Ice::Long hashcode = StrUtil::hashcode(roomIdToString(rid));
		RoomInfoPtr ri = ServiceI::instance().findObject<RoomInfoPtr>(ROOM_INFO, hashcode);
		if (ri) {
			ri->removeAll(rid);
		}
		_addRemoveSqlObj(rid);
	}
}
OccupantSeq BackupManagerI::get(const RoomIdPtr& rid, const Ice::Current&) {
	MCE_DEBUG("call BackupManagerI::get");
	if (rid) {
		Ice::Long hashcode = StrUtil::hashcode(roomIdToString(rid));
		RoomInfoPtr ri = ServiceI::instance().locateObject<RoomInfoPtr>(ROOM_INFO, hashcode);
		if (ri) {
			MCE_DEBUG("BackupManagerI::get   --> return size:"<< ri->get(rid).size());
			return ri->get(rid);
		}
	}
	return OccupantSeq();

}

//------------------------------

void SaveTimer::handle() {
	MCE_DEBUG("SaveTimer::handle");
	SqlMap tmp;
	BackupManagerI::instance().swapSqlObj(tmp);
	MCE_DEBUG("tmp.size:"<<tmp.size());
	bool stop = false;
	for (size_t i = 0; (!stop); ++i) {
		OccupantSeq oseq0, oseq1, oseq2;
		stop = true;
		for (SqlMap::iterator it = tmp.begin(); it != tmp.end(); ++it) {
			if (it->second.size() > i) {
				if(it->second.size() > i + 1){
					stop = false;
				}

				SqlObjPtr s = it->second.at(i);
				if (s->op == 0) {
					//replace
					oseq0.push_back(s->o);
				} else {
					//delete
					if (s->o->fJid) {
						oseq1.push_back(s->o);
					} else {
						oseq2.push_back(s->o);
					}
				}

			}
		}

		//do sql
		MCE_DEBUG("oseq0:"<<oseq0.size()<<"  oseq1:"<<oseq1.size() << " oseq2:"<<oseq2.size());
		if (!oseq0.empty()) {
			//Statement sql;
			ConnectionHolder conn("im_groupchat", CDbRServer," ");
			mysqlpp::Query query = conn.query();
			query
					<< "replace into occupant_backup (hashcode,userid,jid_index,endpoint,rjid,rid,affiliation,role) values ";

			for (size_t j = 0; j < oseq0.size(); ++j) {
				if (j) {
					query << ",";
				}
				Ice::Long hashcode = StrUtil::hashcode(roomIdToString(oseq0.at(j)->rJid->rid));
				query << "(" << hashcode << "," << oseq0.at(j)->fJid->userId << "," << oseq0.at(j)->fJid->index << "," << mysqlpp::quote<<oseq0.at(j)->fJid->endpoint << "," << mysqlpp::quote
						<< roomJidToString(oseq0.at(j)->rJid)<< "," << mysqlpp::quote
						<< roomIdToString(oseq0.at(j)->rJid->rid)<< "," << oseq0.at(j)->aType<<"," << oseq0.at(j)->role << ")";
			}
			try {
				query.store();
				//QueryRunner("im_groupchat", CDbWServer).store(sql);
			} catch (std::exception& e) {
				MCE_WARN(__FUNCTION__ << "-->" << e.what() << " at " << __FILE__ << ":" << __LINE__);

			} catch (...) {
				MCE_WARN(__FUNCTION__ << "-->" "Unkown exception at " << __FILE__ << ":" << __LINE__);

			}
		}
		if (!(oseq1.empty()&&oseq2.empty())) {
			//Statement sql;
			ConnectionHolder conn("im_groupchat", CDbRServer," ");
			mysqlpp::Query query = conn.query();
			query << "delete from occupant_backup where ";

			if (!oseq1.empty()) {
				query << "jid_index in (";
				for (size_t j = 0; j < oseq1.size(); ++j) {
					if (j) {
						query << ",";
					}
					query << oseq1.at(j)->fJid->index;
				}
				query << ") ";
			}

			if (!oseq2.empty()) {
				if (!oseq1.empty()) {
					query << " or ";
				}
				query << " rid in (";
				for (size_t j = 0; j < oseq2.size(); ++j) {
					if (j) {
						query<< ",";
					}
					query << mysqlpp::quote << roomIdToString(oseq2.at(j)->rJid->rid) ;
				}
				query << ")";
			}
			try {
				query.store();
				//QueryRunner("im_groupchat", CDbWServer).store(sql);
			} catch (std::exception& e) {
				MCE_WARN(__FUNCTION__ << "-->" << e.what() << " at " << __FILE__ << ":" << __LINE__);

			} catch (...) {
				MCE_WARN(__FUNCTION__ << "-->" "Unkown exception at " << __FILE__ << ":" << __LINE__);

			}
		}

	}

	TaskManager::instance().schedule(new SaveTimer());
}


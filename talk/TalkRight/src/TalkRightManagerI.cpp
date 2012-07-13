#include "TalkRightManagerI.h"
#include "BuddyCoreAdapter.h"
#include "QueryRunner.h"
#include "TalkRightAdapter.h"
#include "MessageType.h"

using namespace com::xiaonei::talk;
using namespace com::xiaonei::talk::util;
using namespace com::xiaonei::talk::common;
using namespace com::xiaonei::service::buddy;
using namespace com::xiaonei::xce;
using namespace MyUtil;
using namespace com::xiaonei::talk::adapter;

static const int UB = 0;
static const int SMB = 1;
static const int US = 2;

void MyUtil::initialize() {

	ServiceI& service = ServiceI::instance();

	service.registerObjectCache(UB, "UB", new UserBlockSetFactoryI);
	service.registerObjectCache(SMB, "SMB", new SysMsgBlockSetFactoryI);
	service.registerObjectCache(US, "US", new UserStatusFactoryI);

	service.getAdapter()->add(new RightManagerI, service.createIdentity("M", ""));

}

Ice::ObjectPtr UserBlockSetFactoryI::create(Ice::Int id) {
	try {
		MyUtil::IntSeq ids = BuddyCoreAdapter::instance().getBlockSeq(id,0,-1);
		UserBlockSetPtr ptr = new BlockSet<int>();
		for(size_t i = 0; i < ids.size(); ++i) {
			ptr->add(ids.at(i));
		}

		return ptr;
	} catch(...) {
		MCE_WARN("UserBlockSetFactoryI::create err -> id="<<id);
	}
	return 0;
}
ObjectResultPtr UserBlockSetFactoryI::create(const MyUtil::IntSeq& ids) {
	ObjectResultPtr result = new ObjectResult;
	for (size_t i = 0; i < ids.size(); ++i) {

		Ice::ObjectPtr obj = create(ids.at(i));
		if (obj) {
			result->data[ids.at(i)] = obj;
		}
	}
	return result;
}

Ice::ObjectPtr SysMsgBlockSetFactoryI::create(Ice::Int id) {

//	Statement sql;
	ConnectionHolder conn("im", CDbRServer," ");
	mysqlpp::Query query = conn.query();
	query
			<< "select userid,message_type from system_message_block where userid = "
			<< id;

//	DefResSetPtr drs;
//	ConnectionQuery conn("im", CDbRServer);

	try {
		SysMsgBlockSetPtr ptr = new BlockSet<string>();

#ifndef NEWARCH
		mysqlpp::Result res = query.store();
#else
		mysqlpp::StoreQueryResult res = query.store();
		if(!res){
			return 0;
		}
#endif
//		ResSetPtr rs = conn.doSelect(sql);
//		 = DefResSetPtr::dynamicCast(rs);

#ifndef NEWARCH
		for (size_t i= 0; i<res.size(); ++i) {
#else
		for (size_t i = 0; i<res.num_rows(); ++i) {
#endif

#ifndef NEWARCH
			string type = res[i]["MESSAGE_TYPE"].get_string();//ptr->link;
#else
			string type = res[i]["MESSAGE_TYPE"].c_str();//ptr->link;
#endif
			ptr->add(type);
		}

		return ptr;
	} catch(Ice::Exception& e) {
		MCE_WARN(e << " at " << __FILE__ << ":" << __LINE__);
	} catch (std::exception& e) {
		MCE_WARN(e.what() << " at " << __FILE__ << ":" << __LINE__);
	} catch (...) {
		MCE_WARN("Unkown exception at " << __FILE__ << ":" << __LINE__);
	}

	return 0;
}

ObjectResultPtr SysMsgBlockSetFactoryI::create(const MyUtil::IntSeq& ids) {
	ObjectResultPtr result = new ObjectResult;
	for (size_t i = 0; i < ids.size(); ++i) {

		Ice::ObjectPtr obj = create(ids.at(i));
		if (obj) {
			result->data[ids.at(i)] = obj;
		}
	}
	return result;
}

Ice::ObjectPtr UserStatusFactoryI::create(Ice::Int id) {
	try {
		UserStatusPtr ptr = new UserStatus();
		Statement sql;
                sql << "select * from user_state where id = " << id;
                mysqlpp::StoreQueryResult res = QueryRunner("user_state", CDbRServer).store(sql);
                if (!res || res.num_rows()<=0) {
                        MCE_WARN("load state error id=" << id);
                        return 0;
                }
                mysqlpp::Row row = res.at(0);
		int state = (int)row["state"];
		ptr->selected = state&1;

                sql.clear();
                sql << "select * from user_passport where id=" << id;
                res = QueryRunner("user_passport", CDbRServer).store(sql);
                if (!res || res.num_rows()<=0) {
                        MCE_WARN("load status error id="<<id);
                        return 0;
                }
                row = res.at(0);
                ptr->status = (int)row["status"];
		return ptr;
	} catch(Ice::Exception& e) {
		MCE_WARN(e << " at " << __FILE__ << ":" << __LINE__);
	} catch (std::exception& e) {
		MCE_WARN(e.what() << " at " << __FILE__ << ":" << __LINE__);
	} catch (...) {
		MCE_WARN("Unkown exception at " << __FILE__ << ":" << __LINE__);
	}
	return 0;
}

ObjectResultPtr UserStatusFactoryI::create(const MyUtil::IntSeq& ids) {
	ObjectResultPtr result = new ObjectResult;
	for (size_t i = 0; i < ids.size(); ++i) {

		Ice::ObjectPtr obj = create(ids.at(i));
		if (obj) {
			result->data[ids.at(i)] = obj;
		}
	}
	return result;
}
//bool RightManagerI::verify(Ice::Int from, Ice::Int to, Ice::Int type,
//		const Ice::Current&) {
//	if(type == SYSMSG){
//		Ice::ObjectPtr obj = ServiceI::instance().findObject(SMB,to);
//		if(obj){
//			SysMsgBlockSetPtr ptr = SysMsgBlockSetPtr::dynamicCast(obj);
//			return ptr->has();
//		}
//	}
//
//
//}
MyUtil::IntSeq RightManagerI::batchVerify(const VerifySeq& seq,
		const Ice::Current&) {
	MyUtil::IntSeq res;

	for (size_t i = 0; i < seq.size(); ++i) {

		if (seq.at(i)->type == SYSTEM_MESSAGE) {
			SysMsgBlockSetPtr ptr = ServiceI::instance().locateObject<SysMsgBlockSetPtr>(SMB, seq.at(i)->to);
			if (ptr) {
				if (ptr->has(seq.at(i)->sysMsgType)) {
					continue;
				}
			}
		} else {
			// check message
			if (seq.at(i)->type == MESSAGE || seq.at(i)->type == OFFLINE_MESSAGE) {
				Relationship r;
				r.from = seq.at(i)->from;
				r.to = seq.at(i)->to;

				BuddyDesc bd = BuddyCoreAdapter::instance().getRelation(r, 1000);
				MCE_DEBUG("relation:"<<bd<<"  Friend:"<<Friend);
				if (bd != Friend) {
					/*UserStatusPtr us = TalkRightAdapter::instance().getUserStatus(r.from);
					if (us->selected != 1 || us->status != 0) {
						MCE_DEBUG("user:"<<r.from
								<<" can not send message to user:"<<r.to
								<<" fromid's selected:"<<us->selected
								<<" and status:"<<us->status);
						continue;
					}*/
					continue;
				}

			}
			// block
			UserBlockSetPtr ptr = ServiceI::instance().locateObject<UserBlockSetPtr>(UB, seq.at(i)->to);
			if (ptr) {
				if (ptr->has(seq.at(i)->from)) {
					MCE_INFO("from:"<<seq.at(i)->from <<" is blocked by to:"<<seq.at(i)->to);
					continue;
				}
			}

			// hidden
			if (seq.at(i)->type == PRESENCE_COMMON) {
				if (seq.at(i)->from > 0 && seq.at(i)->to > 0) {
					int cluster = TalkRightAdapter::instance().getCluster();
					if ((seq.at(i)->from % cluster) == (seq.at(i)->to % cluster)) {
						if (isHiddenUser(seq.at(i)->from)) {
							continue;
						}
					} else {
						bool flag = 0;
						try {
							flag = TalkRightAdapter::instance().isHiddenUser(seq.at(i)->from);
						} catch(...) {
							MCE_WARN("check hidden user err");
						}
						if (flag) {
							continue;
						}
					}
				}
			}

		}

		res.push_back(seq.at(i)->index);
	}
	return res;
}

bool RightManagerI::isHiddenUser(Ice::Int userId, const Ice::Current&) {
	return _hiddenUsers->has(userId);
}
void RightManagerI::addHiddenUser(Ice::Int userId, const Ice::Current&) {
	_hiddenUsers->add(userId);
}
void RightManagerI::removeHiddenUser(Ice::Int userId, const Ice::Current&) {
	_hiddenUsers->remove(userId);
}
void RightManagerI::reloadBlockUser(Ice::Int userId, const Ice::Current&) {
	MCE_DEBUG("RightManagerI::reloadBlockUser --> userid:"<<userId);
	ServiceI::instance().removeObject(UB, userId);
}

UserStatusPtr RightManagerI::getUserStatus(Ice::Int userId, const Ice::Current&) {
	return ServiceI::instance().locateObject<UserStatusPtr>(US, userId);
}

void RightManagerI::reloadUserStatus(Ice::Int userId, const Ice::Current&) {
	ServiceI::instance().removeObject(US, userId);
}

void RightManagerI::modifySystemMessageBlock(Ice::Int userId,
		const MyUtil::StrSeq& typeSeq, const Ice::Current&) {

	Statement sql;

	sql << "delete from system_message_block where userid = "<<userId;

//	ConnectionQuery conn("im", CDbWServer);
//	conn.doDelete(sql);
	QueryRunner("im", CDbWServer).store(sql);

	if (!typeSeq.empty()) {

		sql.clear();
		sql << "insert into system_message_block (userid,message_type) values ";
		for (size_t i = 0; i < typeSeq.size(); ++i) {
			if (i) {
				sql<<",";
			}
			sql<<"("<<userId<<",'"<<typeSeq.at(i)<<"')";
		}
		//ConnectionQuery conn("im_common", CDbWServer);
		QueryRunner("im", CDbWServer).store(sql);
	}

	ServiceI::instance().removeObject(SMB, userId);

}
//void RightManagerI::removeSystemMessageBlock(Ice::Int userId,
//		const string& type, const Ice::Current&) {
//	Statement sql;
//	sql << "delete from system_message_block where userid = "<<userId << " and message_type='"<<type<<"'";
//
//	ConnectionQuery conn("im_common", CDbWServer);
//	conn.doInsert(sql);
//
//	ServiceI::instance().removeObject(SMB, userId);
//}

MyUtil::StrSeq RightManagerI::getSystemMessageBlock(Ice::Int userId,
		const Ice::Current&) {

	SysMsgBlockSetPtr ptr = ServiceI::instance().locateObject<SysMsgBlockSetPtr>(SMB, userId);
	if (ptr) {
		return ptr->getSeq();
	}
	return MyUtil::StrSeq();
}


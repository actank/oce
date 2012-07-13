#include "TestBaiI.h"
#include "ServiceI.h"

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&TestBaiI::instance(), service.createIdentity("simpleTestBai",""));
  TaskManager::instance().execute(new TestBaiTask());
}
/*
bool ResultHandlerI::handle(mysqlpp::Row& res) const {
	int num = res.size();
	MCE_INFO("num is " << num);
	if (num == 0) {
		return false;
	}

	for (int i = 0; i < num; i++) {
		_tb.add(int(res[i]["a"]), int(res[i]["b"]));
		MCE_INFO(int(res[i]["a"]) << " =========> " << int(res[i]["b"]));
	}
	MCE_INFO("add the data success");
	return true;
}
*/
TestBaiI::TestBaiI() {
	_tb.size(10);
}

void TestBaiI::Insert(Ice::Int uid, Ice::Int count, const Ice::Current&) {
	com::xiaonei::xce::Statement sql;
	sql << "INSERT INTO yuanfei (a, b) VALUES " <<
			"(" << uid << "," << count << ") ;";

	try {
		com::xiaonei::xce::QueryRunner("im_stat", com::xiaonei::xce::CDbWServer).store(sql);
	} catch (Ice::Exception& e) {
		MCE_WARN("TestBaiI::Insert --> error:" << e);
	} catch (std::exception& e) {
		MCE_WARN("TestBaiI::Insert --> error:" << e.what());
	}
}

void TestBaiI::Delete(Ice::Int uid, const Ice::Current&) {
	// 首先从DB中删除，可以确保从cache中彻底删除
	{
		com::xiaonei::xce::Statement sql;
		sql << "delete from yuanfei where a = " << uid << " ;";

		try {
			com::xiaonei::xce::QueryRunner("im_stat", com::xiaonei::xce::CDbWServer).store(sql);
		} catch (Ice::Exception& e) {
			MCE_WARN("TestBaiI::Delete --> error:" << e);
		} catch (std::exception& e) {
			MCE_WARN("TestBaiI::Delete --> error:" << e.what());
		}
	}
	// 从cache中删除
	{
		IceUtil::Mutex::Lock lock(_tb_mutex);

		if (_tb.has(uid)) {
			_tb.remove(uid);
		}
	}
}

Ice::Int TestBaiI::Find(Ice::Int uid, const Ice::Current&) {
	int flag = _tb.find(uid, -1);
	MCE_INFO("find flag is " << flag);
	flag = _tb.locate(uid, -1);
	MCE_INFO("locate flag is " << flag);
	if (flag != -1) {
		return flag;
	}

	//ResultHandlerI handler(_tb);

	com::xiaonei::xce::Statement sql;
	sql << "SELECT * FROM " << DB_TABLE << " where a = " << uid << " limit 1;";
	mysqlpp::StoreQueryResult res;
	res = com::xiaonei::xce::QueryRunner(DB_INSTANCE, com::xiaonei::xce::CDbRServer).store(sql);

	if (res.size() == 0) {
		return -1;
	}

	_tb.add(int(res[0]["b"]), int(res[0]["a"]));
	MCE_INFO(int(res[0]["a"]) << " =========> " << int(res[0]["b"]));

	return int(res[0]["b"]);
}

void TestBaiI::Update(Ice::Int uid, Ice::Int count, const Ice::Current&) {
	// 首先更新数据库
	{
		com::xiaonei::xce::Statement sql;
		sql << "update yuanfei set b = " << count << " where a = " << uid << " ;";

		try {
			com::xiaonei::xce::QueryRunner("im_stat", com::xiaonei::xce::CDbWServer).store(sql);
		} catch (Ice::Exception& e) {
			MCE_WARN("TestBaiI::Update --> error:" << e);
		} catch (std::exception& e) {
			MCE_WARN("TestBaiI::Update --> error:" << e.what());
		}
	}
	// 如果cache中有就更新。没有不更新，值相同也不更新。
	{
		IceUtil::Mutex::Lock lock(_tb_mutex);

		if (_tb.has(uid)) {
			if (_tb.find(uid, -1) == count) {
				return;
			}
			_tb.remove(uid);
			_tb.add(count, uid);
		}
	}
}

void TestBaiI::_load() {
	//ResultHandlerI handler(_tb);

	com::xiaonei::xce::Statement sql;
	sql << "SELECT * FROM " << DB_TABLE << " limit 10;";
	mysqlpp::StoreQueryResult res;
	res = com::xiaonei::xce::QueryRunner(DB_INSTANCE, com::xiaonei::xce::CDbRServer).store(sql);
	MCE_INFO("num is " << res.size());
	for (size_t i = 0; i < res.size(); ++i) {
		_tb.add(int(res[i]["b"]), int(res[i]["a"]));
		MCE_INFO(int(res[i]["a"]) << " =========> " << int(res[i]["b"]));
	}
	//com::xiaonei::xce::QueryRunner(DB_INSTANCE, com::xiaonei::xce::CDbRServer,DB_TABLE).query(sql, handler);
	MCE_INFO("load complete" << getpid());
}

void TestBaiTask::handle() {
  TestBaiI::instance()._load();
}

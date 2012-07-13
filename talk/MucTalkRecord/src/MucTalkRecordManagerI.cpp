#include "MucTalkRecordManagerI.h"
#include "ServiceI.h"
#include <boost/lexical_cast.hpp>
#include "TalkCacheAdapter.h"
#include "XmppTools.h"
#include "TaskManager.h"
#include "Date.h"
#include "QueryRunner.h"
#include "ConnectionQuery.h"
#include "OzeinFilterAdapter.h"
using namespace com::xiaonei::ozein::content::generation;
using namespace com::xiaonei::talk::muc;
using namespace com::xiaonei::talk::util;
using namespace com::xiaonei::talk::common;
using namespace com::xiaonei::talk;
using namespace MyUtil;
using namespace std;
using namespace com::xiaonei::xce;
void MyUtil::initialize(){
	ServiceI& service = ServiceI::instance();
  Ice::PropertiesPtr props = service.getCommunicator()->getProperties();
  service.getObjectCache()->registerCategory(AG, "AG", new ActiveGroupFactoryI, props, true);
	service.getAdapter()->add(&MucTalkRecordManagerI::instance(), service.createIdentity("M", ""));
}

static string GetTableName(int groupid){
	ostringstream os;	
	os << "minigroup_chat_record_" << groupid % 100;
	return os.str();
}

MucTalkMessageSeq ActiveGroup::GetMessage(){
	return _messages;
}
void ActiveGroup::PushMessage(const MucTalkMessagePtr& msg){
	if(MESSAGESIZE == _messages.size()){
		MCE_DEBUG("ActiveGroup::PushMessage --> groupid = " << _groupid << " pos = " << _pos << " _message.size = " << _messages.size());
		_messages.at(_pos) = msg;
	}else{
		MCE_DEBUG("ActiveGroup::PushMessage --> groupid = " << _groupid << " pos = " << _pos << " _message.size = " << _messages.size() << " MESSAGESIZE = " << MESSAGESIZE);
		_messages.push_back(msg);
	}
	_pos = (_pos + 1) % MESSAGESIZE;
}


Ice::ObjectPtr ActiveGroupFactoryI::create(int groupid){
	ActiveGroupPtr ag = new ActiveGroup(groupid);
	string tablename = GetTableName(groupid);
	if(tablename.empty()){
		return ag;
	}
	Statement sql;
	sql << "select userid, message, time from " << tablename << " where minigroup_id = " << groupid <<" order by id desc limit " << MESSAGESIZE;
	mysqlpp::StoreQueryResult res = QueryRunner(DB_INSTANCE, CDbRServer, tablename).store(sql);
	if (res && res.num_rows() > 0) {
		for (size_t i = 0; i < res.num_rows(); ++i) {
			mysqlpp::Row row = res.at(i);
			MucTalkMessagePtr talkmsg = new MucTalkMessage();
			talkmsg->userid = (int)row["userid"];
			talkmsg->message = (string)row["message"];
			MyUtil::Date t((string)row["time"]);
			talkmsg->timestamp = t.time();
			ag->PushMessage(talkmsg);
		}
	}
	
	return ag;
}


MucTalkRecordManagerI::MucTalkRecordManagerI(){
	init();
}
void MucTalkRecordManagerI::init(){
	for(int i = 0; i < DBCLUSTERSIZE; ++i){
		DBWriterPtr writer = new DBWriter(i);
		TaskManager::instance().scheduleRepeated(writer);
		_db_writers.push_back(writer);
	}
}


void MucTalkRecordManagerI::RecordTalkMsg(int userid, int groupid, const string& msg, const Ice::Current& current){
	TimeStat ts;
	MucTalkMessagePtr talkmsg = new MucTalkMessage();
	talkmsg->userid= userid;
	talkmsg->groupid = groupid;
	talkmsg->message= msg;
	talkmsg->timestamp = time(NULL);

	_db_writers[groupid % DBCLUSTERSIZE]->WriteDB(talkmsg);
	{
		ActiveGroupPtr ag = ServiceI::instance().locateObject<ActiveGroupPtr> (AG, groupid);
		IceUtil::Mutex::Lock lock(_mutex[groupid% poolsize]);
		ag->PushMessage(talkmsg);
	}
	MCE_INFO("interface RecordTalkMsg usetime : " << ts.getTime());
}

MucTalkMessageSeq MucTalkRecordManagerI::GetRecentlyMsg(int groupid, const Ice::Current& current){
	TimeStat ts;
	MucTalkMessageSeq ans;
	ActiveGroupPtr ag = ServiceI::instance().locateObject<ActiveGroupPtr> (AG, groupid);
	IceUtil::Mutex::Lock lock(_mutex[groupid % poolsize]);
	ans = ag->GetMessage();
	MCE_INFO("interface GetRecentlyMsg usetime : " << ts.getTime());
	return ans;
}

void DBWriter::WriteDB(const MucTalkMessagePtr& msg){
	IceUtil::Mutex::Lock lock(_mutex);
	_messages.push_back(msg);
	MCE_INFO("DBWriter::WriteDB --> userid = " << msg->userid << " groupid = " << msg->groupid << " message = " << msg->message << " _messages.size = " << _messages.size());
}

void DBWriter::handle(){
	map<int, string> groupmsg_updatetime;
	MucTalkMessageSeq seq;
	{
		IceUtil::Mutex::Lock lock(_mutex);
		_messages.swap(seq);
	}
	if(seq.empty()){
		return;
	}
	MCE_INFO("DBWriter::WriteDB --> write size = " << seq.size());
	Statement sql;
	string tablename = GetTableName(_table_index);
	sql << "insert into " << tablename << " (userid, message, time, minigroup_id) values";

	for(MucTalkMessageSeq::iterator it = seq.begin(); it != seq.end(); ++it){
		MyUtil::Date t((*it)->timestamp);
		groupmsg_updatetime[(*it)->groupid] = t.toDateTime();
		if(it != seq.begin()){
			sql << ", ";
		}
		sql << "( " << (*it)->userid << ", " << mysqlpp::quote << (*it)->message << ", " << mysqlpp::quote << t.toDateTime() << ", " << (*it)->groupid << " )";
	}
	QueryRunner(DB_INSTANCE, CDbWServer, tablename).store(sql);

	tablename = "minigroup_chat_updatetime";
	for(map<int, string>::iterator it = groupmsg_updatetime.begin(); it != groupmsg_updatetime.end(); ++it){
		sql.clear();
		sql << "insert into " << tablename << " (minigroup_id, updatetime) values( " << it->first << ", " << mysqlpp::quote << it->second << " ) on duplicate key update updatetime = values(updatetime)";
		QueryRunner(DB_INSTANCE, CDbWServer, tablename).store(sql);
	}
}



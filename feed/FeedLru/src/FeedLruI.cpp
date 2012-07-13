/*
 * FeedQueueI.cpp
 *
 *  Created on: Feb 17, 2011
 *      Author: yejingwei
 */

#include "FeedLruI.h"
#include "LogWrapper.h"
#include "ServiceI.h"
#include "TaskManager.h"
#include "QueryRunner.h"
using namespace xce::feed;
using namespace std;
using namespace MyUtil;
using namespace com::xiaonei::xce;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&FeedLruI::instance(), service.createIdentity("M",
			""));
  ServantFactory* factory = new  MyFactory;
  service.registerObjectCache(0, "MF", factory);
}

void FeedLruI::Add(int key,
		const Ice::Current&){
  ServiceI::instance().locateObject<LruItemPtr> (0,
      key);
}
int FeedLruI::Get(int key,const Ice::Current&){
  LruItemPtr item = ServiceI::instance().findObject<LruItemPtr> (0,
      key);
  if(item){
  	return item->value;
  }else{
  	return -1;
  }
}
void FeedLruI::RunTask(int start,const Ice::Current &){
	MCE_INFO("FeedLruI::RunTask. start:" << start);
	MyUtil::TaskManager::instance().execute(new PrintTask(start));
}

void FeedLruI::Schedule(int delay,const Ice::Current &){
	MCE_INFO("FeedLruI::RunTask. delay:" << delay);
	MyUtil::TaskManager::instance().schedule(new MyTimerTask(delay));
	
}

string GetTable(int uid){
	ostringstream os;
	os << "feed_feature_" << uid % 1000;
	return os.str();
}
void FeedLruI::InsertDb(int uid,int feature,const Ice::Current &){
  MCE_INFO("FeedLruI::SelectDb. uid:" << uid << " feature:" << feature );
	Statement sql;
	sql << "INSERT INTO "<< GetTable(uid) <<" (toid,feature_id) value (" << uid << "," << feature << " )" ;
	QueryRunner("feed_feature_db_a",CDbWServer,GetTable(uid)).store(sql);
}

void FeedLruI::SelectDb(int uid,const Ice::Current &){
  MCE_INFO("FeedLruI::SelectDb. uid:" << uid );
	Statement sql;
	sql << "SELECT * from " << GetTable(uid) << " WHERE toid = " << uid;
  mysqlpp::StoreQueryResult res = QueryRunner("feed_feature_db_a", CDbRServer,
  		GetTable(uid)).store(sql);
  for (size_t i = 0; i < res.size(); ++i) {
    int to_id = (int) res.at(i)["to_id"];
    int feature_id = (int) res.at(i)["feature_id"];
    MCE_INFO("FeedLruI::SelectDb. to_id:" << to_id << " feature_id:" << feature_id);
  }
}

//CREATE TABLE feed_feature{
//to_id INT NOT NULL  //保存接受者id
//feature_id INT NOT NULL //保存from_id， stype值。现在from_id为正数，
//stype为负数
//click_count INT NOT NULL default 0 //点击数
//reply_count INT NOT NULL default 0　//回复数
//forward_count INT NOT NULL default 0　//转发数(暂时不用)
//
//update_time timestamp(8) NOT NULL
//
//unique (to_id,feature_id)
//}engine = innodb default charset = utf8;

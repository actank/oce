/*
 * FeedCreatorI.h
 *
 *  Created on: May 25, 2011
 *      Author: antonio
 */

#ifndef FEEDCREATORI_H_
#define FEEDCREATORI_H_

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>

#include "TaskManager.h"
#include "FeedCreator.h"
#include "Singleton.h"
#include "pugixml_v1.hpp"
namespace xce {
namespace feed {

typedef unsigned long long int uint64;

class FeedTypeConfig: public IceUtil::Shared {
public:

	int stype;
	int weight;
	int ptype;
	/*push_flag:
	1 var F_PUSH_NEWS = 0x01;
	2 var F_PUSH_MINI = 0x02;
	3 var F_PUSH_IM = 0x04;
	4 var F_PUSH_CLASS = 0x08;
	5 var F_PUSH_MINI_GROUP = 0x10;*/
	int push_flag;
	short news_merge_type;
	short mini_merge_type;
	bool is_update_time;
	bool is_custom_expr;
	bool is_persist_content;
	bool is_persist_feeddb;
	int expired_day;
	int daily_quota;
	/*string news_merge_by_;
	 string mini_merge_by_;
	 string source_by_;
	 string psource_by_;
	 string actor_by_;
	 string dispatch_expr_;
	 */
};
typedef IceUtil::Handle<FeedTypeConfig> FeedTypeConfigPtr;

class DataConfig: public IceUtil::Shared {
public:
	int stype;
	int version;
	int test_tpl_id;
	int using_tpl_id;
	int status; 		//0 = disabled, 1 = test, 2 = read only(可展示, 但不可发送), 3 = dispatching(可展示, 可发送)

	string news_merge_by;
	string mini_merge_by;
	string source_by;
	string psource_by;
	string actor_by;
	string togroup_by;
	string dispatch_expr;

	map<string,bool> keys;
};

typedef IceUtil::Handle<DataConfig> DataConfigPtr;

class FeedCreatorI: public MyUtil::Singleton<FeedCreatorI> {
public:
//	virtual bool Create(Ice::Int stype, Ice::Int version,
//			const MyUtil::Str2StrMap& props, const Ice::Current& = Ice::Current());
//	virtual bool CreateWithReply(Ice::Int stype, Ice::Int version,
//			const MyUtil::Str2StrMap& props, const FeedReplyPtr& reply,
//			const Ice::Current& = Ice::Current());
//
//	virtual void ReloadAllConfig(const Ice::Current& = Ice::Current());
//	virtual void ReloadConfig(Ice::Int stype, const Ice::Current& = Ice::Current());


	FeedTypeConfigPtr GetFeedTypeConfig(int stype);
	DataConfigPtr GetDataConfig(int stype, int version);
//	void UpdateFeedTypeConfig(int stype, const FeedTypeConfigPtr& config);
//	void UpdateDataConfig(int stype, int version, const FeedTypeConfigPtr& config);

	FeedTypeConfigPtr BuildFeedTypeConfig(mysqlpp::Row& row);
	DataConfigPtr BuildDataConfig(mysqlpp::Row& row);

	void LoadAllConfig();
	void LoadConfig(Ice::Int stype);



//	FeedSeedPtr BuildFeedSeed(int stype, int version, const MyUtil::Str2StrMap& props);
	bool DataCheck(const DataConfigPtr& config, const MyUtil::Str2StrMap & keys);

	string CreateExpr(const string& expr, const MyUtil::Str2StrMap& props );
	string Data2XML(int stype, int version, const MyUtil::Str2StrMap& props);

	map<string,bool> DataSchema2Map(const string& schema);
	void XML2Map(pugi::xml_node node,string key,bool optional, map<string,bool>& result);


	bool is_number(const string& str){
		for(string::const_iterator it = str.begin(); it != str.end(); ++it){
			if(*it < '0' ||  *it >'9'){
				return false;
			}
		}
		return true;
	}

private:
	IceUtil::Mutex mutex_;

	map<int,FeedTypeConfigPtr> feed_config_cache_;
	map<uint64,DataConfigPtr> data_config_cache_;

};

class ReloadAllConfigTask : public MyUtil::Task{
public:
	ReloadAllConfigTask():Task(0){};

	virtual void handle();
};

class ReloadConfigTask : public MyUtil::Task{
public:
	ReloadConfigTask(int stype):Task(0),stype_(stype){};

	virtual void handle();

private:
	int stype_;
};

}

}

#endif /* FEEDCREATORI_H_ */

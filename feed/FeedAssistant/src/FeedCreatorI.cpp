/*
 * FeedCreatorI.cpp
 *
 *  Created on: May 25, 2011
 *      Author: antonio
 */

#include <string>
#include <stdlib.h>
#include <boost/lexical_cast.hpp>

#include "pugixml_v1.hpp"
#include "ServiceI.h"
#include "QueryRunner.h"

#include "FeedCreatorI.h"
//#include "FeedAssistantAdapter.h"

using namespace std;
using namespace xce::feed;
using namespace MyUtil;
using namespace pugi;
using namespace com::xiaonei::xce;

//void MyUtil::initialize() {
//
//  ServiceI& service = ServiceI::instance();
//  FeedCreatorI& manager = FeedCreatorI::instance();
//
//  service.getAdapter()->add(&manager, service.createIdentity("M", ""));
//  manager.ReloadAllConfig();
//}


//bool FeedCreatorI::Create(Ice::Int stype, Ice::Int version,
//		const MyUtil::Str2StrMap& props, const Ice::Current&) {
//
//	return CreateWithReply(stype, version, props, 0);
//}
//
//bool FeedCreatorI::CreateWithReply(Ice::Int stype, Ice::Int version,
//		const MyUtil::Str2StrMap& props, const FeedReplyPtr& reply,
//		const Ice::Current&) {
//
//	FeedTypeConfigPtr feed_config = GetFeedTypeConfig(stype);
//	DataConfigPtr data_config = GetDataConfig(stype, version);
//
//	if (!(feed_config && data_config)) {
//		throw FeedTypeConfigNotFoundException();
//	}
//	if(!DataCheck(data_config, props)){
//		throw FeedTypeConfigNotFoundException();
//	}
//	if (data_config->status == 0 || data_config->status == 2) {
//		throw NotAllowedToSendException();
//	}
//
//	//build feedseed ----------------------------------------------------------
//
//	FeedSeedPtr seed = new FeedSeed();
//	seed->feed = 0;
//
//	Str2StrMap::const_iterator it = props.find(data_config->mini_merge_by);
//	seed->miniMerge = it!= props.end() ? boost::lexical_cast<Ice::Long>(it->second):0;
//
//	it = props.find(data_config->news_merge_by);
//	seed->newsMerge = it!= props.end() ? boost::lexical_cast<Ice::Long>(it->second):0;
//	it = props.find(data_config->source_by);
//	seed->source = it!= props.end() ? boost::lexical_cast<Ice::Long>(it->second):0;
//	it = props.find(data_config->psource_by);
//	seed->psource = it!= props.end() ? boost::lexical_cast<Ice::Long>(it->second):0;
//	seed->ptype = feed_config->ptype;
//	it = props.find(data_config->actor_by);
//	seed->actor = it!= props.end() ? boost::lexical_cast<Ice::Long>(it->second):0;
//
//	unsigned int t = 0;
//	t |= feed_config->news_merge_type;
//	t = t << 2;
//	t |= feed_config->mini_merge_type;
//	t = t << (2 + 2 + 2 + 8);
//	t |= version;
//	t = t << 16;
//	t |= stype;
//
//	seed->type = t;
//	seed->time = time(NULL);
//	seed->baseWeight = (feed_config->weight << 16);
//
//	//AppId=110992,base_weight_id=16,expr=f(106074071),publish_mini=0,publish_news=1,save_db=1,send_config=0,update_time=1,
//
//	seed->extraProps["base_weight_id"] = boost::lexical_cast<string>(
//			feed_config->weight);
//	if (feed_config->is_custom_expr) {
//		it = props.find("expr");
//		seed->extraProps["expr"] = it!= props.end() ? it->second:"";
//	} else {
//		seed->extraProps["expr"] = CreateExpr(data_config->dispatch_expr, props);
//	}
//	seed->extraProps["publish_news"] = feed_config->push_flag & 0x01 ? "1"
//			: "0";
//	seed->extraProps["publish_mini"] = feed_config->push_flag & 0x02 ? "1"
//			: "0";
//	seed->extraProps["push_flag"] = boost::lexical_cast<string>(
//			feed_config->push_flag);
//	//send_config 暂时不再使用了
//	seed->extraProps["save_db"] = feed_config->is_persist_content ? "1" : "0";
//	seed->extraProps["save_feeddb"] = feed_config->is_persist_feeddb ? "1"
//			: "0";
//	seed->extraProps["update_time"] = feed_config->is_update_time ? "1" : "0";
//
//	seed->xml = Data2XML(stype,version,props);
//	//-------------------------------------------------------------------------
//	//return true;
//
//	if (data_config->status == 1) {
//		//for test
//
//	} else if (data_config->status == 3) {
//		// for 正常
//		if (reply) {
//			FeedAssistantAdapter::instance().dispatchWithReply(seed, reply);
//		} else {
//			FeedAssistantAdapter::instance().dispatch(seed);
//		}
//	}
//	return true;
//}
//
////FeedSeedPtr FeedCreatorI::BuildFeedSeed(int stype, int version,
////		const MyUtil::Str2StrMap& props) {
////
////}
//
//
//void FeedCreatorI::ReloadAllConfig(const Ice::Current&){
//	TaskManager::instance().execute(new ReloadAllConfigTask());
//}
//void FeedCreatorI::ReloadConfig(int stype, const Ice::Current&){
//	TaskManager::instance().execute(new ReloadConfigTask(stype));
//}

void FeedCreatorI::LoadAllConfig() {
	MCE_INFO("load all config");
	map<int, FeedTypeConfigPtr> feed_config_cache;
	map<uint64, DataConfigPtr> data_config_cache;

	{
		ConnectionHolder conn("feed_admin_db", CDbRServer , "");
		mysqlpp::Query query = conn.query();

		query << "select * from feed_config";

		mysqlpp::UseQueryResult res = query.use();
		if (!res) {
			return;
		}
		MCE_INFO("line:" << __LINE__);
		while (mysqlpp::Row row = res.fetch_row()) {
		MCE_INFO("line:" << __LINE__);
			FeedTypeConfigPtr config = BuildFeedTypeConfig(row);
		MCE_INFO("line:" << __LINE__);
			feed_config_cache[config->stype] = config;
			MCE_INFO("db --> stype" << config->stype);
		}
	}

	{
		ConnectionHolder conn("feed_admin_db", CDbRServer , "");
		mysqlpp::Query query = conn.query();

		query << "select * from data_config";

		mysqlpp::UseQueryResult res = query.use();
		if (!res) {
			return;
		}
		while (mysqlpp::Row row = res.fetch_row()) {
			DataConfigPtr config = BuildDataConfig(row);
			uint64 k = config->stype;
			k = k << 32;
			k |= config->version;
			data_config_cache[k] = config;
		}
	}

	IceUtil::Mutex::Lock lock(mutex_);
	feed_config_cache_.swap(feed_config_cache);
	data_config_cache_.swap(data_config_cache);

}

void FeedCreatorI::LoadConfig(Ice::Int stype) {

	FeedTypeConfigPtr  feed_config;
	map<uint64, DataConfigPtr> data_config_cache;

	{
		ConnectionHolder conn("feed_admin_db", CDbRServer , "");
		mysqlpp::Query query = conn.query();

		query << "select * from feed_config where stype = " << stype;

		mysqlpp::UseQueryResult res = query.use();
		if (!res) {
			return;
		}
		while (mysqlpp::Row row = res.fetch_row()) {
			feed_config = BuildFeedTypeConfig(row);
		}
	}

	{
		ConnectionHolder conn("feed_admin_db", CDbRServer , "");
		mysqlpp::Query query = conn.query();

		query << "select * from data_config where stype = " << stype;

		mysqlpp::UseQueryResult res = query.use();
		if (!res) {
			return;
		}
		while (mysqlpp::Row row = res.fetch_row()) {
			DataConfigPtr config = BuildDataConfig(row);
			uint64 k = config->stype;
			k = k << 32;
			k |= config->version;
			data_config_cache[k] = config;
		}
	}

	IceUtil::Mutex::Lock lock(mutex_);
	if(feed_config){
		feed_config_cache_[feed_config->stype] = feed_config;
	}
	map<uint64, DataConfigPtr>::iterator  itlow, itup;

	uint64 k = stype;
	k = k<<32;
	itlow=data_config_cache_.lower_bound (k);
	itup=data_config_cache_.upper_bound (k|0xffffffff);
	data_config_cache_.erase(itlow,itup);
	data_config_cache_.insert(data_config_cache.begin(),data_config_cache.end());

}

string FeedCreatorI::CreateExpr(const string& expr,
		const MyUtil::Str2StrMap& props) {
	ostringstream os, key;
	bool is_find = false;
	MCE_INFO("FeedCreatorI::CreateExpr --> expr:"<< expr);
	for (string::const_iterator it = expr.begin(); it != expr.end(); ++it) {
		if (*it == '(') {
			os << *it;
			is_find = true;

		} else if (*it == ')') {
			Str2StrMap::const_iterator mit = props.find(key.str());
			if (mit != props.end()) {
				os << mit->second;
			} else {
				return "";
			}
			key.str("");
			os << *it;
			is_find = false;
		} else {
			if (is_find) {
				key << *it;
			} else {
				os << *it;
			}
		}
	}
	return os.str();
}

string FeedCreatorI::Data2XML(int stype, int version,
		const MyUtil::Str2StrMap& props) {
	xml_document doc;
	xml_node f_node = doc.append_child();
	f_node.set_name("f");

	string ver = boost::lexical_cast<string>(version);
	f_node.append_attribute("version").set_value(ver.c_str());
	f_node.append_attribute("tpl") = "cs";

	list<xml_node> loop_node;

	for (Str2StrMap::const_iterator it = props.begin(); it != props.end(); ++it) {
		xml_node n;
		ostringstream os;
		MCE_DEBUG("CS TPL KEY:" << it->first << " -> " << it->second);
		for (string::const_iterator sit = it->first.begin(); sit
				!= it->first.end(); ++sit) {
			if (*sit == '.') {
				if (n) {
					if (is_number(os.str())) {
						bool has_node = false;
						for (xml_node i_node = n; i_node; i_node
								= i_node.next_sibling(n.name())) {
							if (xml_attribute attr = i_node.attribute("loop")) {

								if (attr.value() ==  os.str()) {
									has_node = true;
									n = i_node;
									break;
								}
							} else {
								i_node.append_attribute("loop").set_value(
										os.str().c_str());
								n = i_node;
								has_node = true;
								loop_node.push_back(n);
								break;
							}
						}
						MCE_INFO("number:" << os.str());
						if (!has_node) {
							string name = n.name();
							n = n.parent().append_child();
							n.set_name(name.c_str());
							n.append_attribute("loop").set_value(os.str().c_str());

							loop_node.push_back(n);

						}

					} else {
						xml_node sub_child = n.child(os.str().c_str());
						if(sub_child){
							n = sub_child;
						}else{
							n = n.append_child();
							n.set_name(os.str().c_str());
						}
					}

				} else {
					xml_node sub_child = f_node.child(os.str().c_str());
					if(sub_child){
						n=sub_child;
					}else{
						n = f_node.append_child();
						n.set_name(os.str().c_str());
					}
				}
				os.str("");
			} else {
				os << *sit;
			}
		}
		if(n){
			if(os.str().empty() || it->second.empty()){
				continue;
			}
			if (!is_number(os.str())) {
			    xml_node sub_child = n.child(os.str().c_str());
			    if(sub_child){
				    n = sub_child;
			    }else{
				    n = n.append_child();
				    n.set_name(os.str().c_str());
			    }
				n.append_child(node_pcdata).set_value(it->second.c_str());
			}
			else {
				bool has_node = false;
				for (xml_node i_node = n; i_node; i_node = i_node.next_sibling(n.name())) {
					if (xml_attribute attr = i_node.attribute("loop")) {
					    if (attr.value() ==  os.str()) {
							i_node.append_child(node_pcdata).set_value(it->second.c_str());
							n = i_node;
							has_node=true;
							break;
						}
					} else {
						i_node.append_attribute("loop").set_value(os.str().c_str());
						i_node.append_child(node_pcdata).set_value(it->second.c_str());
						n = i_node;
						loop_node.push_back(n);
						has_node=true;
						break;
					}
				}
				MCE_INFO("number:" << os.str());
                                if (!has_node) {
                                	string name = n.name();
                                	n = n.parent().append_child();
                                	n.set_name(name.c_str());
                                        n.append_attribute("loop").set_value(os.str().c_str());
					n.append_child(node_pcdata).set_value(it->second.c_str());
                               		loop_node.push_back(n);
				}
			}
		}else{
			if(os.str().empty() || it->second.empty()){
                                continue;
                        }
			n = f_node.append_child();
                        n.set_name(os.str().c_str());
			n.append_child(node_pcdata).set_value(it->second.c_str());
		}
	}
	for (list<xml_node>::iterator it = loop_node.begin(); it != loop_node.end(); ++it) {
		it->remove_attribute("loop");
	}

	return doc.xml();

}

FeedTypeConfigPtr FeedCreatorI::GetFeedTypeConfig(int stype) {
	IceUtil::Mutex::Lock lock(mutex_);
	map<int, FeedTypeConfigPtr>::iterator it = feed_config_cache_.find(stype);
	if (it != feed_config_cache_.end()) {
		return it->second;
	}
	return 0;
}
DataConfigPtr FeedCreatorI::GetDataConfig(int stype, int version) {
	IceUtil::Mutex::Lock lock(mutex_);
	uint64 k = stype;
	k = k << 32;
	k |= version;
	map<uint64, DataConfigPtr>::iterator it = data_config_cache_.find(k);
	if (it != data_config_cache_.end()) {
		return it->second;
	}
	return 0;
}

//void FeedCreatorI::UpdateFeedTypeConfig(int stype, const FeedTypeConfigPtr& config) {
//
//
//
//}
//void FeedCreatorI::UpdateDataConfig(int stype, int version,
//		const FeedTypeConfigPtr& config) {
//
//}

FeedTypeConfigPtr FeedCreatorI::BuildFeedTypeConfig(mysqlpp::Row& row) {
	FeedTypeConfigPtr config = new FeedTypeConfig();

	try{
		config->stype = (int) row["stype"];
		config->weight = (int) row["weight"];
		config->ptype = (int) row["ptype"];
		config->push_flag = (int) row["push_flag"];
		config->news_merge_type = (short) row["news_merge_type"];
		config->mini_merge_type = (short) row["mini_merge_type"];
		config->is_update_time = (bool) row["is_update_time"];
		config->is_custom_expr = (bool) row["is_custom_expr"];
		config->is_persist_content = (bool) row["is_persist_content"];
		config->is_persist_feeddb = (bool) row["is_persist_feeddb"];
		config->expired_day = (int) row["lifetime"];
		config->daily_quota = (int) row["daily_quota"];
	}catch(std::exception & e){
		MCE_WARN("FeedCreatorI::BuildFeedTypeConfig --> read db err:"<<e.what()<<", file:" << __LINE__ );
	}
	return config;

}
DataConfigPtr FeedCreatorI::BuildDataConfig(mysqlpp::Row& row) {
	DataConfigPtr config = new DataConfig();
	try{
		config->stype = (int) row["stype"];
		config->version = (int) row["version"];
		config->test_tpl_id = (int) row["test_tpl_id"];
		config->using_tpl_id = (int) row["using_tpl_id"];
		config->status = (int) row["status"]; //0 = disabled, 1 = test, 2 = read only(可展示, 但不可发送), 3 = dispatching(可展示, 可发送)

		config->news_merge_by = row["news_merge_by"].c_str();
		config->mini_merge_by = row["mini_merge_by"].c_str();
		config->source_by = row["source_by"].c_str();
		config->psource_by = row["psource_by"].c_str();
		config->actor_by = row["actor_by"].c_str();
		config->togroup_by = row["togroup_by"].c_str();
		config->dispatch_expr = row["dispatch_expr"].c_str();
		config->keys = DataSchema2Map(row["keys_xml"].c_str());
		MCE_INFO("keys:" << row["keys_xml"].c_str());

		for(map<string,bool>::iterator it = config->keys.begin(); it != config->keys.end(); ++it){
			MCE_INFO("key ->" << it->first << ", optional:" << it->second);
		}
	}catch(std::exception & e){
		MCE_WARN("FeedCreatorI::BuildDataConfig --> read db err:"<<e.what()<<", file:" << __LINE__ );
	}

	return config;

	//		MyUtil::Str2StrMap keys;
}

map<string,bool> FeedCreatorI::DataSchema2Map(const string& schema){
	xml_document doc;
	doc.load(schema.c_str());
	xml_node node = doc.child("key");
	map<string,bool> result;

	for(node = node.first_child();node;node = node.next_sibling()){
		string key;
		XML2Map(node,key,false,result);
	}
	return result;
}
void FeedCreatorI::XML2Map(xml_node node, string key,bool optional, map<string,bool>& result){
	if(!node){
		result.insert(pair<string,bool>(key,optional));
		return;
	}
	if(key != ""){
		key += '.';
	}
	key += node.attribute("name").value();
	if(node.attribute("optional")){
		optional = true;
	}
	if(node.attribute("is_loop")){
		key += ".0";
	}
	node = node.first_child();
	if(!node){
		XML2Map(node,key,optional,result);
	}else{
		for(; node ; node=node.next_sibling()){
			XML2Map(node,key,optional,result);
		}
	}

}

bool FeedCreatorI::DataCheck(const DataConfigPtr & config, const Str2StrMap & keys){
	for(map<string,bool>::iterator it = config->keys.begin(); it != config->keys.end(); ++it){
		Str2StrMap::const_iterator kit = keys.find(it->first);
		if((kit == keys.end()) && (it->second == false)){
			MCE_WARN("FeedCreatorI::DataCheck --> key:" << it->first << " is not found");
			return false;
		}
	}
	return true;
}

void ReloadAllConfigTask::handle(){
	FeedCreatorI::instance().LoadAllConfig();
}

void ReloadConfigTask::handle(){
	FeedCreatorI::instance().LoadConfig(stype_);
}

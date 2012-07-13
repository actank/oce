#include "TalkCacheI.h"
#include "TalkCacheLoaderAdapter.h"
//#include "ConnectionQuery.h"
#include "QueryRunner.h"
#include "ReplaceStat.h"
#include <sstream>
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace com::xiaonei::talk::cache;
using namespace com::xiaonei::talk::cache::loader;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.registerObjectCache(0, "UI", new UserFactoryI);
	service.getAdapter()->add(new CacheManagerI, service.createIdentity("M", ""));
	ReplaceStat::instance().loadFaceTree();	
	TaskManager::instance().scheduleRepeated(&ReplaceStat::instance());
}


string StatLoader::getTableExpression(int id)
{
	ostringstream name;
	name<<"biz_doing_info_"<<id%100;
	return name.str();
}

string StatLoader::getTableName(int id)
{
	ostringstream name;
	name<<"doing_"<<id%100;
	return name.str();
}

string StatLoader::geDbInstance()
{
	return "biz_doing_info";
}


Ice::ObjectPtr UserFactoryI::create(Ice::Int id) {
	try {
		Statement sql;
		sql << "select * from user_names where id = " << id;
		mysqlpp::StoreQueryResult res = QueryRunner("user_names", CDbRServer).store(sql);
		if (!res || res.num_rows()<=0) {
			MCE_WARN("load name error id="<<id);
			return 0;
		}
		mysqlpp::Row row = res.at(0);
		UserPtr ui = new User;
		ui->id = id;
		ui->name = row["name"].c_str();
		
		sql.clear();
		sql << "select * from user_url where id=" << id;
		res = QueryRunner("user_url", CDbRServer).store(sql);
		if (!res || res.num_rows()<=0) {
			MCE_WARN("load name error id="<<id);
			return 0;
		}
		row = res.at(0);
		ui->headurl = row["headurl"].c_str();
		ui->tinyurl = row["tinyurl"].c_str();

		StatLoader::loadStat(ui);
		return ui;
	}catch(Ice::Exception& e) {
		MCE_WARN(e << " at " << __FILE__ << ":" << __LINE__);
	} catch (std::exception& e) {
		MCE_WARN(e.what() << " at " << __FILE__ << ":" << __LINE__);
	} catch (...) {
		MCE_WARN("Unkown exception at " << __FILE__ << ":" << __LINE__);
	}
	return 0;
}

ObjectResultPtr UserFactoryI::create(const IntSeq& keys) {
	MCE_DEBUG("call UserInfoCacheSet::create");
	ObjectResultPtr result = new ObjectResult;
	MCE_DEBUG("Keys.size()="<<keys.size());
	if(keys.empty()) {
		return 0;
	}
		
	Statement sql1;
	sql1 << "select id, name from user_names where id in (";
	for (size_t i = 0; i < keys.size(); ++i) {
		if (i) {
			sql1<<",";
		}
		sql1<< keys.at(i) ;
	}
	sql1<<")";
	Statement sql2;
	sql2 << "select id, headurl, tinyurl from user_url where id in (";
	for (size_t i = 0; i < keys.size(); ++i) {
		if (i) {
			sql2<<",";
		}
		sql2<< keys.at(i) ;
	}
	sql2<<")";

	
	map< int,IntSeq > keySet;
	try {
		mysqlpp::StoreQueryResult res = QueryRunner("user_names", CDbRServer).store(sql1);
		if (!res) {
			return 0;
		}
		for (size_t i = 0; i<res.num_rows(); ++i) {
			mysqlpp::Row row = res.at(i);
			UserPtr ui = new User;
			ui->id = (int)row["id"];//ptr->userId;
			ui->name = row["name"].c_str();//ptr->realName;
			//ui->headurl = row["HEADURL"].c_str();//ptr->link;
			result->data[ui->id] = ui;
			keySet[(ui->id%100)].push_back(ui->id);
		}
		res = QueryRunner("user_url", CDbRServer).store(sql2);
		for (size_t i = 0; i<res.num_rows(); ++i) {
			mysqlpp::Row row = res.at(i);
			int id = (int)row["id"];
			UserPtr ui = UserPtr::dynamicCast(result->data[id]);
			ui->headurl = row["headurl"].c_str();
			ui->tinyurl = row["tinyurl"].c_str();
		}
		for( map< int,IntSeq >::iterator iter = keySet.begin();
			 iter!=keySet.end();
			 ++iter){
			StatLoader::loadStat(result,iter->second);
		}
	} catch(Ice::Exception& e) {
		MCE_WARN(e << " at " << __FILE__ << ":" << __LINE__);
	} catch (std::exception& e) {
		MCE_WARN(e.what() << " at " << __FILE__ << ":" << __LINE__);
	} catch (...) {
		MCE_WARN("Unkown exception at " << __FILE__ << ":" << __LINE__);
	}

	MCE_DEBUG("Create res.size()="<<result->data.size());

	return result;
}


void StatLoader::loadStat( UserPtr& ui )
{
	Statement sql_doing;
	sql_doing<<"select content,type,link from "<< StatLoader::getTableName(ui->id)<<" where userid = "<<ui->id<<" order by dtime desc limit 1";
	
	mysqlpp::StoreQueryResult res =
		QueryRunner( StatLoader::geDbInstance(), CDbRServer, StatLoader::getTableExpression(ui->id)).store(sql_doing);
	if (!res) {
		MCE_WARN("load doing error id=" << ui->id);
		return;
	}
	if(res.num_rows()<=0){
		return;
	}
	mysqlpp::Row row = res.at(0);
	ui->statusOriginal = row["content"].c_str();
	ui->statusShifted = ui->statusOriginal;
	ReplaceStat::instance().replace(
		ui->id,(int)row["type"],ui->statusShifted,row["link"].c_str(),MAX_LENGTH);	
}


void StatLoader::loadStat(	ObjectResultPtr& result, const MyUtil::IntSeq& keys)
{
	if(keys.size() == 0){
		return;
	}
	Statement sql;
	sql << "select userid,content,type,link  from "<< StatLoader::getTableName(keys[0])<<" where userid in (";
	for (size_t i = 0; i < keys.size(); ++i) {
		if (i) {
			sql<<",";
		}
		sql<< keys.at(i) ;
	}
	sql<<")";
	mysqlpp::StoreQueryResult res =
		QueryRunner( StatLoader::geDbInstance(), CDbRServer, StatLoader::getTableExpression(keys[0])).store(sql);
	int userId;
	mysqlpp::Row row;
	if (!res) {
		return;
	}
	UserPtr ui;
	for (size_t i = 0; i<res.num_rows(); ++i) {
		row = res.at(i);
		userId = (int)row["userid"];
		ui = UserPtr::dynamicCast(result->data[userId]);
		ui->statusOriginal = row["content"].c_str();
		ui->statusShifted = ui->statusOriginal;
		ReplaceStat::instance().replace(
			userId,(int)row["type"],ui->statusShifted,row["link"].c_str(),MAX_LENGTH);
	}
}


void CacheManagerI::updateUser(Ice::Int id, const MyUtil::Str2StrMap& props, const Ice::Current&){
	//ServiceI::instance().removeObject(0, id);
	UserPtr u = ServiceI::instance().locateObject<UserPtr>(0,id);
	if(u){
		//MCE_DEBUG("CacheManagerI::updateUser --> has cache");
		UserPtr res = new User();
		res->id = u->id;
		res->name = u->name;
//		res->status = u->status;
		res->statusShifted = u->statusShifted;
		res->statusOriginal = u->statusOriginal;
		res->headurl = u->headurl;
		res->tinyurl = u->tinyurl;
		bool update = false;
		MyUtil::Str2StrMap::const_iterator nit = props.find("NAME");
		if(nit != props.end()){
			update = true;
			res->name = nit->second;
		}
		
		MyUtil::Str2StrMap::const_iterator dit = props.find("DOING");
		if(dit != props.end()){
			update = true;
			res->statusOriginal = dit->second;
			res->statusShifted = dit->second;
			ReplaceStat::instance().replace(res->id, TYPE_NORMAL, res->statusShifted, " ", MAX_LENGTH);	
			MCE_INFO("CacheManagerI::updateUser --> " << res->statusOriginal << " --- " << res->statusShifted);
		}

		MyUtil::Str2StrMap::const_iterator hit = props.find("HEADURL");
		if(hit != props.end()){
			update = true;
			res->headurl = hit->second;
		}
		if(update){
			ServiceI::instance().removeObject(0, id);
			ServiceI::instance().addObject(res, 0, id);
			try{
				TalkCacheLoaderAdapter::instance().updateUser(id, props);
			}
			catch(Ice::Exception& e){
				MCE_WARN("TalkCache::updateUser --> call TalkCacheLoader.updateUser ERR  line:"<<__LINE__<<" err:"<<e);
			}
			catch(std::exception& e){
				MCE_WARN("TalkCache::updateUser--> call TalkCacheLoader.updateUser ERR  line:"<<__LINE__<<" err:"<<e.what());
			}
		}
		//u = res;
	}
	MCE_DEBUG("CacheManagerI::updateUser --> userid:"<<id << "  end");
}

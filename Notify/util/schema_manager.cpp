#include "schema_manager.h"
#include <boost/lexical_cast.hpp>
#include "Notify/util/tpl_modifiers.h"
#include "ServiceI.h"
#include "QueryRunner.h"
#include "Notify/util/MergeConfig.h"
#include "PrintUtil.h"
using namespace xce::notify;
using namespace std;
using namespace com::xiaonei::xce;

bool ParseSchemaKeys(const string & keys, std::set<std::string> & key_set) {
	std::vector<std::string> strs;
	boost::split(strs, keys, boost::is_any_of("\t"), boost::token_compress_on);

	if (strs.size() % 2 != 0) {
		MCE_WARN("illegal schema key");
	}

	for (size_t i = 0; i < strs.size(); i += 2) {
		key_set.insert(strs[i]);
	}
	return true;
}

SchemaManager::SchemaManager() : _load_time(0) {
	static MaxLengthModifier mod_maxlen;
	ctemplate::AddModifier("x-maxlen=", &mod_maxlen);
	static AppTimeFormatModifier mod_apptime;
	ctemplate::AddModifier("x-apptime", &mod_apptime);

	reload();
}

bool SchemaManager::reload() {
  ServiceI& service = ServiceI::instance();
	Ice::PropertiesPtr props = service.getCommunicator()->getProperties();
	int load_hidden = props->getPropertyAsIntWithDefault("Service."
			+ service.getName() + ".LoadHidden", 0);

	// TODO : template表很大的时候，只加载最新更新的template
	SchemaMap schemas;
	set<int> merge_types;
	map<int, int> type2bigtype;
	map<int, vector<int> > bigtype2types;
	// 加载 data_schemas 表信息
	{
		Statement sql;
//		sql << "SELECT id, name, display, home_visible, key_list, type,merge FROM data_schemas";
    sql << "SELECT id, name, display, home_visible, key_list, type,merge,bigtype FROM data_schemas";
		MCE_DEBUG("schema load hidden : " << load_hidden);
		if (!load_hidden)
			sql << " WHERE display > 0";

		try {
			mysqlpp::StoreQueryResult res =
					QueryRunner("notify", CDbRServer).store(sql);
			if (!res) {
				MCE_WARN("select table data_schemas error.");
				return false;
			}

			MCE_DEBUG("load schema size : " << res.num_rows());
			for (size_t i = 0; i < res.num_rows(); ++i) {
				mysqlpp::Row row = res.at(i);

				int id = (int) row["id"];
				schemas[id].id = id;
				schemas[id].name = row["name"].c_str();
				schemas[id].display = (int)row["display"];
				schemas[id].is_reply = (int)row["home_visible"];
				schemas[id].bigtype= (int)row["bigtype"];
				ParseSchemaKeys(row["key_list"].c_str(), schemas[id].keys);

				int type = (int) row["type"];
				bool merge = (int)row["merge"];

				MCE_DEBUG("schema id ------>" << id << " bigtype " << schemas[id].bigtype << " type:" << type);
				type2bigtype[type] = schemas[id].bigtype;

				map<int,vector<int> >::iterator bit = bigtype2types.find(schemas[id].bigtype);
				if(bit == bigtype2types.end()){
					vector<int> v;
					bigtype2types.insert(make_pair<int, vector<int> >(schemas[id].bigtype, v));
				}

				bigtype2types[schemas[id].bigtype].push_back(type);

				if(merge){
				  merge_types.insert(type);
				}
			}
		} catch (std::exception& e) {
			MCE_WARN("load table data_schemas err : " << e.what());
			return false;
		} catch (...) {
			MCE_WARN("load table data_schemas unknown exception");
			return false;
		}
	}

	// 加载 template 表信息
	{
		Statement sql;
		sql
				<< "select id, schema_id, view, template, update_time from template";

		try {
			mysqlpp::StoreQueryResult res =
					QueryRunner("notify", CDbRServer).store(sql);
			if (!res) {
				MCE_WARN("select table 'template' error ");
				return false;
			}

			time_t update_time;
			MCE_DEBUG("load template size : " << res.num_rows());

			for (size_t i = 0; i < res.num_rows(); ++i) {
				mysqlpp::Row row = res.at(i);
				int schema_id = (int) row["schema_id"];
				if (schemas.find(schema_id) == schemas.end()) {
					MCE_DEBUG("inconsistent template item : schema id "
							<< schema_id << " doesn't exist.");
					continue;
				}
				int view = (int) row["view"];
				schemas[schema_id].templates[view]
						= ctemplate::Template::StringToTemplate(
								row["template"].c_str(), DO_NOT_STRIP);

        if(!ctemplate::Template::StringToTemplate(
            row["template"].c_str(), DO_NOT_STRIP)){
          MCE_INFO("NULL return. schema_id:"<<  schema_id << " view:" << view << " << content:" << row["template"]);
        }
				MCE_DEBUG("load template (" << schema_id << ',' << view
						<< ") = " << row["template"].c_str());

				update_time = (time_t) mysqlpp::DateTime(row["update_time"]);
				if (_load_time < update_time) {
					_load_time = update_time;
				}
			}
		} catch (std::exception& e) {
			MCE_WARN("load table template err : " << e.what());
			return false;
		} catch (...) {
			MCE_WARN("load table template unknown exception");
			return false;
		}
	}
	for(map<int, int>::iterator logit = type2bigtype.begin(); logit != type2bigtype.end(); ++logit){
		MCE_DEBUG("Reload schema--> type2bigtype info: type = " << logit->first << " bigtype = " << logit->second);	
	}
	{
		IceUtil::RWRecMutex::WLock lock(_mutex);
		_schema_map.swap(schemas);
		_type2bigtype.swap(type2bigtype);
		_bigtype2types.swap(bigtype2types);
		merge_types_.swap(merge_types);
	}

	MCE_INFO("Reload schema. merge_types size:" << merge_types_.size() << "," << PrintHelper::Print(merge_types_,10000));

	for (SchemaMap::iterator i1 = schemas.begin(); i1 != schemas.end(); ++i1) {
		map<int, ctemplate::Template*>::iterator i2 =
				i1->second.templates.begin(), end = i1->second.templates.end();
		for (; i2 != end; ++i2) {
			delete i2->second;
		}
	}
//	Dump();
	return true;
}

int SchemaManager::isReply(int schema_id) const {
	IceUtil::RWRecMutex::RLock lock(_mutex);
	SchemaMap::const_iterator i = _schema_map.find(schema_id);
	if(i != _schema_map.end())
	{
		return i->second.is_reply;
	}
	return 0;
}

vector<int> SchemaManager::GetTypesByBigtype(int bigtype) const{
	IceUtil::RWRecMutex::RLock lock(_mutex);
	map<int, vector<int> >::const_iterator it = _bigtype2types.find(bigtype);	
	if(it != _bigtype2types.end()){
		MCE_DEBUG("SchemaManager::GetTypesByBigtype--> bigtype:" << bigtype << " types.size():" << it->second.size());
		return it->second;
	}
	MCE_DEBUG("SchemaManager::--> GetTypesByBigtype --> bigtype:" << bigtype<< " empty..");
	return vector<int>();
}

int SchemaManager::GetBigtypeBytype(int type) const{
	IceUtil::RWRecMutex::RLock lock(_mutex);
	map<int, int>::const_iterator it = _type2bigtype.find(type);	
	if(it != _type2bigtype.end()){
		MCE_DEBUG("SchemaManager::GetBigtypeBytype--> type:" << type << it->second);
		return it->second;
	}
	MCE_DEBUG("SchemaManager::GetBigtypeBytype--> type:" << type << " -1");
	return -1;
}

bool SchemaManager::checkDispatch(const MyUtil::Str2StrMap& props) const {
	IceUtil::RWRecMutex::RLock lock(_mutex);
	MyUtil::Str2StrMap::const_iterator it_to = props.find("to");
	MyUtil::Str2StrMap::const_iterator it_tominigroup = props.find("tominigroup");
	if (it_to == props.end() && it_tominigroup == props.end()) {
		MCE_WARN("SchemaManager::checkDispatch() - no target IDs");
		throw KeyAbsentException("to and tominigroup");
	}

	MyUtil::Str2StrMap::const_iterator it_schema = props.find("schema_id");
	if (it_schema == props.end()) {
	  MCE_WARN("SchemaManager::checkDispatch() - no schema_id");
		throw KeyAbsentException("schema_id");
	}
	MyUtil::Str2StrMap::const_iterator it_type = props.find("type");
	if (it_type == props.end()) {
	    MCE_WARN("SchemaManager::checkDispatch() - no type");
	    throw KeyAbsentException("type");
	}

	int schemaid = 0;
	try {
		schemaid = boost::lexical_cast<int>(it_schema->second);
	} catch (boost::bad_lexical_cast & e) {
	  MCE_WARN("checkDispatch schema bad_lexical_cast : " << e.what());
		throw KeyAbsentException("schema_id");
	}

	SchemaMap::const_iterator i = _schema_map.find(schemaid);
	if (i == _schema_map.end()) {
    stringstream ss;
    ss << "no schema id : " << schemaid;
		MCE_INFO(ss.str());
		throw KeyAbsentException(ss.str());
	}

	set<string>::iterator k = i->second.keys.begin();
	for (; k != i->second.keys.end(); ++k) {
		if (props.find(*k) == props.end()) {
		  MCE_WARN("SchemaManager::checkDispatch() err : schema " << schemaid
					<< " no key " << *k);
			throw KeyAbsentException(*k);
		}
	}

	return true;
}
std::string SchemaManager::toReplyTxt(const NotifyBodyPtr& notify, int view, bool is_kaixin,map<Ice::Long, int> &fail_nids) const{
	ctemplate::TemplateDictionary dict("table");

	dict.ShowSection("REPLY_ONE");//确保就算是新模板也能处理

	int schemaid;
	for (map<string, string>::iterator i = notify->props.begin(); i
			!= notify->props.end(); ++i) {
	  dict.SetValue(i->first, i->second);
		//MCE_DEBUG("SchemaManager::toReplyTxt-->i.first:" << i->first << " i.second:" << i->second);
	}
	dict.SetIntValue("nid_list", notify->id);
	dict.SetIntValue("notify_id", notify->id);
	dict.SetIntValue("source", notify->source);
	dict.SetIntValue("owner", notify->owner);
	dict.SetIntValue("from", notify->from);
	dict.SetIntValue("type", ((notify->type) & 65535));
	dict.SetIntValue("time", notify->time);
	//merge count only 1;
	dict.SetIntValue("unreadcount", 1);

	try {
		schemaid = boost::lexical_cast<int>(notify->props["schema_id"]);
	} catch (boost::bad_lexical_cast & e) {
		MCE_WARN(" SchemaManager::toReplyTxt  --> error:" << e.what() << " nid:" << notify->id << " props[schema_id]:" << notify->props["schema_id"] << " notify->type:" << notify->type);
		fail_nids[notify->id] = ((notify->type) & 65535);
		return "";
	}

	dict.SetValue("domain", is_kaixin ? "kaixin" : "renren");

	IceUtil::RWRecMutex::RLock lock(_mutex);

	// TODO : 1) use find(). 2)leak prone?
	SchemaMap::const_iterator schema = _schema_map.find(schemaid);
	if (schema == _schema_map.end()) {
		MCE_INFO("no schema : schemaid = " << schemaid);
		fail_nids[notify->id] = ((notify->type) & 65535);
		return "";
	}

	map<int, ctemplate::Template*>::const_iterator tpl =
			schema->second.templates.find(view);

//	if(tpl == schema->second.templates.end() ){
//	  MCE_INFO("No view");
//	}
//	if(tpl->second == NULL){
//	  MCE_INFO("template NULL");
//	}


	if (tpl == schema->second.templates.end() || tpl->second == NULL) {
	//	MCE_INFO("no schema template view : schemaid = " << schemaid << " view = " << view);


//    ostringstream oss;
//    map<int, ctemplate::Template*> tpls = schema->second.templates;
//    oss << "(";
//    for(map<int, ctemplate::Template*>::iterator j = tpls.begin(); j != tpls.end();++j){
//      oss << j->first << ",";
//    }
//    oss << ")";
//    MCE_INFO(" views:" << oss.str());

		return "";
	}

	string notifyText;
	if(tpl->second)
	{
	  tpl->second->Expand(&notifyText, &dict);
	}
	else
	{
	  MCE_WARN("SchemaManager::toReplyTxt --> null template");
	}
	return notifyText;
}

string SchemaManager::toReplyTxtMerge(const vector<NotifyBodyPtr> & nbs, int view ,bool is_kaixin,map<Ice::Long, int> &fail_nids) const{
  ctemplate::TemplateDictionary dict("table");

  int schemaid;
  for (map<string, string>::iterator i = nbs.at(0)->props.begin(); i
      != nbs.at(0)->props.end(); ++i) {
    dict.SetValue(i->first, i->second);
  }

  size_t nbsize = nbs.size();
  MCE_DEBUG("SchemaManager::toReplyTxtMerge. toReplyTxtMerge nbsize:" << nbsize);

  //为uid去重
  vector<int> index;
  set<int> uniq_uids;
  for(size_t i = 0; i != nbs.size() && uniq_uids.size() <= 3; ++i){
    int uid = nbs.at(i)->from;
    if(!uniq_uids.count(uid)){
      uniq_uids.insert(uid);
      index.push_back(i);
    }
  }

  if(index.size() == 1){
    ctemplate::TemplateDictionary* sub_dict = dict.AddSectionDictionary("REPLY_ONE");
    sub_dict->SetValue("from_name", nbs.at(index.at(0))->props["from_name"]);
    sub_dict->SetIntValue("from", nbs.at(index.at(0))->from);
  }else {
    ctemplate::TemplateDictionary* sub_dict;
    if(index.size() == 2){
      sub_dict = dict.AddSectionDictionary("REPLY_TWO");
    }else{
      sub_dict = dict.AddSectionDictionary("REPLY_MORE");
    }
    sub_dict->SetValue("from_name1",nbs.at(index.at(0))->props["from_name"]);
    sub_dict->SetIntValue("from1", nbs.at(index.at(0))->from);
    sub_dict->SetValue("from_name2",nbs.at(index.at(1))->props["from_name"]);
    sub_dict->SetIntValue("from2", nbs.at(index.at(1))->from);
  }

  ostringstream oss;
  Ice::Long min_nid = nbs.at(0)->id;
  for(size_t i = 0 ; i != nbs.size(); ++i){
    min_nid = min_nid < nbs.at(i)->id ? min_nid : nbs.at(i)->id;
    oss << nbs.at(i)->id;
    if(i != nbs.size()-1){
      oss << "-";
    }
  }


  dict.SetValue("nid_list", oss.str());
  dict.SetIntValue("notify_id", min_nid);
  dict.SetIntValue("source", nbs.at(0)->source);
  dict.SetIntValue("owner", nbs.at(0)->owner);
  dict.SetIntValue("from", nbs.at(0)->from);//TODO 特殊
  dict.SetIntValue("type", ((nbs.at(0)->type) & 65535));
  dict.SetIntValue("time", nbs.at(0)->time);
	//merge count only 1;
	dict.SetIntValue("unreadcount", nbs.size());

  try {
    schemaid = boost::lexical_cast<int>(nbs.at(0)->props["schema_id"]);
  } catch (boost::bad_lexical_cast & e) {
    MCE_WARN(" SchemaManager::toReplyTxtMerge  --> error:" << e.what() << " nid:" << nbs.at(0)->id);
   // fail_nids.push_back(nbs.at(0)->id);
   	fail_nids[nbs.at(0)->id] = ((nbs.at(0)->type) & 65535);
    return "";
  }

  dict.SetValue("domain", is_kaixin ? "kaixin" : "renren");

  IceUtil::RWRecMutex::RLock lock(_mutex);

  // TODO : 1) use find(). 2)leak prone?
  SchemaMap::const_iterator schema = _schema_map.find(schemaid);
  if (schema == _schema_map.end()) {
    MCE_INFO("no schema : schemaid = " << schemaid);
    return "";
  }

  map<int, ctemplate::Template*>::const_iterator tpl =
      schema->second.templates.find(view);
  if (tpl == schema->second.templates.end() || tpl->second == NULL) {
    MCE_INFO("no schema template : schemaid = " << schemaid << " view = "
        << view);
    return "";
  }

  string notifyText;
  if(tpl->second)
  {
    tpl->second->Expand(&notifyText, &dict);
  }
  else
  {
    MCE_WARN("SchemaManager::toReplyTxtMerge --> null template");
  }
  return notifyText;
}

/*
string SchemaManager::toReplyTxtTest(const NotifyBodyPtr& notify, int view,
		bool is_kaixin,vector<Ice::Long> & fail_nids) const {
	ctemplate::TemplateDictionary dict("table");

	dict.ShowSection("REPLY_ONE");//确保就算是新模板也能处理

	int schemaid;
	for (map<string, string>::iterator i = notify->props.begin(); i
			!= notify->props.end(); ++i) {
	  dict.SetValue(i->first, i->second);
		//MCE_DEBUG("SchemaManager::toReplyTxt-->i.first:" << i->first << " i.second:" << i->second);
	}
	dict.SetIntValue("nid_list", notify->id);
	dict.SetIntValue("notify_id", notify->id);
	dict.SetIntValue("source", notify->source);
	dict.SetIntValue("owner", notify->owner);
	dict.SetIntValue("from", notify->from);
	dict.SetIntValue("type", ((notify->type) & 65535));
	dict.SetIntValue("time", notify->time);
	//merge count only 1;
	dict.SetIntValue("unreadcount", 1);

	try {
		schemaid = boost::lexical_cast<int>(notify->props["schema_id"]);
	} catch (boost::bad_lexical_cast & e) {
		MCE_WARN(" SchemaManager::toReplyTxt  --> error:" << e.what() << " nid:" << notify->id << " props[schema_id]:" << notify->props["schema_id"] << " notify->type:" << notify->type);
		fail_nids.push_back(notify->id);
		return "";
	}

	dict.SetValue("domain", is_kaixin ? "kaixin" : "renren");

	IceUtil::RWRecMutex::RLock lock(_mutex);

	// TODO : 1) use find(). 2)leak prone?
	SchemaMap::const_iterator schema = _schema_map.find(schemaid);
	if (schema == _schema_map.end()) {
		MCE_INFO("no schema : schemaid = " << schemaid);
		fail_nids.push_back(notify->id);
		return "";
	}

	map<int, ctemplate::Template*>::const_iterator tpl =
			schema->second.templates.find(view);

//	if(tpl == schema->second.templates.end() ){
//	  MCE_INFO("No view");
//	}
//	if(tpl->second == NULL){
//	  MCE_INFO("template NULL");
//	}


	if (tpl == schema->second.templates.end() || tpl->second == NULL) {
		if(0 == view){
			//如果某些类型的view=0没有模板，但是home_visable却设置成为了现实，那么首页取的时候，就会有问题，所以把这类有问题的index删除掉
			fail_nids.push_back(notify->id);
			MCE_INFO("no schema template view : schemaid = " << schemaid << " view = " << view);
		}

//    ostringstream oss;
//    map<int, ctemplate::Template*> tpls = schema->second.templates;
//    oss << "(";
//    for(map<int, ctemplate::Template*>::iterator j = tpls.begin(); j != tpls.end();++j){
//      oss << j->first << ",";
//    }
//    oss << ")";
//    MCE_INFO(" views:" << oss.str());

		return "";
	}

	string notifyText;
	if(tpl->second)
	{
	  tpl->second->Expand(&notifyText, &dict);
	}
	else
	{
	  MCE_WARN("SchemaManager::toReplyTxt --> null template");
	}
	return notifyText;
}
*/


bool SchemaManager::getIndexViewCheck(int view, const NotifyIndex& index){
	bool ans = true;
	IceUtil::RWRecMutex::RLock lock(_mutex);
	int schemaid = index.type;
	SchemaMap::const_iterator schema = _schema_map.find(schemaid);
	if (schema == _schema_map.end()) {
		MCE_INFO("SchemaManager::getIndexViewCheck --> no schema : schemaid(type) = " << schemaid);
		return ans;
	}
	map<int, ctemplate::Template*>::const_iterator tpl = schema->second.templates.find(view);

	if (tpl == schema->second.templates.end() || tpl->second == NULL) {
		ans = false;
	}
	MCE_DEBUG("SchemaManager::getIndexViewCheck --> index.id:" << index.id << " view=" << view << " ans:" << ans);
	return ans;
}



string SchemaManager::toReplyTxt(const NotifyBodyPtr& notify, int view,
		bool is_kaixin,vector<Ice::Long> & fail_nids) const {
	ctemplate::TemplateDictionary dict("table");

	dict.ShowSection("REPLY_ONE");//确保就算是新模板也能处理

	int schemaid;
	for (map<string, string>::iterator i = notify->props.begin(); i
			!= notify->props.end(); ++i) {
	  dict.SetValue(i->first, i->second);
		//MCE_DEBUG("SchemaManager::toReplyTxt-->i.first:" << i->first << " i.second:" << i->second);
	}
	dict.SetIntValue("nid_list", notify->id);
	dict.SetIntValue("notify_id", notify->id);
	dict.SetIntValue("source", notify->source);
	dict.SetIntValue("owner", notify->owner);
	dict.SetIntValue("from", notify->from);
	dict.SetIntValue("type", ((notify->type) & 65535));
	dict.SetIntValue("time", notify->time);
	//merge count only 1;
	dict.SetIntValue("unreadcount", 1);

	try {
		schemaid = boost::lexical_cast<int>(notify->props["schema_id"]);
	} catch (boost::bad_lexical_cast & e) {
		MCE_WARN(" SchemaManager::toReplyTxt  --> error:" << e.what() << " nid:" << notify->id << " props[schema_id]:" << notify->props["schema_id"] << " notify->type:" << notify->type);
		fail_nids.push_back(notify->id);
		return "";
	}

	dict.SetValue("domain", is_kaixin ? "kaixin" : "renren");

	IceUtil::RWRecMutex::RLock lock(_mutex);

	// TODO : 1) use find(). 2)leak prone?
	SchemaMap::const_iterator schema = _schema_map.find(schemaid);
	if (schema == _schema_map.end()) {
		MCE_INFO("no schema : schemaid = " << schemaid);
		fail_nids.push_back(notify->id);
		return "";
	}

	map<int, ctemplate::Template*>::const_iterator tpl =
			schema->second.templates.find(view);

//	if(tpl == schema->second.templates.end() ){
//	  MCE_INFO("No view");
//	}
//	if(tpl->second == NULL){
//	  MCE_INFO("template NULL");
//	}


	if (tpl == schema->second.templates.end() || tpl->second == NULL) {
		if(0 == view){
			//如果某些类型的view=0没有模板，但是home_visable却设置成为了现实，那么首页取的时候，就会有问题，所以把这类有问题的index删除掉
			fail_nids.push_back(notify->id);
			MCE_INFO("no schema template view : schemaid = " << schemaid << " view = " << view);
		}

//    ostringstream oss;
//    map<int, ctemplate::Template*> tpls = schema->second.templates;
//    oss << "(";
//    for(map<int, ctemplate::Template*>::iterator j = tpls.begin(); j != tpls.end();++j){
//      oss << j->first << ",";
//    }
//    oss << ")";
//    MCE_INFO(" views:" << oss.str());

		return "";
	}

	string notifyText;
	if(tpl->second)
	{
	  tpl->second->Expand(&notifyText, &dict);
	}
	else
	{
	  MCE_WARN("SchemaManager::toReplyTxt --> null template");
	}
	return notifyText;
}


string SchemaManager::toReplyTxtMerge(const vector<NotifyBodyPtr> & nbs, int view,bool is_kaixin,vector<Ice::Long> & fail_nids) const {

  ctemplate::TemplateDictionary dict("table");

  int schemaid;
  for (map<string, string>::iterator i = nbs.at(0)->props.begin(); i
      != nbs.at(0)->props.end(); ++i) {
    dict.SetValue(i->first, i->second);
  }

  size_t nbsize = nbs.size();
  MCE_DEBUG("SchemaManager::toReplyTxtMerge. toReplyTxtMerge nbsize:" << nbsize);

  //为uid去重
  vector<int> index;
  set<int> uniq_uids;
  for(size_t i = 0; i != nbs.size() && uniq_uids.size() <= 3; ++i){
    int uid = nbs.at(i)->from;
    if(!uniq_uids.count(uid)){
      uniq_uids.insert(uid);
      index.push_back(i);
    }
  }

  if(index.size() == 1){
    ctemplate::TemplateDictionary* sub_dict = dict.AddSectionDictionary("REPLY_ONE");
    sub_dict->SetValue("from_name", nbs.at(index.at(0))->props["from_name"]);
    sub_dict->SetIntValue("from", nbs.at(index.at(0))->from);
  }else {
    ctemplate::TemplateDictionary* sub_dict;
    if(index.size() == 2){
      sub_dict = dict.AddSectionDictionary("REPLY_TWO");
    }else{
      sub_dict = dict.AddSectionDictionary("REPLY_MORE");
    }
    sub_dict->SetValue("from_name1",nbs.at(index.at(0))->props["from_name"]);
    sub_dict->SetIntValue("from1", nbs.at(index.at(0))->from);
    sub_dict->SetValue("from_name2",nbs.at(index.at(1))->props["from_name"]);
    sub_dict->SetIntValue("from2", nbs.at(index.at(1))->from);
  }

  ostringstream oss;
  Ice::Long min_nid = nbs.at(0)->id;
  for(size_t i = 0 ; i != nbs.size(); ++i){
    min_nid = min_nid < nbs.at(i)->id ? min_nid : nbs.at(i)->id;
    oss << nbs.at(i)->id;
    if(i != nbs.size()-1){
      oss << "-";
    }
  }


  dict.SetValue("nid_list", oss.str());
  dict.SetIntValue("notify_id", min_nid);
  dict.SetIntValue("source", nbs.at(0)->source);
  dict.SetIntValue("owner", nbs.at(0)->owner);
  dict.SetIntValue("from", nbs.at(0)->from);//TODO 特殊
  dict.SetIntValue("type", ((nbs.at(0)->type) & 65535));
  dict.SetIntValue("time", nbs.at(0)->time);
	//merge count only 1;
	dict.SetIntValue("unreadcount", nbs.size());

  try {
    schemaid = boost::lexical_cast<int>(nbs.at(0)->props["schema_id"]);
  } catch (boost::bad_lexical_cast & e) {
    MCE_WARN(" SchemaManager::toReplyTxtMerge  --> error:" << e.what() << " nid:" << nbs.at(0)->id);
    fail_nids.push_back(nbs.at(0)->id);
    return "";
  }

  dict.SetValue("domain", is_kaixin ? "kaixin" : "renren");

  IceUtil::RWRecMutex::RLock lock(_mutex);

  // TODO : 1) use find(). 2)leak prone?
  SchemaMap::const_iterator schema = _schema_map.find(schemaid);
  if (schema == _schema_map.end()) {
    MCE_INFO("no schema : schemaid = " << schemaid);
    return "";
  }

  map<int, ctemplate::Template*>::const_iterator tpl =
      schema->second.templates.find(view);
  if (tpl == schema->second.templates.end() || tpl->second == NULL) {
    MCE_INFO("no schema template : schemaid = " << schemaid << " view = "
        << view);
    return "";
  }

  string notifyText;
  if(tpl->second)
  {
    tpl->second->Expand(&notifyText, &dict);
  }
  else
  {
    MCE_WARN("SchemaManager::toReplyTxtMerge --> null template");
  }
  return notifyText;
}


bool SchemaManager::MustMerge(int type,int view)const{
  if(view != 0 && view != 1 && view != 16){
    return false;
  }
  return merge_types_.count(type);
}

void SchemaManager::Dump(){
  for(SchemaMap::iterator i = _schema_map.begin(); i != _schema_map.end();++i){
    ostringstream oss;
    map<int, ctemplate::Template*> tpls = i->second.templates;
    oss << "(";
    for(map<int, ctemplate::Template*>::iterator j = tpls.begin(); j != tpls.end();++j){
      oss << j->first << ",";
    }
    oss << ")";
    MCE_INFO("SchemaManager::Dump. schema_id:" << i->first
        << " type:" << i->second.type << " id:" << i->second.id
        << " views:" << oss.str());
  }
  MCE_INFO("SchemaManager::Dump done");
}




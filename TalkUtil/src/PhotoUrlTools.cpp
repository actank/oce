#include "PhotoUrlTools.h"
#include "QueryRunner.h"
#include "ConnectionQuery.h"
#include <boost/lexical_cast.hpp>


using namespace com::xiaonei::talk::util;
using namespace com::xiaonei::xce;
using namespace MyUtil;
using namespace std;

void Group::AddNode(const NodePtr& node){//向group里面添加node
	_nodes.push_back(node);
}

NodePtr Group::GetReadAbleNode(int mod){//从一个group里面取node
	NodePtr ans;
	if(_nodes.empty()){
		return ans;
	}
	return _nodes.at(mod % _nodes.size());
}

void PhotoUrlHelper::StartReloadTimer(int time){
	Reload();
	TaskManager::instance().scheduleRepeated(new ReloadTimer(time));
}

void PhotoUrlHelper::Reload(){//每隔一段时间从db里面重新加载规则
	map<string, GroupPtr> tmp;
	Statement sql;
	sql<< "select * from " << TABLE_NAME << " where status = " << NODE_STATUS_READ << " or status = " << NODE_STATUS_READANDWRIGHT;//去db里面加载可读的，或者 即可读，又可写的node
	mysqlpp::StoreQueryResult res = QueryRunner(DB_INSTANCE, CDbRServer, TABLE_NAME).store(sql);
	if (!res || res.num_rows() <= 0) {
		MCE_WARN("PhotoUrlHelper::reload --> error no data in db");
	}
	for (size_t i = 0; i < res.num_rows(); ++i) {
		NodePtr node = new Node();
		mysqlpp::Row row = res.at(i);
		node->node_name = (string)row["node_name"];
		node->domain = (string)row["domain"];
		node->group_name = (string)row["group_name"];
		node->type = (int)row["type"];
		node->status = (int)row["status"];
		GroupPtr g;
		GroupMapIt it = tmp.find(node->group_name);
		if(it == tmp.end()){
			g = new Group();
			g->name = node->group_name;
			tmp.insert(make_pair<string, GroupPtr>(g->name, g));
		}
		else{
			g = it->second;
		}
		g->AddNode(node);//cache到内存里面
	}
	IceUtil::RWRecMutex::WLock lock(_rwmutex);
	_groupmap.swap(tmp);//swap
	MCE_INFO("PhotoUrlHelper::Reload --> after reload _groupmap.size=" << _groupmap.size());
}

GroupPtr PhotoUrlHelper::GetGroup(const string& group_name){//通过groupname 获取 group
	GroupPtr ans;
	GroupMapIt it = _groupmap.find(group_name);
	if(it != _groupmap.end()){
		ans = it->second;
	}
	return ans;
}


string PhotoUrlHelper::GetFullUrl(string& pattern){
	IceUtil::RWRecMutex::RLock lock(_rwmutex);
	GroupPtr group;
	size_t pos = pattern.find_first_of("/");//找到第一个 /
	if(pos != string::npos){
		group = GetGroup(pattern.substr(0, pos));//截取开始到 "/"  作为group_name
	}
	if(!group){//如果没找到相匹配的Group 做特殊处理 (来自UGC liuwei's java code)
		
		if(pattern.substr(0, 2) == "5q"){
			try{
				string flag = pattern.substr(3);
				string domain = "";
				if(flag.substr(0, 3) == "c02"){
					domain = UrlPhoto2Album;
				}
				else{
					domain = UrlPhotoAlbum;
				}
				string ans = domain + "/" + flag;
				MCE_INFO("PhotoUrlHelper::GetFullUrl --> 5q --> pattern = " << pattern << " ans = " << ans);
				return ans;
			}
			catch(...){
				MCE_WARN("PhotoUrlHelper::GetFullUrl --> get 5q error parrern = " << pattern );
				return "";
			}
		}
		else{
			if(pattern.substr(0, 6) == "pic001"){
				string ans = UrlPic001 + "/" + pattern;
				MCE_INFO("PhotoUrlHelper::GetFullUrl --> pic001 --> pattern = " << pattern << " ans = " << ans);
				return ans;
			}
			else if(pattern.substr(0, 6) == "pick002"){
				string ans = UrlPic002 + "/" + pattern;
				MCE_INFO("PhotoUrlHelper::GetFullUrl --> pic002 --> pattern = " << pattern << " ans = " << ans);
				return ans;
			}
			else{
				string ans = UrlImg + "/photos/" + pattern;
				try{
					string abc = pattern.substr(0, 8);
					int abc_int = boost::lexical_cast<int>(abc);
					if(abc_int >= 20070101){
						ans = UrlHead + "/photos/" + pattern;
					}
				}catch(...){
				}
				MCE_INFO("PhotoUrlHelper::GetFullUrl --> other --> pattern = " << pattern << " ans = " << ans);
				return ans;
			}
		}
		
		return "";
	}
	string filename = pattern.substr(pos + 1);//从第一个 "/" 后，截取到最后，作为图片文件名
	if(filename.empty()){
		return "";
	}
	NodePtr node = group->GetReadAbleNode(filename.size());//去找到一个可用的node
	if(!node){
		return "";
	}
	string servername = node->domain;
	ostringstream os;
	if(group->name.substr(0, 2) == "hd"){ //根据node 对应的 domain 拼出url 规则来自 UGC 的java代码
		os << "http://" << servername << "/photos/" << pattern;
	}else if(group->name.substr(0, 3) == "fmn"){
		os << "http://" << servername << "/" << pattern;
	}else{
		os << "http://" << servername << "/" << filename;
	}
	//MCE_INFO("PhotoUrlHelper::GetFullUrl --> pattern="<< pattern << " ans=" << os.str());
	return os.str();
}













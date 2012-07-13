#include "XmppTools.h"
#include <boost/lexical_cast.hpp>
#include "pugixml.hpp"

using namespace std;
using namespace com::xiaonei::talk::common;
using namespace pugi;
using namespace com::xiaonei::talk::muc;
using namespace com::xiaonei::talk::util;

string com::xiaonei::talk::util::createJid(int userid, Ice::Long index,
		const string& endpoint) {
	ostringstream str;
	str << userid<<"@"<<TALKDOMAIN;
	if (endpoint != "" && index >= 0) {
		str <<"/"<<endpoint<<"_"<<index;
	}
	return str.str();
}

JidPtr com::xiaonei::talk::util::stringToJid(const string& jid) {
	JidPtr ptr = new Jid();
	ptr->index = -1;
	string domain;
	ostringstream str;
	for (size_t i = 0; i <jid.length(); i++) {
		if (jid[i] == '@') {
			ptr->userId = boost::lexical_cast<int>(str.str());
			str.str("");
			continue;
		}
		if(jid[i] == '/')
		{
			domain = str.str();
			str.str("");
			continue;
		}
		if(jid[i] == '_')
		{
			ptr->endpoint = str.str();
			str.str("");
			continue;
		}
		str<<jid[i];
	}
	if(domain == "") {
		return ptr;
	}
	if(ptr->endpoint == "") {
		ptr->endpoint = str.str();
	} else {
		ptr->index = boost::lexical_cast<Ice::Long>(str.str());
	}
	return ptr;
};

SysJidPtr com::xiaonei::talk::util::stringToSysJid(const string& jid) {

	SysJidPtr sjid = new SysJid();

	size_t pos = jid.find('@');

	if (pos != string::npos) {
		sjid->node = jid.substr(0, pos);

		if (pos + 1 < jid.length()) {
			size_t pos1 = jid.find('.', pos+1);
			if (pos1 != string::npos) {
				sjid->category = jid.substr(pos+1, pos1-pos-1);

				return sjid;
			}
		}

	}

	return 0;
}

string com::xiaonei::talk::util::jidToString(const JidPtr& jid, bool hasIndex) {
	if(!jid){
		return "";
	}
	ostringstream str;
	str << jid->userId<<"@"<<TALKDOMAIN;
	if (jid->endpoint != "" && hasIndex) {
		str <<"/"<<jid->endpoint;

		if (jid->index >= 0)
			str << "_" <<jid->index;
	}
	return str.str();
}
;
bool com::xiaonei::talk::util::equal(const JidPtr& jid1, const JidPtr& jid2) {
	if ((!jid1) && (!jid2)) {
		return true;
	}
	if ((!jid1) || (!jid2)) {
		return false;
	}

	if (jid1->index == jid2->index && jid1->userId == jid2->userId
			&& jid1->endpoint == jid2->endpoint) {
		return true;
	}
	return false;
}

string com::xiaonei::talk::util::buildHeadUrl(const string& url, bool isXiaoNei) {
	if (url == "") {
		return "";
	}
	if (url.length() <4) {
		return "";
	}
	string name = url;
	bool isKaixin = false;
	if(url[0]=='k' && url[1]=='x' && url[2]=='/'){
		isKaixin = true;
		//url = url.substr(3);
		name = url.substr(3);
	}

	ostringstream os;
	ostringstream tmp;
	if ((name[0]=='h' && name[1] =='d' && name[2] =='n')) {
	//start with hdn
		os<<"http://hdn";
		for (size_t i = 3; i < name.length(); i++) {
			if (name[i] == '/') {
				int t = 0;
				try{
					t = boost::lexical_cast<int>(tmp.str());
				}catch(...){}
				if(t==121 || t==221 || t==321 || t==421 || t==521){
					os.str("");
					os << "http://hdn";
				}

				if (isKaixin) {
					os<<".rrimg.com/photos/"<<name;
				} else {
					os<<".xnimg.cn/photos/"<<name;
				}
				return os.str();
			}
			if (name[i]>= '0' && name[i]<='9'){
				os<< name[i];
				tmp<<name[i];
			}else{
				break;
			}
		}
	}else if(name[0]=='h' && name[1] =='d'){
		//start with hd
		os<<"http://hd";
		for (size_t i = 2; i < name.length(); i++) {
			if (name[i] == '/') {
				os<<".xiaonei.com/photos/"<<name;
				return os.str();
			}
			if (name[i]>= '0' && name[i]<='9'){
				os<< name[i];
			}else{
				break;
			}
		}
	}else if(name[0]=='k' && name[1] =='x' && name[2] =='h' && name[3] =='d' && name[4] =='n'){
		os<<"http://kxhdn";
		for (size_t i = 5; i < name.length(); i++) {
			if (name[i] == '/') {
				os<<".rrimg.com/photos/"<<name;
				return os.str();
			}
			if (name[i]>= '0' && name[i]<='9'){
				os<< name[i];
			}else{
				break;
			}
		}
	}

	os.str("");
	os<<"http://head.xiaonei.com/photos/"<<name;
	return os.str();

}
string com::xiaonei::talk::util::xmlEncode(const string& str) {

	xml_document doc;
	doc.append_child(node_pcdata).set_value(str.c_str());

	return doc.xml();

}

string com::xiaonei::talk::util::roomIdToString(const RoomIdPtr& rid) {
	ostringstream os;
	os << rid->name << "@" << rid->domain;
	return os.str();
}
string com::xiaonei::talk::util::roomJidToString(const RoomJidPtr& rjid) {
	ostringstream os;
	os << rjid->rid->name << "@" << rjid->rid->domain <<"/"<< rjid->nick;
	return os.str();
}
RoomIdPtr com::xiaonei::talk::util::stringToRoomId(const string& rid) {
	size_t index = rid.find_first_of('@');
	if (index == string::npos || index == rid.length()-1) {
		return 0;
	}

	RoomIdPtr res = new RoomId();
	res->name = rid.substr(0, index);
	res->domain = rid.substr(index+1, rid.length()- index);

	return res;
}
RoomJidPtr com::xiaonei::talk::util::stringToRoomJid(const string& rjid) {
	size_t index = rjid.find_first_of('@');
	if (index == string::npos) {
		return 0;
	}
	size_t index2 = rjid.find_first_of('/', index);
	if (index2 == string::npos || index2 == rjid.length()-1) {
		return 0;
	}

	RoomJidPtr res = new RoomJid();
	res->rid = new RoomId();
	res->rid->name = rjid.substr(0, index);
	res->rid->domain = rjid.substr(index+1, index2-index-1);
	res->nick = rjid.substr(index2+1, rjid.length()-index2);

	return res;
}

MucRoomIdPtr com::xiaonei::talk::util::stringToMucRoomId(const string& rid){
	size_t index = rid.find_first_of('@');
	if (index == string::npos || index == rid.length()-1) {
		return 0;
	}

	MucRoomIdPtr res = new MucRoomId();
	res->roomname = rid.substr(0, index);
	res->domain = rid.substr(index+1, rid.length()- index);

	return res;
}

string com::xiaonei::talk::util::mucRoomIdToString(const MucRoomIdPtr& rid){
	ostringstream os;
	os << rid->roomname << "@" << rid->domain;
	return os.str();
}

int com::xiaonei::talk::util::GetIdFromMucRoomId(const MucRoomIdPtr& rid){
	string roomname = rid->roomname;
	bool isnumber = false;
	ostringstream val;
	for(size_t i = 0; i < roomname.size() && (roomname[i] >= '0' && roomname[i] <= '9'); ++i){
		isnumber = true;
		val << roomname[i];
	}
	if(isnumber){
		try{
			int id = boost::lexical_cast<int>(val.str());
			return id;
		}
		catch (...){
			return 0;
		}
	}
	else{
		if(roomname.size() >= 1){
			int id = abs((int)roomname[0]);
			return id;
		}
	}
	return 0;
}



string com::xiaonei::talk::util::mucUserIdentityToString(const MucUserIdentityPtr& identity){
	ostringstream os;
	os << identity->roomid->roomname<< "@" << identity->roomid->domain <<"/"<< identity->nickname;
	return os.str();
}
MucUserIdentityPtr com::xiaonei::talk::util::stringToMucUserIdentity(const string& identity){
	size_t index = identity.find_first_of('@');
	if (index == string::npos) {
		return 0;
	}
	size_t index2 = identity.find_first_of('/', index);
	if (index2 == string::npos || index2 == identity.length()-1) {
		return 0;
	}
	MucUserIdentityPtr res = new MucUserIdentity();
	res->roomid = new MucRoomId();
	res->roomid->roomname = identity.substr(0, index);
	res->roomid->domain = identity.substr(index+1, index2-index-1);
	res->nickname = identity.substr(index2+1, identity.length()-index2);
	return res;
}

string com::xiaonei::talk::util::makeMucNickname(const JidPtr& jid, const string& name){
	string strid = boost::lexical_cast<string>(jid->userId);
	string strindex = boost::lexical_cast<string>(jid->index);
	return name + "(" + strindex + ")-" + strid;
}
bool com::xiaonei::talk::util::validRoomNickName(const string& rjid, int userid) {
	RoomJidPtr r = stringToRoomJid(rjid);
	if (!r) {
		return false;
	}

	if (r->nick[r->nick.length()-1] != ']') {
		return true;
	}

	size_t index = r->nick.find_last_of("[ID:");
	if (index == string::npos) {
		return true;
	}

	string uid = r->nick.substr(index + 4, r->nick.length() - index - 4);
	if (uid == boost::lexical_cast<string>(userid)) {
		return true;
	}

	return false;

}
bool com::xiaonei::talk::util::validRoomNickName(const RoomJidPtr& r, int userid) {
	if (!r) {
		return false;
	}

	if (r->nick[r->nick.length()-1] != ']') {
		return true;
	}

	size_t index = r->nick.find_last_of("[ID:");
	if (index == string::npos) {
		return true;
	}

	string uid = r->nick.substr(index + 4, r->nick.length() - index - 4);
	if (uid == boost::lexical_cast<string>(userid)) {
		return true;
	}

	return false;
}

com::xiaonei::talk::util::IdType com::xiaonei::talk::util::idType(
		const string& id) {

	string domain;
	size_t index = id.find_first_of('@');
	if (index == string::npos) {
		return UNDEFINE;
	}
	size_t index2 = id.find_first_of('/', index);
	if (index2 == string::npos) {
		domain = id.substr(index+1, id.length());
	} else {
		domain = id.substr(index+1, index2-index-1);
	}

	if (domain=="talk.xiaonei.com") {
		string tmp = id.substr(0, index);
		try {
			boost::lexical_cast<int>(tmp);
		} catch(...) {
			return UNDEFINE;
		}
		return JID;
	} else if (domain == "conference.talk.xiaonei.com" || domain
			== "group.talk.xiaonei.com") {
		if (index2 == string::npos) {
			return ROOMID;
		} else {
			return ROOMJID;
		}
	}
	return UNDEFINE;
}

OnlineStat com::xiaonei::talk::util::onlineStat(const string& show) {
	if (show == "") {
		return ONLINE;
	} else if (show == "away") {
		return AWAY;
	} else if (show == "dnd") {
		return BUSY;
	} else if (show == "xa") {
		return NOTAVAILABLE;
	} else if (show == "invisible") {
		return INVISIABLE;
	}
	return ONLINE;
}
;
string com::xiaonei::talk::util::onlineStatStr(OnlineStat stat) {
	if (stat == ONLINE) {
		return "";
	} else if (stat == AWAY) {
		return "away";
	} else if (stat == BUSY) {
		return "dnd";
	} else if (stat == NOTAVAILABLE) {
		return "xa";
	} else if (stat == INVISIABLE) {
		return "invisible";
	}
	return "";
}
;



void FunInfo::handle(){
	CounterMap tmp;
	{
		IceUtil::Mutex::Lock lock(mutex_);
		tmp.swap(counter_);
	}
	if(tmp.empty()){
		return;
	}
	for(CounterMap::iterator it = tmp.begin(); it != tmp.end(); ++it){
		ostringstream os;
		os << "FUNCTIONSTAT:funname-->" << it->first << "-->call_times-->" << it->second.call_times_ << "-->total_usetimes-->" << it->second.total_usetime_ << "-->timeout_times-->" << it->second.timeout_times_ << "-->ave_usetime-->";
		if(it->second.call_times_){
			os << (it->second.total_usetime_ / (it->second.call_times_ + 0.0)) << "-->timeout_rate-->" << ((it->second.timeout_times_+0.0) / (it->second.call_times_ + 0.0));
		}else{
			os << "0-->timeout_rate-->0";
		}
		MCE_INFO(os.str());
	}
}

void FunInfo::Stat(const string& funname, float usetime, bool istimeout){
	IceUtil::Mutex::Lock lock(mutex_);
	counter_[funname].call_times_++;
	counter_[funname].total_usetime_ += usetime;
	if(istimeout){
		counter_[funname].timeout_times_++;
	}
}

void FunStatManager::Init(){
	pool_.resize(FUNPOOLSIZE);
	for(int i = 0; i < FUNPOOLSIZE; ++i){
		pool_[i] = new FunInfo;
		TaskManager::instance().scheduleRepeated(pool_[i]);
	}
}

void FunStatManager::ProcessStat(const string& funname, float usetime, bool istimeout){
	int index = funname.size() % FUNPOOLSIZE;
	pool_[index]->Stat(funname, usetime, istimeout);
}

void FunStatManager::Stat(const string& funname, float usetime, bool istimeout){
	//int index = funname.size() % FUNPOOLSIZE;
	//pool_[index]->Stat(funname, usetime, istimeout);
	TaskManager::instance().execute(new FunStatTask(funname, usetime, istimeout));
}






















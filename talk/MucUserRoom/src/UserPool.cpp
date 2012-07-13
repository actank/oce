#include "UserPool.h"
#include "MucUserRoomManagerI.h"
#include <boost/lexical_cast.hpp>

#include "MucGateAdapter.h"
#include "MucUserRoomAdapter.h"
#include "MucOnlineCenterAdapter.h"
#include "MucTalkRoomAdapter.h"
using namespace com::xiaonei::talk::muc;
using namespace com::xiaonei::talk::common;
using namespace MyUtil;
using namespace std;


bool UserRoomInfo::AddRoomInfo(const MucRoomIdPtr& roomid, const MucMemberPtr& member){
	MCE_DEBUG("UserRoomInfo::AddRoomInfo --> roomid = " << roomid->roomname << " member = " << member->userid);
  RoomMemberMap::iterator it = _roommember.find(roomid);
  if(it == _roommember.end()){
    _roommember[roomid] = member;
    return true;
  }
  return false;
}
bool UserRoomInfo::ChangeRoomInfo(const MucRoomIdPtr& roomid, const MucMemberPtr& member){
	MCE_DEBUG("UserRoomInfo::ChangeRoomInfo --> change userid = " << _userid << " 's permision = " << member->permision);
  RoomMemberMap::iterator it = _roommember.find(roomid);
  if(it != _roommember.end()){
    _roommember[roomid] = member;
    return true;
  }
  return false;
}

bool UserRoomInfo::RemoveRoomInfo(const MucRoomIdPtr& roomid){
	MCE_DEBUG("UserRoomInfo::RemoveRoomInfo --> remove userid = " << _userid << " 's roomname = " << roomid->roomname);
  RoomMemberMap::iterator it = _roommember.find(roomid);
  if(it != _roommember.end()){
    _roommember.erase(it);
    return true;
  }
  return false;
}

MucMemberPtr UserRoomInfo::GetRoomPermision(const MucRoomIdPtr& roomid){
  MucMemberPtr ans;
  RoomMemberMap::iterator it = _roommember.find(roomid);
  if(it != _roommember.end()){
    ans = it->second;
  	MCE_DEBUG("UserRoomInfo::GetRoomPermision --> userid = " << _userid << " roomname = " << roomid->roomname << " SUCCESS");
  }
	else{
  	MCE_DEBUG("UserRoomInfo::GetRoomPermision --> userid = " << _userid << " roomname = " << roomid->roomname << " FAILED");
	}
  return ans;
}

MucRoomIdSeq UserRoomInfo::GetUserRooms(const string& domain){
  MCE_DEBUG("UserRoomInfo::GetUserRooms --> userid = " << _userid << " domain = " << domain);
  MucRoomIdSeq ans;
  RoomMemberMap::iterator it = _roommember.begin();
  for(; it != _roommember.end(); ++it){
		if(domain == it->first->domain){
    	ans.push_back(it->first);
		}
  }
  return ans;
}

MucRoomIdSeq UserRoomInfo::GetUserRooms(){
  MCE_DEBUG("UserRoomInfo::GetUserRooms --> userid = " << _userid);
  MucRoomIdSeq ans;
  RoomMemberMap::iterator it = _roommember.begin();
  for(; it != _roommember.end(); ++it){
    ans.push_back(it->first);
  }
  return ans;
}

MucRoomMemberSeq UserRoomInfo::GetRoomAndMember(){
  MCE_DEBUG("UserRoomInfo::GetRoomAndMember --> userid = " << _userid);
	MucRoomMemberSeq ans;
  RoomMemberMap::iterator it = _roommember.begin();
  for(; it != _roommember.end(); ++it){
		MucRoomMemberPtr p = new MucRoomMember();
		p->roomid = it->first;
		p->member = it->second;
    ans.push_back(p);
  }
	return ans;
}
//------------------------------------------------------------------------



string UserPool::GetDbTableName(int userid){
	ostringstream os;
	os << "member_minigroup_" << (userid % 100);
	MCE_DEBUG("UserPool::GetDbTableName --> TABLENAME = " << os.str());
	return os.str();
}
MucRoomIdPtr UserPool::MakeRoomId(int roomid, const string& groupname){
	ostringstream os;
	//change_name_request
	//os << roomid << "_" << groupname;
	os << roomid;
	string roomname = os.str();
	MucRoomIdPtr room = new MucRoomId();
	room->roomname = roomname;
	room->domain = "group.talk.xiaonei.com";
	return room;
}

MucMemberPtr UserPool::MakeMember(int userid, int p){
	ostringstream os;
	MucMemberPtr member = new MucMember();
	member->userid = userid;
	if(1 == p){
		p = 2;
	}
	else if(2 == p){
		p = 3;
	}
	else if(3 == p){
		p = 4;
	}
	else if(5 == p || 8 == p){
		return NULL;
	}
	else{
		MCE_WARN("UserPool::MakeMember --> unknow ugc permision = " << p << " we Ignore it");
		return NULL;
	}
	member->permision = Permisions(p);
	return member;
}

void UserPool::AddUserInfo(const UserRoomInfoPtr& userinfoptr){
	MCE_DEBUG("UserPool::AddUserInfo --> userid = " << userinfoptr->_userid);
  std::pair<UserRoomsCache::iterator, bool> p = _cache.push_front(userinfoptr);
  if(p.second){
    MCE_DEBUG("UserPool::AddUserInfo --> add roominfo sucess userid = " << userinfoptr->_userid);
  }
  if(!p.second){
		MCE_DEBUG("UserPool::AddUserInfo --> add roominfo err userid exist already");
    _cache.replace(p.first, userinfoptr);
    _cache.relocate(_cache.begin(), p.first);
  }else if(_cache.size() > USERSIZE){
    _cache.pop_back();
  }
}

UserRoomInfoPtr UserPool::SelectUserInfo(int userid){
  UserRoomInfoPtr ans = new UserRoomInfo(userid);
  Statement sql, sql_name;
	string tablename = GetDbTableName(userid);
  sql<< "select minigroup_id, access_id from " << tablename << " where member_id = " << userid;
  mysqlpp::StoreQueryResult res = QueryRunner(DB_INSTANCE, CDbRServer, tablename).store(sql);
  if (!res || res.num_rows() <= 0) {
		return ans;
	}
	for (size_t i = 0; i < res.num_rows(); ++i) {
		mysqlpp::Row row = res.at(i);
		int permision = (int)row["access_id"];
		MucMemberPtr member = MakeMember(userid, permision);
		if(!member){
			continue;
		}
		int rid = (int)row["minigroup_id"];
		sql_name.clear();
		string minigroup_table_name = "minigroup_" + boost::lexical_cast<string>(rid % 100);
		sql_name << "select name from " << minigroup_table_name << " where id = " << rid;
		mysqlpp::StoreQueryResult res_name = QueryRunner(DB_INSTANCE, CDbRServer, minigroup_table_name).store(sql_name);
		if(!res_name || res_name.num_rows() <= 0){
			MCE_DEBUG("UserPool::SelectUserInfo --> query name failed groupid = " << rid);
			continue;
		}
		string group_name = (string)res_name.at(0)["name"];
		MucRoomIdPtr roomid = MakeRoomId(rid, group_name);
		ans->AddRoomInfo(roomid, member);
	}
  return ans;
}

UserRoomInfoPtr UserPool::GetUserRoomInfo(int userid){
  MCE_DEBUG("UserPool::GetUserRoomInfo --> userid = " << userid);
	UserRoomInfoPtr ans;
	{
    IceUtil::Mutex::Lock lock(_cachemutex);
    UserIndex& uid_index = _cache.get<1>();
    UserIndex::iterator it = uid_index.find(userid);
    if(it != uid_index.end()){
      ans = (*it);
      SequencedIndex& seq_index = _cache.get<0>();
      _cache.relocate(seq_index.begin(), _cache.project<0>(it));
    }
	}
	if(!ans){
	  ans = SelectUserInfo(userid);
	  if(ans){
	    IceUtil::Mutex::Lock lock(_cachemutex);
	    UserIndex& uid_index = _cache.get<1>();
      UserIndex::iterator it = uid_index.find(userid);
      if(it == uid_index.end()){
        AddUserInfo(ans);
      }
      else{
        ans = (*it);
      }
	  }
	}
	return ans;
}

MucRoomIdSeq UserPool::GetUserRooms(int userid, const string& domain){
	MucRoomIdSeq ans;
	UserRoomInfoPtr umiptr = GetUserRoomInfo(userid);
	if(umiptr){
		IceUtil::RWRecMutex::RLock lock(_rwmutex);
		ans = umiptr->GetUserRooms(domain);
	}
  MCE_DEBUG("UserPool::GetUserRooms --> userid = " << userid << " domain = " << domain << " rooms.size = " << ans.size());
	for(MucRoomIdSeq::iterator logit = ans.begin(); logit != ans.end(); ++logit){
  	MCE_DEBUG("UserPool::GetUserRooms --> userid = " << userid << " domain = " << domain << " room is  = " << mucRoomIdToString((*logit)));
	}
	return ans;
}

MucRoomIdSeq UserPool::GetUserRooms(int userid){
	MucRoomIdSeq ans;
	UserRoomInfoPtr umiptr = GetUserRoomInfo(userid);
	if(umiptr){
		IceUtil::RWRecMutex::RLock lock(_rwmutex);
		ans = umiptr->GetUserRooms();
	}
  MCE_DEBUG("UserPool::GetUserRooms --> userid = " << userid << " rooms.size = " << ans.size());
	return ans;
}

bool UserPool::AddRoom(int userid, const MucRoomIdPtr& roomid, const MucMemberPtr& member){
  MCE_DEBUG("UserPool::AddRoom --> userid = " << userid << " roomname = " << roomid->roomname);
  bool ans = false;
  UserRoomInfoPtr umiptr = GetUserRoomInfo(userid);
  if(umiptr){
    IceUtil::RWRecMutex::WLock lock(_rwmutex);
    ans = umiptr->AddRoomInfo(roomid, member);
  }
  return ans;
}

bool UserPool::RemoveRoom(int userid, const MucRoomIdPtr& roomid){
  MCE_DEBUG("UserPool::RemoveRoom --> userid = " << userid << " roomname = " << roomid->roomname);
  bool ans = false;
  UserRoomInfoPtr umiptr = GetUserRoomInfo(userid);
  if(umiptr){
    IceUtil::RWRecMutex::WLock lock(_rwmutex);
    ans = umiptr->RemoveRoomInfo(roomid);
  }
  return ans;
}

MucMemberPtr UserPool::GetMemberPermision(int userid, const MucRoomIdPtr& roomid){
  MCE_DEBUG("UserPool::GetMemberPermision --> userid = " << userid << " roomname = " << roomid->roomname);
  MucMemberPtr ans;
  UserRoomInfoPtr umiptr = GetUserRoomInfo(userid);
  if(umiptr){
    IceUtil::RWRecMutex::RLock lock(_rwmutex);
    ans = umiptr->GetRoomPermision(roomid);
  }
  return ans;
}

MucRoomMemberSeq UserPool::GetUserRoomAndMember(int userid){
  MCE_DEBUG("UserPool::GetUserRoomAndMember --> userid = " << userid);
	MucRoomMemberSeq ans;
  UserRoomInfoPtr umiptr = GetUserRoomInfo(userid);
  if(umiptr){
    IceUtil::RWRecMutex::RLock lock(_rwmutex);
    ans = umiptr->GetRoomAndMember();
  }
	return ans;
}
void UserPool::SetUserPermision(int targetuser, const MucRoomIdPtr& roomid, Permisions permision){
  MCE_DEBUG("UserPool::SetUserPermision --> targetuser = " << targetuser << " roomname = " << roomid->roomname << " permision = " << permision );
  MucMemberPtr targetmember = GetMemberPermision(targetuser, roomid);
  if(targetmember){
    UserRoomInfoPtr umiptr = GetUserRoomInfo(targetuser);
    if(umiptr){
      MucMemberPtr newmember = new MucMember();
      newmember->userid = targetuser;
      newmember->permision = permision;
      IceUtil::RWRecMutex::WLock lock(_rwmutex);
      umiptr->ChangeRoomInfo(roomid, newmember);
    }
  }
}



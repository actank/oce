#include "MucTalkRoomManagerI.h"
#include "ServiceI.h"
#include "RoomPool.h"
#include <boost/lexical_cast.hpp>
#include "OnlineCenterAdapter.h"
#include "OnlineBitmapAdapter.h"
#include "PageCacheAdapter.h"
#include "MucGateAdapter.h"
#include "MucUserRoomAdapter.h"
#include "MucOnlineCenterAdapter.h"
#include "FeedMemcProxy/client/talk_cache_client.h"

using namespace xce::talk;
using namespace com::xiaonei::talk::muc;
using namespace com::xiaonei::talk::common;
using namespace MyUtil;
using namespace std;
using namespace talk::onlinebitmap;
using namespace talk::online::adapter;
using namespace talk::http;

bool RoomMemberList::AddMember(int userid){
	InsertRes ires = _members.insert(userid);
	if(!ires.second){
    MCE_WARN("RoomMemberList::AddMember--> already exist userid = " << userid);
	}
	MCE_DEBUG("RoomMemberList::AddMember --> add userid = " << userid << " OK");
  return ires.second;
}

IntSeq RoomMemberList::GetAllMembersSeq(){
  IntSeq ans;
  MemberSetIt it = _members.begin();
  for(; it != _members.end(); ++it){
    ans.push_back((*it));
  }
	MCE_DEBUG("RoomMemberList::GetAllMemberSeq --> ans.size =  "<<ans.size());
  return ans;
}


bool RoomMemberList::KickMember(int userid){
	MCE_DEBUG("RoomMemberList::KickMember --> kick userid = " << userid << " OK");
	return _members.erase(userid);
}


//------------------------------------------------------------------------
string RoomPool::GetDbTableName(int irid){
	ostringstream os;
	os << "minigroup_member_"<< (irid % 100);
	MCE_DEBUG("RoomPool::GetDbTableName --> tablename = " << os.str());
	return os.str();
}

void RoomPool::AddRoom(const RoomMemberListPtr& rmlptr){

  std::pair<RoomMemberCache::iterator, bool> p = _cache.push_front(rmlptr);
  if(p.second){
    MCE_DEBUG("RoomPool::AddRoom --> roomname = " << rmlptr->_roomname << " OK");
  }
  if(!p.second){
		MCE_DEBUG("RoomPool::AddRoom --> add err same roomid exist and replace and reloate");
    _cache.replace(p.first, rmlptr);
    _cache.relocate(_cache.begin(), p.first);
  }else if(_cache.size() > ROOMSIZE){
		MCE_DEBUG("RoomPool::AddRoom --> pop last");
    _cache.pop_back();
  }
}

RoomMemberListPtr RoomPool::SelectRoomMembers(const MucRoomIdPtr& roomid){
  int irid = GetIdFromMucRoomId(roomid);
	if(!irid){
		return NULL;
	}
  Statement sql;
  string tablename = GetDbTableName(irid);
  RoomMemberListPtr rmlptr = new RoomMemberList(roomid->roomname, irid);
  sql<< "select member_id from " << tablename << " where minigroup_id = " << irid;
  mysqlpp::StoreQueryResult res = QueryRunner(DB_INSTANCE, CDbRServer, tablename).store(sql);
  if (res && res.num_rows() > 0) {
    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      int userid = (int)row["member_id"];
      rmlptr->AddMember(userid);
    }
  }
	return rmlptr;
}

RoomMemberListPtr RoomPool::GetMemberList(const MucRoomIdPtr& roomid){
  RoomMemberListPtr ans;
  //int irid = GetRoomId(roomid->roomname);
  int irid = GetIdFromMucRoomId(roomid);
  {
    IceUtil::Mutex::Lock lock(_cachemutex);
    GroupIdIndex& id_index = _cache.get<1>();
    GroupIdIndex::iterator idit = id_index.find(irid);
    if(idit != id_index.end()){
      ans = (*idit);
      SequencedIndex& seq_index = _cache.get<0>();
      _cache.relocate(seq_index.begin(), _cache.project<0>(idit));
    }
  }
  if(!ans){
    ans = SelectRoomMembers(roomid);
    if(ans){
      IceUtil::Mutex::Lock lock(_cachemutex);
      GroupIdIndex& id_index = _cache.get<1>();
      GroupIdIndex::iterator idit = id_index.find(irid);
      if(idit == id_index.end()){
        AddRoom(ans);
      }
      else{
        ans = (*idit);
      }
    }else{
			MCE_DEBUG("RoomPool::GetMemberList --> Select ok is null");
		}
  }
	MCE_DEBUG("RoomPool::GetMemberList --> get roommlist roomname = " << roomid->roomname);
  return ans;
}







void RoomPool::CreateRoom(const MucRoomIdPtr& roomid){
  RoomMemberListPtr rmlptr = SelectRoomMembers(roomid);
  if(rmlptr){
    IceUtil::Mutex::Lock lock(_cachemutex);
    AddRoom(rmlptr);
    MCE_DEBUG("RoomPool::CreateRoom --> roomname = " << roomid->roomname << " OK");
  }
}

void RoomPool::RemoveRoom(const MucRoomIdPtr& roomid){
  IceUtil::Mutex::Lock lock(_cachemutex);
  GroupIdIndex& id_index = _cache.get<1>();
  //int irid = GetRoomId(roomid->roomname);
  int irid = GetIdFromMucRoomId(roomid);
  GroupIdIndex::iterator idit = id_index.find(irid);
  if(idit != id_index.end()){
		MCE_DEBUG("RoomPool::RemoveRoom --> remove success group_id = " << irid);
    id_index.erase(idit);
  }
  else{
    MCE_DEBUG("RoomPool::RemoveRoom --> remove err no group_id = " << irid);
  }
}


void RoomPool::AddRoomMember(int userid, const MucRoomIdPtr& roomid){
  RoomMemberListPtr rmlptr;
  bool addok = false;
  rmlptr = GetMemberList(roomid);
  if(rmlptr){
    IceUtil::RWRecMutex::WLock lock(_rwmutex);
    addok = rmlptr->AddMember(userid);
  }
  MCE_DEBUG("RoomPool::AddRoomMember --> userid = " << userid << " roomname = " << roomid->roomname << " addok = " << addok);
}

void RoomPool::RemoveRoomMember(int userid, const MucRoomIdPtr& roomid){

  RoomMemberListPtr rmlptr;
  bool removeok = false;
  rmlptr = GetMemberList(roomid);
  if(rmlptr){
    IceUtil::RWRecMutex::WLock lock(_rwmutex);
    removeok = rmlptr->KickMember(userid);
  }
  MCE_DEBUG("RoomPool::RemoveRoomMember --> userid = " << userid << " roomname = " << roomid->roomname << " removeok = " << removeok);
}

IntSeq RoomPool::GetRoomMember(const MucRoomIdPtr& roomid){

  RoomMemberListPtr rmlptr;
  IntSeq ans;
  rmlptr = GetMemberList(roomid);
  if(rmlptr){
    IceUtil::RWRecMutex::RLock lock(_rwmutex);
    ans = rmlptr->GetAllMembersSeq();
  }
  MCE_DEBUG("RoomPool::GetRoomMember --> roomname = " << roomid->roomname << " ANS.SIZE = " << ans.size());
  return ans;
}


MucActiveUserSeq RoomPool::GetRoomActiveUserSeq(const MucRoomIdPtr& roomid){
  MCE_DEBUG("RoomPool::GetRoomActiveUserSeq --> ROOMNAME = " << roomid->roomname);
  RoomMemberListPtr rmlptr;
  MucActiveUserSeq ans;

  MyUtil::IntSeq userids;
  rmlptr = GetMemberList(roomid);
  if(rmlptr){
    IceUtil::RWRecMutex::RLock lock(_rwmutex);
		userids = rmlptr->GetAllMembersSeq();
  }
  if(userids.empty()){
		MCE_DEBUG("RoomPool::GetRoomActiveUserSeq --> userids is empty so return");
    return ans;
  }
	JidSeqMap jseqmap;
	try{
	  MCE_DEBUG("RoomPool::GetRoomActiveUserSeq --> call OnlineCenterAdapter.getUsersJids userids.size = " << userids.size());
  	jseqmap = OnlineCenterAdapter::instance().getUsersJids(userids);
	}
	catch(Ice::Exception& e){
		MCE_WARN("RoomPool::GetRoomActiveUserSeq-->OnlineCenter::getUserJids-->line:"<<__LINE__<<" err:"<<e);
	}
	catch(std::exception& e){
		MCE_WARN("RoomPool::GetRoomActiveUserSeq--> call OnlineCenter.getUserJids ERR  line:"<<__LINE__<<" err:"<<e.what());
	}
  if(jseqmap.empty()){
    return ans;
  }
	TalkUserMap uinfoPtr;
	try{
		uinfoPtr = TalkCacheClient::instance().GetUserBySeqWithLoad(0, userids);
	}catch(Ice::Exception& e){
		MCE_WARN("RoomPool::GetRoomActiveUserSeq-->TalkCacheClient::GetUserBySeqWithLoad-->error:" << e);
	}
  MyUtil::IntSeq::iterator uidit = userids.begin();
  for(; uidit != userids.end(); ++uidit){
    int userid = (*uidit);
    JidSeqMap::const_iterator jit = jseqmap.find(userid);
    if(jit == jseqmap.end()){
      continue;
    }

    Permisions permision = PMember;
		try{
		  MCE_DEBUG("RoomPool::GetRoomActiveUserSeq --> CALL MucUserRoomAdapter.GetMemberPermision(" << userid << ", " << roomid->roomname << ")");
			MucMemberPtr m = MucUserRoomAdapter::instance().GetMemberPermision(userid, roomid);
			if(m){
				permision = m->permision;
			}
			else{
				MCE_DEBUG("RoomPool::GetRoomActiveUserSeq --> CALL MucUserRoomAdapter.GetMemberPermision's return is NULL, it means the user is not in the room so continue!");
				continue;
			}
		}
		catch(Ice::Exception& e){
			MCE_WARN("RoomPool::GetRoomActiveUserSeq-->MucUserRoom::GetMemberPermision-->line:"<<__LINE__<<" err:"<<e);
		}
		catch(std::exception& e){
			MCE_WARN("RoomPool::GetRoomActiveUserSeq--> call MucUserRoom.GetMemberPermision ERR  line:"<<__LINE__<<" err:"<<e.what());
		}

    string nickname = "";
    TalkUserMap::iterator infoit = uinfoPtr.find(userid);
    if(infoit == uinfoPtr.end()){
      //取不到用户的名称，默认用id 
      try{
        nickname = boost::lexical_cast<string>(userid);
      }
      catch (exception& e){
        MCE_WARN("RoomPool::GetRoomActiveUserSeq-->boost::lexical_cast to string exception  val = " << userid);
      }
    }
    else{
      TalkUserPtr u = infoit->second;
      nickname = u->name;
    }


    for (size_t index = 0; index < jit->second.size(); ++index) {
      char endpoint = jit->second.at(index)->endpoint[0];
      if(endpoint != 'T' && endpoint != 'W'){
        continue;
      }
      MucRoomIdPtr roomid = new MucRoomId();
      roomid->roomname = rmlptr->_roomname;
      roomid->domain = MUCDOMAIN;
      MucUserIdentityPtr identity = new MucUserIdentity();
      identity->roomid = roomid;
      identity->nickname = makeMucNickname(jit->second.at(index), nickname);
      identity->userid = userid;
      MucActiveUserPtr activeuserptr = new MucActiveUser();
      activeuserptr->jid = jit->second.at(index);
      activeuserptr->identity = identity;
      activeuserptr->permision = permision;
      ans.push_back(activeuserptr);
    }
  }
  MCE_DEBUG("RoomPool::GetRoomActiveUserSeq --> roomname = " << roomid->roomname << " ans.size = " << ans.size());
  return ans;
}
void RoomPool::PageNotifyOnline(int onlineuser, const MucRoomIdPtr& roomid){
	/*MCE_DEBUG("RoomPool::PageNotifyOnline --> onlineuser = " << onlineuser << " roomid = " << mucRoomIdToString(roomid));
	IntSeq members;
	members = GetRoomMember(roomid);
	if(!members.empty()){
		for(IntSeq::iterator mit = members.begin(); mit != members.end(); ++mit){
			PageCacheAdapter::instance().MemberOnline((*mit), GetRoomId(roomid->roomname), onlineuser);
		}
	}
	*/
}
void RoomPool::PageNotifyOffline(int offlineuser, const MucRoomIdPtr& roomid){
	/*
	MCE_DEBUG("RoomPool::PageNotifyOffline --> offlineuser = " << offlineuser << " roomid = " << mucRoomIdToString(roomid));
	IntSeq members;
	members = GetRoomMember(roomid);
	if(!members.empty()){
		for(IntSeq::iterator mit = members.begin(); mit != members.end(); ++mit){
			PageCacheAdapter::instance().MemberOffline((*mit), GetRoomId(roomid->roomname), offlineuser);
		}
	}
	*/
}

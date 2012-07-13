#include "MucUserRoomManagerI.h"
#include "UserDangerAdapter.h"
#include "ServiceI.h"
#include <boost/lexical_cast.hpp>
#include "OnlineBitmapAdapter.h"
#include "OnlineCenterAdapter.h"
#include "MessageMsgHandlerAdapter.h"
#include "XmppTools.h"
#include "MucGateAdapter.h"
#include "MucUserRoomAdapter.h"
#include "MucOnlineCenterAdapter.h"
#include "MucTalkRoomAdapter.h"
#include "PageCacheAdapter.h"
#include "FeedMemcProxy/client/talk_cache_client.h"
using namespace MyUtil;
using namespace xce::userdanger;
using namespace xce::talk;
using namespace com::xiaonei::talk::muc;
using namespace com::xiaonei::talk::util;
using namespace com::xiaonei::talk::common;
using namespace talk::online;
using namespace talk::adapter;
using namespace talk::online::adapter;
using namespace talk::onlinebitmap;
using namespace MyUtil;
using namespace std;
using namespace talk::http;

void MyUtil::initialize(){
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&MucUserRoomManagerI::instance(), service.createIdentity("M", ""));
	MucUserRoomManagerI::instance().Init();
}



void MucUserRoomManagerI::Init(){
	for(size_t i = 0; i < POOLSIZE; ++i){
		UserPoolPtr userpool = new UserPool();
		_pool.push_back(userpool);
	}
}


UserPoolPtr MucUserRoomManagerI::GetPool(int userid){
	UserPoolPtr ans;
	if(_pool[userid % POOLSIZE]){
		return _pool[userid % POOLSIZE];
	}
	return ans;
}


MucMemberPtr MucUserRoomManagerI::GetUserRoomPermision(int userid, const MucRoomIdPtr& roomid){
	MucMemberPtr ans;
	UserPoolPtr userpool = GetPool(userid);
	if(userpool){
		ans = userpool->GetMemberPermision(userid, roomid);
	}
	return ans;
}
MucMemberMap MucUserRoomManagerI::GetUserRoomPermision(const MyUtil::IntSeq& userids, const MucRoomIdPtr& roomid){
	MucMemberMap ans;
	for(MyUtil::IntSeq::const_iterator it = userids.begin(); it != userids.end(); ++it){
		MucMemberPtr p = GetUserRoomPermision((*it), roomid);
		if(p){
			ans[(*it)] = p;
		}
	}
	return ans;
}


MucRoomIdSeq MucUserRoomManagerI::GetUserRooms(int userid, const string& domain, const Ice::Current& current){
	TimeStat ts;
	MucRoomIdSeq ans;
	UserPoolPtr userpool = GetPool(userid);
	if(userpool){
		ans = userpool->GetUserRooms(userid, domain);
	}
	MCE_INFO("interface GetUserRooms usetime : " << ts.getTime());
	return ans;
}

bool MucUserRoomManagerI::AddRoom(int userid, const MucRoomIdPtr& roomid, const MucMemberPtr& member, const Ice::Current& current){
	TimeStat ts;
  bool ans = false;
	UserPoolPtr userpool = GetPool(userid);
	if(userpool){
	  ans = userpool->AddRoom(userid, roomid, member);
	}
	MCE_INFO("interface AddRoom usetime : " << ts.getTime());
}
bool MucUserRoomManagerI::RemoveRoom(int userid, const MucRoomIdPtr& roomid, const Ice::Current& current){
	TimeStat ts;
  bool ans = false;
	UserPoolPtr userpool = GetPool(userid);
	if(userpool){
	  ans = userpool->RemoveRoom(userid, roomid);
	}
	MCE_INFO("interface RemoveRoom usetime : " << ts.getTime());
}

MucMemberPtr MucUserRoomManagerI::GetMemberPermision(int userid, const MucRoomIdPtr& roomid, const Ice::Current& current){
	TimeStat ts;
	MucMemberPtr ans;
	ans = GetUserRoomPermision(userid, roomid);
	MCE_INFO("interface GetMemberPermision usetime : " << ts.getTime());
	return ans;
}


MucMemberMap MucUserRoomManagerI::GetMemberPermisionBySeq(const MyUtil::IntSeq& userids, const MucRoomIdPtr& roomid, const Ice::Current& current){
	TimeStat ts;
	MucMemberMap ans;
	ans = GetUserRoomPermision(userids, roomid);
	MCE_INFO("interface GetMemberPermisionBySeq usetime : " << ts.getTime());
	return ans;
}


MucRoomMemberSeq MucUserRoomManagerI::GetUserRoomAndMember(int userid, const Ice::Current& current){
	TimeStat ts;
	MucRoomMemberSeq ans;
	UserPoolPtr userpool = GetPool(userid);
	if(userpool){
	  ans = userpool->GetUserRoomAndMember(userid);
	}
	MCE_INFO("interface GetUserRoomAndMember usetime : " << ts.getTime());
	return ans;
}

void MucUserRoomManagerI::SetUserPermision(int targetuser, const MucRoomIdPtr& roomid, Permisions permision, const Ice::Current& current){
	TimeStat ts;
	UserPoolPtr userpool = GetPool(targetuser);
	if(userpool){
	  userpool->SetUserPermision(targetuser, roomid, permision);
	}
	MCE_INFO("interface SetUserPermision usetime : " << ts.getTime());
}


//------------------------------------------PUSH-----------------------------------------------------

void MucUserRoomManagerI::ProcessOnline(const JidPtr& onlinejid){
	MCE_DEBUG("MucUserRoomManagerI::ProcessOnline --> onlinejid = " << jidToString(onlinejid));
  MucRoomMemberSeq roomandmember;
  int userid = onlinejid->userId;
		
	bool userdanger = false;
	try{
		userdanger = UserDangerAdapter::instance().isMiniGroupHide(userid);
	}catch(Ice::Exception& e){
		MCE_WARN("MucUserRoomManagerI::UserOnline-->UserDangerAdapter::isMiniGroupHide-->line:"<<__LINE__<<" err:"<<e);
	}
	if(userdanger){
		MCE_INFO("MucUserRoomManagerI::UserOnline --> user : " << userid << " is danger user so do not enter any room");
		return;
	}
	
  UserPoolPtr userpool = GetPool(userid);
  if(!userpool){
    return;
  }
  roomandmember = userpool->GetUserRoomAndMember(userid);
  if(roomandmember.empty()){
    return;
  }

  string username = "";
  try{
    TalkUserPtr userptr = TalkCacheClient::instance().GetUserByIdWithLoad(userid);
		if(userptr){
    	username = userptr->name;
		}
  }
  catch(Ice::Exception& e){
    MCE_WARN("MucUserRoomManagerI::UserOnline --> call TalkCacheClient.GetUserWithLoad ERR  line:"<<__LINE__<<" err:"<<e);
  }
  catch(std::exception& e){
    MCE_WARN("MucUserRoomManagerI::UserOnline --> call TalkCacheClient.GetUserWithLoad ERR  line:"<<__LINE__<<" err:"<<e.what());
  }
  string nickname = "";
  if(username.empty()){
    nickname = boost::lexical_cast<string>(userid);
  }
  nickname = makeMucNickname(onlinejid, username);

  MucRoomMemberSeq::iterator rmit = roomandmember.begin();
  for(; rmit != roomandmember.end(); ++rmit){
		if((onlinejid->endpoint[0] != 'T') && (TEMPORARY == (*rmit)->roomid->domain)){
			MCE_DEBUG("MucUserRoomManagerI::UserOnline --> web user online, do not push it into temporary room");
			continue;
		}
    MucUserIdentityPtr identity = new MucUserIdentity();
    identity->roomid = (*rmit)->roomid;
    identity->nickname = nickname;
		identity->userid = onlinejid->userId;
    Permisions permision = (*rmit)->member->permision;
    MucActiveUserPtr p = new MucActiveUser();
    p->identity = identity;
    p->jid = onlinejid;
    p->permision = permision;
    try{
			MCE_DEBUG("MucUserRoomManagerI::UserOnline --> CALL MucOnlineCenter.UserOnline activeuser is : jid = " << jidToString(p->jid) << " roomname = " << p->identity->roomid->roomname << " permision = " << p->permision);
      MucOnlineCenterAdapter::instance().UserOnline(p);
    }
    catch(Ice::Exception& e){
      MCE_WARN("MucUserRoomManagerI::UserOnline-->MucOnlineCenter::UserOnline-->line:"<<__LINE__<<" err:"<<e);
    }
    catch(std::exception& e){
      MCE_WARN("MucUserRoomManagerI::UserOnline --> call MucOnlineCenter.UserOnline ERR  line:"<<__LINE__<<" err:"<<e.what());
    }
  }
}

void MucUserRoomManagerI::UserOnline(const JidPtr& onlinejid, const Ice::Current& current){
	TimeStat ts;
  ProcessOnline(onlinejid);
	MCE_INFO("interface UserOnline usetime : " << ts.getTime());
}


void MucUserRoomManagerI::UserOffline(const JidPtr& offlinejid, const Ice::Current& current){
	TimeStat ts;
	MCE_DEBUG("MucUserRoomManagerI::UserOffline --> jid = " << jidToString(offlinejid) );
  MucRoomIdSeq roomseq;
  MucRoomIdSeq temorary_roomseq;

  int userid = offlinejid->userId;
  UserPoolPtr userpool = GetPool(userid);
  if(!userpool){
    return;
  }
  roomseq = userpool->GetUserRooms(userid);
  if(roomseq.empty()){
    return;
  }
	MucRoomIdSeq::iterator rit = roomseq.begin();
	for(; rit != roomseq.end(); ++rit){
	  if(TEMPORARY == (*rit)->domain){
	    temorary_roomseq.push_back((*rit));
	  }
		/*
		else{
			//notify PageCache for group.talk.xiaonei.com
			try{
				MCE_DEBUG("MucUserRoomManagerI::UserOffline --> call PageCache.MemberOffline userid = " << offlinejid->userId << " roomid = " << mucRoomIdToString((*rit)));
    		PageCacheAdapter::instance().MemberOffline(GetIdFromMucRoomId(*rit), offlinejid->userId);
  		}
  		catch(Ice::Exception& e){
    		MCE_WARN("MucUserRoomManagerI::UserOffline --> call PageCache.MemberOffline ERR  line:"<<__LINE__<<" err:"<<e);
  		}
  		catch(std::exception& e){
    		MCE_WARN("MucUserRoomManagerI::UserOffline --> call PageCache.MemberOffline ERR  line:"<<__LINE__<<" err:"<<e.what());
  		}
		}
		*/

  	try{
			MCE_DEBUG("MucUserRoomManagerI::UserOffline --> CALL MucOnlineCenter.AwayRoom (" << jidToString(offlinejid) << ", " << (*rit)->roomname  << ")");
    	MucOnlineCenterAdapter::instance().AwayRoom(offlinejid, (*rit));
  	}
  	catch(Ice::Exception& e){
    	MCE_WARN("MucUserRoomManagerI::UserOffline-->MucOnlineCenter::UserOffline-->line:"<<__LINE__<<" err:"<<e);
  	}
  	catch(std::exception& e){
    	MCE_WARN("MucUserRoomManagerI::UserOffline --> call MucOnlineCenter.UserOffline ERR  line:"<<__LINE__<<" err:"<<e.what());
  	}
	}



	if(!temorary_roomseq.empty()){
		/*
		JidSeq imjids;
	  try{
			MCE_DEBUG("MucUserRoomManagerI::UserOnline --> CALL OnlineCenter.getUserJids userid = " << offlinejid->userId);
      imjids = OnlineCenterAdapter::instance().getUserJids(offlinejid->userId, 4);
    }
    catch(Ice::Exception& e){
      MCE_WARN("MucUserRoomManagerI::UserOnline --> call OnlineCenter.getUserJids ERR  line:"<<__LINE__<<" err:"<<e);
    }
    catch(std::exception& e){
      MCE_WARN("MucUserRoomManagerI::UserOnline --> call OnlineCenter.getUserJids ERR  line:"<<__LINE__<<" err:"<<e.what());
    }
		if(!imjids.empty()){
			MCE_DEBUG("MucUserRoomManagerI::UserOffline --> user :" << offlinejid->userId << " has im avatar so do not erase the room");
			return;
		}
		*/
		UserOnlineTypePtr offline_user_onlinetype; 
		try{
			MCE_DEBUG("MucUserRoomManagerI::UserOffline --> CALL OnlineBitmap.getUserStat userid = " << offlinejid->userId);
			offline_user_onlinetype = OnlineBitmapAdapter::instance().getUserType(offlinejid->userId);
		}
		catch(Ice::Exception& e){
			MCE_WARN("MucUserRoomManagerI::UserOffline-->OnlineBitmap::getUserStat-->line:"<<__LINE__<<" err:"<<e);
		}
		catch(std::exception& e){
			MCE_WARN("MucUserRoomManagerI::UserOffline --> call OnlineBitmap.getUserStat ERR  line:"<<__LINE__<<" err:"<<e.what());
		}
		if((offline_user_onlinetype->onlineType & 4)){
			MCE_DEBUG("MucUserRoomManagerI::UserOffline --> user :" << offlinejid->userId << " has im avatar so do not erase the room");
			return;
		}
		MCE_DEBUG("MucUserRoomManagerI::UserOffline --> NO IMAVATAR AT ALL SO REMOVE TEMORARY ROOMS size = " << temorary_roomseq.size());
		rit = temorary_roomseq.begin();
		for(; rit != temorary_roomseq.end(); ++rit){
			bool removeok = false;
	  	removeok = userpool->RemoveRoom(userid, (*rit));
    	if(!removeok){
      	MCE_WARN("MucUserRoomManagerI::UserOffline --> remove FAILED means the user don't has the room = " << (*rit)->roomname);
    	}
		}
	}
	MCE_INFO("interface UserOffline usetime : " << ts.getTime());
}

void MucUserRoomManagerI::EnterRoomForTemporary(const MucActiveUserPtr& enteruser, const Ice::Current& current){
  MCE_DEBUG("MucUserRoomManagerI::EnterRoomForTemporary --> enterjid = " << jidToString(enteruser->jid) << " roomname = " << enteruser->identity->roomid->roomname);
	TimeStat ts;
  int userid = enteruser->jid->userId;
  MucMemberPtr member = GetUserRoomPermision(userid, enteruser->identity->roomid);
  if(!member){
    member = new MucMember();
    member->userid = userid;
    member->permision = PMember;
		//这里没有必要知道正确的permision, 这个地方只是用来记录临时房间用户的房间信息的
    UserPoolPtr userpool = GetPool(userid);
    bool addroomok = false;
    if(userpool){
      addroomok = userpool->AddRoom(userid, enteruser->identity->roomid, member);
      if(!addroomok){
        MCE_WARN("MucUserRoomManagerI::EnterRoomForTemporary --> add room for user = " << userid << " room = " << enteruser->identity->roomid->roomname << " error!");
        return;
      }
    }else{
      MCE_WARN("MucUserRoomManagerI::EnterRoomForTemporary --> get no userpool userid = " << userid);
      return;
    }
  }
  //else{
  //  MCE_WARN("MucUserRoomManagerI::EnterRoomForTemporary --> user for Temporary enter room : " << enteruser->identity->roomid->roomname);
  //}
/*
  string username = "";
  try{
    TalkUserPtr userptr = TalkCacheClient::instance().GetUserByIdWithLoad(userid);
    username = userptr->name;
  }
  catch(Ice::Exception& e){
    MCE_WARN("MucUserRoomManagerI::EnterRoomForTemporary --> call TalkCacheClient.getUser ERR  line:"<<__LINE__<<" err:"<<e);
  }
  catch(std::exception& e){
    MCE_WARN("MucUserRoomManagerI::EnterRoomForTemporary --> call TalkCacheClient.getUser ERR  line:"<<__LINE__<<" err:"<<e.what());
  }
  string nickname = "";
  if(username.empty()){
    nickname = boost::lexical_cast<string>(userid);
  }
  //nickname = MakeNickname(username, userid);

  MucUserIdentityPtr identity = new MucUserIdentity();
  identity->roomid = roomid;
  identity->nickname = nickname;
  Permisions permision = member->permision;
  MucActiveUserPtr p = new MucActiveUser();
  p->identity = identity;
  p->jid = enterjid;
  p->permision = permision;
*/
  try{
    MCE_DEBUG("MucUserRoomManagerI::EnterRoomForTemporary --> CALL MucOnlineCenter.EnterRoom , info : " << jidToString(enteruser->jid) << ", " << enteruser->identity->roomid->roomname);
    MucOnlineCenterAdapter::instance().EnterRoom(enteruser);
  }
  catch(Ice::Exception& e){
    MCE_WARN("MucUserRoomManagerI::EnterRoomForTemporary-->MucOnlineCenter::EnterRoom-->line:"<<__LINE__<<" err:"<<e);
  }
  catch(std::exception& e){
    MCE_WARN("MucUserRoomManagerI::EnterRoomForTemporary --> call MucOnlineCenter.EnterRoom ERR  line:"<<__LINE__<<" err:"<<e.what());
  }
	MCE_INFO("interface EnterRoomForTemporary usetime : " << ts.getTime());
}
void MucUserRoomManagerI::AwayRoomForTemporary(const JidPtr& awayjid, const MucRoomIdPtr& roomid, const Ice::Current& current){
	TimeStat ts;
  int userid = awayjid->userId;
	bool isremoveroom = false;
	/*
	JidSeq imjids;
	try{
		MCE_DEBUG("MucUserRoomManagerI::AwayRoomForTemporary--> CALL OnlineCenter.getUserJids userid = " << awayjid->userId);
    imjids = OnlineCenterAdapter::instance().getUserJids(awayjid->userId, 4);
  }
  catch(Ice::Exception& e){
    MCE_WARN("MucUserRoomManagerI::AwayRoomForTemporary--> call OnlineCenter.getUserJids ERR  line:"<<__LINE__<<" err:"<<e);
  }
  catch(std::exception& e){
    MCE_WARN("MucUserRoomManagerI::AwayRoomForTemporary--> call OnlineCenter.getUserJids ERR  line:"<<__LINE__<<" err:"<<e.what());
  }
	if(imjids.empty()){
		MCE_DEBUG("MucUserRoomManagerI::AwayRoomForTemporary--> user :" << awayjid->userId << " do not has any im avatar so erase the room");
		isremoveroom = true;
	}
	else{
		MCE_DEBUG("MucUserRoomManagerI::AwayRoomForTemporary--> user :" << awayjid->userId << " user has im avatar online do not erase the room");
	}
	*/
	UserOnlineTypePtr away_user_onlinetype; 
	try{
		MCE_DEBUG("MucUserRoomManagerI::AwayRoomForTemporary--> CALL OnlineBitmap.getUserStat userid = " << awayjid->userId);
    away_user_onlinetype = OnlineBitmapAdapter::instance().getUserType(awayjid->userId);
  }
  catch(Ice::Exception& e){
    MCE_WARN("MucUserRoomManagerI::AwayRoomForTemporary-->OnlineBitmap::getUserStat-->line:"<<__LINE__<<" err:"<<e);
  }
  catch(std::exception& e){
    MCE_WARN("MucUserRoomManagerI::AwayRoomForTemporary--> call OnlineBitmap.getUserStat ERR  line:"<<__LINE__<<" err:"<<e.what());
  }
	if(!(away_user_onlinetype->onlineType & 4)){
		MCE_DEBUG("MucUserRoomManagerI::AwayRoomForTemporary--> user :" << awayjid->userId << " DO NOT HAS any im avatar so erase the room");
		isremoveroom = true;
	}
	else{
		MCE_DEBUG("MucUserRoomManagerI::AwayRoomForTemporary--> user :" << awayjid->userId << " user has im avatar online do not erase the room");
	}
	if(isremoveroom){
  	bool removeok = false;
  	UserPoolPtr userpool = GetPool(userid);
  	if(userpool){
    	removeok = userpool->RemoveRoom(userid, roomid);
    	if(!removeok){
      	MCE_WARN("MucUserRoomManagerI::AwayRoomForTemporary --> remove FAILED means the user don't has the room = " << roomid->roomname);
    	}
    	else{
      	MCE_DEBUG("MucUserRoomManagerI::AwayRoomForTemporary --> remove ok jid = " << jidToString(awayjid) << " room = " << roomid->roomname);
    	}
  	}else{
    	MCE_WARN("MucUserRoomManagerI::AwayRoomForTemporary --> no userpool awayjid = " << jidToString(awayjid) << " room = " << roomid->roomname);
    	return;
  	}
	}

  try{
    MCE_DEBUG("MucUserRoomManagerI::AwayRoomForTemporary --> CALL MucOnlineCenter.AwayRoom , info : " << jidToString(awayjid) << ", " << roomid->roomname);
    MucOnlineCenterAdapter::instance().AwayRoom(awayjid, roomid);
  }
  catch(Ice::Exception& e){
    MCE_WARN("MucUserRoomManagerI::AwayRoomForTemporary-->MucOnlineCenter::AwayRoom-->line:"<<__LINE__<<" err:"<<e);
  }
  catch(std::exception& e){
    MCE_WARN("MucUserRoomManagerI::AwayRoomForTemporary --> call MucOnlineCenter.AwayRoom ERR  line:"<<__LINE__<<" err:"<<e.what());
  }
	MCE_INFO("interface AwayRoomForTemporary usetime : " << ts.getTime());
}


/*void MucUserRoomManagerI::UserOnlineBySeq(const JidSeq& onlinejids, const Ice::Current& current){
  JidSeq::const_iterator it = onlinejids.begin();
  for(; it != onlinejids.end(); ++it){
    ProcessOnline((*it));
  }
}*/

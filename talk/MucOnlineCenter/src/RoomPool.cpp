#include "MucOnlineManagerI.h"
#include "MucGateAdapter.h"
#include "MucUserRoomAdapter.h"
#include "MucTalkRoomAdapter.h"
#include "ServiceI.h"
#include "RoomPool.h"
#include "OnlineCenterAdapter.h"
#include "PageCacheAdapter.h"
using namespace talk::http;
using namespace com::xiaonei::talk::muc;
using namespace talk::online::adapter;
using namespace com::xiaonei::talk::common;
using namespace MyUtil;
using namespace std;


bool RoomInfo::NicknameCheck(const string& checkNickname, int checkUserid){
  Jid2ActiveUserMap::iterator nit = _jid2activeuser.begin();
  for(; nit != _jid2activeuser.end(); ++nit){
    if(nit->second->identity->nickname == checkNickname){
      MCE_DEBUG("RoomInfo::NicknameCheck --> cheknick = " << checkNickname << " userid = " << checkUserid << " error!!");
      return false;
    }
  }
  MCE_DEBUG("RoomInfo::NicknameCheck --> cheknick = " << checkNickname << " userid = " << checkUserid << " OK~~~");
  return true;
}


/*
一个房间内增加一个活跃着，代表一个副本上线了，这里对副本的cache做到 "上下线配对后从cache里面删除" 的原则 用这种方法来中和因多线程下执行顺序不一致，导致的下线比上线早到达而产生永久在线用户的的这类用户
*/
bool RoomInfo::AddActiveUser(const MucActiveUserPtr& activeuser){
	activeuser->statuscode = STATUSCODE_ONLINE;//将上线的群聊副本的状态改为 在线的状态码
	activeuser->offlinetime = -1;
	time_t onlinetime = time(NULL);
  if(NicknameCheck(activeuser->identity->nickname, activeuser->jid->userId)){
		Jid2ActiveUserMap::iterator it = _jid2activeuser.find(activeuser->jid);
		//检查内存中 这个jid的目前的在线状态
		if(it == _jid2activeuser.end()){
			//如果内存中目前没有这个jid的在线状态  一定是之前被中和掉了或者没上线过。  正常情况
			MCE_INFO("RoomInfo::AddActiveUser --> onlinejid = " << jidToString(activeuser->jid) << " 上线时间 = " << onlinetime  << " 无 + 上线 ==> 上线 ");
			_jid2activeuser[activeuser->jid] = activeuser;
		}else{
			if(STATUSCODE_ONLINE == it->second->statuscode){
				MCE_INFO("RoomInfo::AddActiveUser --> onlinejid = " << jidToString(activeuser->jid) << " 上线时间 = " << onlinetime << " 上线 + 上线 ==> 不会出现");
			}
			else if(STATUSCODE_OFFLINE == it->second->statuscode){
				int error_time = onlinetime - it->second->offlinetime;
				if(error_time <= IGNORE_TIMESTAMP){//我们认为下线在上线之前，且间隔时间在 IGNORE_TIMESTAMP 之内的，为多线程导致的永久在线用户 忽略这次上线，并将内存里的在线状态清除掉
					MCE_INFO("RoomInfo::AddActiveUser --> onlinejid = " << jidToString(activeuser->jid) << " 上线时间 = " << onlinetime << " 下线 + 上线(时间差:" << error_time << " < 忽略时间差:" << IGNORE_TIMESTAMP << ") ==> 无 忽略这次上线，并清除内存cache 上线失败");
					_jid2activeuser.erase(it);
					return false;
				}else{
					MCE_INFO("RoomInfo::AddActiveUser --> onlinejid = " << jidToString(activeuser->jid) << " 上线时间 = " << onlinetime << " 下线 ==> 上线(时间差:" << error_time << " > 忽略时间差:" << IGNORE_TIMESTAMP << ") ==> 无 错误下线后上线，更改cache在线状态为上线");
					_jid2activeuser[activeuser->jid] = activeuser;
				}
			}
		}
		//TaskManager::instance().execute(new PageCacheNotifyTask("online", activeuser->jid->userId, activeuser->identity->roomid));
		//MCE_DEBUG("RoomInfo::AddActiveUser --> jid = " << jidToString(activeuser->jid) << " roomname = " << activeuser->identity->roomid->roomname << " permision = " << activeuser->permision << " OK!");
    return true;
  }
  MCE_DEBUG("RoomInfo::AddActiveUser --> onlinejid = " << jidToString(activeuser->jid) << " NICKNAME CHECK FAILED!");
  return false;
}

bool RoomInfo::AwayRoom(const JidPtr& opuser){
  MCE_DEBUG("RoomInfo::AwayRoom --> offlinejid = " << jidToString(opuser));
	//只要不是下线碰到上线的配对，那么都是有问题的配对
  Jid2ActiveUserMap::iterator nit = _jid2activeuser.find(opuser);
	time_t offlinetime = time(NULL);
  if(nit != _jid2activeuser.end()){
		if(STATUSCODE_ONLINE == nit->second->statuscode){
			MCE_INFO("RoomInfo::AwayRoom --> offlinejid = " << jidToString(opuser) << " 下线时间 = " << offlinetime << " 上线 + 下线 ==> 无  正常下线 清除内存对象");
			_jid2activeuser.erase(nit);
    	return true;
		}
		else{
			MCE_INFO("RoomInfo::AwayRoom --> offlinejid = " << jidToString(opuser) << " 下线时间 = " << offlinetime << " 下线 + 下线 ==> 下线 更新下线对象的下线时间戳");
			nit->second->offlinetime = offlinetime;
		}
		//TaskManager::instance().execute(new PageCacheNotifyTask("offline", opuser->userId,nit->second->identity->roomid));
  }else{
		MucRoomIdPtr roomid = new MucRoomId();
		roomid->roomname = _roomname;
		roomid->domain = _domain;
		MucUserIdentityPtr identity = new MucUserIdentity();
		identity->roomid = roomid;
		identity->nickname = "";
		//这样一个下线状态的用户是不会被用到的，所以没有必要去调用一次他的名字
		identity->userid = opuser->userId;
		MucActiveUserPtr activeuser = new MucActiveUser();
		activeuser->identity = identity;
		activeuser->jid = opuser;
		activeuser->permision = PMember;
		//权限这里也是随便放置一个
		activeuser->statuscode = STATUSCODE_OFFLINE;
		activeuser->offlinetime = offlinetime;
		MCE_INFO("RoomInfo::AwayRoom --> offlinejid = " << jidToString(opuser) << " 下线时间 = " << offlinetime << " 无 + 下线 ==> 下线 有可能是下线比上线先到达，生成一个下线状态对象，更新cache状态为OFFLINE， 并等待与其配对的上线到达");
		_jid2activeuser[opuser] = activeuser;
	}
  return false;
}



bool RoomInfo::AwayRoom(int userid){
  bool awayok = false;
  JidPtr jid = new Jid();
  jid->userId = userid;
  jid->index = 0;
	

	JidSeq offlinejids;
  Jid2ActiveUserMap::iterator nit = _jid2activeuser.lower_bound(jid);
  for(; nit != _jid2activeuser.end(); ++nit){
    if(nit->second->jid->userId == userid){
      offlinejids.push_back(nit->second->jid);
			//TaskManager::instance().execute(new PageCacheNotifyTask("offline", userid, nit->second->identity->roomid));
      awayok = true;
    }
    else{
      break;
    }
  }
	for(JidSeq::iterator jit = offlinejids.begin(); jit != offlinejids.end(); ++jit){
		AwayRoom((*jit));
	}
  //MCE_DEBUG("RoomInfo::AwayRoom --> userid = " << userid << " awayok = " << awayok);
  return awayok;
}



MucActiveUserPtr RoomInfo::GetActiveUser(int userid){

  JidPtr jid = new Jid();
  jid->userId = userid;
  jid->index = 0;

  MucActiveUserPtr ans;
  Jid2ActiveUserMap::iterator nit = _jid2activeuser.lower_bound(jid);
  for(; nit != _jid2activeuser.end(); ++nit){
    if(nit->second->jid->userId == userid){
      if(POutcast != nit->second->permision){
				if(STATUSCODE_OFFLINE != nit->second->statuscode){
        	MCE_DEBUG("RoomInfo::GetActiveUser --> userid = " << userid << " success");
        	return nit->second;
				}else{
					MCE_DEBUG("RoomInfo::GetActiveUser --> jid = " << jidToString(nit->second->jid) << " is OFFLINE_JID so IGNORE");
				}
      }
      else{
        MCE_DEBUG("RoomInfo::GetActiveUser --> userid = " << userid << " user is a outcast so failed");
        return ans;
      }
    }
    else{
      break;
    }
  }
  MCE_DEBUG("RoomInfo::GetActiveUser --> userid = " << userid << " failed");
  return ans;
}


MucActiveUserPtr RoomInfo::GetActiveUser(const JidPtr& jid){
  MucActiveUserPtr ans;
  Jid2ActiveUserMap::iterator nit = _jid2activeuser.find(jid);
  if(nit != _jid2activeuser.end()){
    if(nit->second->permision != POutcast){
			if(STATUSCODE_OFFLINE != nit->second->statuscode){
				MCE_DEBUG("RoomInfo::GetActiveUser --> jid = " << jidToString(jid) << " OK");
				return nit->second;
			}else{
				MCE_DEBUG("RoomInfo::GetActiveUser --> jid = " << jidToString(jid) << " is OFFLINE_JID so IGNORE");
			}
    }else{
      MCE_DEBUG("RoomInfo::GetActiveUser --> userid = " << jid->userId << " user is a outcast so failed");
		}
  }
  MCE_DEBUG("RoomInfo::GetActiveUser --> jid = " << jidToString(jid) << " FAILED");
  return ans;
}
MucActiveUserPtr RoomInfo::GetActiveUser(const string& nickname){
  MucActiveUserPtr ans;
  //string strjid = jidToString(jid);
  Jid2ActiveUserMap::iterator nit = _jid2activeuser.begin();
  for(; nit != _jid2activeuser.end(); ++nit){
		if(nit->second->identity->nickname == nickname && POutcast != nit->second->permision && STATUSCODE_OFFLINE != nit->second->statuscode ){
			MCE_DEBUG("RoomInfo::GetActiveUser --> nickname = " << nickname << " OK");
			return nit->second;
		}
  }
  MCE_DEBUG("RoomInfo::GetActiveUser --> nickname = " << nickname << " FAILED");
  return ans;
}


MucActiveUserSeq RoomInfo::GetActiveUserSeq(int userid, bool filter){

  JidPtr jid = new Jid();
  jid->userId = userid;
  jid->index = 0;

  MucActiveUserSeq ans;
  Jid2ActiveUserMap::iterator nit = _jid2activeuser.lower_bound(jid);
  for(; nit != _jid2activeuser.end(); ++nit){
    if(nit->second->jid->userId == userid){
			if(STATUSCODE_OFFLINE == nit->second->statuscode){
				continue;
			}
      if(!filter){
        ans.push_back(nit->second);
      }
			else{
				if(POutcast != nit->second->permision){
        	ans.push_back(nit->second);
				}else{
					//如果有一个副本是黑名单了， 那就无须检查了， 其他都是。
					break;
				}
			}
			/*
      else if(nit->second->permision != POutcast){
        ans.push_back(nit->second);
      }
      else{
        return ans;
      }*/
    }
    else{
      break;
    }
  }
  MCE_DEBUG("RoomInfo::GetActiveUserSeq --> userid = " << userid << " ans.size = " << ans.size());
  return ans;
}

MucActiveUserSeq RoomInfo::GetActiveUserSeq(Permisions permision){

  MucActiveUserSeq ans;
  Jid2ActiveUserMap::iterator nit = _jid2activeuser.begin();
  for(; nit != _jid2activeuser.end(); ++nit){
		if(STATUSCODE_OFFLINE == nit->second->statuscode){
			continue;
		}
    if(nit->second->permision == permision){
      ans.push_back(nit->second);
    }
  }
  MCE_DEBUG("RoomInfo::GetActiveUserSeq --> permision = " << permision << " ans.size = " << ans.size());
  return ans;
}

JidSeq RoomInfo::GetAllActiveJids(){
  JidSeq ans;
  Jid2ActiveUserMap::iterator nit = _jid2activeuser.begin();
  for(; nit != _jid2activeuser.end(); ++nit){
    if(POutcast != nit->second->permision && STATUSCODE_OFFLINE != nit->second->statuscode){
      ans.push_back(nit->first);
    }
  }
  MCE_DEBUG("RoomInfo::GetAllActiveJids --> ans.size = " << ans.size());
  return ans;
}

MucActiveUserSeq RoomInfo::GetAllActiveUsers(){
  MucActiveUserSeq ans;
  Jid2ActiveUserMap::iterator nit = _jid2activeuser.begin();
  for(; nit != _jid2activeuser.end(); ++nit){
    if(POutcast != nit->second->permision && STATUSCODE_OFFLINE != nit->second->statuscode){
    	ans.push_back(nit->second);
		}
  }
  MCE_DEBUG("RoomInfo::GetAllActiveUsers --> ans.size = " << ans.size());
  return ans;
}
MucUserIdentitySeq RoomInfo::GetAllUserIdentity(){
  MucUserIdentitySeq ans;
  set<int> uids;
  Jid2ActiveUserMap::iterator nit = _jid2activeuser.begin();
  for(; nit != _jid2activeuser.end(); ++nit){
    if(POutcast != nit->second->permision && STATUSCODE_OFFLINE != nit->second->statuscode){
			int userid = nit->second->jid->userId;
			pair<set<int>::iterator, bool> insertres = uids.insert(userid);
			if(insertres.second){
				ans.push_back(nit->second->identity);
			}
		}
  }
  MCE_DEBUG("RoomInfo::GetAllUserIdentity --> ans.size = " << ans.size());
  return ans;
}

int RoomInfo::GetUserMucStatus(int userid){
	int ans = 0;
  JidPtr jid = new Jid();
  jid->userId = userid;
  jid->index = 0;

  Jid2ActiveUserMap::iterator nit = _jid2activeuser.lower_bound(jid);
  for(; nit != _jid2activeuser.end(); ++nit){
    if(nit->second->jid->userId == userid){
    	if(POutcast != nit->second->permision){
				if(STATUSCODE_OFFLINE != nit->second->statuscode){
					if('W' == nit->second->jid->endpoint[0]){
						MCE_DEBUG("RoomInfo::GetUserMucStatus --> userid = " << userid << " web online ans |= 2");
						ans |= 2;
					}else if('T' == nit->second->jid->endpoint[0]){
						MCE_DEBUG("RoomInfo::GetUserMucStatus --> userid = " << userid << " IM online ans |= 4");
						ans |= 4;
					}else if('P' == nit->second->jid->endpoint[0]){
						MCE_DEBUG("RoomInfo::GetUserMucStatus --> userid = " << userid << " Phone online ans |= 8");
						ans |= 8;
					}
				}
      }
      else{
				MCE_DEBUG("RoomInfo::GetUserMucStatus --> userid = " << userid << " user is an outcast return 0");
        return ans;
      }
    }
    else{
      break;
    }
  }
  MCE_DEBUG("RoomInfo::GetUserMucStatus --> userid = " << userid << " status = " << ans);
  return ans;
}
int RoomInfo::GetUserCount(){
  return _jid2activeuser.size();
}

void RoomInfo::SetPermision(int targetuser, Permisions permision){
  if(permision == PNone){
    MCE_DEBUG("RoomInfo::SetPermision --> PERMISION = NONE awayroom");
    AwayRoom(targetuser);
    return;
  }
  MCE_DEBUG("RoomInfo::SetPermision --> PERMISION = " << permision);
  MucActiveUserSeq s = GetActiveUserSeq(targetuser, false);
  MucActiveUserSeq::iterator sit = s.begin();
  for(; sit != s.end(); ++sit){
    (*sit)->permision = permision;
  }
}

bool RoomInfo::SetPermisionForIM(int opuser, Permisions permision, int targetuser){
  MucActiveUserPtr opaptr = GetActiveUser(opuser);
  if(opaptr && opaptr->permision >= PAdmin){
    if(permision == PAdmin || permision == POwner){
      if(opaptr->permision != POwner){
        MCE_DEBUG("RoomInfo::SetPermisionForIM --> permision = " << permision << " but opuser is not an owner so FAILED");
        return false;
      }
    }
    //if(permision == POutcast || permision == PNone){
    if(permision == PNone){
			MucActiveUserPtr targetaptr = GetActiveUser(targetuser);
			if(targetaptr && (opaptr->permision > targetaptr->permision)){
      	MCE_DEBUG("RoomInfo::SetPermisionForIM --> PERMISION = NONE SO user AwayRoom");
      	AwayRoom(targetuser);
			}
			else{
				return false;
			}
    }
    else{
      MucActiveUserSeq s = GetActiveUserSeq(targetuser, false);
      MucActiveUserSeq::iterator sit = s.begin();
      for(; sit != s.end(); ++sit){
        MCE_DEBUG("RoomInfo::SetPermisionForIM --> set jid = " << jidToString((*sit)->jid) << " permision = " << permision);
        (*sit)->permision = permision;
      }
    }
    MCE_DEBUG("RoomInfo::SetPermisionForIM --> SET OK!");
    return true;
  }
  MCE_DEBUG("RoomInfo::SetPermisionForIM --> opser is not admin or owner so FAILED OR opuser is null");
  return false;
}

//-----------------------------------------------------------

RoomInfoPtr RoomPool::GetRoom(const MucRoomIdPtr& roomId){

  RoomInfoPtr ans;
  Cache::iterator cit = _cache.find(roomId->domain);
  if(cit != _cache.end()){
    RoomMap::iterator rit = cit->second.find(roomId->roomname);
    if(rit != cit->second.end()){
      MCE_DEBUG("RoomPool::GetRoom --> roomname = " << roomId-> roomname << " domain = " << roomId->domain << " OK");
      return rit->second;
    }
  }
  MCE_DEBUG("RoomPool::GetRoom --> roomname = " << roomId-> roomname << " domain = " << roomId->domain << " FAILED");
  return ans;
}

bool RoomPool::KickRoom(const MucRoomIdPtr& roomId){

  Cache::iterator cit = _cache.find(roomId->domain);
  if(cit != _cache.end()){
    RoomMap::iterator rit = cit->second.find(roomId->roomname);
    cit->second.erase(rit);
    MCE_DEBUG("RoomPool::KickRoom --> roomname = " << roomId->roomname << " OK");
    return true;
  }
  MCE_DEBUG("RoomPool::KickRoom --> roomname = " << roomId->roomname << " FAILED");
  return false;
}


RoomInfoPtr RoomPool::CreateRoom(const MucRoomIdPtr& roomId){
  MCE_DEBUG("RoomPool::CreateRoom --> roomname = " << roomId->roomname << " domain = " << roomId->domain << " OK");
  RoomInfoPtr room = new RoomInfo();
  room->_roomname = roomId->roomname;
  room->_domain = roomId->domain;
  _cache[roomId->domain][roomId->roomname] = room;
  return room;
}

RoomInfoPtr RoomPool::JoinRoom(const MucActiveUserPtr& activeuser){
  MCE_DEBUG("RoomPool::JoinRoom --> activeuser info : jid = " << jidToString(activeuser->jid) << " roomname = " << activeuser->identity->roomid->roomname << " domain = " << activeuser->identity->roomid->domain);
  RoomInfoPtr riptr;
	bool isnewroom = false;
	bool enterok = true;
	riptr = GetRoom(activeuser->identity->roomid);
	if(!riptr){
		isnewroom = true;
		riptr = CreateRoom(activeuser->identity->roomid);
	}
	if(riptr){
		if(TEMPORARY == activeuser->identity->roomid->domain){
			if(isnewroom){
				MCE_DEBUG("RoomPool::JoinRoom --> the room " << activeuser->identity->roomid->roomname << " is confrenc group and the user = " << activeuser->jid->userId << " is the creater of the group so he is the owner");
				MucActiveUserPtr changePermisionuser = new MucActiveUser();
				changePermisionuser->identity = activeuser->identity;
				changePermisionuser->jid = activeuser->jid;
				changePermisionuser->permision = POwner;
				enterok = riptr->AddActiveUser(changePermisionuser);
			}
			else{
				MucActiveUserPtr temporary_user = riptr->GetActiveUser(activeuser->jid->userId);
				if(temporary_user){
					if(POutcast == temporary_user->permision){
						MCE_DEBUG("RoomPool::JoinRoom --> the user " << activeuser->jid->userId << " is an outcast user so ignore the request");
						return 0;
					}
					MCE_DEBUG("RoomPool::JoinRoom --> the user " << activeuser->jid->userId << " has joined the room befor  so change the permision to " << temporary_user->permision << " and add the jid " << jidToString(activeuser->jid));
					MucActiveUserPtr changePermisionuser = new MucActiveUser();
					changePermisionuser->identity = activeuser->identity;
					changePermisionuser->jid = activeuser->jid;
					changePermisionuser->permision = temporary_user->permision;
					enterok = riptr->AddActiveUser(changePermisionuser);
				}
				else{
					MCE_DEBUG("RoomPool::JoinRoom --> add activeuser for confrence.xiaonei.com jid = " << jidToString(activeuser->jid));
					enterok = riptr->AddActiveUser(activeuser);
				}
			}
		}
		else{
			enterok = riptr->AddActiveUser(activeuser);
		}
	}
	MCE_DEBUG("RoomPool::JoinRoom --> jid = " << jidToString(activeuser->jid) << " activeuser's roomname = " << activeuser->identity->roomid->roomname << " domain = " << activeuser->identity->roomid->domain << " enterok = " << enterok);

	if(enterok){
		return riptr;
	}
	return 0;
}

void RoomPool::ChatSetingChange(int userid, const MucRoomIdPtr& roomid, int set){
	RoomInfoPtr riptr;
	MucActiveUserSeq seq;
	MCE_DEBUG("RoomPool::ChatSetingChange --> userid = " << userid << " roomid = " << mucRoomIdToString(roomid) << " set = " << set);
	{
		IceUtil::RWRecMutex::RLock lock(_rwmutex);
		riptr = GetRoom(roomid);
		if(riptr){
			seq = riptr->GetActiveUserSeq(userid);
		}
	}
	if(seq.empty()){
		MCE_DEBUG("RoomPool::ChatSetingChange --> userid = " << userid << " roomid = " << mucRoomIdToString(roomid) << " set = " << set << " no MucActiveUserSeq");
		return;
	}
	JidSeq webjids;
	for(MucActiveUserSeq::iterator ait = seq.begin(); ait != seq.end(); ++ait){
		if('W' == (*ait)->jid->endpoint[0]){
			webjids.push_back((*ait)->jid);
		}
	}
	if(webjids.empty()){
		MCE_DEBUG("RoomPool::ChatSetingChange --> userid = " << userid << " roomid = " << mucRoomIdToString(roomid) << " set = " << set << " no webjids");
		return;
	}
	try{
		MucGateAdapter::instance().NotifyChatSetingChange(roomid, webjids, set);
	}catch(Ice::Exception& e){
		MCE_WARN("RoomPool::ChatSetingChange--> call MucGate.NotifyChatSetingChange ERR  line:"<<__LINE__<<" err:"<<e);
	}
	catch(std::exception& e){
		MCE_WARN("RoomPool::ChatSetingChange--> call MucGate.NotifyChatSetingChange ERR  line:"<<__LINE__<<" err:"<<e.what());
	}
}

void RoomPool::EnterRoom(const MucActiveUserPtr& activeuser){
  MCE_DEBUG("RoomPool::EnterRoom --> activeuser info : jid = " << jidToString(activeuser->jid) << " roomname = " << activeuser->identity->roomid->roomname << " domain = " << activeuser->identity->roomid->domain);

  RoomInfoPtr riptr;
  MucActiveUserPtr opaptr;
  MucActiveUserSeq presenceUsers;
  bool hasroom = true;
	int mucstatus = 0;
	bool enterok = false;
  if(TEMPORARY == activeuser->identity->roomid->domain){
		//临时房间
		{
    	MCE_DEBUG("RoomPool::EnterRoom --> domain == " << TEMPORARY);
    	IceUtil::RWRecMutex::WLock lock(_rwmutex);
			riptr = JoinRoom(activeuser);
			if(riptr){
				//临时房间用户可能会改变他的权限。
				opaptr = riptr->GetActiveUser(activeuser->jid);
				if(opaptr){
					enterok = true;
				}
			}
		}
		if(!enterok){
			try{
				MCE_DEBUG("RoomPool::EnterRoom --> CALL MucGate.NotifyNicknameError jid:" << jidToString(activeuser->jid) << " room:" << mucRoomIdToString(activeuser->identity->roomid));
				MucGateAdapter::instance().NotifyNicknameError(activeuser->identity->roomid, activeuser->jid);
			}
			catch(Ice::Exception& e){
				MCE_WARN("RoomPool::EnterRoom --> call MucGate.NotifyNicknameError ERR  line:"<<__LINE__<<" err:"<<e);
			}
			catch(std::exception& e){
				MCE_WARN("RoomPool::EnterRoom --> call MucGate.NotifyNicknameError ERR  line:"<<__LINE__<<" err:"<<e.what());
			}
			return;
		}
		/*
		//临时房间聊天发起者多副本同步，要向其他副本发送邀请
		JidSeq imjids;
		try{
			MCE_DEBUG("RoomPool::EnterRoom --> CALL OnlineCenter.getUserJids for invite the other jids. userid = " << activeuser->jid->userId);
			imjids = OnlineCenterAdapter::instance().getUserJids(activeuser->jid->userId, 4);
		}
		catch(Ice::Exception& e){
			MCE_WARN("RoomPool::EnterRoom --> call OnlineCenter.getUserJids ERR  line:"<<__LINE__<<" err:"<<e);
		}
		catch(std::exception& e){
			MCE_WARN("RoomPool::EnterRoom --> call OnlineCenter.getUserJids ERR  line:"<<__LINE__<<" err:"<<e.what());
		}
		if(imjids.empty()){
			return;
		}
		for(JidSeq::iterator jit = imjids.begin(); jit != imjids.end(); ++jit){
			if(equal((*jit), activeuser->jid)){
				imjids.erase(jit);
				MCE_DEBUG("RoomPool::EnterRoom --> erase the activeuser->jid from want to invite");
				break;
			}
		}
		if(!imjids.empty()){
			MCE_DEBUG("RoomPool::EnterRoom --> invite the user : " << activeuser->jid->userId << "'s other avatar into the room");
			try{
				MucGateAdapter::instance().InviteIMusers(activeuser->identity->roomid, imjids);
			}
			catch(Ice::Exception& e){
				MCE_WARN("RoomPool::EnterRoom --> call MucGate.InviteImUsers ERR  line:"<<__LINE__<<" err:"<<e);
			}
			catch(std::exception& e){
				MCE_WARN("RoomPool::EnterRoom --> call OnlineCenter.InviteImUsers ERR  line:"<<__LINE__<<" err:"<<e.what());
			}
		}
		*/
  }
  else if(PERMANENT == activeuser->identity->roomid->domain){
		opaptr = activeuser;
    MCE_DEBUG("RoomPool::EnterRoom --> domain == " << PERMANENT);
    {
      IceUtil::RWRecMutex::RLock lock(_rwmutex);
      riptr = GetRoom(activeuser->identity->roomid);
    }
    if(riptr){
			//如果房间存在，直接加人
      MCE_DEBUG("RoomPool::EnterRoom --> riptr is not null so call it AddActiveUser function");
      IceUtil::RWRecMutex::WLock lock(_rwmutex);
      enterok = riptr->AddActiveUser(activeuser);
    }
    else{
      MCE_DEBUG("RoomPool::EnterRoom --> riptr is null .No " << activeuser->identity->roomid->roomname << " in cache pull from MucTalkRoom");
      hasroom = false;
      JidSeq imusers;
      MCE_DEBUG("RoomPool::EnterRoom --> CALL MucTalkRoomAdapter.GetRoomActiveUserSeq roomname = " << activeuser->identity->roomid->roomname);
      try{
        presenceUsers = MucTalkRoomAdapter::instance().GetRoomActiveUserSeq(activeuser->identity->roomid);
      }
      catch(Ice::Exception& e){
        MCE_WARN("RoomPool::EnterRoom --> call MucTalkRoom.GetRoomActiveUserSeq ERR  line:"<<__LINE__<<" err:"<<e);
      }
      catch(std::exception& e){
        MCE_WARN("RoomPool::EnterRoom --> call MucTalkRoom.GetRoomActiveUserSeq ERR  line:"<<__LINE__<<" err:"<<e.what());
      }
      {
        IceUtil::RWRecMutex::WLock lock(_rwmutex);
        bool againcheck = false;
        riptr = GetRoom(activeuser->identity->roomid);
        if(riptr){
          MCE_DEBUG("RoomPool::EnterRoom --> after check again bool againcheck = true so it means the users has pull hera already so only add it");
          againcheck = true;
					hasroom = true;
          enterok = riptr->AddActiveUser(activeuser);
        }
        if(!againcheck){
          MCE_DEBUG("RoomPool::EnterRoom --> after check again  bool againcheck = false so push the web presenceUsers(get from MucTalkRoom) into room and send invite to IM users");
          bool hasaddroom = false;
          MucActiveUserSeq::iterator it = presenceUsers.begin();
          for(; it != presenceUsers.end();){
            if((*it)->jid->endpoint[0] == 'W'){
              hasaddroom = true;
              riptr = JoinRoom((*it));
							++it;
            }
						else if(equal(activeuser->jid, (*it)->jid)){
							MCE_DEBUG("RoomPool::EnterRoom --> the activeuser self enter room");
							//第三方客户端可能会起不同的名字所以在这里要加入房间的参数activeuser 并且修改这个seq里面的元素。
							hasaddroom = true;
							riptr = JoinRoom(activeuser);
							(*it) = activeuser;
							++it;
						}
            else if((*it)->jid->endpoint[0] == 'T'){
              imusers.push_back((*it)->jid);
							//对于客户端用户，我们发送邀请。所以，它并不在房间内，我们要从广播用户内删除他们。
							it = presenceUsers.erase(it);
            }
          }
          if(!hasaddroom){
            riptr = CreateRoom(activeuser->identity->roomid);
          }
					if(riptr){
						//判断拉进来的用户里面有没有自己
						opaptr = riptr->GetActiveUser(activeuser->jid);
						if(opaptr){
							enterok = true;
						}
					}
        }
      }
      if(enterok && !imusers.empty()){
        MCE_DEBUG("RoomPool::EnterRoom --> send invite to imiusers size = " << imusers.size());
        MucGateAdapter::instance().InviteIMusers(activeuser->identity->roomid, imusers);
      }
    }
  }
	
	if(!enterok){
		MCE_DEBUG("RoomPool::EnterRoom --> jid = " << jidToString(activeuser->jid) << " enterok = false return");
		return;
	}
	
  if(hasroom){
    IceUtil::RWRecMutex::RLock lock(_rwmutex);
    presenceUsers = riptr->GetAllActiveUsers();
  }
  if(opaptr && !presenceUsers.empty()){
    MCE_DEBUG("RoomPool::EnterRoom --> jid = " << jidToString(activeuser->jid) << " OK");
		for(MucActiveUserSeq::iterator logit = presenceUsers.begin(); logit != presenceUsers.end(); ++logit){
			MCE_DEBUG("RoomPool::EnterRoom --> presenceUser : info " << jidToString((*logit)->jid) << " room : " << mucUserIdentityToString((*logit)->identity) << " permision = " << (*logit)->permision);
		}
		if(PERMANENT == activeuser->identity->roomid->domain && riptr){
    	IceUtil::RWRecMutex::RLock lock(_rwmutex);
			mucstatus = riptr->GetUserMucStatus(activeuser->jid->userId);
		}
    MucGateAdapter::instance().NotifyUserEnterRoom(opaptr, presenceUsers, mucstatus);
  }
}

void RoomPool::UserOnline(const MucActiveUserPtr& activeuser){
  MCE_DEBUG("RoomPool::UserOnline --> jid = " << jidToString(activeuser->jid));
  MucActiveUserPtr opaptr;
  MucActiveUserSeq presenceUsers;
  RoomInfoPtr riptr;
  bool addok = false;
	int mucstatus = 0;
  {
    //针对频繁上下线，不能上写锁。
    IceUtil::RWRecMutex::RLock lock(_rwmutex);
    riptr = GetRoom(activeuser->identity->roomid);
    if(!riptr){
			//TaskManager::instance().execute(new PageCacheNotifyTask("online", activeuser->jid->userId, activeuser->identity->roomid));
      return;
    }
		presenceUsers = riptr->GetAllActiveUsers();
		presenceUsers.push_back(activeuser);    //加自己
  }
  if(riptr){
		//如果用户是IM用户，而且房间也cache了，这样发送邀请。
		if('T' == activeuser->jid->endpoint[0]){
			MCE_DEBUG("RoomPool::UserOnline --> user is an im user and room is in cache. so send invite to it!");
			try{
				JidSeq jids;
				jids.push_back(activeuser->jid);
				MucGateAdapter::instance().InviteIMusers(activeuser->identity->roomid, jids);
			}catch(Ice::Exception& e){
      	MCE_WARN("RoomPool::UserOnline--> call MucTalkRoom.GetRoomActiveUserSeq ERR  line:"<<__LINE__<<" err:"<<e);
   	 	}
    	catch(std::exception& e){
      	MCE_WARN("RoomPool::EnterRoom --> call MucTalkRoom.GetRoomActiveUserSeq ERR  line:"<<__LINE__<<" err:"<<e.what());
    	}
			return;
		}

    //如果这个房间被cache住了,在线中心会响应这个用户的上线行为
    IceUtil::RWRecMutex::WLock lock(_rwmutex);
    addok = riptr->AddActiveUser(activeuser);
		if(addok){
			mucstatus = riptr->GetUserMucStatus(activeuser->jid->userId);
		}
  }

  if(addok && !presenceUsers.empty()){
    MucGateAdapter::instance().NotifyUserEnterRoom(activeuser, presenceUsers, mucstatus);
  }
}

void RoomPool::AwayRoom(const JidPtr& opuser, const MucRoomIdPtr& roomid){
  //如果一个用户的副本进入了在线中心，那么这个用户退出在线中心的行为是下线。
  //第三方副本可以awayroom
  RoomInfoPtr riptr;
  MucActiveUserPtr opaptr;
  //MucActiveUserSeq presenceUsers;
  JidSeq presenceUsers;
  bool awayok = false;
  bool kickroomok = false;
	int mucstatus = 0;
  {
    IceUtil::RWRecMutex::WLock lock(_rwmutex);
    riptr = GetRoom(roomid);
		if(!riptr){
			return;
		}
    opaptr = riptr->GetActiveUser(opuser);
    awayok = riptr->AwayRoom(opuser);
    if(awayok && !riptr->GetUserCount()){
      MCE_DEBUG("RoomPool::AwayRoom --> user away ok and room is empty so kick it");
      kickroomok = KickRoom(roomid);
    }
  }

  if(awayok){
    if(!kickroomok){
      IceUtil::RWRecMutex::RLock lock(_rwmutex);
      presenceUsers = riptr->GetAllActiveJids();
			mucstatus = riptr->GetUserMucStatus(opuser->userId);
    }
		if(opaptr){
    	presenceUsers.push_back(opaptr->jid);
		}
  }

  if(awayok && opaptr && !presenceUsers.empty()){
    MucGateAdapter::instance().NotifyUserAwayRoom(opaptr, presenceUsers, mucstatus);
  }
  MCE_DEBUG("RoomPool::AwayRoom --> jid = " << jidToString(opuser) << " roomname = " << roomid->roomname << " awayok = " << awayok << " mucstatus = " << mucstatus);
}

void RoomPool::SendTalkMessage(const JidPtr& opuser, const MucRoomIdPtr& roomid, const string& msg){
  MCE_DEBUG("RoomPool::SendTalkMessage --> jid = " << jidToString(opuser) << " roomname = " << roomid->roomname << " msg = " << msg);
  MucActiveUserPtr opaptr;
  JidSeq presenceUsers;
	RoomInfoPtr riptr;
  {
    IceUtil::RWRecMutex::RLock lock(_rwmutex);
    riptr = GetRoom(roomid);
		if(!riptr){
			return;
		}
		opaptr = riptr->GetActiveUser(opuser);
		if(opaptr){
			presenceUsers = riptr->GetAllActiveJids();
		}else{
    	MCE_WARN("RoomPool::SendTalkMessage --> no activeuser in room.....it is so amazing pull the jid :" << jidToString(opuser) << " into the room");
			TaskManager::instance().execute(new PullLostUserTask(opuser));
		}
  }
  if(opaptr && !presenceUsers.empty()){
    MCE_DEBUG("RoomPool::SendTalkMessage --> presenceUsers.size = " << presenceUsers.size());
    MucGateAdapter::instance().NotifyGroupChat(opaptr, presenceUsers, msg);
  }
}

int RoomPool::GetUserCount(const MucRoomIdPtr& roomid){
  IceUtil::RWRecMutex::RLock lock(_rwmutex);
  RoomInfoPtr riptr = GetRoom(roomid);
  if(riptr){
    return riptr->GetUserCount();
  }
}
void RoomPool::SetPermision(int opuser, Permisions permision, int targetuser, const MucRoomIdPtr& roomid){
  MCE_DEBUG("RoomPool::SetPermision --> opuser = " << opuser << " permision = " << permision << " targetuser = " << targetuser << " roomname = " << roomid->roomname);
  MucActiveUserPtr opaptr;
  MucActiveUserSeq targetseq;
  JidSeq presenceUsers;
  RoomInfoPtr riptr;
  {
    IceUtil::RWRecMutex::WLock lock(_rwmutex);
    riptr = GetRoom(roomid);
    if(riptr){
      targetseq = riptr->GetActiveUserSeq(targetuser);
      riptr->SetPermision(targetuser, permision);
    }
  }
  if(riptr){
    IceUtil::RWRecMutex::RLock lock(_rwmutex);
    opaptr = riptr->GetActiveUser(opuser);
    if(opaptr){
      presenceUsers = riptr->GetAllActiveJids();
      if(permision == POutcast || permision == PNone){
				//如果是踢人的话，要把自己加上,notify self
        for(MucActiveUserSeq::iterator it = targetseq.begin(); it != targetseq.end(); ++it){
          presenceUsers.push_back((*it)->jid);
        }
      }
    }
  }
  if(riptr && opaptr && !presenceUsers.empty()){
    MucGateAdapter::instance().NotifySetPermision(opaptr, presenceUsers, targetseq, permision);
  }
}

bool RoomPool::SetPermisionForIM(int opuser, Permisions permision, int targetuser, const MucRoomIdPtr& roomid){
  bool setok = false;
  MucActiveUserPtr opaptr;
  MucActiveUserSeq targetseq;
  JidSeq presenceUsers;
  RoomInfoPtr riptr;
  {
    IceUtil::RWRecMutex::WLock lock(_rwmutex);
    riptr = GetRoom(roomid);
    if(riptr){
      targetseq = riptr->GetActiveUserSeq(targetuser);
      setok = riptr->SetPermisionForIM(opuser, permision, targetuser);
    }
    else{
      return false;
    }
  }
  if(setok){
    IceUtil::RWRecMutex::RLock lock(_rwmutex);
    opaptr = riptr->GetActiveUser(opuser);
    if(opaptr){
      presenceUsers = riptr->GetAllActiveJids();
      if(permision == POutcast || permision == PNone){
        for(MucActiveUserSeq::iterator it = targetseq.begin(); it != targetseq.end(); ++it){
          presenceUsers.push_back((*it)->jid);
        }
      }
    }
  }
  if(setok && opaptr && !presenceUsers.empty()){
    MucGateAdapter::instance().NotifySetPermision(opaptr, presenceUsers, targetseq, permision);
  }
  return setok;
}

JidSeq RoomPool::GetActiveJids(const MucRoomIdPtr& roomid){
	MCE_DEBUG("RoomPool::GetActiveJids --> roomname = " << roomid->roomname << " roomdomain = " << roomid->domain);
	bool hasroom = false;
	JidSeq imusers;
	MucActiveUserSeq activeusers;
	JidSeq ans;
	RoomInfoPtr riptr;
	{
		IceUtil::RWRecMutex::RLock lock(_rwmutex);
		riptr = GetRoom(roomid);
		if(riptr){
			hasroom = true;
			ans = riptr->GetAllActiveJids();
		}
	}
	if(!hasroom && PERMANENT == roomid->domain){
		MCE_DEBUG("RoomPool::GetActiveJids --> roomname = " << roomid->roomname << " pull from MucTalkRoom");
		try{
			activeusers = MucTalkRoomAdapter::instance().GetRoomActiveUserSeq(roomid);
		}
		catch(Ice::Exception& e){
			MCE_WARN("RoomPool::GetActiveJids --> call MucTalkRoom.GetRoomActiveUserSeq ERR  line:"<<__LINE__<<" err:"<<e);
		}
		catch(std::exception& e){
			MCE_WARN("RoomPool::GetActiveJids --> call MucTalkRoom.GetRoomActiveUserSeq ERR  line:"<<__LINE__<<" err:"<<e.what());
		}
		if(activeusers.empty()){
			MCE_DEBUG("RoomPool::GetActiveJids --> call MucTalkRoom.GetRoomActiveUserSeq empty result so return@@");
			for(MucActiveUserSeq::iterator ait = activeusers.begin(); ait != activeusers.end(); ++ait){
				ans.push_back((*ait)->jid);
			}
			return ans;
		}
		bool checkagain = false;
		IceUtil::RWRecMutex::WLock lock(_rwmutex);
		riptr = GetRoom(roomid);
		if(riptr){
			MCE_DEBUG("RoomPool::GetActiveJids --> after check again bool againcheck = true so only return room's all activeusers ");
			checkagain = true;
			ans = riptr->GetAllActiveJids();
		}
		if(!checkagain){
			MCE_DEBUG("RoomPool::GetActiveJids --> after check again bool againcheck = false we add web activeusers into server cache and send invite to imusers if have");
			bool hasaddroom = false;
			set<int> uids;
			MucActiveUserSeq::iterator it = activeusers.begin();
			for(; it != activeusers.end(); ++it){
				if((*it)->jid->endpoint[0] == 'W'){
					hasaddroom = true;
					MCE_DEBUG("RoomPool::GetActiveJids--> PUSH ACIVEUSER INFO : jid = " << jidToString((*it)->jid) << " roomname = " << (*it)->identity->roomid->roomname << " domain = " << (*it)->identity->roomid->domain << " permision = " << (*it)->permision);
					JoinRoom((*it));
				}
				else if((*it)->jid->endpoint[0] == 'T'){
					imusers.push_back((*it)->jid);
				}
				pair<set<int>::iterator, bool > insertres = uids.insert((*it)->jid->userId);
				if(insertres.second){
					ans.push_back((*it)->jid);
				}
			}
			if(!hasaddroom){
				CreateRoom(roomid);
			}
		}
	}
	if(!imusers.empty()){
		MCE_DEBUG("RoomPool::GetActiveJids send invite to imusers : size = " << imusers.size());
		MucGateAdapter::instance().InviteIMusers(roomid, imusers);
	}
	MCE_DEBUG("RoomPool::GetActiveJids --> roomname = " << roomid->roomname << " all activeusers.size = " << ans.size());
	for(JidSeq::iterator logit = ans.begin(); logit != ans.end(); ++logit){
		MCE_DEBUG("RoomPool::GetActiveJids --> activeuser info : roomname = " << roomid->roomname << " jid = " << jidToString((*logit)));
	}
	return ans;

}

MucUserIdentitySeq RoomPool::GetActiveUsers(const MucRoomIdPtr& roomid){
  MCE_DEBUG("RoomPool::GetActiveUsers --> roomname = " << roomid->roomname << " roomdomain = " << roomid->domain);
  bool hasroom = false;
  JidSeq imusers;
  MucActiveUserSeq activeusers;
  MucUserIdentitySeq ans;
  RoomInfoPtr riptr;
  {
    IceUtil::RWRecMutex::RLock lock(_rwmutex);
    riptr = GetRoom(roomid);
    if(riptr){
      hasroom = true;
      ans = riptr->GetAllUserIdentity();
    }
  }
  if(!hasroom && PERMANENT == roomid->domain){
    MCE_DEBUG("RoomPool::GetActiveUsers --> roomname = " << roomid->roomname << " pull from MucTalkRoom");
		try{
    	activeusers = MucTalkRoomAdapter::instance().GetRoomActiveUserSeq(roomid);
		}
		catch(Ice::Exception& e){
			MCE_WARN("RoomPool::GetActiveUsers --> call MucTalkRoom.GetRoomActiveUserSeq ERR  line:"<<__LINE__<<" err:"<<e);
		}
		catch(std::exception& e){
			MCE_WARN("RoomPool::GetActiveUsers --> call MucTalkRoom.GetRoomActiveUserSeq ERR  line:"<<__LINE__<<" err:"<<e.what());
		}
		if(activeusers.empty()){
			MCE_DEBUG("RoomPool::GetActiveUsers --> call MucTalkRoom.GetRoomActiveUserSeq empty result so return@@");
		  return ans;
		}
		bool checkagain = false;
		IceUtil::RWRecMutex::WLock lock(_rwmutex);
		riptr = GetRoom(roomid);
    if(riptr){
      MCE_DEBUG("RoomPool::GetActiveUsers --> after check again bool againcheck = true so only return room's all activeusers ");
      checkagain = true;
      ans = riptr->GetAllUserIdentity();
    }
    if(!checkagain){
      MCE_DEBUG("RoomPool::GetActiveUsers --> after check again bool againcheck = false we add web activeusers into server cache and send invite to imusers if have");
      bool hasaddroom = false;
      set<int> uids;
      MucActiveUserSeq::iterator it = activeusers.begin();
      for(; it != activeusers.end(); ++it){
        if((*it)->jid->endpoint[0] == 'W'){
          hasaddroom = true;
					MCE_DEBUG("RoomPool::GetActiveUsers --> PUSH ACIVEUSER INFO : jid = " << jidToString((*it)->jid) << " roomname = " << (*it)->identity->roomid->roomname << " domain = " << (*it)->identity->roomid->domain << " permision = " << (*it)->permision);
          JoinRoom((*it));
        }
        else if((*it)->jid->endpoint[0] == 'T'){
          imusers.push_back((*it)->jid);
        }
        pair<set<int>::iterator, bool > insertres = uids.insert((*it)->jid->userId);
        if(insertres.second){
          ans.push_back((*it)->identity);
        }
      }
      if(!hasaddroom){
        CreateRoom(roomid);
      }
    }
  }
  if(!imusers.empty()){
    MCE_DEBUG("RoomPool::GetActiveUsers send invite to imusers : size = " << imusers.size());
    MucGateAdapter::instance().InviteIMusers(roomid, imusers);
  }
  MCE_DEBUG("RoomPool::GetActiveUsers --> roomname = " << roomid->roomname << " all activeusers.size = " << ans.size());
	for(MucUserIdentitySeq::iterator logit = ans.begin(); logit != ans.end(); ++logit){
  	MCE_DEBUG("RoomPool::GetActiveUsers --> activeuser info : roomname = " << (*logit)->roomid->roomname << " nickname = " << (*logit)->nickname);
	}
  return ans;
}


MucActiveUserSeq RoomPool::GetUserByPermision(const MucRoomIdPtr& roomid, Permisions permision){
  MucActiveUserSeq ans;
  RoomInfoPtr riptr;
  IceUtil::RWRecMutex::RLock lock(_rwmutex);
  riptr = GetRoom(roomid);
  if(!riptr){
    return ans;
  }
  ans = riptr->GetActiveUserSeq(permision);
  return ans;
}
void RoomPool::SendPrivateMsg(const JidPtr& sender, const MucUserIdentityPtr& recidentity, const string& msg){
	MCE_DEBUG("RoomPool::SendPrivateMsg --> sender = " << jidToString(sender) << " recidentity = " << mucUserIdentityToString(recidentity) << " msg = " << msg);
	RoomInfoPtr riptr;
	MucActiveUserPtr opaptr, targetptr;
	{
  	IceUtil::RWRecMutex::RLock lock(_rwmutex);
  	riptr = GetRoom(recidentity->roomid);
  	opaptr = riptr->GetActiveUser(sender);
  	if(!opaptr){
			MCE_WARN("RoomPool::SendPrivateMsg --> get sender : " << jidToString(sender) << " 'S activeuser error");
    	return;
  	}
  	targetptr = riptr->GetActiveUser(recidentity->nickname);
  	if(!targetptr){
			MCE_WARN("RoomPool::SendPrivateMsg --> get targetptr ERROR");
    	return;
  	}
	}
	MCE_DEBUG("RoomPool::SendPrivateMsg --> CALL MucGate.NotifyPrivateChat");
  MucGateAdapter::instance().NotifyPrivateChat(opaptr, targetptr->jid->userId, msg);
}

//------------------------------------------------------------------------------

void PageCacheNotifyTask::handle(){
	/*
	MCE_DEBUG("PageCacheNotifyTask::handle --> action = " << _action << "_activeuser = " << _activeuser << " roomid = " << mucRoomIdToString(_roomid));
	if("online" == _action){
		PageCacheAdapter::instance().MemberOnline(GetIdFromMucRoomId(_roomid), _activeuser);
	}
	else if("offline" == _action){
		PageCacheAdapter::instance().MemberOffline(GetIdFromMucRoomId(_roomid), _activeuser);
	}
	*/
}
void PullLostUserTask::handle(){
	MCE_DEBUG("PullLostUserTask::handle --> jid = " << jidToString(_jid));
	try{
		MucGateAdapter::instance().UserOnline(_jid);
	}
	catch(Ice::Exception& e){
		MCE_WARN("PullLostUserTask::handle --> Call MucGate.UserOnline ERR  line:"<<__LINE__<<" err:"<<e);
	}
	catch(std::exception& e){
		MCE_WARN("PullLostUserTask::handle --> call MucGate.UserOnline ERR  line:"<<__LINE__<<" err:"<<e.what());
	}
}


#include "APNSManagerI.h"
#include "ServiceI.h"
using namespace xce::notify::apns;

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  string pemFile =
      service.getCommunicator()->getProperties()->getPropertyWithDefault(
          "Service." + ServiceI::instance().getName() + ".APNS_CLIENT_PEM",
          "/data/xce/Notify/etc/apns_client.pem");
  service.getAdapter()->add(&APNSManagerI::instance(), service.createIdentity(
      "M", ""));
  APNSManagerI::instance().setPemFile(pemFile);
  TaskManager::instance().scheduleRepeated(new KickTask());
}

UserPool::UserPool() {
  MCE_INFO("UserPool::UserPool --> Init _blockSet");
  _blockSet = new bool[BLOCK_SIZE];
  memset(_blockSet, 0, sizeof(_blockSet));
}

bool UserPool::getUserIfBlock(int userId) {
  return _blockSet[userId / 8] & (true << (userId % 8));
}

void UserPool::setUserToBlock(int userId) {
  _blockSet[userId / 8] |= (true << (userId % 8));
}

void UserPool::removeUserFromBlock(int userId) {
  _blockSet[userId / 8] &= !(true << (userId % 8));
}

MyUtil::Int2StrMap UserPool::locateTokenByUserIdSeq(
    const MyUtil::IntSeq& userIds) {
  MyUtil::Int2StrMap mp;
  set<int> missIds;
  MyUtil::IntSeq unblockSeq;

  for (size_t i = 0; i < userIds.size(); ++i) {
    if (!getUserIfBlock(userIds.at(i))) {
      unblockSeq.push_back(userIds.at(i));
    }
  }

  MCE_INFO("UserPool::locateTokenByUserIdSeq --> get users not in block befor size : " << userIds.size() << "seq size : " << unblockSeq.size());
  {
    IceUtil::RWRecMutex::RLock lock(_mutex);
    for (size_t i = 0; i < unblockSeq.size(); ++i) {
      UserIdIndex& index = _users.get<1> ();
      UserIdIndex::iterator iter = index.find(unblockSeq.at(i));
      if (iter != index.end()) {
        mp[unblockSeq.at(i)] = iter->token;
      } else {
        missIds.insert(unblockSeq.at(i));
      }
    }
    MCE_INFO("UserPool::locateTokenByUserIdSeq --> get miss id size :  " << missIds.size());
  }
  if (!missIds.empty()) {
    MCE_INFO("select missIds from db");
    Statement sql;
    sql << "select userid,token from " << DB_TABLE << " where userid in ( ";
    for (set<int>::iterator it = missIds.begin(); it != missIds.end(); ++it) {
      if (it != missIds.begin()) {
        sql << ",";
      }
      sql << *it;
    }
    sql << ");";

    {
      IceUtil::RWRecMutex::WLock lock(_mutex);
      mysqlpp::StoreQueryResult res = QueryRunner(DB_INSTANCE, CDbRServer,
          DB_TABLE).store(sql);
      if (res && res.num_rows() > 0) {
        for (size_t i = 0; i < res.num_rows(); ++i) {
          mysqlpp::Row row = res.at(i);
          string token = row["token"].c_str();
          int uid = (int) row["userid"];
          missIds.erase(uid);
          mp[uid] = token;

          std::pair<TokenContainer::iterator, bool> p = _users.push_front(
              Token(uid, token));
          if (!p.second) {
            MCE_WARN("sync err uid:"<<uid << ", token:"<<token);
          }

        }
      }
    }
    MCE_INFO("UserPool::locateTokenByUserIdSeq --> set userId in block which not find in db");
    for (set<int>::iterator it = missIds.begin(); it != missIds.end(); ++it) {
      MCE_INFO("UserPool::locateTokenByUserIdSeq --> block userId = " << *it);
      setUserToBlock(*it);
    }

  }

  return mp;
}

void UserPool::addUser(int userId, const string& token) {
  MCE_INFO("UserPool::addUser  userId = " << userId);
  {
    IceUtil::RWRecMutex::WLock lock(_mutex);
    MCE_DEBUG("UserPool::addUser --> remove out of block    userId = " << userId);
    removeUserFromBlock(userId);
    std::pair<TokenContainer::iterator, bool> p = _users.push_front(Token(
        userId, token));
    if (!p.second) {
      MCE_DEBUG("UserPool::addUser --> replace   userId=" << userId);
      Token tmp = *(p.first);
      tmp.userId = userId;
      tmp.token = token;
      tmp.timestamp = time(NULL);
      _users.replace(p.first, tmp);
      MCE_DEBUG("UserPool::addUser --> relocate  userId="<< userId );
      _users.relocate(_users.begin(), p.first);
    } else if (_users.size() > USER_SIZE) {
      MCE_DEBUG("UserPool::addUser --> pop_back");
      _users.pop_back();
    }
  }
  MCE_INFO("UserPool::addUser  DB --> insert into db duplicate key update");

  {
    Statement sql;
    sql << "delete  from " << DB_TABLE << " where userid = " << userId;
    QueryRunner(DB_INSTANCE, CDbWServer, DB_TABLE).store(sql);
  }

  {
    Statement sql;
    sql << "insert into " << DB_TABLE << "(userid, token, logintime) values("
        << userId << ", '" << token << "', '" << Date(::time(NULL)).str(
        "%Y-%m-%d")
        << "') on duplicate key update userid = values(userid), token = values(token), logintime = values(logintime);";
    QueryRunner(DB_INSTANCE, CDbWServer, DB_TABLE).store(sql);
  }
}
void UserPool::removeUserByToken(const string& token) {
  MCE_INFO("UserPool::removeUserByToken  DB --> removefromdb");
  Statement sql;
  sql << "delete from " << DB_TABLE << " where token = '" << token << "');";
  QueryRunner(DB_INSTANCE, CDbWServer, DB_TABLE).store(sql);
  MCE_INFO("UserPool::removeUserByToken  cache --> removefrom cache");
  IceUtil::RWRecMutex::WLock lock(_mutex);
  TokenIndex& index = _users.get<2> ();
  TokenIndex::iterator iter = index.find(token);
  MCE_DEBUG("UserPool::removeUserByToken --> set user to block userId = " << iter->userId);
  setUserToBlock(iter->userId);
  if (iter != index.end()) {
    index.erase(iter);
  }
}

void UserPool::kick() {
  MCE_INFO("UserPool::durationEvict  -->  ");
  MyUtil::IntSeq seq;
  int my_timeout = 48 * 60 * 60;
  {
    MCE_INFO("UserPool::durationEvict --> get EvictId");
    IceUtil::RWRecMutex::RLock lock(_mutex);
    SequencedIndex& index = _users.get<0> ();
    SequencedIndex::iterator iter = index.begin();
    for (; iter != index.end(); ++iter) {
      int now = time(NULL);
      if (now - (iter->timestamp) >= my_timeout) {
        seq.push_back(iter->userId);
      }
    }
  }
  if (seq.size()) {
    MCE_INFO("UserPool::durationEvict --> delete from db");
    Statement sql;
    sql << "delete from " << DB_TABLE << " where userid in (";
    for (size_t i = 0; i < seq.size(); ++i) {
      if (i) {
        sql << ", ";
      }
      sql << seq.at(i);
    }
    sql << ");";
    QueryRunner(DB_INSTANCE, CDbWServer, DB_TABLE).store(sql);
  }
  {
    MCE_INFO("UserPool::durationEvict --> delete cache");
    IceUtil::RWRecMutex::WLock lock(_mutex);
    SequencedIndex& index = _users.get<0> ();
    SequencedIndex::iterator iter = index.begin();
    for (; iter != index.end(); ++iter) {
      int now = time(NULL);
      if (now - (iter->timestamp) > my_timeout) {
        index.erase(iter);
      }
    }
  }
}

PusherPtr APNSManagerI::getPusher(int userId) {
  if (userId >= 0) {
    return _pusher[userId % PUSHER_SIZE];
  } else {
    MCE_WARN("APNSManagerI::getPusher --> userId out of range;");
  }
  return NULL;
}

void APNSManagerI::kick() {
  MCE_INFO("APNSManagerI::durationEvict  --->");
  _pool.kick();
}
void APNSManagerI::setPemFile(const string& pf) {
  MCE_INFO("APNSManagerI::setPemFile --> " << pf);
  _pemFile = pf;
  init();
}

void APNSManagerI::init() {
  MCE_INFO("APNSManager::init --> Init _pusherInit");
  _pusher.resize(PUSHER_SIZE);
  for (size_t i = 0; i < PUSHER_SIZE; ++i) {
    _pusher[i] = new APNSPusher(_pemFile, "1234");
  }
  TaskManager::instance().execute(new InitPusherTask());
}

void APNSManagerI::addUser(int userId, const string& token, const Ice::Current&) {
  MCE_INFO("APNSManagerI::addUser --> userId = " << userId << "token = " << token);
  _pool.addUser(userId, token);
}
void APNSManagerI::addIphoneNotifyBySeq(const MyUtil::IntSeq& userIds,
    const string& msg, const Ice::Current&) {
  MCE_INFO("APNSManagerI::addIphoneNotifyBySeq ---> msg = " << msg);
  MyUtil::Int2StrMap mp = _pool.locateTokenByUserIdSeq(userIds);
  MyUtil::Int2StrMap::iterator it = mp.begin();
  for (; it != mp.end(); ++it) {
    MsgEntry me;
    MCE_DEBUG("push msg : userId = " << it->first << "   token = " << it->second << "  msg = " << msg);
    me.tokenStr = it->second;
    me.payload = msg;
    PusherPtr pusherPtr = getPusher(it->first);
    pusherPtr->insertMsg(me);
  }
}

void APNSManagerI::removeUserByToken(const string& token, const Ice::Current&) {
  MCE_INFO("APNSManagerI::removeUserByToken --> token = " << token);
  _pool.removeUserByToken(token);
}
void APNSManagerI::initPusher() {
  MCE_INFO("APNSManagerI::initPusher--> Init PuserPool");
  for (size_t i = 0; i < PUSHER_SIZE; ++i) {
    //MCE_DEBUG(__LINE__ << ", p:"<< _pusher[i]);
    if (_pusher[i]->init()) {
      MCE_INFO("APNSManagerI::initPusher--> Init PuserPool  ----->  index = " << i << "     OK");
    } else {
      MCE_WARN("APNSManagerI::initPusher-->    Init Puser  index " << i << " ERROR!!!!!!!!!!!!!!!!!!" );
    }
  }

}

void KickTask::handle() {
  MCE_INFO("DurationStatTask::handle --> durationEvict");
  APNSManagerI::instance().kick();
}

void InitPusherTask::handle() {
  MCE_INFO("InitPusherTask::handle--> Init PuserPool");
  APNSManagerI::instance().initPusher();
}


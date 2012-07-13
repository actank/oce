/*
 * parser.h
 *
 *  Created on: Apr 8, 2010
 *  Author: yejingwei
 */
#include "FeedConfigI.h"
#include <exception>
#include <iostream>
#include <ostream>
#include <QueryRunner.h>
#include "ServiceI.h"
#include <Ice/Exception.h>
using namespace std;
using namespace xce::feed;
using namespace com::xiaonei::xce;
using namespace boost::multi_index;
using namespace boost;

void MyUtil::initialize() {

  ServiceI& service = ServiceI::instance();
  ConfigManagerI& config_manager = ConfigManagerI::instance();

  service.getAdapter()->add(&config_manager, service.createIdentity("M", ""));

}

//---------------------------------------------------------------------------
static int RealType(int & type, int appid) {
  if (type == 81 ) {
    return 0 - appid;
  } else {
    return type;
  }
}

//---------------------------------------------------------------------------

bool SendConfig::Find(int type_or_appid) {
  MCE_DEBUG("SendConfig::Find --> type_or_appid: " << type_or_appid);

  set<int>::iterator sit = config_.find(type_or_appid);

  if (sit != config_.end()) {

    MCE_DEBUG("SendConfig::Find -->"
        << " user : " << uid_
        <<" type_or_appid: " << type_or_appid
        <<" newsConfig: false " );

    return true; //找到type_or_appid

  } else {
    MCE_DEBUG("SendConfig::Find -->"
        << "user :" << uid_ << " no config for:"
        <<" type_or_appid " << type_or_appid);

    return false; //没找到
  }
}

void SendConfig::Update(int type_or_appid, bool news_config) {
  /*  MCE_DEBUG("SendConfig::Update --> "
   << " type_or_appid = " << type_or_appid
   << " news_config = " << news_config);*/

  if (news_config) {
    set<int>::iterator it = config_.find(type_or_appid);
    if (it != config_.end()) {
      config_.erase(it);
      /*      MCE_DEBUG("SendConfig::Update --> erase "
       << " type_or_appid = " << type_or_appid);*/
    }

  } else {
    config_.insert(type_or_appid);
    /*    MCE_DEBUG("SendConfig::Update --> insert "
     << " type_or_appid = " << type_or_appid);*/
  }
}

//---------------------------------------------------------------------------


void DefaultConfigCache::Set(const Ice::Int uid, bool is_default) {
  //	MCE_DEBUG("DefaultConfigCache::Set --> uid = " << uid << " is_default = " << is_default);
  size_t array_index = uid / CHAR_BIT;
  size_t bit_index = uid % CHAR_BIT;

  unsigned char mask = 1 << bit_index;

  if (is_default) { //说明用户没有设定任何type
    cache_array_[array_index] |= mask;//对应的位设为1
  } else { //false，说明用户设定了至少一种type
    mask = ~mask;
    cache_array_[array_index] &= mask;//对应为设为0
  }

}

bool DefaultConfigCache::Get(const Ice::Int uid) {
  //MCE_DEBUG("DefaultConfigCache::Get");
  size_t array_index = uid / CHAR_BIT;
  size_t bit_index = uid % CHAR_BIT;

  unsigned char mask = 1 << bit_index;

  bool ret = cache_array_[array_index] &= mask;

  /*MCE_DEBUG("DefaultConfigCache::Get --> uid: " << uid << " is_default: " << ret);*/
  return ret;

}

size_t DefaultConfigCache::CountSetBits() {
  size_t result = 0;
  for (size_t i = 0; i < kDefaultConfigCacheSize; ++i) {
    unsigned char tmp = cache_array_[i];
    for (int j = 0; j < CHAR_BIT; ++j) {
      unsigned char mask = 1 << j;
      if (!(tmp & mask)) { //第j位为0，用户已经设置了
        ++result;
      }
    }
  }

  return result;
}
//---------------------------------------------------------------------------

void SendConfigCache::SetConfig(Ice::Int uid, Ice::Int type, Ice::Int appid,
    bool news_config) {

  MCE_DEBUG("SendConfigCache::SetConfig");

  if (news_config) { //设置发送，则删除db和send_config_cache中相关数据
    MCE_DEBUG("SendConfigCache::SetConfig  --> delete from db and cache");
    Statement sql;
    sql << "DELETE FROM feed_send_config WHERE uid = " << uid
        << " AND feed_type = " << type << " AND app_id = " << appid;
    try {
      QueryRunner("feed_db", CDbRServer, "feed_send_config").store(sql);
    } catch (Ice::Exception& e) {
      MCE_WARN("SendConfigCache::SetConfig --> error:" << e);
    } catch (std::exception& e) {
      MCE_WARN("SendConfigCache::SetConfig --> error:" << e.what());
    }

    Update(uid, type, appid, true);

  } else { //不发送， 插入db和send_config_cache
    MCE_DEBUG("SendConfigCache::SetConfig  --> insert into db and cache");
    Statement sql;
    sql << "REPLACE feed_send_config (uid,feed_type,app_id,news,mini) VALUE( "
        << uid << "," << type << "," << appid << "," << 0 << "," << 0 << ")";
    try {
      QueryRunner("feed_db", CDbRServer, "feed_send_config").store(sql);
    } catch (Ice::Exception& e) {
      MCE_WARN("SendConfigCache::SetConfig --> error:" << e);
    } catch (std::exception& e) {
      MCE_WARN("SendConfigCache::SetConfig --> error:" << e.what());
    }

    Update(uid, type, appid, false);
  }

}

bool SendConfigCache::GetConfig(Ice::Int uid, Ice::Int type, Ice::Int appid) {

  int type_or_appid = RealType(type, appid);
  MCE_DEBUG("SendConfigCache::GetConfig --> "
      << "uid = " << uid
      << ",type = " << type
      << ",appid = " << appid
      << ",type_or_appid = " << type_or_appid);

  //TODO: default_config_cache_
  if (default_config_cache_.Get(uid)) {

    MCE_DEBUG("SendConfigCache::GetConfig --> "
        "default_config_cache_ contains uid: " << uid);
    return true;
  }

  MCE_DEBUG("SendConfigCache::GetConfig --> "
      "default_config_cache_ NO uid: " << uid);

  bool result = true;

  if (Find(uid, type_or_appid, result)) { //在cache中找到uid
    if (result) { //没有该type_or_appid记录
      MCE_DEBUG("SendConfigCache::GetConfig --> cache:  "
          << "uid: " << uid
          << " type_or_appid: " << type_or_appid
          << " result: " << result);
      return true;
    } else {//有该type_or_appid记录
      MCE_DEBUG("SendConfigCache::GetConfig --> cache:  "
          << "uid: " << uid
          << " type_or_appid: " << type_or_appid
          << " result: " << result);
      return false;
    }

  }

  MCE_DEBUG("SendConfigCache::GetConfig --> "
      "cache_ NO uid: " << uid);
  //如果cache中没有该uid
  Statement sql;
  sql << "SELECT * FROM feed_send_config WHERE uid=" << uid;
  mysqlpp::StoreQueryResult res = QueryRunner("feed_db", CDbRServer,
      "feed_send_config").store(sql);

  if (res.empty()) { //cache和数据库中都没有该uid记录
    MCE_DEBUG("SendConfigCache::GetConfig --> user uid: " << uid
        << " does NOT set ANY config " );

    //TODO::
    default_config_cache_.Set(uid, true);

    MCE_DEBUG("SendConfigCache::GetConfig --> set default_config_cache_["
        << uid << "] = 1");

    return true;
  }

  //cache中没有该uid记录，但是db中有

  MCE_DEBUG("SendConfigCache::GetConfig --> user uid: " << uid
      << " set some configs in db.");
  SendConfigPtr config = new SendConfig; //关于uid的所有设置都放入一个config

  result = true;

  for (size_t i = 0; i < res.size(); ++i) {
    int type = (int) res.at(i)["feed_type"];
    int appid = (int) res.at(i)["app_id"];

    MCE_DEBUG("SendConfigCache::GetConfig --> one row of db result: "
        <<" uid: " << uid
        <<",type: " << type
        <<",appid: " << appid);

    int tmp_type = RealType(type,appid);
    if (tmp_type == type_or_appid) {
      MCE_DEBUG("\n\t this type match ");
      result = false;
    }
    config->Update(tmp_type, false);//从数据库中查到的type对应的news_config都是false
  }
  config->uid_ = uid;
  Add(config);//读取db后更新cache

  return result;
}

void SendConfigCache::Update(int uid, int type, int appid, bool news) {
  IceUtil::Mutex::Lock lock(mutex_);
  MCE_DEBUG("SendConfigCache::Update");

  int realtype = RealType(type, appid);
  UidIndex & index = container_.get<1> ();
  UidIndex::iterator uit = index.find(uid);
  if (uit != index.end()) {
    (*uit)->Update(realtype, news);

    //TODO::
    if (news) { //如果删除了config
      //如果cache中的一个uid的send config更新后为空，
      //说明该用户已经全部为默认设置
      if (!(*uit)->Size()) {
        MCE_DEBUG("SendConfigCache::Update --> set default_config_cache_["
            << uid <<"] = 1");
        default_config_cache_.Set(uid, true);
        MCE_DEBUG("SendConfigCache::Update --> erase uid: " << uid << " from cache.");
        index.erase(uit);
      }
    } else { //如果添加了config
      MCE_DEBUG("SendConfigCache::Update --> set default_config_cache_["
          << uid <<"] = 0");
      default_config_cache_.Set(uid, false);
    }

    MCE_DEBUG("SendConfigCache::Update --> update  user uid = "
        << uid << ",realtype  = " << realtype << ",news = " << news);
  } else {
    MCE_DEBUG("SendConfigCache::Update --> no uid:" << uid << " in SendConfigCache, no update.");
    default_config_cache_.Set(uid, false);
  }
}

bool SendConfigCache::Find(int uid, int type_or_appid, bool& result) {
  MCE_DEBUG("SendConfigCache::Find --> cache size : " << container_.size());
  IceUtil::Mutex::Lock lock(mutex_);
  UidIndex & index = container_.get<1> ();
  UidIndex::iterator uit = index.find(uid);

  if (uit != index.end()) { //在cache有

    if ((*uit)->Find(type_or_appid)) {
      result = false; //cache中有该type_or_appid,结果为false
    } else {
      result = true;
    }

    SeqIndex::iterator sit = container_.project<0> (uit);//新查的放到前面
    container_.relocate(container_.get<0> ().begin(), sit);

    MCE_DEBUG("SendConfigCache::Find --> "
        << "User: " << uid << " is in cache.  newsConfig: " << result);
    return true;
  }
  MCE_DEBUG("SendConfigCache::Find --> "
      << "User: " << uid << " is NOT in cache");
  return false;
}

void SendConfigCache::Add(const SendConfigPtr& input) {
  MCE_DEBUG("SendConfigCache::Add -->"
      << "input config uid = " << input->uid_
      << " size = " << input->Size());

  IceUtil::Mutex::Lock lock(mutex_);
  SeqIndex & sindex = container_.get<0> ();
  std::pair<SeqIndex::iterator, bool> p = sindex.push_front(input);
  if (!p.second) { //插入不成功，说明原来已经有这个元素
    MCE_DEBUG("SendConfigCache::Add --> "
        << "insert fail,relocate ...");
    sindex.relocate(sindex.begin(), p.first);
  } else { //插入成功，运行kick
    MCE_DEBUG("SendConfigCache::Add --> "
        << "insert success,Kick ...");
    Kick();
  }
}

void SendConfigCache::Kick() {
  MCE_DEBUG("SendConfigCache::Kick " << "cache size" << container_.size());
  while (container_.size() > kSendPoolSize) {
    container_.pop_back();
  }
}

void SendConfigCache::Preload() {

  Statement sql;
  sql << "SELECT count(*) , count(distinct uid) ,max(id) FROM feed_send_config";
  mysqlpp::StoreQueryResult res = QueryRunner("feed_db", CDbRServer,
      "feed_send_config").store(sql);
  int maxid = res[0]["max(id)"];//最大的id值
  int ConfigRows = res[0]["count(*)"];//所有config的数量
  int Uidsdistinct = res[0]["count(distinct uid)"];//所有uid数量
  MCE_DEBUG("RecvConfigCache::Preload -->  total feed_send_config rows in db size = " << ConfigRows
      << ", distinct uid = " << Uidsdistinct << ", maxid = " << maxid );

  int step = 10000;
  for (int i = 0; i < maxid; i += step) {
    Statement sql;
    sql << "SELECT * FROM feed_send_config WHERE id > " << i << " AND id <= "
        << i + step;
    mysqlpp::StoreQueryResult res = QueryRunner("feed_db", CDbRServer,
        "feed_send_config").store(sql);
    MCE_DEBUG("SendConfigCache::Preload --> query db , result size = " << res.size());
    for (size_t j = 0; j < res.size(); ++j) {
      int uid = (int) res.at(j)["uid"];
      int type = (int) res.at(j)["feed_type"];
      int appid = (int) res.at(j)["app_id"];
      bool news = (bool) res.at(j)["news"];
      int realType = RealType(type,appid);
      UidIndex& index = container_.get<1> ();
      UidIndex::iterator uit = index.find(uid);
      if (uit == index.end()) {//在cache中没有这个id
        SendConfigPtr config = new SendConfig;
        //MCE_DEBUG("Preload::Preload --> new uid config --> uid = " << uid);
        config->uid_ = uid;
        config->Update(realType, news);
        index.insert(config);
        continue;
      }
      (*uit)->Update(realType, news);
    }
    MCE_DEBUG("SendConfigCache::Preload --> SendConfigCache size = " << container_.size());
  }
  MCE_INFO("SendConfigCache::Preload --> FINISH. SendConfigCache TOTAL size = " << container_.size());

}

//---------------------------------------------------------------------------

bool RecvConfig::Find(int type_or_appid) {
  /*	MCE_DEBUG("RecvConfig::Find");*/

  set<int>::iterator sit = config_.find(type_or_appid);
  if (sit != config_.end()) {

    /*		MCE_DEBUG("RecvConfig::Find -->"
     << " user : " << uid_
     <<" type_or_appid: " << type_or_appid
     <<" newsConfig: false " );*/

    return true;//找到type_or_appid
  } else {
    /*		MCE_DEBUG("RecvConfig::Find -->"
     << "user :" << uid_ << " no config for:"
     <<" type_or_appid " << type_or_appid);*/
    return false;
  }

}

void RecvConfig::Update(int type_or_appid, bool news_config) {
  /*  MCE_DEBUG("RecvConfig::Update --> "
   << " type_or_appid = " << type_or_appid
   << " news_config = " << news_config);*/

  if (news_config) { //true
    set<int>::iterator it = config_.find(type_or_appid);
    if (it != config_.end()) {
      config_.erase(it);
      /*      MCE_DEBUG("RecvConfig::Update --> erase "
       << " type_or_appid = " << type_or_appid);*/
    }

  } else {
    config_.insert(type_or_appid);
    //MCE_DEBUG("RecvConfig::Update --> insert " 	<< " type_or_appid = " << type_or_appid);
  }

}

//---------------------------------------------------------------------------


void RecvConfigCache::SetConfig(Ice::Int uid, Ice::Int type, Ice::Int appid,
    bool news_config) {
  MCE_DEBUG("RecvConfigCache::SetConfig");

  if (news_config) { //设置发送，删除db和recv_config_cache中相关数据
    MCE_DEBUG("RecvConfigCache::SetConfig  --> delete from db and cache");
    Statement sql;
    sql << "DELETE FROM feed_recv_config WHERE uid = " << uid
        << " AND feed_type = " << type << " AND app_id = " << appid;
    try {
      QueryRunner("feed_db", CDbRServer, "feed_recv_config").store(sql);
    } catch (Ice::Exception& e) {
      MCE_WARN(" RecvConfigCache::SetConfig --> error:" << e);
    } catch (std::exception& e) {
      MCE_WARN(" RecvConfigCache::SetConfig --> error:" << e.what());
    }

    Update(uid, type, appid, true);

  } else { //不发送， 插入db和recv_config_cache
    MCE_DEBUG("RecvConfigCache::SetConfig  --> insert into db and cache");
    Statement sql;
    sql << "REPLACE feed_recv_config (uid,feed_type,app_id,news) VALUE( "
        << uid << "," << type << "," << appid << "," << 0 << ")";
    try {
      QueryRunner("feed_db", CDbRServer, "feed_recv_config").store(sql);
    } catch (Ice::Exception& e) {
      MCE_WARN(" RecvConfigCache::SetConfig --> error:" << e);
    } catch (std::exception& e) {
      MCE_WARN(" RecvConfigCache::SetConfig --> error:" << e.what());
    }

    Update(uid, type, appid, false);
  }

}

MyUtil::IntArray RecvConfigCache::Filter(const MyUtil::IntArray& target,
    Ice::Int type, Ice::Int appid) {
  int type_or_appid = RealType(type, appid);//判断是普通feed还是app
  MCE_DEBUG("RecvConfigCache::Filter --> type = " << type
      << " , appid = " << appid << ", target size = " << target.size());

  set<int> uncached;//在内存中没要找到的，需要在db中查找的

  MyUtil::IntArray rt_seq;

  set<int> not_recv_ids;

  not_recv_ids = Find(target, type_or_appid, uncached);//在cache中查找，并过滤
  rt_seq.assign(not_recv_ids.begin(), not_recv_ids.end());
  MCE_DEBUG("RecvConfigCache::Filter --> target size = " << target.size()
      << " not_recv_ids size = " << not_recv_ids.size()
      << " uncached size =" << uncached.size());

  if (uncached.size() == 0) { //不需要在数据库中查询
    MCE_DEBUG("RecvConfigCache::Filter --> All recv configs are in cache.");
    return rt_seq;
  }

  //查找db，将config加入cache，并且返回
  Statement sql;
  sql << "SELECT * FROM feed_recv_config WHERE uid IN (";
  set<int>::iterator itr = uncached.begin();
  sql << *itr;
  for (++itr; itr != uncached.end(); ++itr) {
    sql << "," << *itr;
  }
  sql << ") ORDER BY uid"; //将uid所有的记录都取出。
  mysqlpp::StoreQueryResult res;
  try {
    res = QueryRunner("feed_db", CDbRServer, "feed_recv_config").store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN(" RecvConfigCache::Filter --> error:" << e);
  } catch (std::exception& e) {
    MCE_WARN(" RecvConfigCache::Filter --> error:" << e.what());
  }

  if (res.empty()) {
    MCE_DEBUG("RecvConfigCache::Filter --> no uid in uncached is found from db");

    //TODO::
    for (set<int>::const_iterator it = uncached.begin(); it != uncached.end(); it++) {
      default_config_cache_.Set(*it, true);
      /*			MCE_DEBUG("RecvConfigCache::Filter --> set default_config_set_ :" << *it);*/
    }

    return rt_seq;
  }

  //部分uncached 中的uid在db中查到
  MCE_DEBUG("\tRecvConfigCache::Filter --> Found some uids in uncached from db");

  RecvConfigPtr cur_config;
  int pre_uid = -1;
  for (size_t i = 0; i < res.size(); i++) {
    int uid = (int) res.at(i)["uid"];
    int type = (int) res.at(i)["feed_type"];
    int appid = (int) res.at(i)["app_id"];
    bool news = (bool) res.at(i)["news"];

    int realType = RealType(type,appid);

    uncached.erase(uid);

    if (realType == type_or_appid) { // 数据库中有该一个uid type_or_appid记录，表示不接收

      rt_seq.push_back(uid);

    }

    //更新cache
    if (pre_uid != uid) {//当前uid和pre_uid不同,说明pre_uid的记录已经遍历完毕
      cur_config = new RecvConfig;
      cur_config->uid_ = uid;
      Add(cur_config);
      //MCE_DEBUG("RecvConfigCache::Filter --> Update id:" << uid);
      cur_config->Update(realType, false);
      pre_uid = uid;
    } else {
      /*			MCE_DEBUG("RecvConfigCache::Filter --> Update id:" << uid);*/
      cur_config->Update(realType, false);
    }

    if (news) {
      /*			MCE_WARN("RecvConfigCache::Filter --> db data error:  uid: "
       << uid <<" type: " << type << " appid:" <<appid);*/
      default_config_cache_.Set(uid, false);
      Update(uid, type, appid, true);
    }

  }

  //TODO::
  //uncached中没有在db存储的uid，进入default_config_cache_
  for (set<int>::iterator sit = uncached.begin(); sit != uncached.end(); ++sit) {

    default_config_cache_.Set(*sit, true);
    //MCE_DEBUG("RecvConfigCache::Filter --> set default_config_cache_ :" << *sit);
  }

  return rt_seq;

}

void RecvConfigCache::Update(int uid, int type, int appid, bool news) {
  IceUtil::Mutex::Lock lock(mutex_);
  MCE_DEBUG("SendConfigCache::Update");

  int realtype = RealType(type, appid);
  UidIndex & index = container_.get<1> ();
  UidIndex::iterator uit = index.find(uid);
  if (uit != index.end()) {
    (*uit)->Update(realtype, news);

    //TODO::
    if (news) { //如果删除了config
      //如果cache中的一个uid的send config更新后为空，
      //说明该用户已经全部为默认设置
      if (!(*uit)->Size()) {
        MCE_DEBUG("RecvConfigCache::Update --> set default_config_cache_["
            << uid <<"] = 1");
        default_config_cache_.Set(uid, true);
        MCE_DEBUG("RecvConfigCache::Update --> erase uid: " << uid << " from cache.");
        index.erase(uit);
      }
    } else { //如果添加了config
      MCE_DEBUG("RecvConfigCache::Update --> set default_config_cache_["
          << uid <<"] = 0");
      default_config_cache_.Set(uid, false);
    }

    MCE_DEBUG("RecvConfigCache::Update --> update  user uid = "
        << uid << ",realtype  = " << realtype << ",news = " << news);

  } else {
    MCE_DEBUG("RecvConfigCache::Update --> no uid:" << uid << " in RecvConfigCache, no update.");
    default_config_cache_.Set(uid, false);
  }
}

set<int> RecvConfigCache::Find(const MyUtil::IntArray& target,
    int type_or_appid, set<int> &uncached) {
  MCE_DEBUG("RecvConfigCache::Find --> cache size: " << container_.size()
      << " target size: " << target.size() << ", type_or_appid :" << type_or_appid );
  IceUtil::Mutex::Lock lock(mutex_);

  //TODO::
  MyUtil::IntArray target_not_default_cache;

  for (MyUtil::IntArray::const_iterator it = target.begin(); it != target.end(); it++) {

    if (!default_config_cache_.Get(*it)) {

      target_not_default_cache.push_back(*it);
      /*			MCE_DEBUG("RecvConfigCache::Find --> target_not_default_cache.push_back: "<< *it
       << "  target_not_default_cache size: " << target_not_default_cache.size());*/
    }
  }
  MCE_DEBUG("RecvConfigCache::Find --> target_not_default_cache size: " << target_not_default_cache.size());
  set<int> not_recv_ids;

  UidIndex& index = container_.get<1> ();

  for (MyUtil::IntArray::const_iterator target_itr =
      target_not_default_cache.begin(); target_itr
      != target_not_default_cache.end(); ++target_itr) {

    UidIndex::iterator uit = index.find(*target_itr);

    if (uit == index.end()) {//在cache中没有这个id
      uncached.insert(*target_itr);//把这个id加入uncached中
      /*      MCE_DEBUG("RecvConfigCache::Find --> uncached insert: " << *target_itr
       << " uncached size= " << uncached.size());*/
      continue;
    }

    SeqIndex::iterator sit = container_.project<0> (uit); //更新cache
    container_.relocate(container_.begin(), sit);

    if ((*uit)->Find(type_or_appid)) {//cache中有这个id，并且这个id有type_or_appid记录，则表示不发送

      /*    	MCE_DEBUG("RecvConfigCache::Find -->  uid: " << *target_itr << " type_or_appid: " << type_or_appid
       << " news_config: false");*/

      not_recv_ids.insert(*target_itr);
      continue;

    } else {
      /*			MCE_DEBUG("RecvConfigCache::Find -->  uid: " << *target_itr << " type_or_appid: " << type_or_appid
       << " NOT in cache.  news_config: true");*/
    }

  }
  if (uncached.empty()) {
    return not_recv_ids;
  }
  MCE_DEBUG("RecvConfigCache::Find --> uncached size = " << uncached.size());
  return not_recv_ids;
}

void RecvConfigCache::Add(const RecvConfigPtr& input) {
  /*	MCE_DEBUG("RecvConfigCache::Add --> uid = " << input->uid_
   << " size = " << input->Size());*/

  IceUtil::Mutex::Lock lock(mutex_);
  std::pair<iterator, bool> p = container_.push_front(input);
  if (!p.second) {
    container_.relocate(container_.begin(), p.first);
  } else {
    Kick();
  }
}

void RecvConfigCache::Kick() {
  while (container_.size() > kSendPoolSize) {
    container_.pop_back();
  }
}

void RecvConfigCache::Preload() {
  Statement sql;
  sql << "SELECT count(*),count(distinct uid) ,max(id) FROM feed_send_config";
  mysqlpp::StoreQueryResult res = QueryRunner("feed_db", CDbRServer,
      "feed_send_config").store(sql);
  int maxid = res[0]["max(id)"];
  int ConfigRows = res[0]["count(*)"];
  int Uidsdistinct = res[0]["count(distinct uid)"];
  MCE_DEBUG("RecvConfigCache::Preload -->  total feed_send_config rows in db size = " << ConfigRows
      << ", distinct uid = " << Uidsdistinct << ", maxid = " << maxid);
  //RecvConfigCount = 800000;
  //MCE_DEBUG("Preload::Preload --> feed_send_config debug size = " << RecvConfigCount );
  int step = 10000;
  for (int i = 0; i < maxid; i += step) {
    Statement sql;
    sql << "SELECT * FROM feed_recv_config WHERE id > " << i << " AND id <= "
        << i + step;
    mysqlpp::StoreQueryResult res = QueryRunner("feed_db", CDbRServer,
        "feed_send_config").store(sql);
    MCE_DEBUG("RecvConfigCache::Preload --> db result size = " << res.size());
    for (size_t j = 0; j < res.size(); ++j) {
      int uid = (int) res.at(j)["uid"];
      int type = (int) res.at(j)["feed_type"];
      int appid = (int) res.at(j)["app_id"];
      bool news = (bool) res.at(j)["news"];
      int realType = RealType(type,appid);

      UidIndex& index = container_.get<1> ();
      UidIndex::iterator uit = index.find(uid);
      if (uit == index.end()) {//在cache中没有这个id
        RecvConfigPtr config = new RecvConfig;
        //MCE_DEBUG("Preload::Preload --> new uid config --> uid = " << uid);
        config->uid_ = uid;
        config->Update(realType, news);
        index.insert(config);
        continue;
      }
      (*uit)->Update(realType, news);
    }
    MCE_DEBUG("RecvConfigCache::Preload --> RecvConfigCache size = " << container_.size() );
  }
  MCE_INFO("RecvConfigCache::Preload --> FINISH. RecvConfigCache TOTAL size = " << container_.size());
}

//-----------------------------------------------------------------------------------------------


void ConfigManagerI::SetSendConfig(Ice::Int uid, Ice::Int type,
    Ice::Int appid, bool newsconfig, const ::Ice::Current&) {

  send_config_cache_ -> SetConfig(uid, type, appid, newsconfig);
}

bool ConfigManagerI::GetSendConfig(Ice::Int uid, Ice::Int type,
    Ice::Int appid, const ::Ice::Current&) {
  return send_config_cache_ -> GetConfig(uid, type, appid);
}

void ConfigManagerI::SetRecvConfig(Ice::Int uid, Ice::Int type,
    Ice::Int appid, bool newsconfig, const ::Ice::Current&) {
  recv_config_cache_ -> SetConfig(uid, type, appid, newsconfig);
}

MyUtil::IntArray ConfigManagerI::FilterRecvConfig(
    const MyUtil::IntArray& target, Ice::Int type, Ice::Int appid,
    const ::Ice::Current&) {

  return recv_config_cache_ -> Filter(target, type, appid);
}

void ConfigManagerI::SetUserConfig(Ice::Int uid, Ice::Int type,
    Ice::Int appid, bool sendConfig, bool recvConfig, const ::Ice::Current&) {

  send_config_cache_ -> SetConfig(uid, type, appid, sendConfig);
  recv_config_cache_ -> SetConfig(uid, type, appid, recvConfig);

}

UserConfig ConfigManagerI::GetUserConfig(Ice::Int uid, Ice::Int type,
    Ice::Int appid, const ::Ice::Current&) {

  bool send_config = send_config_cache_ -> GetConfig(uid, type, appid);

  bool recv_config = true;

  MyUtil::IntArray target;
  target.push_back(uid);
  MyUtil::IntArray no_recv_uids = recv_config_cache_ -> Filter(target, type,
      appid);
  if (no_recv_uids.size()) {
    recv_config = false;
  }

  UserConfig rt_config;
  rt_config.sendConfig = send_config;
  rt_config.recvConfig = recv_config;

  return rt_config;
}

void ConfigManagerI::Preload(const ::Ice::Current&) {
  try {
    send_config_cache_->Preload();
    recv_config_cache_->Preload();
  } catch (Ice::Exception& e) {
    MCE_WARN("ConfigManagerI::Preload --> exception :" <<e);
  } catch (std::exception& e) {
    MCE_WARN("ConfigManagerI::Preload --> exception :" << e.what());
  }
}


UserConfigDict ConfigManagerI::GetUserBlockedAppDict(int uid,const ::Ice::Current&)
{
  UserConfigDict dict;
 
  {
  Statement sql;

  sql << "SELECT * FROM feed_send_config WHERE uid=" << uid;
  mysqlpp::StoreQueryResult res = QueryRunner("feed_db", CDbRServer,
      "feed_send_config").store(sql);

  if (!res.empty()) { //cache和数据库中都没有该uid记录
    for (size_t i = 0; i < res.size(); i++) {
      int type = (int) res.at(i)["feed_type"];

      if (type != 81) {
        continue;
      }

      int appid = (int) res.at(i)["app_id"];
      UserConfig userconfig;
      userconfig.sendConfig = false;
      userconfig.recvConfig = true;

      dict.insert(make_pair(appid, userconfig));
    }
  }
  }
    
{   
  Statement sql;
  sql << "SELECT * FROM feed_recv_config WHERE uid=" << uid;
   mysqlpp::StoreQueryResult res = QueryRunner("feed_db", CDbRServer,
      "feed_recv_config").store(sql);

  if (!res.empty()) { //cache和数据库中都没有该uid记录
    for (size_t i = 0; i < res.size(); i++) {
      int type = (int) res.at(i)["feed_type"];

      if (type != 81) {
        continue;
      }

      int appid = (int) res.at(i)["app_id"];
      
      UserConfigDict::iterator it = dict.find(appid);

      if(it!=dict.end()){
         it->second.recvConfig = false; 

      }else{
      UserConfig userconfig;
      userconfig.recvConfig = false;
      userconfig.sendConfig = true;


      dict.insert(make_pair(appid, userconfig));

      }

    }
  }
}  
  

  return dict;

}

//-------------------------------------------------------------------------------------



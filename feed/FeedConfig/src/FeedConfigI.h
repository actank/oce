/*
 * parser.h
 *
 *  Created on: Apr 8, 2010
 *
 */
#ifndef __FEED_FEEDCONFIG_H__
#define __FEED_FEEDCONFIG_H__

#include<climits>
#include <set>
#include <iostream>
#include <bitset>
#include <IceUtil/Mutex.h>
#include <IceUtil/Shared.h>
#include <IceUtil/Handle.h>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/composite_key.hpp>
#include "Singleton.h"
#include "FeedConfig.h"




namespace xce {
namespace feed {

using namespace MyUtil;
using namespace std;
using namespace boost::multi_index;

const size_t kSendPoolSize = 20000000;
const size_t kRecvPoolSize = 20000000;
const size_t kDefaultConfigCacheSize = 2UL * 1024 * 1024 * 1024 / 8;


class SendConfig : public IceUtil::Shared{
public:
  int uid_;
  void Update(int type_or_appid, bool news_config);

  bool Find(int type_or_appid);

  size_t Size() {
  	return config_.size();
  }


private:
  set<int> config_;
};

class RecvConfig: public IceUtil::Shared {
public:
  int uid_;
  void Update(int type_or_appid, bool news_config);
  bool Find(int type_or_appid);

  size_t Size() {
		return config_.size();
	}
private:
  set<int> config_; //保存不需要发送的类型的type，如果某种类型改为需要发送，则从中删除
};

typedef IceUtil::Handle<SendConfig> SendConfigPtr;
typedef IceUtil::Handle<RecvConfig> RecvConfigPtr;


class DefaultConfigCache {

public:
	DefaultConfigCache() {
		cache_array_ = new unsigned char[kDefaultConfigCacheSize]();
	}

	~DefaultConfigCache() {
		delete[] cache_array_;
	}

	void Set(const Ice::Int uid, bool is_default);

	bool Get(const Ice::Int uid);

	size_t CountSetBits();

private:
	unsigned char* cache_array_;

};



class SendConfigCache {

public:

  void SetConfig(Ice::Int uid, Ice::Int type, Ice::Int appid,
      bool newsconfig);

  bool GetConfig(Ice::Int uid, Ice::Int type, Ice::Int appid);

  void Preload();




private:
  void Update(int uid,int type,int appid,bool news);
  bool Find(int uid, int type_or_appid, bool& result);
  void Add(const SendConfigPtr& input);

  typedef boost::multi_index_container<
      SendConfigPtr,
      indexed_by<
        sequenced<> ,
        ordered_unique<
              BOOST_MULTI_INDEX_MEMBER(SendConfig, int,uid_)
        >
      >
  > SendConfigContainer;

  typedef SendConfigContainer::nth_index<0>::type SeqIndex;
  typedef SendConfigContainer::nth_index<1>::type UidIndex;
  typedef SendConfigContainer::iterator iterator;

  SendConfigContainer container_;
  void Kick();

  DefaultConfigCache default_config_cache_;

  IceUtil::Mutex mutex_;
};

class RecvConfigCache {
public:
  void SetConfig(Ice::Int uid, Ice::Int type, Ice::Int appid, bool newsconfig);

  MyUtil::IntArray Filter(const MyUtil::IntArray& target, Ice::Int type, Ice::Int appid);

  void Preload();


private:

  typedef boost::multi_index_container<
    RecvConfigPtr,
      indexed_by<
        sequenced<> ,
        ordered_unique<
          BOOST_MULTI_INDEX_MEMBER(RecvConfig, int,uid_)
        >
      >
  > RecvConfigContainer;

  typedef RecvConfigContainer::nth_index<0>::type SeqIndex;
  typedef RecvConfigContainer::nth_index<1>::type UidIndex;
  typedef RecvConfigContainer::iterator iterator;
  void Add(const RecvConfigPtr&);
  set<int> Find(const MyUtil::IntArray& target, int type_or_appid,
  		set<int> &uncached);

  void Update(int uid,int type,int appid,bool news);


  void Kick();
  RecvConfigContainer container_;
  DefaultConfigCache default_config_cache_;
  IceUtil::Mutex mutex_;
};


class ConfigManagerI: public ConfigManager, public Singleton<ConfigManagerI>{
public:

  ConfigManagerI() {
		send_config_cache_ = new SendConfigCache();
		recv_config_cache_ = new RecvConfigCache();
	}


	void SetSendConfig(Ice::Int uid, Ice::Int type, Ice::Int appid,
	      bool newsconfig, const ::Ice::Current& = ::Ice::Current());

	bool GetSendConfig(Ice::Int uid, Ice::Int type,
			Ice::Int appid, const ::Ice::Current& = ::Ice::Current() );



	void SetRecvConfig(Ice::Int uid, Ice::Int type, Ice::Int appid,
			bool newsconfig,  const ::Ice::Current& = ::Ice::Current());

	MyUtil::IntArray FilterRecvConfig(const MyUtil::IntArray& target, Ice::Int type,
			Ice::Int appid,  const ::Ice::Current& = ::Ice::Current());

  void SetUserConfig(Ice::Int uid, Ice::Int type, Ice::Int appid,
  		bool sendConfig, bool recvConfig, const ::Ice::Current& = ::Ice::Current());

  UserConfig GetUserConfig(Ice::Int uid, Ice::Int type,
  		Ice::Int appid, const ::Ice::Current& = ::Ice::Current());

  void Preload(const ::Ice::Current& = ::Ice::Current());

  UserConfigDict GetUserBlockedAppDict(int uid, const ::Ice::Current& = ::Ice::Current());

private:

	SendConfigCache* send_config_cache_;
	RecvConfigCache* recv_config_cache_;


};

};
};

#endif

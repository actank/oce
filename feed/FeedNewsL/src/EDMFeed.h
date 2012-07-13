#ifndef EDM_FEED_H_
#define EDM_FEED_H_

#include "RFeed.h"
#include "Singleton.h"
#include "IceLogger.h"
#include "QueryRunner.h"
//#include "TaskManager.h"

using namespace com::xiaonei::xce;
using namespace xce::feed;

typedef map<int, vector<FeedItem> > UserFeedsPool;

class EDMFeedManager : public MyUtil::Singleton<EDMFeedManager>{
public:
	EDMFeedManager() : _inited(false){}
	void ProcessTime(FeedItem & item){
		int stype = item.type & 0xFFFF;
		if(stype == 8015 || stype ==8016){//3G edm
			item.time = -2;
		}else{
			item.time = -60 * 8;
		}
	}
	FeedItemSeq getEDMFeed(int userid){
	  //return FeedItemSeq();//待删除 TODO
		if(!_inited){
			FeedItemSeq seq;
			MCE_INFO("EDMFeedManager::getEDMFeed --> get from db " << userid);
			Statement sql;
//			sql << "SELECT * FROM user_edm_feed_" << GetTableIndex(userid) << " where userid=" << userid;
			sql << "SELECT feed,merge,type,weight,userid,UNIX_TIMESTAMP(time) as utime FROM user_edm_feed_" << GetTableIndex(userid) << " where userid=" << userid;
			mysqlpp::StoreQueryResult res = QueryRunner("feed_db", CDbRServer, "user_edm_feed").store(sql);
			if (!res) {
				MCE_WARN("EDMFeedManager::getEDMFeed --> Fail to load edm " << userid);
				return FeedItemSeq();
			}
			mysqlpp::Row row;
			for (size_t i = 0; i<res.num_rows(); ++i) {
				row = res.at(i);
				FeedItem item;
				item.feed = row["feed"];
				item.merge = row["merge"];
				item.type = row["type"];
				item.weight = row["weight"];
				item.actor = row["userid"];
				item.time = row["utime"];
				item.time = -(item.time);
				ProcessTime(item);
				seq.push_back(item);
			}
			MCE_INFO("EDMFeedManager::getEDMFeed --> get from db " << userid << " size:" << seq.size());
			return seq;
			
		}
	
		IceUtil::Mutex::Lock lock(_mutex[index(userid)]);
		UserFeedsPool::iterator it = _pool[index(userid)].find(userid);
		if(it != _pool[index(userid)].end()){
			return it->second;
		}
		return FeedItemSeq();
	}
	void addEDMFeed(const FeedItem& item, int userid){
		if(item.time> 0) return;
		try{
			Statement sql;
			sql << "insert into user_edm_feed_" << GetTableIndex(userid) << "(userid, feed, merge, type, weight,time) values("
					<< userid << ", " << item.feed << ", " << item.merge << ", " << item.type << ", "
					<< item.weight << ", FROM_UNIXTIME(" << -(item.time) << "))";
			QueryRunner("feed_db", CDbWServer, "user_edm_feed").store(sql);
		}catch (Ice::Exception& e) {
			MCE_WARN("EDMFeedManager::addEDMFeed. uid:" << userid << " fid:" << item.feed
					<< " time:" << item.time
					<< " insert db fail. ice error:"<< e);
			return;
		}catch (std::exception& e) {
			MCE_WARN("EDMFeedManager::addEDMFeed. uid:" << userid << " fid:" << item.feed
					<< " time:" << item.time
					<< " insert db fail. std error:" << e.what());
			return;
	  }

		IceUtil::Mutex::Lock lock(_mutex[index(userid)]);
		_pool[index(userid)][userid].push_back(item);
		MCE_INFO("EDMFeedManager::addEDMFeed. insert mem. uid:" << userid << " fid:" << item.feed  << " time:" << item.time);
	}
	void addEDMFeed(const FeedItem& item, const MyUtil::IntSeq& userids){
		if(item.time > 0) return;
		for(int i=0; i<(int)userids.size(); i++){
			addEDMFeed(item, userids.at(i));
		}
	}
	void load(){
	  if(_index >= 0 && _index < 20){
	    MCE_INFO("EDMFeedManager::load --> begin load edm feed. index:" << _index);
	  }else{
	    MCE_INFO("EDMFeedManager::load --> index not in [0,19], no load. index:" << _index);
	    return;
	  }
		try{
			Statement sql;
//			sql<<"SELECT * FROM user_edm_feed_" << _index;
			//sql<<"SELECT feed,merge,type,weight,userid,UNIX_TIMESTAMP(time) as utime FROM user_edm_feed_" << _index << " where userid in (347348947,347735846)";
			sql<<"SELECT feed,merge,type,weight,userid,UNIX_TIMESTAMP(time) as utime FROM user_edm_feed_" << _index ;
			mysqlpp::StoreQueryResult res = QueryRunner("feed_db", CDbRServer, "user_edm_feed").store(sql);
			if (!res) {
				MCE_WARN("EDMFeedManager::load --> Fail to load edm");
				return;
			}
			mysqlpp::Row row;
			for (size_t i = 0; i<res.num_rows(); ++i) {
				row = res.at(i);
				FeedItem item;
				item.feed = row["feed"];
				item.merge = row["merge"];
				item.type = row["type"];
				item.weight = row["weight"];
				item.actor = row["userid"];
				item.time = row["utime"];
				item.time = -(item.time);
				int dbtime = item.time;
				ProcessTime(item);
				if(item.actor == 347348947 || item.actor == 347735846){
					MCE_INFO("EDMFeedManager::load. actor:" << item.actor << " fid:" << item.feed
							<< " type:" << (item.type && 0xFFFF)
							<< " time:" << item.time
							<< " dbtime:" << dbtime
					);
				}
				IceUtil::Mutex::Lock lock(_mutex[index(item.actor)]);
				_pool[index(item.actor)][item.actor].push_back(item);
			}
		}catch(std::exception& e){
			MCE_WARN("EDMFeedManager::load --> load edm err, " << e.what());
		}
		_inited = true;
		MCE_INFO("EDMFeedManager::load --> load edm feed end, size:" << size() << " index:" << _index);
	}
	/*void removeEDMFeed(int userid, Ice::Long feed){
		IceUtil::Mutex::Lock lock(_mutex[index(userid)]);
		UserFeedsPool::iterator it = _pool[index(userid)].find(userid);
		if(it != _pool[index(userid)].end()){
			vector<FeedItem>::iterator vit = it->second.begin();
			for(; vit!=it->second.end(); ++vit){
				if((*vit).feed == feed){
					it->second.erase(vit);
					if(it->second.empty()){
						_pool[index(userid)].erase(it);
					}
					break;
				}
			}
		}
	}*/
	void removeUser(int userid){
		bool exist = false;
		IceUtil::Mutex::Lock lock(_mutex[index(userid)]);
		UserFeedsPool::iterator it = _pool[index(userid)].find(userid);
                if(it != _pool[index(userid)].end()){
			_pool[index(userid)].erase(it);
			exist = true;
		}
		//if((!_inited) || exist){
			Statement sql;
			sql << "delete FROM user_edm_feed_" << GetTableIndex(userid) << " where userid=" << userid;
			mysqlpp::StoreQueryResult res = QueryRunner("feed_db", CDbWServer, "user_edm_feed").store(sql);
		//}
	}
	int index(int userid){
		return (abs(userid)>>7)%POOL_SIZE;
	}
	int GetTableIndex(int userid){
	  return userid % 20;
	}
	int size(){
		int sum = 0;
		for(int i=0; i<POOL_SIZE; i++){
			sum += _pool[i].size();
		}
		return sum;
	}
	void setIndex(int index){
		_index = index;
	}
private:
	static const int POOL_SIZE = 100;
	UserFeedsPool	_pool[POOL_SIZE];
	IceUtil::Mutex	_mutex[POOL_SIZE];
	bool		_inited;
	int		_index;
};

class EdmLoadTask : public MyUtil::Task{
public:
	void handle(){
		EDMFeedManager::instance().load();
	}
};
#endif

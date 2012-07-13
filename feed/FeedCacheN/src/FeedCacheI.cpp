#include <boost/lexical_cast.hpp>
#include "FeedCacheI.h"
#include "ServiceI.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "FeedCacheLogicAdapter.h"
#include <iostream>
#include "PerformanceStat.h"
//#include "FeedTest.h"
#include <vector>
#include <set>

using namespace xce::feed;
using namespace std;
using namespace ::xce::serverstate;



void MyUtil::initialize() {

  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&FeedCacheI::instance(), service.createIdentity("M", ""));
  TaskManager::instance().scheduleRepeated(&PerformanceStat::instance());



//  int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("FeedCacheN.Mod");
//  int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("FeedCacheN.Interval", 5);
//  ServerStateSubscriber::instance().initialize("ControllerFeedCacheNR",&FeedCacheI::instance(),mod,interval, new XceFeedControllerChannel);
//  MCE_INFO("mod:" << mod << " interval:" << interval);


}

//////////////////////////////////////////////////////////////////
bool UserFeed::put(const FeedItem& item){
  if(item.feed <= 0) {
    MCE_WARN("UserFeed::put --> feed id:" << item.feed << " _userId:" << _userId);
    return false;
  }

  if(_feeds[_end].feed == 0){
    _size = ((++_size>FEED_COUNT) ? FEED_COUNT : _size);
  }
  _feeds[_end] = item;
  _end = (_end + 1)%FEED_COUNT;

  if (_size >= FEED_COUNT) {
    return true; 
  } else {
    return false;
  } 

}

void UserFeed::modify(const FeedItem & item) {
  if(item.feed == 0) {
    MCE_WARN("UserFeed::modify --> feed id is 0! _userId:" << _userId);
    return ;
  }

  int tmp = FEED_COUNT + _end - 1;

  for(int c = 0; c<_size&&tmp>=_end; tmp--) {

    if(_feeds[tmp%FEED_COUNT].feed != 0) {
      c++;
    }

    if(_feeds[tmp%FEED_COUNT].feed == item.feed) {
      MCE_INFO("UserFeed::modify --> feed id:" << item.feed << " _userId:" << _userId);
      _feeds[tmp%FEED_COUNT] = item;
      break;
    }
  }

}


FeedItemSeq UserFeed::getMore(Ice::Long maxFeed) {
  FeedItemSeq seq;
  int tmp = FEED_COUNT + _end - 1;

  for(int c = 0; c<_size&&tmp>=_end; tmp--) {
    if(_feeds[tmp%FEED_COUNT].feed != 0) {
      c++;
      if(_feeds[tmp%FEED_COUNT].feed > maxFeed) {
        seq.push_back(_feeds[tmp%FEED_COUNT]);
      }
    }
  }

  return seq;
}

/*
   FeedItemSeq UserFeed::get(){
   FeedItemSeq seq;
   seq.reserve(_size);
   int tmp = FEED_COUNT + _end - 1;
   for(int c=0; c<_size&&tmp>=_end; tmp--){
   if(_feeds[tmp%FEED_COUNT].feed!=0){
   c++;
   seq.push_back(_feeds[tmp%FEED_COUNT]);
   }
   }
//MCE_DEBUG("UserFeed::get --> " << _userid << " _size:" << _size << " end:" << _end << " beg:" << tmp%FEED_COUNT);
return seq;
}
 */

void UserFeed::remove(Ice::Long feedId){
  FeedCacheConfigPtr cfg = FeedCacheI::instance().getConfig();

  if(feedId == 0){
    return;
  }
  bool tidy = (_deletedCount >= cfg->deletedCount);
  bool deleted = false;
  int begin = -1;
  int c = 0;
  for(int i=_end+FEED_COUNT-1; i>=_end&&c<_size; i--){
    if(_feeds[i%FEED_COUNT].feed > 0){ 
      c++;
    } 
    if(_feeds[i%FEED_COUNT].feed == feedId){
      MCE_INFO("UserFeed::remove --> deleted. _userId:" << _userId << " feedId:" << feedId << " size:" << _size << " index:" << (i%FEED_COUNT));
      _feeds[i%FEED_COUNT].feed = 0;
      deleted = true;
      if(!tidy){
        break;
      }
    }
    if(tidy && (begin==-1) && (_feeds[i%FEED_COUNT].feed==0)){
      MCE_INFO("UserFeed::remove --> tidy. _userId:" << _userId << " size:" << _size << " index:" << (i%FEED_COUNT));
      begin = i;
    }
    if(begin>0 && _feeds[i%FEED_COUNT].feed > 0){
      _feeds[begin%FEED_COUNT] = _feeds[i%FEED_COUNT];
      _feeds[i%FEED_COUNT].feed = 0;
      begin--;
    }
  }
  if(deleted){
    _deletedCount++;
    _size--;
  }
  if(tidy){
    _deletedCount = 0;
  }

}



void UserFeed::removeAll() {
  MCE_INFO("UserFeed::removeAll --> uid:" << _userId);
  _end = 0;
  _size = 0;
  _deletedCount = 0;
  memset(_feeds, 0, sizeof(FeedItem)*FEED_COUNT); 
} 

MyUtil::LongSeq UserFeed::removeByMerge(Ice::Long mergeId, int stype) {

  int tmp = FEED_COUNT + _end - 1;
  MyUtil::LongSeq deletedIds;


  for(int c = 0; c<_size&&tmp>=_end; tmp--) {

    if(_feeds[tmp%FEED_COUNT].feed != 0) {
      c++;
    }

    const FeedItem & item = _feeds[tmp%FEED_COUNT];
    if( (item.merge == mergeId) && ((item.type & 0xffff) == stype) ) {
      deletedIds.push_back(item.feed);
    }
  }

  for(MyUtil::LongSeq::iterator it = deletedIds.begin(); it != deletedIds.end(); it++) {
    remove(*it);
  }

  MCE_INFO("UserFeed::removeByMerge --> uid:" << _userId << " merge:" << mergeId <<  " stype:" << stype << " feeds size:" << _size << " deletedIds size:" << deletedIds.size());
  return deletedIds;

}

MyUtil::LongSeq UserFeed::removeMergeByFeedId(Ice::Long feedId) {

  MyUtil::LongSeq deletedIds;
  int stype = -1;
  Ice::Long mergeId = -1;

  int index = FEED_COUNT + _end - 1;


  for(int cn = 0; cn<_size && index >= _end; index--) {

    if(_feeds[index%FEED_COUNT].feed != 0) {
      cn++;
    }

    const FeedItem & item = _feeds[index%FEED_COUNT];
    if(item.feed == feedId) {
      stype = (item.type & 0xffff);
      mergeId = item.merge;
      break;
    }

  }

  if(stype == -1 || mergeId == -1) {
    MCE_WARN("UserFeed::removeMergeByFeedId --> no feed id:" << feedId << " _userId:" << _userId);
    return deletedIds;
  }

  deletedIds = removeByMerge(mergeId, stype);

  MCE_INFO("UserFeed::removeMergeByFeedId --> uid:" << _userId << " feedId:" << feedId << " feeds size:" << _size << " deletedIds size:" << deletedIds.size());
  return deletedIds;
}

void UserFeed::reject(const vector<Ice::Long> & rejectedIds) {

  set<Ice::Long> rejectedSet;
  for(vector<Ice::Long>::const_iterator it = rejectedIds.begin(); it != rejectedIds.end(); it++) {
    rejectedSet.insert(*it); 
  }

  int begin = -1;
  int c = 0;
  int rejectedNum = 0;
  for(int i=_end+FEED_COUNT-1; i>=_end&&c<_size; i--){
    if(_feeds[i%FEED_COUNT].feed > 0){ 
      c++;
    } 

    if(rejectedSet.count(_feeds[i%FEED_COUNT].feed)){
      _feeds[i%FEED_COUNT].feed = 0;
      rejectedNum++;
    }

    if((begin==-1) && (_feeds[i%FEED_COUNT].feed==0)){
      begin = i;
    }

    if(begin>0 && _feeds[i%FEED_COUNT].feed > 0){
      _feeds[begin%FEED_COUNT] = _feeds[i%FEED_COUNT];
      _feeds[i%FEED_COUNT].feed = 0;
      begin--;
    }
  }

  _size -= rejectedNum;

  _deletedCount = 0;

}


//////////////////////////////////////////////////////////////////
/*
   FeedItemSeq UserFeedPool::get(int userId){
   MCE_INFO("UserFeedPool::get --> userId:" << userId);
   FeedCacheConfigPtr cfg = FeedCacheI::instance().getConfig();
   timeval beg, end;
   gettimeofday(&beg, NULL);
   if(cfg->enableCache){
   IceUtil::Mutex::Lock lock(_mutex);
   UserIdIndex& index = _users.get<1>();
   UserIdIndex::iterator it = index.find(userId);
   if(it != index.end()){
   SequenceIndex& seqIndex = _users.get<0>();
   seqIndex.relocate(seqIndex.begin(), _users.project<0>(it));
   const FeedItemSeq& fis = (*it)->get();

   gettimeofday(&end, NULL);
   float timeuse=1000000*(end.tv_sec-beg.tv_sec)+end.tv_usec-beg.tv_usec;
   timeuse/=1000;
   PerformanceStat::instance().incHitGet(timeuse);
   MCE_INFO("UserFeedPool::get --> userId:" << userId << " seq size:" << fis.size() << " time use:" << timeuse);

   return fis;
   }
   }

//可能被调用多次
// 读DB逻辑转移到FeedCacheLogic
FeedItemSeq items;
try{
if(cfg->loadFrom == "FeedCacheLogic") {
timeval beg, end;
gettimeofday(&beg, NULL);

items = FeedCacheLogicAdapter::instance().get(userId, 0, UserFeed::FEED_COUNT);

gettimeofday(&end, NULL);
float timeuse=1000000*(end.tv_sec-beg.tv_sec)+end.tv_usec-beg.tv_usec;
timeuse/=1000;
PerformanceStat::instance().incLoadGet(timeuse);
MCE_INFO("UserFeedPool::get --> load from FeedCacheLogic:" << userId << " time use:" << timeuse);
}
} catch(Ice::Exception& e) {
MCE_WARN("UserFeedPool::get --> load items from FeedCacheLogic exception. uid:" << userId << " e:" << e);
return items;
} catch(Ice::Exception& e) {
MCE_WARN("UserFeedPool::get --> load items from FeedCacheLogic exception. uid:" << userId << " e:" << e.what());
return items;
} catch(Ice::Exception& e) {
MCE_WARN("UserFeedPool::get --> load items from FeedCacheLogic exception. uid:" << userId << " unknown exception!");
return items;
}


if(items.empty()){
return items;
}

{
IceUtil::Mutex::Lock lock(_mutex);
UserIdIndex& index = _users.get<1>();
UserIdIndex::iterator it = index.find(userId);
if(it != index.end()) {
return (*it)->get();
}


timeval beg, end;
gettimeofday(&beg, NULL);
UserFeedPtr user = new UserFeed(userId);
for(int i=(int)items.size()-1; i>=0; i--) {
user->put(items.at(i));
}
_users.push_front(user);
int maxSize = cfg->cacheSize/FeedCacheI::POOL_SIZE;
if((int)_users.size() > maxSize) {
  while((int)_users.size() > maxSize) {

    SequenceIndex& index = _users.get<0>();
    UserFeedPtr p = index.back();
    index.pop_back();
    delete p;
    MCE_INFO("UserFeedPool::get --> evict user. uid:" << p->userId() << " _feeds size:" << _users.size());
  }
}

gettimeofday(&end, NULL);
float timeuse=1000000*(end.tv_sec-beg.tv_sec)+end.tv_usec-beg.tv_usec;
timeuse/=1000;
PerformanceStat::instance().incCreateUser(timeuse);
MCE_INFO("UserFeedPool::get --> create user " << userId << " rawSeq size:" << items.size() << " time use:" << timeuse);	
return items;
}
}
*/

FeedItemSeq UserFeedPool::getFromCache(int userId, Ice::Long maxFeed) {

  FeedCacheConfigPtr cfg = FeedCacheI::instance().getConfig();

  timeval beg, end;
  gettimeofday(&beg, NULL);
  if(cfg->enableCache){
    IceUtil::Mutex::Lock lock(_mutex);
    UserIdIndex& index = _users.get<1>();
    UserIdIndex::iterator it = index.find(userId);
    if(it != index.end()){
      SequenceIndex& seqIndex = _users.get<0>();
      seqIndex.relocate(seqIndex.begin(), _users.project<0>(it));

      const FeedItemSeq& fis = (*it)->getMore(maxFeed);

      gettimeofday(&end, NULL);
      float timeuse=1000000*(end.tv_sec-beg.tv_sec)+end.tv_usec-beg.tv_usec;
      timeuse/=1000;
      PerformanceStat::instance().incHitGet(timeuse); //这里的数值需要减去loadDB的数目
      MCE_INFO("UserFeedPool::getFromCache --> userId:" << userId << " maxFeed:" << maxFeed << " feeds size:" << fis.size() << " time use:" << timeuse);
      return fis;
    } else {
      FeedItemSeq fis;
      return fis;
    }
  }

}

FeedItemSeq UserFeedPool::getMore(int userId, Ice::Long maxFeed) {
  timeval beg, end;
  gettimeofday(&beg, NULL);

  FeedItemSeq retSeq;
  retSeq = getFromCache(userId, maxFeed);

  if (retSeq.size()) {
    return retSeq;
  } else {
    loadUser(userId);
  }

  retSeq = getFromCache(userId, maxFeed);

  gettimeofday(&end, NULL);
  float timeuse=1000000*(end.tv_sec-beg.tv_sec)+end.tv_usec-beg.tv_usec;
  timeuse/=1000;
  MCE_INFO("UserFeedPool::getMore --> uid:" << userId << " maxFeed:" << maxFeed << " retSeq:" << retSeq.size() << " time use:" << timeuse);	

  return retSeq;
}

void UserFeedPool::loadUser(int userId) {

  FeedCacheConfigPtr cfg = FeedCacheI::instance().getConfig();
  //可能被调用多次
  // 读DB逻辑转移到FeedCacheLogic
  timeval beg, end;
  gettimeofday(&beg, NULL);

  FeedItemSeq items;
  try{
    if(cfg->loadFrom == "FeedCacheLogic") {
      items = FeedCacheLogicAdapter::instance().get(userId, 0, UserFeed::FEED_COUNT);

      gettimeofday(&end, NULL);
      float timeuse=1000000*(end.tv_sec-beg.tv_sec)+end.tv_usec-beg.tv_usec;
      timeuse/=1000;
      PerformanceStat::instance().incLoadGet(timeuse);
      MCE_INFO("UserFeedPool::loadUser --> load from FeedCacheLogic:" << userId << " feeds size:" << items.size() << " time use:" << timeuse);
    }
  } catch(Ice::Exception& e) {
    MCE_WARN("UserFeedPool::loadUser --> load items from FeedCacheLogic exception. uid:" << userId << " e:" << e);
    return ;
  } catch(Ice::Exception& e) {
    MCE_WARN("UserFeedPool::loadUser --> load items from FeedCacheLogic exception. uid:" << userId << " e:" << e.what());
    return ;
  } catch(Ice::Exception& e) {
    MCE_WARN("UserFeedPool::loadUser --> load items from FeedCacheLogic exception. uid:" << userId << " unknown exception!");
    return ;
  }


  {
    IceUtil::Mutex::Lock lock(_mutex);
    UserIdIndex& index = _users.get<1>();
    UserIdIndex::iterator it = index.find(userId);
    if(it != index.end()) {
      return ;
    }

    timeval beg, end;
    gettimeofday(&beg, NULL);
    UserFeedPtr user = new UserFeed(userId);
    for(int i=(int)items.size()-1; i>=0; i--) {
      user->put(items.at(i));
    }
    _users.push_front(user);
    int maxSize = cfg->cacheSize/FeedCacheI::POOL_SIZE;
    if((int)_users.size() > maxSize) {
      while((int)_users.size() > maxSize) {
        SequenceIndex& index = _users.get<0>();
        UserFeedPtr p = index.back();
        index.pop_back();
        delete p;
        MCE_INFO("UserFeedPool::loadUser --> evict user. uid:" << p->userId() << " _users size:" << _users.size());
      }
    }
  }

  gettimeofday(&end, NULL);
  float timeuse=1000000*(end.tv_sec-beg.tv_sec)+end.tv_usec-beg.tv_usec;
  timeuse/=1000;
  PerformanceStat::instance().incCreateUser(timeuse);
  MCE_INFO("UserFeedPool::loadUser --> create user:" << userId << " feeds size:" << items.size() << " time use:" << timeuse);
}

void UserFeedPool::modify(int userId, const FeedItem& item) {
  timeval beg, end;
  gettimeofday(&beg, NULL);

  IceUtil::Mutex::Lock lock(_mutex);
  UserIdIndex& index = _users.get<1>();
  UserIdIndex::iterator uIt = index.find(userId);
  (*uIt)->modify(item);

  gettimeofday(&end, NULL);
  float timeuse=1000000*(end.tv_sec-beg.tv_sec)+end.tv_usec-beg.tv_usec;
  timeuse/=1000;
  PerformanceStat::instance().incModify(timeuse);
  MCE_INFO("UserFeedPool::modify --> user id:" << userId << " feed id:"  << item.feed << " time use:" << timeuse);
}

void UserFeedPool::putByUserId(int userId, const FeedItemSeq& items){
  IceUtil::Mutex::Lock lock(_mutex);
  UserIdIndex& index = _users.get<1>();
  UserIdIndex::iterator uIt = index.find(userId);
  if(uIt!=index.end()){
    return;
  }
  UserFeedPtr user = new UserFeed(userId);
  for(int i=(int)items.size()-1; i>=0; i--) {
    user->put(items.at(i));
  }
  _users.push_front(user);
}

void UserFeedPool::put(const FeedItem& itemArg, const MyUtil::Int2IntMap& usersWeight){
  FeedCacheConfigPtr cfg = FeedCacheI::instance().getConfig();

  FeedItem item = itemArg;
  //pair<int, int> si = size();

  //map<int, ::xce::feed::FeedItemSeq > user2FeedsMap;
  //map<int, vector<Ice::Long> > rejectedMap;

  {
    IceUtil::Mutex::Lock lock(_mutex);
    MyUtil::Int2IntMap::const_iterator it = usersWeight.begin();
    for(; it!=usersWeight.end(); ++it){
      const int uid = it->first;
      UserIdIndex& index = _users.get<1>();
      UserIdIndex::iterator uIt = index.find(uid);
      if(uIt != index.end()){

        item.weight = it->second;
        bool isRejected = (*uIt)->put(item);

        if (isRejected) {
          const ::xce::feed::FeedItemSeq & rawFeedSeq = (*uIt)->getMore();
					MCE_INFO("UserFeedPool::put --> reject uid:" << uid << " rawFeedSeq size:" << rawFeedSeq.size());
					TaskManager::instance().execute(new RejectTask(uid, rawFeedSeq));
        }
      } 

    }
  }

}

void  UserFeedPool::reject(int uid, const vector<Ice::Long> & rejectedIds) { 

	IceUtil::Mutex::Lock lock(_mutex);
	UserIdIndex& index = _users.get<1>();
	UserIdIndex::iterator uIt = index.find(uid); 
	if(uIt != index.end()) {

		(*uIt)->reject(rejectedIds);
	}

}

void UserFeedPool::remove(int userId, Ice::Long feedId){
  MCE_INFO("UserFeedPool::remove --> " << userId << " feed:" << feedId);
  IceUtil::Mutex::Lock lock(_mutex);

  UserIdIndex& index = _users.get<1>();
  UserIdIndex::iterator it = index.find(userId);
  if(it != index.end()){
    return (*it)->remove(feedId);
  } else {
    MCE_INFO("UserFeedPool::remove --> no user:" << userId);
  }
}

void UserFeedPool::removeUser(int userId){
  MCE_INFO("UserFeedPool::removeUser --> uid:" << userId);
  IceUtil::Mutex::Lock lock(_mutex);
  UserIdIndex& index = _users.get<1>();
  UserIdIndex::iterator it = index.find(userId);
  if(it != index.end()){
    UserFeedPtr p = *it;
    index.erase(it);
    delete p; 
  } else {
    MCE_INFO("UserFeedPool::removeUser --> no user:" << userId);
  }

}


void UserFeedPool::removeAll(int userId) {
  MCE_INFO("UserFeedPool::removeAll --> uid:" << userId);
  IceUtil::Mutex::Lock lock(_mutex);
  UserIdIndex& index = _users.get<1>();
  UserIdIndex::iterator it = index.find(userId);
  if(it != index.end()){
    (*it)->removeAll();
  } 
} 

MyUtil::LongSeq UserFeedPool::removeByMergeFromCache(int userId, Ice::Long merge, int stype) {
  MCE_INFO("UserFeedPool::removeByMergeFromCache --> uid:" << userId << " merge:" << merge << " stype:" << stype);
  {
    IceUtil::Mutex::Lock lock(_mutex);
    UserIdIndex& index = _users.get<1>();
    UserIdIndex::iterator it = index.find(userId);
    if(it != index.end()) {
      return (*it)->removeByMerge(merge, stype);
    } else {
      MyUtil::LongSeq retSeq;
      return retSeq;
    }
  }
}

MyUtil::LongSeq UserFeedPool::removeByMerge(int userId, Ice::Long merge, int stype ) {
  //MCE_INFO("UserFeedPool::removeByMerge --> uid:" << userId << " merge:" << merge << " stype:" << stype);
  /*MyUtil::LongSeq retSeq;
  retSeq = removeByMergeFromCache(userId, merge, stype);
  if(retSeq.size()) {
    return retSeq;
  } else {
    loadUser(userId);
  }*/

  return removeByMergeFromCache(userId, merge, stype);

}

MyUtil::LongSeq UserFeedPool::removeMergeByFidFromCache(int userId, Ice::Long feedId) {
  MCE_INFO("UserFeedPool::removeMergeByFidFromCache --> uid:" << userId << " feedId:" << feedId);
  {
    IceUtil::Mutex::Lock lock(_mutex);
    UserIdIndex& index = _users.get<1>();
    UserIdIndex::iterator it = index.find(userId);
    if(it != index.end()) {
      return  (*it)->removeMergeByFeedId(feedId);
    } else {
      MyUtil::LongSeq ret;
      return ret;
    }
  }
}

MyUtil::LongSeq UserFeedPool::removeMergeByFeedId(int userId, Ice::Long feedId) {

  //MCE_INFO("UserFeedPool::removeMergeByFeedId --> uid:" << userId << " feedId:" << feedId);
  /*MyUtil::LongSeq retSeq;

  retSeq = removeMergeByFidFromCache(userId, feedId);
  if(retSeq.size()) {
    return retSeq;
  } else {
    loadUser(userId);
  }*/

  return removeMergeByFidFromCache(userId, feedId);

}


int UserFeedPool::getUserFeedSize(int userId){
  MCE_INFO("UserFeedPool::getUserFeedSize --> uid:" << userId);
  {
    IceUtil::Mutex::Lock lock(_mutex);
    UserIdIndex& index = _users.get<1>();
    UserIdIndex::iterator it = index.find(userId);
    if(it != index.end()){
      return (*it)->size();
    }
  }
  return getMore(userId).size();	
}

pair<int, int> UserFeedPool::size(){
	IceUtil::Mutex::Lock lock(_mutex);
	pair<int, int> p = make_pair<int, int>(0, 0);
	p.first = _users.size();
	SequenceIndex& index = _users.get<0>();	
	SequenceIndex::iterator it = index.begin();
	for(; it!=index.end(); ++it){
		p.second += (*it)->size();
	}
	return p;
}

MyUtil::IntSeq UserFeedPool::getIds() {
	IceUtil::Mutex::Lock lock(_mutex);
	MyUtil::IntSeq ret;
	SequenceIndex& index = _users.get<0>();	
	SequenceIndex::iterator it = index.begin();
	for(; it!=index.end(); ++it){
		ret.push_back((*it)->userId());
	}

	return ret;
}
//////////////////////////////////////////////////////////////////
FeedItemSeq FeedCacheI::get(int userId, const Ice::Current&){

  if(userId >= 0) {
    //StatFunc stat("get stat --> ");
    return _pool[(userId>>7)%POOL_SIZE].getMore(userId);
  } else {
    MCE_WARN("FeedCacheI::get --> userId:" << userId);
    FeedItemSeq seq;
    return seq;
  }
}

FeedItemSeq FeedCacheI::getMore(int userId, Ice::Long maxFeed, const Ice::Current&){
  //StatFunc stat("get stat --> ");

  if(userId >= 0) {
    return _pool[(userId>>7)%POOL_SIZE].getMore(userId, maxFeed);
  } else {
    MCE_WARN("FeedCacheI::getMore --> userId:" << userId);
    FeedItemSeq seq;
    return seq;
  }
}

int FeedCacheI::getCount(int userId, const Ice::Current&){
  if(userId >= 0) {
    return _pool[(userId>>7)%POOL_SIZE].getUserFeedSize(userId);
  } else {
    MCE_WARN("FeedCacheI::getCount --> userId:" << userId);
    return 0;
  }
}

void FeedCacheI::modify(int userId, const FeedItem& feed_item, const Ice::Current&) {

  if(userId >= 0) {
    return _pool[(userId>>7)%POOL_SIZE].modify(userId, feed_item);
  } else {
    MCE_WARN("FeedCacheI::modify --> userId:" << userId);
  }
}


void FeedCacheI::putByUserId(int userId, const FeedItemSeq& items, const Ice::Current&){
  timeval beg, end;
  gettimeofday(&beg, NULL);

  if(userId >= 0) {
    _pool[(userId>>7)%POOL_SIZE].putByUserId(userId, items);
  } else {
    MCE_WARN("FeedCacheI::putByUserId --> userId:" << userId << " items size:" << items.size());
  }

  gettimeofday(&end, NULL);
  float timeuse=1000000*(end.tv_sec-beg.tv_sec)+end.tv_usec-beg.tv_usec;
  timeuse/=1000;
  PerformanceStat::instance().incPutByUserId(timeuse);
  MCE_INFO("FeedCacheI::putByUserId --> userId:" << userId << " items size:" << items.size() << " time use:" << timeuse);
}

PoolUserIdPtr FeedCacheI::getPoolUserIds(int poolIndex, const Ice::Current&) {
  timeval beg, end;
  gettimeofday(&beg, NULL);

  PoolUserIdPtr ret = new PoolUserId;
  ret->poolIndex = -1;
  if(poolIndex < 0) {
    MCE_WARN("FeedCacheI::getPoolUserIds --> poolIndex:" << poolIndex);
    return ret;
  }
  
  ret->poolIndex = poolIndex % POOL_SIZE;
  ret->ids = _pool[poolIndex % POOL_SIZE].getIds();

  gettimeofday(&end, NULL);
  float timeuse=1000000*(end.tv_sec-beg.tv_sec)+end.tv_usec-beg.tv_usec;
  timeuse/=1000;
  PerformanceStat::instance().incGetPoolUserIds(timeuse);
  MCE_INFO("FeedCacheI::getPoolUserIds --> poolIndexArg:" << poolIndex << " poolIndex:" << poolIndex % POOL_SIZE << " ids size:" << ret->ids.size() << " time use:" << timeuse);

  return ret;
}

void FeedCacheI::put(const FeedItem& feed, const MyUtil::Int2IntMap& usersWeight, const Ice::Current&){
  timeval beg, end;
  gettimeofday(&beg, NULL);

  map<int, map<int, int> > splits;
  MyUtil::Int2IntMap::const_iterator it = usersWeight.begin();
  for(; it!=usersWeight.end(); ++it) {
    if(it->first >= 0) {    
      splits[(it->first>>7)%POOL_SIZE][it->first] = it->second;
    } else {
      MCE_WARN("FeedCacheI::put --> user id:" << it->first);   
      continue;
    }
  }

  map<int, map<int, int> >::iterator sIt = splits.begin();
  for(; sIt!=splits.end(); ++sIt) {
    _pool[sIt->first].put(feed, sIt->second);
  }

  gettimeofday(&end, NULL);
  float timeuse=1000000*(end.tv_sec-beg.tv_sec)+end.tv_usec-beg.tv_usec;
  timeuse/=1000;
  PerformanceStat::instance().incPut(timeuse);
  MCE_INFO("FeedCacheI::put --> feedId:" << feed.feed << " usersWeight size:" << usersWeight.size() << " time use:" << timeuse);

}

void FeedCacheI::remove(int userId, Ice::Long feedId, const Ice::Current&){
  timeval beg, end;
  gettimeofday(&beg, NULL);

  if(userId >= 0) {
    _pool[(userId>>7)%POOL_SIZE].remove(userId, feedId);
  } else {
    MCE_WARN("FeedCacheI::remove --> userId:" << userId);
  }

  gettimeofday(&end, NULL);
  float timeuse=1000000*(end.tv_sec-beg.tv_sec)+end.tv_usec-beg.tv_usec;
  timeuse/=1000;
  PerformanceStat::instance().incRemove(timeuse);
  MCE_INFO("FeedCacheI::remove --> userId:" << userId << " feed id:" << feedId << " time use:" << timeuse);
}

void FeedCacheI::removeUser(int userId, const Ice::Current&){
  timeval beg, end;
  gettimeofday(&beg, NULL);

  if(userId >= 0) {
    _pool[(userId>>7)%POOL_SIZE].removeUser(userId);
  } else {
    MCE_WARN("FeedCacheI::removeUser --> userId:" << userId);
  }

  gettimeofday(&end, NULL);
  float timeuse=1000000*(end.tv_sec-beg.tv_sec)+end.tv_usec-beg.tv_usec;
  timeuse/=1000;
  PerformanceStat::instance().incRemoveUser(timeuse);
  MCE_INFO("FeedCacheI::removeUser --> userId:" << userId  << " time use:" << timeuse);
}

void FeedCacheI::removeAll(int userId, const Ice::Current&) {
  timeval beg, end;
  gettimeofday(&beg, NULL);

  if(userId >= 0) {
    _pool[(userId>>7)%POOL_SIZE].removeAll(userId);
  } else {
    MCE_WARN("FeedCacheI::removeAll --> userId:" << userId);
  }

  gettimeofday(&end, NULL);
  float timeuse=1000000*(end.tv_sec-beg.tv_sec)+end.tv_usec-beg.tv_usec;
  timeuse/=1000;
  PerformanceStat::instance().incRemoveAll(timeuse);
  MCE_INFO("FeedCacheI::removeAll --> userId:" << userId  << " time use:" << timeuse);
}


MyUtil::LongSeq FeedCacheI::removeByMerge(int userId, Ice::Long merge, int stype, const Ice::Current&) {

  timeval beg, end;
  gettimeofday(&beg, NULL);

  if(userId >= 0) {
    const MyUtil::LongSeq& ret = _pool[(userId>>7)%POOL_SIZE].removeByMerge(userId, merge, stype);
    gettimeofday(&end, NULL); 
    float timeuse=1000000*(end.tv_sec-beg.tv_sec)+end.tv_usec-beg.tv_usec;
    timeuse/=1000;
    PerformanceStat::instance().incRemoveByMerge(timeuse);
    MCE_INFO("FeedCacheI::removeByMerge --> userId:" << userId << " merge:" << merge << " stype:" << stype << " deleted size:" << ret.size() << " time use:" << timeuse);
    return ret;
  } else {
    MCE_WARN("FeedCacheI::removeByMerge --> userId:" << userId);
    MyUtil::LongSeq seq;
    return seq;
  }

}

MyUtil::LongSeq FeedCacheI::removeMergeByFeedId(int userId, Ice::Long feedId, const Ice::Current&) {
  timeval beg, end;
  gettimeofday(&beg, NULL);

  if(userId >= 0) {
    const MyUtil::LongSeq& ret = _pool[(userId>>7)%POOL_SIZE].removeMergeByFeedId(userId, feedId);

    gettimeofday(&end, NULL);
    float timeuse=1000000*(end.tv_sec-beg.tv_sec)+end.tv_usec-beg.tv_usec;
    timeuse/=1000;
    PerformanceStat::instance().incRemoveMergeByFid(timeuse);
    MCE_INFO("FeedCacheI::removeMergeByFeedId --> userId:" << userId << " feedId:" << feedId << " deleted size:" << ret.size() << " time use:" << timeuse);
    return ret;
  } else {
    MCE_WARN("FeedCacheI::removeMergeByFeedId --> userId:" << userId);
    MyUtil::LongSeq seq;
    return seq;

  }

}



pair<Ice::Long, Ice::Long> FeedCacheI::queryCacheCount(){
  pair<Ice::Long, Ice::Long> c;
  c.first = 0;
  c.second = 0;
  for(int i=0; i<POOL_SIZE; i++){
    pair<int, int> p = _pool[i].size();
    c.first += p.first;
    c.second += p.second;
  }
  return c;
}

void FeedCacheI::reject(int userId, const vector<Ice::Long> & rejectedIds, const Ice::Current&) {
  if(userId >= 0) {
    _pool[(userId>>7)%POOL_SIZE].reject(userId, rejectedIds);

  } else {
    MCE_WARN("FeedCacheI::reject --> userId error:" << userId);
  }
}

void FeedCacheI::updateConfig(const FeedCacheConfigPtr& cfg, const Ice::Current&){

  IceUtil::Mutex::Lock lock(_config_mutex);

  FeedCacheConfigPtr tmp = const_cast<FeedCacheConfigPtr&>(cfg);
  if(tmp->cacheSize < 0){
    tmp->cacheSize = _cfg->cacheSize;
  }
  if(tmp->deletedCount < 0){
    tmp->deletedCount = _cfg->deletedCount;
  }
  if(tmp->loadFrom.empty()){
    tmp->loadFrom = _cfg->loadFrom;
  }
  _cfg = tmp;

  MCE_INFO("FeedCacheI::updateConfig --> conf.");
  MCE_INFO("FeedCacheI::updateConfig --> cacheSize:" << _cfg->cacheSize);
  MCE_INFO("FeedCacheI::updateConfig --> deletedCount:" << _cfg->deletedCount);
  MCE_INFO("FeedCacheI::updateConfig --> loadFrom:" << _cfg->loadFrom);
  MCE_INFO("FeedCacheI::updateConfig --> enableCache:" << _cfg->enableCache);
  MCE_INFO("FeedCacheI::updateConfig --> enableCacheReject:" << _cfg->enableCacheReject);
  MCE_INFO("FeedCacheI::updateConfig --> enableLogicReject:" << _cfg->enableLogicReject);

  for(FeedDBCfgsMap::const_iterator it = _cfg->feedDBCfgs.begin(); it != _cfg->feedDBCfgs.end(); it++) {
    MCE_INFO("FeedCacheI::updateConfig --> feedDBCfgs. first:" << it->first << " syncPut:" << it->second->syncPut << " syncRemove:" << it->second->syncRemove << " syncRemoveAll:" << it->second->syncRemoveAll);

  }


}

bool FeedCacheI::setConfigItem(ConfigItem ci, int itemValue, const Ice::Current&) {
  IceUtil::Mutex::Lock lock(_config_mutex);
  switch(ci) {
    case LogicReject:
      if(1 == itemValue) {
        _cfg->enableLogicReject = true; 
        MCE_INFO("FeedCacheI::setConfigItem --> LogicReject: true.");
      } else if (0 == itemValue) {
        _cfg->enableLogicReject = false; 
        MCE_INFO("FeedCacheI::setConfigItem --> LogicReject: false.");
      }   
      break;
    case CacheReject:
      if(1 == itemValue) {
        _cfg->enableCacheReject = true; 
        MCE_INFO("FeedCacheI::setConfigItem --> CacheReject: true.");
      } else if (0 == itemValue) {
        _cfg->enableCacheReject = false; 
        MCE_INFO("FeedCacheI::setConfigItem --> CacheReject: false.");
      }   
      break;
    case CacheSize:
      if(itemValue < 1000000) {
        MCE_WARN("FeedCacheI::setConfigItem --> CacheSize error.");
        return false;
      } else {
        _cfg->cacheSize = itemValue; 
        MCE_INFO("FeedCacheI::setConfigItem --> CacheSize:" << itemValue);
      }   
      break;
    default:
      MCE_WARN("FeedCacheI::setConfigItem --> ConfigItem error.");
      return false;
    return true;
  }
}

void FeedCacheI::printThreadPoolInfoByLevel(int level) {	

//	ThreadPoolExecutorPtr pool = TaskManager::instance().findExecutor(level);
//  size_t poolSize = pool->getPoolSize();
//	size_t corePoolSize = pool->getCorePoolSize(); 
//	size_t maxPoolSize = pool->getMaximumPoolSize();
//	size_t queueSize = pool->getQueueSize();
//	size_t waitSize = pool->getWaitThreadSize();
//	size_t completedSize = pool->getCompletedTaskSize();
//	Ice::Long completedCount = pool->getCompletedTaskCount();
//
//	MCE_INFO("FeedCacheI::printThreadPoolInfoByLevel --> level: " << level << " poolSize:" << poolSize << " corePoolSize:" << corePoolSize << " maxPoolSize:" << maxPoolSize << " queueSize:" << queueSize << " waitSize:" << waitSize << " completedSize:" << completedSize << " completedCount:" << completedCount);
	
}

void FeedCacheI::test(const FeedItem& feed, const MyUtil::Int2IntMap& usersWeight, const Ice::Current&){
  //debug-------------------------------------

  //  MCE_DEBUG("FeedCacheI::test --> feedId:" << feed.feed);

  //  for (Int2IntMap::const_iterator it = usersWeight.begin(); it != usersWeight.end(); it++) {
  //    MCE_DEBUG("user:" << it->first << " weight:" << it->second);    
  //  } 

  //testLogic(feed, usersWeight); 
}
void FeedCacheI::testLogic(const FeedItem& feed, const MyUtil::Int2IntMap& usersWeight){
  //  for (MyUtil::Int2IntMap::const_iterator it = usersWeight.begin(); it != usersWeight.end(); it++) {
  //     get(it->first);
  //   }
  //   put(feed, usersWeight);

//  FeedDBTAdapter::instance().put(feed, usersWeight);
 // return ;
}

FeedCacheConfigPtr FeedCacheI::getConfig(const Ice::Current&){
  IceUtil::Mutex::Lock lock(_config_mutex);
  return _cfg;	
}
/*MyUtil::Str2StrMap FeedCacheI::getConfig(const Ice::Current&){
  MyUtil::Str2StrMap cfgs;
  cfgs["deleted_count_cfg"] = boost::lexical_cast<string>(FeedCacheConfig::instance().deletedCount());
  cfgs["cache_szie_cfg"] = boost::lexical_cast<string>(FeedCacheConfig::instance().cacheSize());
  return cfgs;	
  }*/

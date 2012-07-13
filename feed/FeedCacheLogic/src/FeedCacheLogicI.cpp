/*
 * FeedCacheLogicI.cpp
 *
 *  Created on: 2011-8-3
 *      Author: wu
 */

#include "FeedCacheNAdapter.h"
#include "FeedCacheNReplicaAdapter.h"
#include "FeedNewsAdapter.h"
#include "FeedDBTAdapter.h"
#include "SSDFeedDBTAdapter.h"
#include <boost/lexical_cast.hpp>
#include "FeedCacheLogicI.h"
#include "ServiceI.h"
#include "FeedMemcProxy/client/feed_user_property.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

#include <algorithm>
#include <functional>
#include <vector>
#include <map>
#include <time.h>
#include <sstream>

using namespace xce::feed;

using namespace MyUtil;

using namespace std;

const float ScoreFeedItem::interaction = 0.4;

const int FeedCacheLogicI::FEED_RETURN_COUNT = 220;
const int FeedCacheLogicI::FEED_COUNT = 250;

bool xce::feed::operator< (const ScoreFeedItem & s_feed_item1, const ScoreFeedItem & s_feed_item2) {
  return s_feed_item1.score() < s_feed_item2.score();
}


void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&FeedCacheLogicI::instance(), service.createIdentity("M", ""));

  //---------------HA---------------//
  
  int mod = service.getCommunicator()->getProperties()->getPropertyAsInt(
      "FeedCacheLogic.Mod");
  int interval =service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault(
      "FeedCacheLogic.Interval", 5);
  xce::serverstate::ServerStateSubscriber::instance().initialize("ControllerFeedCacheLogicR",
      &FeedCacheLogicI::instance(), mod, interval, new XceFeedControllerChannel());
  MCE_INFO("MyUtil::initialize. mod:" << mod << " interval:" << interval);
   

}


Ice::Long FeedCacheLogicI::findMaxFeedId(const ::xce::feed::FeedItemSeq & seq) {
  Ice::Long maxId = 0;     
  for(::xce::feed::FeedItemSeq::const_iterator it = seq.begin(); it != seq.end(); it++) {
    if (it->feed > maxId) {
      maxId = it->feed;
    }
  }
  return maxId;
}

MyUtil::LongSeq FeedCacheLogicI::resortFeedItems(::Ice::Int uid, const ::xce::feed::FeedItemSeq & seq, const ::Ice::Current& current) {
  MCE_INFO("FeedCacheLogicI::resortFeedItems --> uid:" << uid << " raw seq size:" << seq.size());
  LongSeq retSeq;

  timeval beg, end;
  gettimeofday(&beg, NULL);
  Ice::Long max_read_fid  = 0;
  Ice::Long seq_max_fid  = 0;
  try {
   FeedUserPropertyData data = FeedUserPropertyClient::instance().Get(uid);
   max_read_fid = data.GetMaxFeedId();
  }catch(Ice::Exception& e) {
    MCE_WARN("FeedCacheLogicI::resortFeedItems --> FeedUserPropertyClient exception! userId:" << uid << " exception:" << e);
  }catch(std::exception& e) {
    MCE_WARN("FeedCacheLogicI::resortFeedItems --> FeedUserPropertyClient exception! userId:" << uid << " exception:" << e.what());
  }catch(...) {
    MCE_WARN("FeedCacheLogicI::resortFeedItems --> FeedUserPropertyClient exception! userId:" << uid << " unknown exception.");
  }

  gettimeofday(&end, NULL);
  float timeuse=1000000*(end.tv_sec-beg.tv_sec)+end.tv_usec-beg.tv_usec;
  timeuse/=1000;
   MCE_INFO("FeedCacheLogicI::resortFeedItems --> uid:" << uid << " max_read_fid:"<< max_read_fid << " time use:" << timeuse);  

  if ((int)seq.size() < FEED_COUNT) {
    MCE_WARN("FeedCacheLogicI::resortFeedItems --> uid:" << uid << " raw seq size error." << seq.size() << " con:" << current.con->toString());
    return retSeq;
  }

  seq_max_fid = findMaxFeedId(seq);
  
  map<long, int> friendToFeedsCountMap;

  vector<ScoreFeedItem> sortSeq;
  map<long, int>::iterator it;

  for(int i=0; i < seq.size(); i++)
  {
    ScoreFeedItem sFeedItem(seq[i], i, max_read_fid, seq_max_fid, uid);
    //sFeedItem.setRawIndex(i);
    sortSeq.push_back(sFeedItem); 
  }

  //First sort
  stable_sort(sortSeq.begin(), sortSeq.end(),less<ScoreFeedItem>());

  for(int i=0; i < sortSeq.size(); i++) {
    long actor = sortSeq[i].item().actor;
    it = friendToFeedsCountMap.find(actor);
    if (it != friendToFeedsCountMap.end()) {
      (it->second)++;
      sortSeq[i].setFriendPos(it->second);
    } else {
      friendToFeedsCountMap[actor] = 1;
      sortSeq[i].setFriendPos(1);
    }
    sortSeq[i].setSecondScore();
  }


  stable_sort(sortSeq.begin(), sortSeq.end(),less<ScoreFeedItem>());

  int retsize = (int)sortSeq.size()- FEED_RETURN_COUNT;
  for(int i = 0; i < retsize; i++) {
    retSeq.push_back(sortSeq[i].item().feed);
  }

  /*
     try{
     if(retSeq.size()) {
     FeedNews50Adapter::instance().EraseUser(uid);
     MCE_INFO("FeedCacheLogicI::ReSortFeedItems --> FeedNews EraseUser uid:" << uid << " retSeq size:" << retSeq.size() << " sortSeq size:" << sortSeq.size());
     }
     }catch(Ice::Exception& e) {
     MCE_WARN("FeedCacheLogicI::ReSortFeedItems --> FeedNews EraseUser uid:" << uid << " retSeq size:" << retSeq.size() << " exception:" << e);
     }catch(std::exception& e) {
     MCE_WARN("FeedCacheLogicI::ReSortFeedItems --> FeedNews EraseUser uid:" << uid << " retSeq size:" << retSeq.size() << " exception:" << e.what());
     }catch(...) {
     MCE_WARN("FeedCacheLogicI::ReSortFeedItems --> FeedNews EraseUser uid:" << uid << " retSeq size:" << retSeq.size() << " unknow exception!");
     }
   */

  gettimeofday(&end, NULL);
  float timeuse2=1000000*(end.tv_sec-beg.tv_sec)+end.tv_usec-beg.tv_usec;
  timeuse2/=1000;

  MCE_INFO("FeedCacheLogicI::resortFeedItems --> uid:" << uid << " retSeq size:" << retSeq.size() << " time use" << timeuse2);
  ostringstream os; 
  for(LongSeq::const_iterator it = retSeq.begin(); it != retSeq.end(); it++) { 
    os << *it << " "; 
  }
  MCE_INFO("FeedCacheLogicI::resortFeedItems --> uid:" << uid << " feedid list:" << os.str());
  return retSeq;
}


FeedItemSeq   FeedCacheLogicI::get(int userId, int begin, int limit,  const Ice::Current& current) {
  timeval beg, end;
  gettimeofday(&beg, NULL);

  FeedItemSeq rawSeq;
  try{
    //rawSeq = SSDFeedDBTAdapter::instance().get(userId, begin, limit);
    rawSeq = FeedDBTAdapter::instance().get(userId, begin, limit);
  }catch(Ice::Exception& e) {
    MCE_WARN("FeedCacheLogicI::get --> SSDFeedDBTAdapter exception! userId:" << userId << "  begin:" << begin << " limit" << limit << " exception:" << e);
  }catch(std::exception& e) {
    MCE_WARN("FeedCacheLogicI::get --> SSDFeedDBTAdapter exception! userId:" << userId << "  begin:" << begin << " limit" << limit << " exception:" << e.what());
  }catch(...) {
    MCE_WARN("FeedCacheLogicI::get --> SSDFeedDBTAdapter exception! userId:" << userId << "  begin:" << begin << " limit" << limit << " unknow exception!");
  }

  gettimeofday(&end, NULL);
  float timeuse=1000000*(end.tv_sec-beg.tv_sec)+end.tv_usec-beg.tv_usec;
  timeuse/=1000;
  MCE_INFO("FeedCacheLogicI::get --> get FeedDB time:" << timeuse << "userId:" << userId << "  begin:" << begin << " limit" << limit);

  FeedItemSeq  items;

  if ((int)rawSeq.size() >= FeedCacheLogicI::FEED_COUNT) {

    const vector<Ice::Long> & rejectedIds = resortFeedItems(userId, rawSeq);

    set<Ice::Long> rejectedSet;
    for(vector<Ice::Long>::const_iterator it = rejectedIds.begin(); it != rejectedIds.end(); it++) {
      rejectedSet.insert(*it);
    }

    FeedItemSeq::iterator it = rawSeq.begin();
    for(; it != rawSeq.end(); it++) {
      if(!rejectedSet.count(it->feed)) {
        items.push_back(*it);
      }
    }
  } else {
    MCE_INFO("FeedCacheLogicI::get --> userId:" << userId << " begin:" << begin << " limit:" << limit << " rawSeq size:" << rawSeq.size());
    return rawSeq;
  }

  MCE_INFO("FeedCacheLogicI::get --> userId:" << userId << " begin:" << begin << " limit:" << limit << " rawSeq size:" << rawSeq.size() << " rejectedSeq size:" << items.size());
  return items;
}

void    FeedCacheLogicI::put(const FeedItem& item, const MyUtil::Int2IntMap& usersWeight, bool syncFeedDB, const Ice::Current&) {
  if(item.time < 0) {
    return ;
  }
  timeval beg, end;
  gettimeofday(&beg, NULL);

/*
  try{
    FeedCacheNAdapter::instance().put(item, usersWeight);   
  }catch(Ice::Exception& e) {
    MCE_WARN("FeedCacheLogicI::put--> FeedCacheNAdapter exception! feedId:" << item.feed << "  usersWeight size:" << usersWeight.size() << " exception:" << e);
  }catch(std::exception& e) {
    MCE_WARN("FeedCacheLogicI::put--> FeedCacheNAdapter exception! feedId:" << item.feed << "  usersWeight size:" << usersWeight.size() << " exception:" << e.what());
  }catch(...) {
    MCE_WARN("FeedCacheLogicI::put--> FeedCacheNAdapter exception! feedId:" << item.feed << "  usersWeight size:" << usersWeight.size() << " unknown exception!");
  }
*/

  FeedCacheNReplicaAdapter::instance().put(item, usersWeight);   ////

  gettimeofday(&end, NULL);
  float timeuse=1000000*(end.tv_sec-beg.tv_sec)+end.tv_usec-beg.tv_usec;
  timeuse/=1000;
  MCE_INFO("FeedCacheLogicI::put --> put FeedCache time:" << timeuse << " feedId:" << item.feed << "  usersWeight size:" << usersWeight.size()); 

  if (syncFeedDB) {
    FeedCacheConfigPtr cfg = getConfig();
    map<string, FeedDBConfigPtr>::iterator cIt = cfg->feedDBCfgs.begin();
    for(; cIt!=cfg->feedDBCfgs.end(); ++cIt){
      if(cIt->first == "FeedDB" && cIt->second->syncPut){
        try{
          timeval beg, end;
          gettimeofday(&beg, NULL);
          FeedDBTAdapter::instance().put(item, usersWeight);
          gettimeofday(&end, NULL);
          float timeuse=1000000*(end.tv_sec-beg.tv_sec)+end.tv_usec-beg.tv_usec;
          timeuse/=1000;
          MCE_INFO("FeedCacheLogicI::put --> FeedDB feed:" << item.feed << " actor:" << item.actor 
              << " stype:" << (item.type&0xFFFF) << " size:" << usersWeight.size() << " timeuse:" << timeuse);
        }catch(Ice::Exception& e){
          MCE_WARN("FeedCacheLogicI::put --> FeedDBTAapter expception.  feedid:" << item.feed << " "  << e);
        }catch(std::exception& e) {
          MCE_WARN("FeedCacheLogicI::put --> FeedDBTAapter exception. feedid:" << item.feed <<  " exception:" << e.what());
        }catch(...) {
          MCE_WARN("FeedCacheLogicI::put --> FeedDBTAapter exception. feedid:" << item.feed <<  " unknown exception!");
        }

        try{
          timeval beg, end;
          gettimeofday(&beg, NULL);
          SSDFeedDBTAdapter::instance().put(item, usersWeight);
          gettimeofday(&end, NULL);
          float timeuse=1000000*(end.tv_sec-beg.tv_sec)+end.tv_usec-beg.tv_usec;
          timeuse/=1000;
          MCE_INFO("FeedCacheLogicI::put --> SSDFeedDB feed:" << item.feed << " actor:" << item.actor 
              << " stype:" << (item.type&0xFFFF) << " size:" << usersWeight.size() << " timeuse:" << timeuse);
        }catch(Ice::Exception& e){
          MCE_WARN("FeedCacheLogicI::put --> SSDFeedDBTAapter expception.  feedid:" << item.feed << " "  << e);
        }catch(std::exception& e) {
          MCE_WARN("FeedCacheLogicI::put --> SSDFeedDBTAapter exception. feedid:" << item.feed <<  " exception:" << e.what());
        }catch(...) {
          MCE_WARN("FeedCacheLogicI::put --> SSDFeedDBTAapter exception. feedid:" << item.feed <<  " unknown exception!");
        }

      }
    }
  }

//  timeval beg_sql, end_sql;
//  gettimeofday(&beg_sql, NULL);
//
//  //PersistenceHelper::write(item);
//
//  gettimeofday(&end_sql, NULL);
//  float timeuse_sql=1000000*(end_sql.tv_sec-beg_sql.tv_sec)+end_sql.tv_usec-beg_sql.tv_usec;
//  timeuse_sql/=1000;
//  MCE_INFO("FeedCacheLogicI::put --> write sql. feedid:" << item.feed << " actor:" << item.actor 
//      << " stype:" << (item.type&0xFFFF) << " size:" << usersWeight.size() << " timeuse:" << timeuse_sql);
}

void    FeedCacheLogicI::remove(int userId, Ice::Long feedId, bool syncFeedDB, const Ice::Current&) {
  MCE_INFO("FeedCacheLogicI::removeMergeByFeedId -->  userId:" << userId << " feedId:" << feedId << " syncFeedDB:" << syncFeedDB);
  MyUtil::LongSeq  deletedSeq;

  timeval beg, end;
  gettimeofday(&beg, NULL);

/*
  try {
    deletedSeq = FeedCacheNAdapter::instance().removeMergeByFeedId(userId, feedId); 
  }catch(Ice::Exception& e) {
    MCE_WARN("FeedCacheLogicI::removeMergeByFeedId --> FeedCacheNAdapter exception! userId:" << userId << " feedId:" << feedId << " syncFeedDB" << syncFeedDB << " exception:" << e);
  }catch(std::exception& e) {
    MCE_WARN("FeedCacheLogicI::removeMergeByFeedId --> FeedCacheNAdapter exception! userId:" << userId << " feedId:" << feedId << " syncFeedDB" << syncFeedDB << " exception:" << e.what());
  }catch(...) {
    MCE_WARN("FeedCacheLogicI::removeMergeByFeedId --> FeedCacheNAdapter exception! userId:" << userId << " feedId:" << feedId <<  " syncFeedDB" << syncFeedDB << " unknown exception!");
  }
*/

  deletedSeq = FeedCacheNReplicaAdapter::instance().removeMergeByFeedId(userId, feedId); ///////

  gettimeofday(&end, NULL);
  float timeuse=1000000*(end.tv_sec-beg.tv_sec)+end.tv_usec-beg.tv_usec;
  timeuse/=1000;
  MCE_INFO("FeedCacheLogicI::removeMergeByFeedId --> removeMergeByFeedId FeedCacheN time:" << timeuse << " userId:" << userId << " feedId:" << feedId << " deletedIds size:" << deletedSeq.size() << " syncFeedDB:" << syncFeedDB);


  if (syncFeedDB) {
    FeedCacheConfigPtr cfg = getConfig();
    map<string, FeedDBConfigPtr>::iterator it = cfg->feedDBCfgs.begin();
    for(; it!=cfg->feedDBCfgs.end(); ++it){
      if(it->first == "FeedDB" && it->second->syncRemove){
        try{
          timeval beg, end;
          gettimeofday(&beg, NULL);

          ///////////////////
          if (deletedSeq.size() == 0) {
            FeedDBTAdapter::instance().remove(userId, feedId);
            MCE_INFO("FeedCacheLogicI::removeMergeByFeedId --> force remove. userId:" << userId << " feedId:" << feedId);
          } 
          /////////////////////////

          for(MyUtil::LongSeq::iterator sit = deletedSeq.begin(); sit != deletedSeq.end(); sit++) {
            FeedDBTAdapter::instance().remove(userId, *sit);
          }

          gettimeofday(&end, NULL);
          float timeuse=1000000*(end.tv_sec-beg.tv_sec)+end.tv_usec-beg.tv_usec;
          timeuse/=1000;
          MCE_INFO("FeedCacheLogicI::removeMergeByFeedId --> remove fromFeedDB time:" << timeuse << "userId:" << userId << " feedId:" << feedId << " deletedIds size:" << deletedSeq.size() << " syncFeedDB:" << syncFeedDB);

        }catch(Ice::Exception& e){
          MCE_WARN("FeedCacheLogicI::removeMergeByFeedId --> invoke FeeddB err, " << e);
        }catch(std::exception& e) {
          MCE_WARN("FeedCacheLogicI::removeMergeByFeedId --> FeedDB exception! userId:" << userId << " feedId:" << feedId << " syncFeedDB" << syncFeedDB << " exception:" << e.what());
        }catch(...) {
          MCE_WARN("FeedCacheLogicI::removeMergeByFeedId --> FeedDB exception! userId:" << userId << " feedId:" << feedId << " syncFeedDB" << syncFeedDB << " unknown exception!");
        }

        try{
          timeval beg, end;
          gettimeofday(&beg, NULL);

          ///////////////////
          if (deletedSeq.size() == 0) {
            SSDFeedDBTAdapter::instance().remove(userId, feedId);
            MCE_INFO("FeedCacheLogicI::removeMergeByFeedId --> force remove. userId:" << userId << " feedId:" << feedId);
          } 
          /////////////////////////

          for(MyUtil::LongSeq::iterator sit = deletedSeq.begin(); sit != deletedSeq.end(); sit++) {
            SSDFeedDBTAdapter::instance().remove(userId, *sit);
          }

          gettimeofday(&end, NULL);
          float timeuse=1000000*(end.tv_sec-beg.tv_sec)+end.tv_usec-beg.tv_usec;
          timeuse/=1000;
          MCE_INFO("FeedCacheLogicI::removeMergeByFeedId --> remove from SSDFeedDB time:" << timeuse << "userId:" << userId << " feedId:" << feedId << " deletedIds size:" << deletedSeq.size() << " syncFeedDB:" << syncFeedDB);

        }catch(Ice::Exception& e){
          MCE_WARN("FeedCacheLogicI::removeMergeByFeedId --> invoke FeeddB err, " << e);
        }catch(std::exception& e) {
          MCE_WARN("FeedCacheLogicI::removeMergeByFeedId --> SSDFeedDB exception! userId:" << userId << " feedId:" << feedId << " syncFeedDB" << syncFeedDB << " exception:" << e.what());
        }catch(...) {
          MCE_WARN("FeedCacheLogicI::removeMergeByFeedId --> SSDFeedDB exception! userId:" << userId << " feedId:" << feedId << " syncFeedDB" << syncFeedDB << " unknown exception!");
        }

      }
    }
  }

}
/*
void    FeedCacheLogicI::remove(int userId, Ice::Long feedId, bool syncFeedDB, const Ice::Current&) {
	MCE_INFO("FeedCacheLogicI::remove -->  userId:" << userId << " feedId:" << feedId << " syncFeedDB:" << syncFeedDB);
  timeval beg, end;
  gettimeofday(&beg, NULL);

  try{
    FeedCacheNAdapter::instance().remove(userId, feedId); 
  }catch(Ice::Exception& e) {
    MCE_WARN("FeedCacheLogicI::remove --> FeedCacheNAdapter exception! userId:" << userId << "  feedId:" << feedId<< " syncFeedDB" << syncFeedDB << " exception:" << e);
  }catch(std::exception& e) {
    MCE_WARN("FeedCacheLogicI::remove --> FeedCacheNAdapter exception! userId:" << userId << "  feedId:" << feedId<< " syncFeedDB" << syncFeedDB << " exception:" << e.what());
  }catch(...) {
    MCE_WARN("FeedCacheLogicI::remove --> FeedCacheNAdapter exception! userId:" << userId << "  feedId:" << feedId<< " syncFeedDB" << syncFeedDB << " unknown exception!");
  }

  gettimeofday(&end, NULL);
  float timeuse=1000000*(end.tv_sec-beg.tv_sec)+end.tv_usec-beg.tv_usec;
  timeuse/=1000;
  MCE_INFO("FeedCacheLogicI::get --> remove FeedCacheN time:" << timeuse << "userId:" << userId << "  feedId:" << feedId << " syncFeedDB:" << syncFeedDB);


  if (syncFeedDB) {
    FeedCacheConfigPtr cfg = getConfig();
    map<string, FeedDBConfigPtr>::iterator it = cfg->feedDBCfgs.begin();
    for(; it!=cfg->feedDBCfgs.end(); ++it){
      if(it->first == "FeedDB" && it->second->syncRemove){
        MCE_INFO("FeedCacheLogicI::remove --> FeedDB " << userId << " " << feedId);
        try{
          timeval beg, end;
          gettimeofday(&beg, NULL);

          FeedDBTAdapter::instance().remove(userId, feedId);

          gettimeofday(&end, NULL);
          float timeuse=1000000*(end.tv_sec-beg.tv_sec)+end.tv_usec-beg.tv_usec;
          timeuse/=1000;
          MCE_INFO("FeedCacheLogicI::remove --> remove FeedDB time:" << timeuse << "userId:" << userId << "  feedId:" << feedId << " syncFeedDB:" << syncFeedDB);


        }catch(Ice::Exception& e){
          MCE_WARN("FeedCacheLogicI::remove --> invoke FeeddB err, " << feedId << " " << e);
        }catch(std::exception& e) {
          MCE_WARN("FeedCacheLogicI::remove --> FeedDB exception! userId:" << userId << "  feedId:" << feedId<< " syncFeedDB" << syncFeedDB << " exception:" << e.what());
        }catch(...) {
          MCE_WARN("FeedCacheLogicI::remove --> FeedDB exception! userId:" << userId << "  feedId:" << feedId<< " syncFeedDB" << syncFeedDB << " unknown exception!");
        }

      }
    }
  }
}
*/

void    FeedCacheLogicI::removeAll(int userId, bool syncFeedDB, const Ice::Current&) {
  MCE_INFO("FeedCacheLogicI::removeAll -->  userId:" << userId << " syncFeedDB:" << syncFeedDB);

  timeval beg, end;
  gettimeofday(&beg, NULL);

/*
  try{
    FeedCacheNAdapter::instance().removeAll(userId); 
  }catch(Ice::Exception& e) {
    MCE_WARN("FeedCacheLogicI::removeAll --> FeedCacheNAdapter exception! userId:" << userId << " syncFeedDB" << syncFeedDB << " exception:" << e);
  }catch(std::exception& e) {
    MCE_WARN("FeedCacheLogicI::removeAll --> FeedCacheNAdapter exception! userId:" << userId << " syncFeedDB" << syncFeedDB << " exception:" << e.what());
  }catch(...) {
    MCE_WARN("FeedCacheLogicI::removeAll --> FeedCacheNAdapter exception! userId:" << userId << " syncFeedDB" << syncFeedDB << " unknown exception!");
  }
*/

  FeedCacheNReplicaAdapter::instance().removeAll(userId); 

  gettimeofday(&end, NULL);
  float timeuse=1000000*(end.tv_sec-beg.tv_sec)+end.tv_usec-beg.tv_usec;
  timeuse/=1000;
  MCE_INFO("FeedCacheLogicI::removeAll --> removeAll FeedCacheN time:" << timeuse << "userId:" << userId << " syncFeedDB:" << syncFeedDB);


  if (syncFeedDB) {
    FeedCacheConfigPtr cfg = getConfig();
    map<string, FeedDBConfigPtr>::iterator it = cfg->feedDBCfgs.begin();
    for(; it!=cfg->feedDBCfgs.end(); ++it){
      if(it->first == "FeedDB" && it->second->syncRemove){
        try{
          timeval beg, end;
          gettimeofday(&beg, NULL);

          FeedDBTAdapter::instance().removeAll(userId);

          gettimeofday(&end, NULL);
          float timeuse=1000000*(end.tv_sec-beg.tv_sec)+end.tv_usec-beg.tv_usec;
          timeuse/=1000;
          MCE_INFO("FeedCacheLogicI::removeAll --> removeAll FeedDB time:" << timeuse << "userId:" << userId << " syncFeedDB:" << syncFeedDB);

        }catch(Ice::Exception& e){
          MCE_WARN("FeedCacheLogicI::removeAll --> invoke FeeddB err, " << e);
        }catch(std::exception& e) {
          MCE_WARN("FeedCacheLogicI::removeAll --> FeedDB exception! userId:" << userId << " syncFeedDB" << syncFeedDB << " exception:" << e.what());
        }catch(...) {
          MCE_WARN("FeedCacheLogicI::removeAll --> FeedDB exception! userId:" << userId << " syncFeedDB" << syncFeedDB << " unknown exception!");
        }

        try{
          timeval beg, end;
          gettimeofday(&beg, NULL);

          SSDFeedDBTAdapter::instance().removeAll(userId);

          gettimeofday(&end, NULL);
          float timeuse=1000000*(end.tv_sec-beg.tv_sec)+end.tv_usec-beg.tv_usec;
          timeuse/=1000;
          MCE_INFO("FeedCacheLogicI::removeAll --> removeAll SSDFeedDB time:" << timeuse << "userId:" << userId << " syncFeedDB:" << syncFeedDB);

        }catch(Ice::Exception& e){
          MCE_WARN("FeedCacheLogicI::removeAll --> invoke FeeddB err, " << e);
        }catch(std::exception& e) {
          MCE_WARN("FeedCacheLogicI::removeAll --> SSDFeedDB exception! userId:" << userId << " syncFeedDB" << syncFeedDB << " exception:" << e.what());
        }catch(...) {
          MCE_WARN("FeedCacheLogicI::removeAll --> SSDFeedDB exception! userId:" << userId << " syncFeedDB" << syncFeedDB << " unknown exception!");
        }

      }
    }
  }
}

void  FeedCacheLogicI::removeByMerge(int userId, long merge, int stype, bool syncFeedDB,  const Ice::Current&) {
  MCE_INFO("FeedCacheLogicI::removeByMerge -->  userId:" << userId << " syncFeedDB:" << syncFeedDB);
  MyUtil::LongSeq  deletedSeq;

  timeval beg, end;
  gettimeofday(&beg, NULL);

/*
  try {
    deletedSeq = FeedCacheNAdapter::instance().removeByMerge(userId, merge, stype); 
  }catch(Ice::Exception& e) {
    MCE_WARN("FeedCacheLogicI::removeByMerge --> FeedCacheNAdapter exception! userId:" << userId << " merge:" << merge << " stype:" << stype << " syncFeedDB" << syncFeedDB << " exception:" << e);
  }catch(std::exception& e) {
    MCE_WARN("FeedCacheLogicI::removeByMerge --> FeedCacheNAdapter exception! userId:" << userId << " merge:" << merge << " stype:" << stype << " syncFeedDB" << syncFeedDB << " exception:" << e.what());
  }catch(...) {
    MCE_WARN("FeedCacheLogicI::removeByMerge --> FeedCacheNAdapter exception! userId:" << userId << " merge:" << merge << " stype:" << stype << " syncFeedDB" << syncFeedDB << " unknown exception!");
  }
*/

  deletedSeq = FeedCacheNReplicaAdapter::instance().removeByMerge(userId, merge, stype); 

  gettimeofday(&end, NULL);
  float timeuse=1000000*(end.tv_sec-beg.tv_sec)+end.tv_usec-beg.tv_usec;
  timeuse/=1000;
  MCE_INFO("FeedCacheLogicI::removeByMerge --> removeByMerge FeedCacheN time:" << timeuse << "userId:" << userId << " merge:" << merge << " stype:" << stype << " syncFeedDB:" << syncFeedDB);


  if (syncFeedDB) {
    FeedCacheConfigPtr cfg = getConfig();
    map<string, FeedDBConfigPtr>::iterator it = cfg->feedDBCfgs.begin();
    for(; it!=cfg->feedDBCfgs.end(); ++it){
      if(it->first == "FeedDB" && it->second->syncRemove){
        try{
          timeval beg, end;
          gettimeofday(&beg, NULL);

          for(MyUtil::LongSeq::iterator sit = deletedSeq.begin(); sit != deletedSeq.end(); sit++) {
            FeedDBTAdapter::instance().remove(userId, *sit);
          }

          gettimeofday(&end, NULL);
          float timeuse=1000000*(end.tv_sec-beg.tv_sec)+end.tv_usec-beg.tv_usec;
          timeuse/=1000;
          MCE_INFO("FeedCacheLogicI::removeByMerge --> removeByMerge FeedDB time:" << timeuse << "userId:" << userId << " syncFeedDB:" << syncFeedDB);

        }catch(Ice::Exception& e){
          MCE_WARN("FeedCacheLogicI::removeByMerge --> invoke FeeddB err, " << e);
        }catch(std::exception& e) {
          MCE_WARN("FeedCacheLogicI::removeByMerge --> FeedDB exception! userId:" << userId << " syncFeedDB" << syncFeedDB << " exception:" << e.what());
        }catch(...) {
          MCE_WARN("FeedCacheLogicI::removeByMerge --> FeedDB exception! userId:" << userId << " syncFeedDB" << syncFeedDB << " unknown exception!");
        }

        try{
          timeval beg, end;
          gettimeofday(&beg, NULL);

          for(MyUtil::LongSeq::iterator sit = deletedSeq.begin(); sit != deletedSeq.end(); sit++) {
            SSDFeedDBTAdapter::instance().remove(userId, *sit);
          }

          gettimeofday(&end, NULL);
          float timeuse=1000000*(end.tv_sec-beg.tv_sec)+end.tv_usec-beg.tv_usec;
          timeuse/=1000;
          MCE_INFO("FeedCacheLogicI::removeByMerge --> removeByMerge SSDFeedDB time:" << timeuse << "userId:" << userId << " syncFeedDB:" << syncFeedDB);

        }catch(Ice::Exception& e){
          MCE_WARN("FeedCacheLogicI::removeByMerge --> invoke FeeddB err, " << e);
        }catch(std::exception& e) {
          MCE_WARN("FeedCacheLogicI::removeByMerge --> SSDFeedDB exception! userId:" << userId << " syncFeedDB" << syncFeedDB << " exception:" << e.what());
        }catch(...) {
          MCE_WARN("FeedCacheLogicI::removeByMerge --> SSDFeedDB exception! userId:" << userId << " syncFeedDB" << syncFeedDB << " unknown exception!");
        }

      }
    }
  }
}

void    FeedCacheLogicI::removeMergeByFeedId(int userId, Ice::Long feedId, bool syncFeedDB, const Ice::Current&) {
  MCE_INFO("FeedCacheLogicI::removeMergeByFeedId -->  userId:" << userId << " feedId:" << feedId << " syncFeedDB:" << syncFeedDB);
  MyUtil::LongSeq  deletedSeq;

  timeval beg, end;
  gettimeofday(&beg, NULL);

/*
  try {
    deletedSeq = FeedCacheNAdapter::instance().removeMergeByFeedId(userId, feedId); 
  }catch(Ice::Exception& e) {
    MCE_WARN("FeedCacheLogicI::removeMergeByFeedId --> FeedCacheNAdapter exception! userId:" << userId << " feedId:" << feedId << " syncFeedDB" << syncFeedDB << " exception:" << e);
  }catch(std::exception& e) {
    MCE_WARN("FeedCacheLogicI::removeMergeByFeedId --> FeedCacheNAdapter exception! userId:" << userId << " feedId:" << feedId << " syncFeedDB" << syncFeedDB << " exception:" << e.what());
  }catch(...) {
    MCE_WARN("FeedCacheLogicI::removeMergeByFeedId --> FeedCacheNAdapter exception! userId:" << userId << " feedId:" << feedId <<  " syncFeedDB" << syncFeedDB << " unknown exception!");
  }
*/
  deletedSeq = FeedCacheNReplicaAdapter::instance().removeMergeByFeedId(userId, feedId); 

  gettimeofday(&end, NULL);
  float timeuse=1000000*(end.tv_sec-beg.tv_sec)+end.tv_usec-beg.tv_usec;
  timeuse/=1000;
  MCE_INFO("FeedCacheLogicI::removeMergeByFeedId --> removeMergeByFeedId FeedCacheN time:" << timeuse << " userId:" << userId << " feedId:" << feedId << " deletedIds size:" << deletedSeq.size() << " syncFeedDB:" << syncFeedDB);


  if (syncFeedDB) {
    FeedCacheConfigPtr cfg = getConfig();
    map<string, FeedDBConfigPtr>::iterator it = cfg->feedDBCfgs.begin();
    for(; it!=cfg->feedDBCfgs.end(); ++it){
      if(it->first == "FeedDB" && it->second->syncRemove){
        try{
          timeval beg, end;
          gettimeofday(&beg, NULL);

          ///////////////////
          if (deletedSeq.size() == 0) {
            FeedDBTAdapter::instance().remove(userId, feedId);
            MCE_INFO("FeedCacheLogicI::removeMergeByFeedId --> force remove. userId:" << userId << " feedId:" << feedId);
          } 
          /////////////////////////

          for(MyUtil::LongSeq::iterator sit = deletedSeq.begin(); sit != deletedSeq.end(); sit++) {
            FeedDBTAdapter::instance().remove(userId, *sit);
          }

          gettimeofday(&end, NULL);
          float timeuse=1000000*(end.tv_sec-beg.tv_sec)+end.tv_usec-beg.tv_usec;
          timeuse/=1000;
          MCE_INFO("FeedCacheLogicI::removeMergeByFeedId --> remove fromFeedDB time:" << timeuse << "userId:" << userId << " feedId:" << feedId << " deletedIds size:" << deletedSeq.size() << " syncFeedDB:" << syncFeedDB);

        }catch(Ice::Exception& e){
          MCE_WARN("FeedCacheLogicI::removeMergeByFeedId --> invoke FeeddB err, " << e);
        }catch(std::exception& e) {
          MCE_WARN("FeedCacheLogicI::removeMergeByFeedId --> FeedDB exception! userId:" << userId << " feedId:" << feedId << " syncFeedDB" << syncFeedDB << " exception:" << e.what());
        }catch(...) {
          MCE_WARN("FeedCacheLogicI::removeMergeByFeedId --> FeedDB exception! userId:" << userId << " feedId:" << feedId << " syncFeedDB" << syncFeedDB << " unknown exception!");
        }

        try{
          timeval beg, end;
          gettimeofday(&beg, NULL);

          ///////////////////
          if (deletedSeq.size() == 0) {
            SSDFeedDBTAdapter::instance().remove(userId, feedId);
            MCE_INFO("FeedCacheLogicI::removeMergeByFeedId --> force remove. userId:" << userId << " feedId:" << feedId);
          } 
          /////////////////////////

          for(MyUtil::LongSeq::iterator sit = deletedSeq.begin(); sit != deletedSeq.end(); sit++) {
            SSDFeedDBTAdapter::instance().remove(userId, *sit);
          }

          gettimeofday(&end, NULL);
          float timeuse=1000000*(end.tv_sec-beg.tv_sec)+end.tv_usec-beg.tv_usec;
          timeuse/=1000;
          MCE_INFO("FeedCacheLogicI::removeMergeByFeedId --> remove from SSDFeedDB time:" << timeuse << "userId:" << userId << " feedId:" << feedId << " deletedIds size:" << deletedSeq.size() << " syncFeedDB:" << syncFeedDB);

        }catch(Ice::Exception& e){
          MCE_WARN("FeedCacheLogicI::removeMergeByFeedId --> invoke FeeddB err, " << e);
        }catch(std::exception& e) {
          MCE_WARN("FeedCacheLogicI::removeMergeByFeedId --> SSDFeedDB exception! userId:" << userId << " feedId:" << feedId << " syncFeedDB" << syncFeedDB << " exception:" << e.what());
        }catch(...) {
          MCE_WARN("FeedCacheLogicI::removeMergeByFeedId --> SSDFeedDB exception! userId:" << userId << " feedId:" << feedId << " syncFeedDB" << syncFeedDB << " unknown exception!");
        }

      }
    }
  }

}
void    FeedCacheLogicI::updateConfig(const FeedCacheConfigPtr& cfg, const Ice::Current&) {
  MCE_INFO("FeedCacheLogicI::updateConfig --> conf. cacheSize:" << cfg->cacheSize << " deletedCount:" << cfg->deletedCount << " loadFrom:" << cfg->loadFrom << " enableCache:" << cfg->enableCache);
  FeedDBCfgsMap fgs = cfg->feedDBCfgs;
  for(FeedDBCfgsMap::iterator it = fgs.begin(); it != fgs.end(); it++) {
    MCE_INFO("FeedCacheLogicI::updateConfig --> feedDBCfgs:" << it->first << "=" << it->second);
  }


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

}

FeedCacheConfigPtr FeedCacheLogicI::getConfig(const Ice::Current&) {
  return _cfg;

}

void    FeedCacheLogicI::test(const FeedItem& feed, const MyUtil::Int2IntMap& usersWeight, bool syncDB, const Ice::Current&) {

}

void    FeedCacheLogicI::testLogic(const FeedItem& feed, const MyUtil::Int2IntMap& usersWeight, bool syncDB) {

}



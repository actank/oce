#include<Ice/Ice.h>
#include"FeedNewestI.h"
#include"ServiceI.h"
#include<QueryRunner.h>

using namespace com::xiaonei::xce;

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&FeedNewestI::instance(), service.createIdentity("simpleFeedNewest",""));
  TaskManager::instance().scheduleRepeated(new FeedNewestTimer());
}


void FeedNewestI::set(::Ice::Int uid, ::Ice::Long feedid, ::Ice::Int type,  const Ice::Current&) {
//	MCE_INFO("FeedNewestI::set --> " << uid);
  int containIndex = getContainIndex(uid);
  MCE_INFO("FeedNewestI::set the user--> " << uid <<" is divided into --> "<<containIndex<< "th container");
  {
    IceUtil::Mutex::Lock lock(mutex_[containIndex]);
//    MCE_INFO("error is not at -->1");
    UidIndex & index = container_[containIndex].get<1> ();
    UidIndex::iterator uit = index.find(uid);
//    MCE_INFO("error is not at -->2");
    if(uit != index.end()) {
      if(uit->IndexFeedtype == NULL)
        MCE_INFO("error is in the 1");
    if((type==0) && (feedid > (uit->IndexFeedtype->IndexFeedtype0))) { 
      uit->IndexFeedtype->IndexFeedtype0 = feedid;
      uit->setStat(true);
      }

//      MCE_INFO("error is not at -->3");
//      if(uit->IndexFeedtype == NULL)
//        MCE_INFO("error is here");
    if((type==1) && (feedid > (uit->IndexFeedtype->IndexFeedtype1))) {
//       MCE_INFO("error is not at -->4");                          
      uit->IndexFeedtype->IndexFeedtype1 = feedid;
//         MCE_INFO("error is not at -->5");
      uit->setStat(true);
//         MCE_INFO("error is not at -->6");
      }
//      MCE_INFO("error is not at -->4");
    SeqIndex::iterator sit = container_[containIndex].project<0> (uit);
    container_[containIndex].relocate(container_[containIndex].get<0> ().begin(),sit);
    MCE_INFO("FeedNewestI::set the user-->" <<uid <<"is indeed in the container -->  " << containIndex);
    return;
    }
  }
  MCE_INFO("FeedNewestI::set the user -->"<< uid <<"is not in container--> " << containIndex);
	//不在cache中
  NewestFeedIndexPtr result_db = getDB (uid);
  {
    IceUtil::Mutex::Lock lock(mutex_[containIndex]);

    if(result_db == NULL) {
    MCE_INFO("FeedNewest::setDB --> the user -->"<<uid<< "is not in DB");
    NewestFeedIdMap feedidmap;
    feedidmap.uid = uid;
    if(type == 0) {
      feedidmap.IndexFeedtype = new NewestFeedIndex();
      feedidmap.IndexFeedtype->IndexFeedtype0 = feedid;
      feedidmap.IndexFeedtype->IndexFeedtype1 = 0;
    }
    if(type == 1) {
      feedidmap.IndexFeedtype = new NewestFeedIndex();
      feedidmap.IndexFeedtype->IndexFeedtype0 = 0;
      feedidmap.IndexFeedtype->IndexFeedtype1 = feedid;
    }
    feedidmap.setStat(true);
    SeqIndex &sindex = container_[containIndex].get<0>();
    pair<SeqIndex::iterator,bool> p = container_[containIndex].push_front(feedidmap);
    if(feedidmap.IndexFeedtype == NULL) {
      MCE_INFO("err is in the 2");
    }
      
    if(!p.second) {
      sindex.relocate(sindex.begin(), p.first);
    }
    MCE_INFO("FeedNewestI::set -->the user--> " << uid<<" not in DB now is put into container-->  "<<containIndex);
    return ;
    }		
    MCE_INFO("FeedNewest::setDB --> the user -->"<<uid<< "is in DB");
    NewestFeedIdMap feedidmap;
    feedidmap.uid = uid;
    feedidmap.IndexFeedtype = new NewestFeedIndex();

    feedidmap.IndexFeedtype->IndexFeedtype0 = 0;
    feedidmap.IndexFeedtype->IndexFeedtype1 = 0;
    if((type==0) && (feedid > result_db->IndexFeedtype0)) {
		//	feedidmap.IndexFeedtype = new NewestFeedIndex();
      feedidmap.IndexFeedtype->IndexFeedtype0 = feedid;
      feedidmap.IndexFeedtype->IndexFeedtype1 = result_db->IndexFeedtype1;
      feedidmap.setStat(true);
    }

    if((type==1) && (feedid > result_db->IndexFeedtype1)) {
    //  feedidmap.IndexFeedtype = new NewestFeedIndex();
      feedidmap.IndexFeedtype->IndexFeedtype1 = feedid;
      feedidmap.IndexFeedtype->IndexFeedtype0 = result_db->IndexFeedtype0;
      feedidmap.setStat(true);
    }

    SeqIndex &sindex = container_[containIndex].get<0>();
    pair<SeqIndex::iterator,bool> p = container_[containIndex].push_front(feedidmap);
    if(feedidmap.IndexFeedtype == NULL) {
      MCE_INFO("err is in the 3");
    }
    if(!p.second) {
      sindex.relocate(sindex.begin(), p.first);
    }
    MCE_INFO("FeedNewestI::set -->the user--> " << uid<<" in DB now is put into container-->  "<<containIndex);

  } 

}

NewestFeedIndexPtr FeedNewestI::get(const int uid, const Ice::Current&) {
  int containIndex = getContainIndex(uid);

  {
    IceUtil::Mutex::Lock lock(mutex_[containIndex]);
    UidIndex & index =container_[containIndex].get<1> ();
    UidIndex::iterator uit = index.find(uid);
    if(uit != index.end()) {
      return(uit->IndexFeedtype); 
    }
	

  }
	//不在cache
  NewestFeedIndexPtr result_db = getDB (uid);
  {
    IceUtil::Mutex::Lock lock(mutex_[containIndex]);
    if(!result_db) {
      MCE_INFO("the user doesn't exit");
      return NULL;
    }
    NewestFeedIdMap feedidmap;
    feedidmap.uid = uid;
    feedidmap.IndexFeedtype = new NewestFeedIndex();	
    feedidmap.IndexFeedtype->IndexFeedtype0 = result_db->IndexFeedtype0;
    feedidmap.IndexFeedtype->IndexFeedtype1 = result_db->IndexFeedtype1;

    SeqIndex &sindex = container_[containIndex].get<0>();
    pair<SeqIndex::iterator,bool> p = container_[containIndex].push_front(feedidmap);

    if(!p.second){
      sindex.relocate(sindex.begin(), p.first);
    }
    return result_db;
  }	
}
void FeedNewestTask::handle() {
  MCE_INFO("the task is called");
  FeedNewestI::instance().setDB(FeedNewestI::instance().container_);
  MCE_INFO("the task is finished");
}

void FeedNewestTimer::handle() {
//	MCE_INFO("the timer is called");
  TaskManager::instance().execute(new FeedNewestTask());
}

NewestFeedIndexPtr  FeedNewestI :: getDB(int uid) {
  Statement sql;
  sql<<"SELECT newsfeed,favoritefeed FROM feed_maxid_viewed WHERE userid ="<<uid;
  mysqlpp::StoreQueryResult res;
  try {
    res = QueryRunner("im_stat", CDbRServer).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN("FeedNewest::getDB --> error:" << e);
  } catch (std::exception& e) {
    MCE_WARN("FeedNewest::getDB --> error:" << e.what());
  }
  if(!res) {
    MCE_WARN("FeedNewest::GetDB --> StoryQueryResult wrong");
  }
  if(res.empty()) {
    return NULL;
  }
  NewestFeedIndexPtr result = new NewestFeedIndex();
  result->IndexFeedtype0 = res[0]["newsfeed"];
  result->IndexFeedtype1 = res[0]["favoritefeed"];
  return result;

}
void FeedNewestI::setDB(NewestFeedIdMapContainer* container_) {
  for(int j=0;j<containnum;j++) {
    {
      IceUtil::Mutex::Lock lock(mutex_[j]);

      UidIndex &index = (container_+j)->get<1>();
      int ItemCount = 0;
      
      Statement sql;
      for(UidIndex::iterator uit = index.begin(); uit != index.end(); uit++)  {
     //    Statement sql;
        if(uit->getStat()) {
          uit->setStat(false);
          if(ItemCount == 0) {
            sql << "INSERT INTO feed_maxid_viewed (userid, newsfeed, favoritefeed) VALUES "<< 
            "(" << uit->uid << "," << uit->IndexFeedtype->IndexFeedtype0<<","<< uit->IndexFeedtype->IndexFeedtype1 << ")"
            <<"on duplicate key update newsfeed=values(newsfeed), favoritefeed=values(favoritefeed), time=values(time)";
            MCE_INFO("the container :"<< j << "is called");

          }else if(ItemCount < SqlItemCount) {
            sql << "," << "(" << uit->uid << "," << uit->IndexFeedtype->IndexFeedtype0<<","<< uit->IndexFeedtype->IndexFeedtype1 << ")";
            ItemCount++;
            MCE_INFO("the container :"<< j << "is called");
            continue;
          }
          if(ItemCount == SqlItemCount) {
            try {
              QueryRunner("im_stat", CDbRServer).store(sql);
            } catch (Ice::Exception& e) {
              MCE_WARN("FeedNewest::setDB --> error:" << e);
            } catch (std::exception& e) {
              MCE_WARN("FeedNewest::setDB --> error:" << e.what());
            }
            ItemCount = 0;
            sql.clear();
          }
        }
      }
      MCE_INFO("the remainder is query");
      if(!sql.empty()){
        MCE_INFO("the sql is not empty");
        try{
          QueryRunner("im_stat", CDbRServer).store(sql);
        } catch (Ice::Exception& e) {
          MCE_WARN("FeedNewest::setDB --> error:" << e);
        } catch (std::exception& e) {
          MCE_WARN("FeedNewest::setDB --> error:" << e.what());
        }
      }
      MCE_INFO("the sql is  empty");
      sql.clear();
    }

  }
}

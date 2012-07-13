/*
 * FeedCacheLogicI.h
 *
 *  Created on: 2011-8-3
 *      Author: wu
 */

#ifndef FEEDCACHELOGICI_H_
#define FEEDCACHELOGICI_H_

#include <math.h>
#include <IceUtil/IceUtil.h>
#include <Ice/Ice.h>
//#include <boost/multi_index_container.hpp>
//#include <boost/multi_index/mem_fun.hpp>
//#include <boost/multi_index/ordered_index.hpp>
//#include <boost/multi_index/sequenced_index.hpp>
//#include <boost/multi_index/composite_key.hpp>
//#include <boost/shared_ptr.hpp>
//#include <boost/multi_index/identity.hpp>
//#include <boost/multi_index/hashed_index.hpp>
//#include <boost/multi_index/member.hpp>
//#include "FeedCache.h"
#include "FeedCacheLogic.h"
#include "Singleton.h"
#include <time.h>
#include <sstream>
#include "IceLogger.h"

using namespace MyUtil;
using namespace std;

namespace xce{
  namespace feed{

    class UgcFilter:public Singleton<UgcFilter>{
      public:

        UgcFilter() {
          int a[] = {5,6,7};
          btypes_.insert(a,a+sizeof(a)/sizeof(a[0]));
          MCE_INFO("UgcFilter:: --> byptes size:" << btypes_.size());
          //ostringstream oss;
          //for(set<int>::const_iterator it = btypes_.begin(); it != btypes_.end(); it++) {
          //  oss << *it << " ";
          //}
          //MCE_INFO("UgcFilter:: --> types:" << oss.str());
        }
        bool check(int uid,int stype){
        
          //MCE_INFO("UgcFilter:: --> stype:" << stype << " uid:" << uid << " ret:" << btypes_.count(stype / 100));
          return btypes_.count(stype / 100);
        }

      private:

          
        set<int> btypes_;
    };

    class InterestingFilter:public Singleton<InterestingFilter>{
      public:

        InterestingFilter() {
          int a[] = {1,16,20,37};
          btypes_.insert(a,a+sizeof(a)/sizeof(a[0]));
          MCE_INFO("InterestingFilter:: --> byptes size:" << btypes_.size());
          //ostringstream oss;
          //for(set<int>::const_iterator it = btypes_.begin(); it != btypes_.end(); it++) {
          //  oss << *it << " ";
          //}
          //MCE_INFO("InterestingFilter:: --> types:" << oss.str());
        }
        bool check(int uid,int stype){
        
          //MCE_INFO("InterestingFilter:: --> stype:" << stype << " uid:" << uid << " ret:" << btypes_.count(stype / 100));
          return btypes_.count(stype / 100);
        }

      private:
          
        set<int> btypes_;
    };


    class ScoreFeedItem {
      private:
        FeedItem item_;
        float score_;
        int friend_pos_;

        int weight_id_;
        int frank_;
        int raw_index_;

        Ice::Long max_read_feed_;
        Ice::Long seq_max_feed_;
        Ice::Int uid_;


        void setFirstScore() {
         // bool isUgc = TypeFilter::instance().check(uid_, item_.type& 0xFFFF); 
          
          Ice::Long n = seq_max_feed_ - max_read_feed_; 
          
          double base_num = time(NULL) - item_.time + 1;

          if(base_num <= 0 || weight_id_ <= 0) {
            MCE_WARN("ScoreFeedItem::setFirstScore --> base_num:" << base_num << " weight_id_:" << weight_id_ << "  feed id:" << item_.feed);
            score_ = 0;
          } else {
            float ft = 0.0;
            if (uid_ % 10 == 6 || uid_ % 10 == 7) { //stat2

              ft = 8000 * (1 + weight_id_ / 100.0) / pow(base_num, 1.0 / (weight_id_));
              score_ = float(ft + (6 * weight_id_ + 4 * frank_));

            } else if(uid_ % 10 == 8) {  //stat3
              float m  = 0;

              if ((7 * n) != 0) { 
                m = (0.0 + n - 300) / (7 * n); 
              } 

              if (m < 0) {
                m = 0;
              }  

              ft = 10000 * (1 + weight_id_ / 100.0) / pow(base_num, 1.0 / (weight_id_));
              ft *= (1-m);
              score_ = float(ft + (6 * weight_id_ + 4 * frank_));

            } else if(uid_ % 10 == 4 || uid_ % 10 == 5) {
              ft = 10000 * (1 + weight_id_ / 50.0) / pow(base_num, 1.0 / (weight_id_));
              score_ = float(ft + (6 * weight_id_ + 4 * frank_));
              
            } else { //default
              ft = 10000 * (1 + weight_id_ / 100.0) / pow(base_num, 1.0 / (weight_id_));
              score_ = float(ft + (6 * weight_id_ + 4 * frank_));

            } 

            if (item_.feed > max_read_feed_) {
              score_ += 3000;
            }
    
            if (uid_ % 10 == 3) {
              float m  = 0;

              int i = uid_ % 100 / 10; 

              double j = i * 0.4 + 7;
              if ((j * n) != 0) { 
                m = (0.0 + n - 300) / (j * n); 
              } 

              if (m < 0) {
                m = 0;
              }  

              bool isInteresting = InterestingFilter::instance().check(uid_, item_.type& 0xFFFF); 
              bool isUgc = UgcFilter::instance().check(uid_, item_.type& 0xFFFF); 
              if(isInteresting) {
                score_ *= (1-m);
              }
              if(!isUgc && !isInteresting) {
                score_ *= ((1-m) * (1-m));
              }
              
            }
          }
        }

      public:
        const static float interaction;


        explicit ScoreFeedItem(const FeedItem & fi, int index, Ice::Long maxReadFid, Ice::Long seqMaxFid, Ice::Int userId)
          :item_(fi), score_(0.0), friend_pos_(0), raw_index_(index), max_read_feed_(maxReadFid), seq_max_feed_(seqMaxFid), uid_(userId) {

          weight_id_ = item_.weight >> 16 & 0xFF;
          frank_ = item_.weight >> 24 & 0xFF;  
          setFirstScore();
        }


        void setSecondScore() {
        
          score_ += 20000;
          score_ -= friend_pos_ * (100-(frank_/5 + 25)); 

          if(uid_ % 10 == 2) {
            score_ -= friend_pos_ * (100-(frank_/5 + 25)); 
          }

        }


        void setFriendPos(int p) {
          friend_pos_ = p;
        }

        void setRawIndex(int ri) {
          raw_index_ = ri;
        }

        int rawIndex() {
          return raw_index_;
        }


        FeedItem item() const {
          return item_;
        } 

        float score() const{
          return score_;
        }  

        int friend_pos() {
          return friend_pos_;
        }

        int weightId() {
          return weight_id_;
        }

        int frank() {
          return frank_;
        }

    };


    class FeedCacheLogicI : public FeedCacheLogic,
    public MyUtil::Singleton<FeedCacheLogicI> {

      public:
        static const int CACHE_SIZE = 3000000;
        FeedCacheLogicI() {
          _cfg = new FeedCacheConfig;
          _cfg->cacheSize = CACHE_SIZE;
          _cfg->deletedCount = 20; 
          _cfg->loadFrom = "FeedDB";
          _cfg->enableCache = true;
          _cfg->feedDBCfgs["FeedDB"] = new FeedDBConfig;
          _cfg->feedDBCfgs["FeedDB"]->syncPut = true;
          _cfg->feedDBCfgs["FeedDB"]->syncRemove = true;
          _cfg->feedDBCfgs["FeedDB"]->syncRemoveAll = true;
          _cfg->feedDBCfgs["SSDFeedDB"] = new FeedDBConfig;
          _cfg->feedDBCfgs["SSDFeedDB"]->syncPut = false;
          _cfg->feedDBCfgs["SSDFeedDB"]->syncRemove = true;
          _cfg->feedDBCfgs["SSDFeedDB"]->syncRemoveAll = true;

        }

        static const int FEED_RETURN_COUNT;
        static const int FEED_COUNT;

        MyUtil::LongSeq resortFeedItems(::Ice::Int, const ::xce::feed::FeedItemSeq &, const ::Ice::Current& = ::Ice::Current());

        FeedItemSeq    get(int userId, int begin, int limit, const Ice::Current& = Ice::Current());
        void    put(const FeedItem& item, const MyUtil::Int2IntMap& usersWeight, bool syncFeedDB, const Ice::Current& = Ice::Current());
        void    remove(int userId, Ice::Long feedId, bool syncFeedDB, const Ice::Current& = Ice::Current());
        void    removeAll(int userId, bool syncFeedDB, const Ice::Current& = Ice::Current());
        void    removeByMerge(int userId, long merge, int stype, bool syncFeedDB, const Ice::Current& = Ice::Current());
        void    removeMergeByFeedId(int userId, long feedId, bool syncFeedDB, const Ice::Current& = Ice::Current());

        void    updateConfig(const FeedCacheConfigPtr& cfg, const Ice::Current& = Ice::Current());
        FeedCacheConfigPtr    getConfig(const Ice::Current& = Ice::Current());

        void    test(const FeedItem& item, const MyUtil::Int2IntMap& usersWeight, bool syncDB, const Ice::Current& = Ice::Current());
        void    testLogic(const FeedItem& item, const MyUtil::Int2IntMap& usersWeight, bool syncDB);

      private:
        Ice::Long findMaxFeedId(const ::xce::feed::FeedItemSeq & seq);
      private:
        FeedCacheConfigPtr  _cfg;


    };

    bool operator < (const ScoreFeedItem & s_feed_item1, const ScoreFeedItem & s_feed_item2);
  }
}

#endif /* FEEDCACHELOGICI_H_ */

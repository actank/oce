/*
 * AdRecommendCacheI.cpp
 *
 *  Created on: Dec 2, 2010
 *      Author: zhaohui.tang
 */

#include "AdRecommendCacheI.h"


namespace xce {
namespace ad {

void Recommend::UpdateResult(int uid, const AdResultE& r) {
  AdResultE new_result = r;

  MCE_INFO("Recommend::--> before UpdateResult, uid = " << uid << " old result size = "
      << ad_result_.size() <<", new result size = " << new_result.size());
  {
    IceUtil::RWRecMutex::WLock lock(mutex_);
    ad_result_.clear();
    ad_result_ = new_result;
    last_update_time_ = time(NULL);
  }
  MCE_INFO("Recommend::--> after UpdateResult, uid = " << uid << " old result size = "
      << ad_result_.size() <<", new result size = " << new_result.size());


}


AdResultE AdRecommendCacheI::GetAdResult(int user_id) {
  AdResultE result;
  RecommendPtr recommend;
  int pindex = GetPoolIndex(user_id);
  {
    recommend = evictor_[pindex].locate(user_id, NULL);
    MCE_DEBUG("AdRecommendCacheI::GetAdResult--> user_id = "<< user_id <<" evictor size = " << evictor_[pindex].size());
  }
  if (!recommend || recommend->IsTimeout()) {
      bool success = true;
      success = AdRecommendLoader::instance().AddUserListH(user_id);

      MCE_INFO("AdRecommendCacheI::GetAdResult-->AdRecommendLoader::AddUserListH user_id = "<< user_id);

      if(!recommend) {
        result = AdResultE();
        result.insert(pair<int,double>(-1,0));
      } else {
        result = recommend->ad_result();
      }

  } else {
    result = recommend->ad_result();
    MCE_DEBUG("AdRecommendCacheI::GetAdResult--> user_id = "<< user_id <<" is find, result size = " << result.size());
    if (user_id == 128487631 || user_id == 1244171572 || user_id == 338113787) {
      for (AdResultE::iterator rit = result.begin(); rit != result.end(); rit++) {
        MCE_INFO("AdRecommendCacheI::GetAdResult--> user_id = "<< user_id <<" is find, groupid = " << rit->first << ", score = " << rit->second);
      }
    }
  }

  if (recommend) {
    recommend->Accessed();
  }
  return result;
}

bool AdRecommendCacheI::Insert(int user_id, const AdResultE & ad_result) {
  MCE_INFO("AdRecommendCacheI::Insert--> Enter user_id = "<< user_id
      <<", ad_result.size = " << ad_result.size());
  int pindex = GetPoolIndex(user_id);
  RecommendPtr newrp = new Recommend(ad_result);
  {
    RecommendPtr rp = evictor_[pindex].find(user_id, NULL);
    if (rp) {
      rp->UpdateResult(user_id, ad_result);
      MCE_INFO("AdRecommendCacheI::Insert--> user_id = "<< user_id <<", merge");
    } else {
      MCE_INFO("AdRecommendCacheI::Insert--> user_id = "<< user_id <<", add new");
      evictor_[pindex].add(newrp, user_id);
    }
  }
  MCE_INFO("AdRecommendCacheI::Insert--> user_id = "<< user_id
      <<", evictor_ size = " << evictor_[pindex].size());
  return true;
}

void AdRecommendCacheI::DisposeClick(int userid, long groupid) {
  map<long, double> result;
  int pindex = GetPoolIndex(userid);
  {
    RecommendPtr recommend = evictor_[pindex].find(userid, NULL);
    if (recommend) {
      result = recommend->ad_result();
      map<long, double>::iterator it = result.find(groupid);
      if(it != result.end()) {
        result.erase(it);
        //it->second *= 0.01;
        recommend->set_ad_result(result);
        MCE_INFO("AdRecommendCacheI::DisposeClick userid:" << userid
            << " groupid:" << groupid);
      }
    }
  }
  {
    RecommendPtr rec = evictor_[pindex].find(userid, NULL);
    if (rec) {
      map<long ,double> result = rec->ad_result();
      string res;
      map<long, double>::iterator it = result.begin();
      for(; it != result.end(); ++it) {
        string str = boost::lexical_cast<string> (it->first);
        res += str;
        res += ",";
      }
      MCE_DEBUG("AdRecommendCacheI::DisposeClick after dispose recom:" <<res);
    }

  }
}

bool AdRecommendCacheI::ReCalculate(int user_id) {
  try {
    AdRecommendLoader::instance().AddUserListH(user_id);
    MCE_INFO("AdRecommendCacheI::ReCalculate--> AdRecommendLoader.AddUserListH user_id = "<< user_id);
  } catch (Ice::Exception& e) {
    MCE_WARN("AdRecommendCacheI::ReCalculate call AdRecommendLoader.AddUserListH--> ERR  line:"
            << __LINE__ << " err:" << e);
  } catch (std::exception& e) {
    MCE_WARN("AdRecommendCacheI::ReCalculate call AdRecommendLoader.AddUserListH ERR  line:"
            << __LINE__ << " err:" << e.what());
  }

}

void AdRecommendCacheI::Reload() {
  TimeStat ts;
  MCE_INFO("AdRecommendCacheI::Reload Begin");
  Evictor<int, RecommendPtr> evictor[POOLSIZE];
  {
    TimeStat subts;
    for(int i = 0; i < POOLSIZE; ++i) {
      {
        evictor[i] = evictor_[i];
        MCE_DEBUG("AdRecommendCacheI::Reload copy evictor used time = " << subts.getTime() << "temp evictor size = " << evictor[i].size());
      }
    }
  }
  for(int i = 0; i < POOLSIZE; ++i) {
    for (Evictor<int, RecommendPtr>::iterator it = evictor[i].begin(); it != evictor[i].end(); it++) {
      RecommendPtr r = it->second.obj;
      if (r && r->IsTimeout() && r->IsJustAccessed()) {
        try {
          MCE_INFO("AdRecommendCacheI::Reload AddUserListL user_id =  " << it->first);
            if(!AdRecommendLoader::instance().AddUserListL(it->first)) {

            usleep(1000 * 1000);
          }
        } catch (Ice::Exception& e) {
          MCE_WARN("LoaderManager.AddUserListL--> ERR  line:"
                  << __LINE__ << " err:" << e);
        } catch (std::exception& e) {
          MCE_WARN("LoaderManager.AddUserListL ERR  line:"
                  << __LINE__ << " err:" << e.what());
        }
      }
    }
  }

  MCE_INFO("AdRecommendCacheI::Reload End, used time = " << ts.getTime());
}

} // end namespace ad
} // end namespace xce

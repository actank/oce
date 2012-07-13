/*
 * AdRecommendCacheI.cpp
 *
 *  Created on: Dec 2, 2010
 *      Author: zhaohui.tang
 */

#include "AdRecommendCacheI.h"


namespace xce {
namespace ad {

/*void Recommend::IncPv(Ice::Long group_id) {
  IceUtil::RWRecMutex::WLock lock(mutex_);
  map<Ice::Long, int>::iterator it = pv_map_.find(group_id);
  if (it != pv_map_.end()) {
    it->second++;
  } else {
    pv_map_[group_id] = 1;
  }
  MCE_DEBUG("Recommend::--> after IncPv group_id = " << group_id << "pv = "
      << pv_map_[group_id]);
}

int Recommend::GetPv(Ice::Long group_id) {
  IceUtil::RWRecMutex::RLock lock(mutex_);
  map<Ice::Long, int>::iterator it = pv_map_.find(group_id);
  if (it != pv_map_.end()) {
    MCE_DEBUG("Recommend::--> GetPv group_id = " << group_id << "pv = "
        << it->second);
    return it->second;
  } else {
    MCE_DEBUG("Recommend::--> GetPv group_id = " << group_id << "pv = 0");
    return 0;
  }
}
void Recommend::Merge(int uid, const Recommend & r) {
  AdResult new_result = r.ad_result();

  MCE_INFO("Recommend::--> before merge, uid = " << uid << " old result size = "
      << ad_result_.size() <<", new result size = " << new_result.size());
  {
    IceUtil::RWRecMutex::RLock lock(mutex_);
    for (AdResult::iterator it = ad_result_.begin(); it != ad_result_.end(); it++) {
      if (GetPv(it->first) > 30) {
        ad_result_.erase(it++);
      } else {
        it->second *= 0.9;
      }
    }
  }

  for (AdResult::iterator nit = new_result.begin(); nit != new_result.end(); nit++) {
    if (GetPv(nit->first) > 30) {
      ad_result_.erase(nit++);
    }
  }

  {
    IceUtil::RWRecMutex::WLock lock(mutex_);
    ad_result_.insert(new_result.begin(), new_result.end());
    last_update_time_ = time(NULL);
  }

  MCE_INFO("Recommend::--> after merge, uid = " << uid << " old result size = "
      << ad_result_.size() <<", new result size = " << new_result.size());
//  AdResult::iterator fit = ad_result_.begin(), sit = rp->ad_result().begin();
//  while (fit != ad_result_.end() || sit != rp->ad_result().end()) {
//    if (fit == ad_result_.end() && sit == rp->ad_result().end()) {
//      break;
//    } else if (fit == ad_result_.end()) {
//      result.insert(sit, rp->ad_result().end());
//      break;
//    } else if (sit == rp->ad_result().end()) {
//      result.insert(fit, ad_result_.end());
//      break;
//    } else if (fit->first == sit->first) {
//      if (fit->second < sit->second) {
//        result.insert(pair<Ice::Long, int> (sit->first, sit->second));
//      }
//      sit++;
//      fit++;
//    } else if (fit->first > sit->first) {
//      result.insert(pair<Ice::Long, int> (sit->first, sit->second));
//      sit++;
//    } else if (fit->first < sit->first) {
//      result.insert(pair<Ice::Long, int> (fit->first, fit->second));
//      fit++;
//    }
//  }

}*/

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
//    IceUtil::RWRecMutex::RLock lock(mutex_[pindex]);
    recommend = evictor_[pindex].locate(user_id, NULL);
    MCE_DEBUG("AdRecommendCacheI::GetAdResult--> user_id = "<< user_id <<" evictor size = " << evictor_[pindex].size());
  }
  if (!recommend || recommend->IsTimeout()) {
 //   try {
 //     AdRecommendAdapter::instance().AddUserListH(user_id,
 //      AdMatchManagerI::instance().server_index()); // Add to load list high priority
      bool success = true;
      //success = LoaderManager::instance().AddUserListH(user_id);
      success = AdRecommendLoader::instance().AddUserListH(user_id);

      MCE_INFO("AdRecommendCacheI::GetAdResult-->AdRecommendLoader::AddUserListH user_id = "<< user_id);
 //   } catch (Ice::Exception& e) {
  //    MCE_WARN("AdRecommendAdapter.AddUserListH--> ERR  line:"
 //             << __LINE__ << " err:" << e);
 //   } catch (std::exception& e) {
 //     MCE_WARN("AdRecommendAdapter.AddUserListH ERR  line:"
 //             << __LINE__ << " err:" << e.what());
 //   }
    result = AdResultE();
    result.insert(pair<int,double>(-1,0));
 //   if (recommend) {
 //     MCE_DEBUG("AdRecommendCacheI::GetAdResult--> user_id = "<< user_id <<" is time out");
 //   } else {
 //     MCE_DEBUG("AdRecommendCacheI::GetAdResult--> user_id = "<< user_id <<" is not find");
 //   }
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

  /////todo 上线要删除
//  for (map<Ice::Long, double>::iterator it = result.begin(); it != result.end(); ++it) {
//      MCE_INFO("AdRecommendCacheI::GetAdResult-->userid:"<<userid <<" result Adid:" << it->first << " value:" << it->second);
//    }

  return result;
}

bool AdRecommendCacheI::Insert(int user_id, const AdResultE & ad_result) {
  MCE_INFO("AdRecommendCacheI::Insert--> Enter user_id = "<< user_id
      <<", ad_result.size = " << ad_result.size());
  int pindex = GetPoolIndex(user_id);
  RecommendPtr newrp = new Recommend(ad_result);
  {
//    IceUtil::RWRecMutex::WLock lock(mutex_[pindex]);
    RecommendPtr rp = evictor_[pindex].find(user_id, NULL);
    if (rp) {
      //rp->Merge(user_id, *newrp);
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
//    IceUtil::RWRecMutex::WLock lock(mutex_[pindex]);
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
  ///debug
  {
//    IceUtil::RWRecMutex::RLock lock(mutex_[pindex]);
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
 //   AdRecommendAdapter::instance().AddUserListH(user_id,
 //    AdMatchManagerI::instance().server_index()); // Add to load list high priority
   // LoaderManager::instance().AddUserListH(user_id);
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
 //   IceUtil::RWRecMutex::RLock lock(mutex_);
    TimeStat subts;
    for(int i = 0; i < POOLSIZE; ++i) {
      {
 //       IceUtil::RWRecMutex::RLock lock(mutex_[i]);
        evictor[i] = evictor_[i];
        MCE_DEBUG("AdRecommendCacheI::Reload copy evictor used time = " << subts.getTime() << "temp evictor size = " << evictor[i].size());
      }
    }
//    evictor = evictor_;
 //   MCE_DEBUG("AdRecommendCacheI::Reload copy evictor used time = " << subts.getTime() << "temp evictor size = " << evictor[i].size());
  }
  for(int i = 0; i < POOLSIZE; ++i) {
    for (Evictor<int, RecommendPtr>::iterator it = evictor[i].begin(); it != evictor[i].end(); it++) {
      RecommendPtr r = it->second.obj;
      if (r && r->IsTimeout() && r->IsJustAccessed()) {
        try {
          MCE_INFO("AdRecommendCacheI::Reload AddUserListL user_id =  " << it->first);
  //        if (!AdRecommendAdapter::instance().AddUserListL(it->first,
  //            AdMatchManagerI::instance().server_index())) { // Add to load list low priority
  //          if(!LoaderManager::instance().AddUserListL(it->first)) {
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
/*void AdRecommendCacheI::IncPv(int userid, long groupid) {
  IceUtil::RWRecMutex::RLock lock(mutex_);
  RecommendPtr rp = evictor_.find(userid, NULL);
  if (rp) {
    rp->IncPv(groupid);
  }
}*/
} // end namespace ad
} // end namespace xce

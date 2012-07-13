/*
 * AdRecommendation.cpp
 *
 *  Created on: 2010-11-21
 *      Author: sunzz
 */
#include <QueryRunner.h>
#include <exception>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <Ice/Exception.h>
#include <string>
#include <set>
#include <math.h>
#include "AdRecommendation.h"
#include "AdRecommendSAdapter.h"

#include "AdStruct.h"
#include "AdInvertedIndex.h"
#include "FeedMemcProxy/client/user_profile_client.h"
#include "AdUserCacheLoaderAdapter.h"
#include <fstream>

using namespace xce::ad;
using namespace com::xiaonei::xce;
using namespace boost;

void ItemUsersPool::Init() {//
  ItemUsersEvictor container;
  map<Ice::Long, int> gidclustermap;

  InitFromCategory(gidclustermap); //初始化广告的类别信息

  ///////////////////////////////////////////////////////
//  InitFromDB(container, GidTypeBrand);
  /****************************************************/
  InitFromDB(container, gidclustermap, GidType63);
  RecentClickCache::instance().Init();

  {
    IceUtil::Mutex::Lock  lock(mutex_);
    gidmap_.swap(container);
    gid_cluster_map.swap(gidclustermap);
    MCE_INFO("ItemUsersPool::Init done-->cluster size:" << gid_cluster_map.size() <<",gid count:" <<gidmap_.size());
  }

  MCE_INFO("ItemUsersPool::Init.UpdateKNNTimer() start");
  TaskManager::instance().schedule(new UpdateKNNTimer());

}


void ItemUsersPool::InitFromCategory(map<Ice::Long, int> &gidmap) {
  string file_name = "../etc/ad_cluseter.data";
  MCE_INFO("ItemUsersPool::InitFromCategory--> start");
    try {
      ifstream data(file_name.c_str());
      string line;

      if (data) {
        while (data >> line) {
          vector < string > tmp;
          boost::algorithm::split_regex(tmp, line, regex("\\:"));

          Ice::Long gid = boost::lexical_cast<Ice::Long>(tmp[0]);
          int cluster = boost::lexical_cast<int>(tmp[1]);
          gidmap.insert(pair<Ice::Long,int>(gid,cluster));
          MCE_DEBUG("ItemUsersPool::InitFromCategory-->gid:" << gid << ",cluster:" << cluster);

        }
      } else {
        MCE_INFO("no file " << file_name);
      }
      if (data) {
        data.close();
      }
    } catch (Ice::Exception& e) {
      MCE_WARN("ItemUsersPool::InitFromCategory error : " << e);
    } catch (...) {
      MCE_WARN("ItemUsersPool::InitFromCategory error : unkown error \"...\"");
    }
    MCE_INFO("ItemUsersPool::InitFromCategory--> stop" <<",cluster size:" << gidmap.size());

}

void ItemUsersPool::InitFromDB(ItemUsersEvictor &container, map<Ice::Long, int> &clustermap,int type) {

  string tablename;
  if(type == GidType63) {
    tablename = "ad_gid_uid_inpage";
  } else {
    tablename = "ad_gid_uid_brand";
  }
  MCE_INFO("ItemUsersPool::InitFromDb-->from DB type:" << tablename);
  Statement sql;
  sql << "SELECT * FROM " << tablename;
  mysqlpp::StoreQueryResult res;
  MCE_INFO("ItemUsersPool::Init from " << tablename);
  try {
    res = QueryRunner("ads_db", CDbRServer, tablename).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN("ItemUsersPool::Init from DB:" << tablename<< "--> error:" << e);
  } catch (std::exception& e) {
    MCE_WARN("ItemUsersPool::Init from DB:" << tablename<< "--> error:" << e.what());
  }


  set<int> userset;
  int maxuserid = 0;
  Ice::Long gidscount = 0;

  if (res && res.num_rows() > 0) {
    for (int i = 0; i < res.num_rows(); i++) {
      Ice::Long gid = boost::lexical_cast<Ice::Long>(res[i]["gids"]);
      MCE_DEBUG("ItemUsersPool::Init load from DB gid:" << gid <<",DB type:" << type);
      if (gid <= 0)
        continue;

      gidscount++;
      string result = (mysqlpp::Null<string, mysqlpp::NullIsBlank>)(res[i]["userid"]);

      vector < string > users;
      boost::algorithm::split_regex(users, result, regex("\\,"));
      if(users.empty()) //没有点击行为不插入
        continue;


      //////////////////////////////////////////////////////
      set<int> u;
      for (vector<string>::iterator it = users.begin(); it
          != users.end(); it++) {
        MCE_DEBUG("ItemUsersPool::Init-->gid:" << gid << ", this users = " << *it);
        int userid = 0;
        try {
          userid = boost::lexical_cast<int>(*it);
        } catch (std::exception & e) {
          MCE_WARN("ItemUsersPool::Init --> sting2 cast error :-"
              << e.what());
        }
        if (userid > 0) {
          u.insert(userid);
        }
      }

      ////////////////////////////////////////////////////////
      map<Ice::Long, int>::iterator itc = clustermap.find(gid);
      if(itc != clustermap.end()) {
        int cluster = itc->second;
        ItemUsersEvictor::iterator itg = container.find(cluster);
        if(itg != container.end()) {
          itg->second->gids.insert(gid);
          itg->second->merge_users.insert(u.begin(),u.end());
          MCE_DEBUG("ItemUsersPool::InitFromDB cluster exist:" <<cluster <<",gid:" << gid);

        } else {
          ItemUsersPtr upt = new ItemUsers();
          upt->cluster = cluster;
          upt->gids.insert(gid);
          upt->merge_users.insert(u.begin(),u.end());
          container.insert(pair<int, ItemUsersPtr>(cluster, upt));
          MCE_DEBUG("ItemUsersPool::InitFromDB creat new cluster:" <<cluster <<",gid:" << gid);
        }
      }
    }
  }
  MCE_INFO("ItemUsersPool::InitFromDB-->usrset size: " << userset.size() << " gidscount:"
      << gidscount <<",gid type:" <<type << ",container size:" << container.size());
}

void UpdateTask::UpdateGidUid(int gidtype) {
  /*
  MCE_INFO("UpdateTask::UpdateGidUid execute, gid type:" << gidtype <<",gid:" << upt->gid);
  Statement sql;
  ostringstream os;
  for(set<int>::iterator it = upt->users.begin(); it != upt->users.end(); ++it) {
    if(it != upt->users.begin()){
      os << ",";
    }
    os << *it;
  }

  string tablename;
  if(gidtype == GidType63) {
    tablename = "ad_gid_uid_inpage";
  } else {
    tablename = "ad_gid_uid_brand";
  }
  sql <<"insert into " << tablename << " (gids, userid) values(" << upt->gid << ","
      << mysqlpp::quote << os.str() << ")"
      << "on duplicate key update userid=values(userid),gids=values(gids)";
  try {
    QueryRunner("ads_db", CDbWServer,tablename).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN("UpdateTask::UpdateGidUid DB:" << tablename<<" --> error:" << e);
  } catch (std::exception& e) {
    MCE_WARN("UpdateTask::UpdateGidUid DB:" << tablename<<" --> error:" << e.what());
  }
  */
}

bool ItemUsersPool::IsGidExist(int groupid) {
  /*
  IceUtil::Mutex::Lock  lock(mutex_);
  map<Ice::Long, ItemUsersPtr>::iterator it = gidmap_.find(groupid);
  if (it != gidmap_.end()) {
    return false;
  }
  return true;*/

}
void ItemUsersPool::UpdateClick(int userid, Ice::Long groupid, bool isbrand) {
//  int pos = GetIndex(container_, userid, adid);//如果不存在，需要新分配index 里面有锁，不能放在锁里
//  int gidtype;
//
//  if(isbrand) {
//    gidtype = GidTypeBrand;
//  } else {
//    gidtype = GidType63;
//  }
//  IceUtil::Mutex::Lock  lock(mutex_);
//  ItemUsersPtr upt;
//
//  map<Ice::Long, ItemUsersPtr>::iterator git = gidmap_.find(groupid);
//	bool flag = true;
//	if (git != gidmap_.end()) {
//
//	  upt = git->second;
//	  if(upt->users.count(userid)) {
//	    flag = false;
//
//	  } else {
//	    upt->users.insert(userid);
//	  }
//
//	} else {
//
//		upt = new ItemUsers();
//		upt->gid = groupid;
//		upt->gidtype = gidtype;
//		upt->users.insert(userid);
//		gidmap_.insert(pair<Ice::Long, ItemUsersPtr>(groupid, upt));
//
//		MCE_INFO("ItemUsersPool::UpdateClick-->new groupid is created, gid:" << groupid <<
//		    " clicked by userid:" << userid << ",gidtype:" << gidtype);
//
//	}
//  if(flag) {
//    TaskManager::instance().execute(new UpdateTask(upt));
//  } else {
//    MCE_DEBUG("ItemUsersPool::UpdateClick-->user clicked past" <<userid << ",gid:"<< groupid <<"need not update db");
//  }
}
double ItemUsersPool::GetItemPropDistance(const ItemUsersPtr& item_ptrx,
    const ItemUsersPtr& item_ptry) {
//
//  map<int,double>& itemx = item_ptrx->category;
//  map<int,double>& itemy = item_ptry->category;
//  if(itemx.empty() || itemy.empty()) { //如果其中的广告没有标签，不计算相似度
//    return 0.0;
//  }
//  map<int,double>::iterator itx = itemx.begin();
//  map<int,double>::iterator ity = itemy.begin();
//  double distance = 0;
//  double sumxy = 0;
//  double sumx = 0;
//  double sumy = 0;
//  for(; (itx != itemx.end()) && (ity != itemy.end()); ) {
//    if(itx->first == ity->first) {
//
//      sumxy += itx->second * ity->second;
//      sumx += itx->second * itx->second;
//      sumy += ity->second * ity->second;
//      ++itx;
//      ++ity;
//    } else if (itx->first > ity->first) {
//
//      sumy += ity->second * ity->second;
//      ++ity;
//    } else {
//      sumx += itx->second * itx->second;
//      ++itx;
//    }
//
//  }
//  for(; itx != itemx.end(); ++itx) {
//
//    sumx += itx->second * itx->second;
//  }
//  for(; ity != itemy.end(); ++ity) {
//    sumy += ity->second * ity->second;
//  }
//  return sumxy / sqrt(sumx * sumy);
}


double ItemUsersPool::GetItemDistance(const ItemUsersPtr& itemptrx, //just for item is 0 or 1
		const ItemUsersPtr& itemptry, int least_com_num) {

	  set<int>& usersx = itemptrx->merge_users;
	  set<int>& usersy = itemptry->merge_users;


	  int sumxy = 0;
	  int sumxsq = 0;
	  int sumysq = 0;
	  set<int>::iterator itx = usersx.begin();
	  set<int>::iterator ity = usersy.begin();
	  for(; (itx != usersx.end()) && (ity != usersy.end());) {
	    if((*itx) == (*ity)) {
	      sumxy += 1;
	      ++itx;
	      ++ity;
	    }else if((*itx) > (*ity)) {
	      ++ity;
	    } else {
	      ++itx;
	    }
	  }

	  sumxsq = usersx.size();
	  sumysq = usersy.size();
	  if (sumxsq == 0 || sumysq == 0) {
	    return 0;
	  }

/*  if (sumxy == sumxsq) {//delete subset//test
    MCE_INFO("ItemUsersPool::new GetItemDistance-->inter gid:"<<itemptry->gid
     <<" has subset:"<<",gid:"<<itemptrx->gid );
    return 0.0;
  }*/
  if(sumxy < least_com_num) { //如果共同数比较少，不能进入邻居集
    return 0.0;
  }
  int com = sumxsq + sumysq - sumxy; 
  return (double)sumxy / sqrt((double)sumxsq * (double)sumysq);
//  return (double)sumxy / sqrt((double)sumxsq);
//   return (double)sumxy / (double)(com);


}
void ItemUsersPool::UpdateKNNItem() {
	TimeStat ts;
	MCE_INFO("ItemUsersPool::UpdateKNNItem--> start");
  ItemUsersEvictor container;
  {
    IceUtil::Mutex::Lock  lock(mutex_);
    container = gidmap_;
  }

  int knn_count = GetKNNCount();
  double sim_threshold = GetSimThreshold();
  int least_com_num = GetLeastCommonNum();
  map<int,map<int, double> > knn;

  map<int,ItemUsersPtr>::iterator git = container.begin();
//  GidIndex & gindex = container.get<1> ();
//  GidIndex::iterator git = gindex.begin();
  for(; git != container.end(); ++git) {
    int gid = git->first;

    multimap<double, int> knn_tmp;
    map<int, double> gidknn;
//    GidIndex::iterator otherit = gindex.begin();
    map<int,ItemUsersPtr>::iterator otherit = container.begin();
    for(; otherit != container.end(); ++otherit) {
      if(otherit->first == git->first) {  //自己不算做邻居
        continue;
      }
      ///todo need care:
      //if(!AdGroupPool::instance().HasAdGroup(gid)) { //无效广告不计算
      //  continue;
      //}


      double sim = GetItemDistance(git->second, otherit->second, least_com_num);

      if(sim > 0) {
        knn_tmp.insert(pair<double, int>(sim, otherit->first));
      }

    }
    int k = 0;

    multimap<double,int>::reverse_iterator it = knn_tmp.rbegin();
    for(; (it != knn_tmp.rend()) && (k < knn_count); ++it, k++) {

      if(it->first < sim_threshold) {
        MCE_INFO("ItemUsersPool::UpdateKNNItem--> less than threshold:" << sim_threshold);
        break;
      }
      gidknn.insert(pair<int,double>(it->second, it->first));
    }

    knn.insert(pair<int ,map<int,double> >(gid, gidknn));
    MCE_INFO("ItemUsersPool::UpdateKNNItem knn-->gid:" << gid <<",has Knn:" << gidknn.size());
  }
  {
    IceUtil::Mutex::Lock  lock(mutex_);
    knncache_ = knn;
    MCE_INFO("ItemUsersPool::UpdateKNNItem--> stop, used time = " << ts.getTime() << "knncache size:" << knncache_.size());
  }


}
map<int,double> ItemUsersPool::GetKNNItem(int cluster) {
  MCE_INFO("ItemUsersPool::GetKNNItem-->cluster:" << cluster);
  map<int,double> result;

  IceUtil::Mutex::Lock  lock(mutex_);
  map<int, map<int, double> >::iterator itk = knncache_.find(cluster);
  if(itk != knncache_.end()) {
    result = itk->second;
  }
  return result;
}
vector<int> ItemUsersPool::LookUpGidCluster(vector<Ice::Long>click) {
  vector<int> result;
  for(vector<Ice::Long>::iterator itc = click.begin(); itc != click.end(); ++itc) {
    IceUtil::Mutex::Lock  lock(mutex_);
    map<Ice::Long, int>::iterator itg = gid_cluster_map.find(*itc);
    if(itg != gid_cluster_map.end()) {
      result.push_back(itg->second);
    }
  }

  return result;
}
set<Ice::Long> ItemUsersPool::FindBestGidInCluster(int userid,int cluster){
  set<Ice::Long>result;
  string serialized;
  if (userid > 0) {
    bool b = UserProfileClient::instance().GetSerialized(userid, &serialized);

    if (!b) {
      MCE_INFO("profile not found in memcache. id=" << userid);
      try {
        serialized = AdUserCacheLoaderAdapter::instance().GetById(userid);
      } catch(Ice::Exception& e) {
        MCE_WARN("AdUserCacheLoaderAdapter GetById error : " << e << " uid=" << userid);
      } catch(std::exception& e) {
        MCE_WARN("AdUserCacheLoaderAdapter GetById error : " << e.what() << " uid=" << userid);
      }
    }
  }

  UserProfile profile;
  profile.ParseFromString(serialized);
  profile.set_id(userid);
  Ice::Long zone = 100000000063;
  set<AdGroupPtr>ad_orient = DimIndexMatcher::instance().Get(zone,profile);


  IceUtil::Mutex::Lock  lock(mutex_);
  map<int, ItemUsersPtr>::iterator itg = gidmap_.find(cluster);
  if(itg != gidmap_.end()) {
    set<Ice::Long>& gids = itg->second->gids;
    int blocknum = 0;
    for(set<Ice::Long>::iterator it = gids.begin(); it != gids.end(); ++it) {
      if(!AdGroupPool::instance().HasAdGroup(*it)) { //无效广告不计算
        continue;
      }
      ///定向条件过滤
      set<AdGroupPtr>::iterator ito = ad_orient.begin();
      for(; ito != ad_orient.end(); ++ito) {
        if((*ito)->group_id() == (*it)) {
          break;
        }
      }
      if(ito == ad_orient.end()) { //定向不到该用户
        blocknum ++;
        MCE_DEBUG("ItemUsersPool::FindBestGidInCluster-->user:" <<userid <<",cant seen gid:" << *it);
        continue;
      }
      ///
      result.insert(*it);
      if(result.size() > 2) {
        break;
      }
    }
    MCE_INFO("ItemUsersPool::FindBestGidInCluster-->user:"<<userid <<",gid blockednum:" <<blocknum);
  }

  return result;

}

map<Ice::Long, double> ItemUsersPool::GetRecommendation(int userid) {//该算法没有去除均值（只有0、1项）中，均值在后续会考虑去均值的效果.

//  TimeStat subts;
  TimeStat ts;
  MCE_DEBUG("ItemUsersPool::GetRecommendation-->userid" << userid);
  map<Ice::Long, double> result;
  multimap<double, int> result_tmp;

  vector<Ice::Long> clickAd;
  clickAd = RecentClickCache::instance().GetClickedGid(userid);
  if(clickAd.empty()) {
    MCE_INFO("ItemUsersPool::GetRecommendation--> has no click, userid:" << userid);
    return result;
  }
  vector<int> click_ad_cluster = LookUpGidCluster(clickAd);
  map<int, double> total;
  map<int, double> simsum; // 留给点击率用
  double brand_percent = GetBrandPercent();
  vector<int>::iterator itc = click_ad_cluster.begin();
  for(; itc != click_ad_cluster.end(); ++itc) {
   // MCE_INFO("ItemUsersPool::GetRecommendation-->userid:" << userid << ",re depend on clicked gid:" <<*itc);
    int gcluster = *itc;


    MCE_INFO("ItemUsersPool::GetRecommendation-->userid:" << userid << ",re depend on clicked gid:" <<*itc);
    MCE_INFO("ItemUsersPool::GetRecommendation-->userid:" << userid << ",gid:" <<*itc);
    map<int, double> knn = GetKNNItem(gcluster);
    for(map<int, double>::iterator itk = knn.begin(); itk != knn.end(); ++itk) {
      map<int, double>::iterator itt = total.find(itk->first);
      if(itt != total.end()) {
        itt->second += itk->second;
        simsum[itk->first] += 1.0;
      } else {
        total.insert(pair<int, double>(itk->first, itk->second));
        simsum.insert(pair<int, double>(itk->first, 1.0));
      }
    }
  }

  double rec_threshold = GetReThreshold();
  int max_rec_count = GetMaxReCount();

  map<int, double>::iterator itt = total.begin();
  for(; itt != total.end(); ++itt) {
    result_tmp.insert(pair<double, int>(itt->second, itt->first));
  }

  for (multimap<double, int>::reverse_iterator itm = result_tmp.rbegin(); itm
       != result_tmp.rend(); ++itm) {
    if(result.size() > max_rec_count) {
      break;
    }
    if(itm->first < rec_threshold) {
      break;
    }
    set<Ice::Long> gids = FindBestGidInCluster(userid,itm->second);//负责找到定向给他的有效广告
    for(set<Ice::Long>::iterator itgs = gids.begin(); itgs != gids.end(); ++itgs) {
      result.insert(pair<Ice::Long,double>((*itgs),itm->first));
    }

  }
  if(result.size() < 2) { //推荐量不够，希望从历史点击的同类广告中获取
    MCE_INFO("ItemUsersPool::GetRecommendation-->the re count not enough,need to get re from history click")
    for(vector<int>::iterator itcc = click_ad_cluster.begin();itcc != click_ad_cluster.end(); ++itcc) {
      set<Ice::Long> gids = FindBestGidInCluster(userid,*itcc);//负责找到定向给他的有效广告
      for(set<Ice::Long>::iterator itgs = gids.begin(); itgs != gids.end(); ++itgs) {
        result.insert(pair<Ice::Long,double>((*itgs),0.0001));
      }
    }
  }

  MCE_INFO("ItemUsersPool::GetRecommendation from re_I-->after filt recom size:" << result.size() <<" userid:" <<userid);
  for (map<Ice::Long, double>::iterator it = result.begin(); it != result.end(); ++it) {
    MCE_INFO("ItemUsersPool::GetRecommendation from re_I-->after filt recom userid:"<<userid <<" result Adid:" <<
        it->first << " value:" << it->second);
  }

  MCE_INFO("ItemUsersPool::GetRecommendation--> userid = " << userid <<", used time = " << ts.getTime());
  return result;
}
void ItemUsersPool::UniFormResult(map<Ice::Long, double> & result, double max, double min) {
  if(max > 0) {
    double max_distance = max - min;
    if(max_distance < 0) return;
    for(map<Ice::Long, double>::iterator it = result.begin(); it != result.end(); ++it) {
      if(max_distance > 0) {
        it->second = (it->second - min)/ max_distance + 0.001;
      } else {
        it->second = 1.0;
      }
    }
  }
}

/*void ItemUsersPool::Merge(map<Ice::Long,double>& result, vector<Ice::Long>& clickad, double factor) {
  for(vector<Ice::Long>::iterator it = clickad.begin(); it != clickad.end(); ++it){
    MCE_INFO("ItemUsersPool::Merge-->click:gid:" <<*it);
    map<Ice::Long, double> knn = GetKNNItemProp(*it);
    map<Ice::Long, double>::iterator itk = knn.begin();
    for(; itk != knn.end(); ++itk) {
      MCE_INFO("ItemUsersPool::Merge-->click:gid:" <<*it <<",neighbour:" <<itk->first);
      vector<Ice::Long>::iterator ita = find(clickad.begin(), clickad.end(), itk->first);
      if(ita != clickad.end()) {
        continue;
      }
      map<Ice::Long, double>::iterator itr = result.find(itk->first);
      if(itr != result.end()) {
        continue;
      }
      if(!AdGroupPool::instance().HasAdGroup(itk->first)) { //无效广告不计算
        continue;
      }
 //     result.insert(pair<Ice::Long, double>(itk->first, itk->second * factor));
      result[itk->first] = itk->second * factor;
    }
  }
}*/
void ItemUsersPool::MergeResult(map<Ice::Long, double> & to, map<Ice::Long, double> & from, double percent) {
  for(map<Ice::Long, double>::iterator it = from.begin(); it != from.end(); ++it) {
    map<Ice::Long, double>::iterator itt = to.find(it->first);
    if(itt != to.end()) {
      itt->second = itt->second * (1 - percent) + it->second * percent;
    } else {
      to.insert(pair<Ice::Long, double>(it->first, it->second * percent));
    }
  }
}
void RecentClickCache::Init() {
  UserEvictor evictor;
  InitFromDB(evictor, GidType63);
  {
    IceUtil::Mutex::Lock  lock(mutex_);
    evictor_.swap(evictor);
    MCE_INFO("RecentClickCache::Init done-->container size:" << evictor_.size());
  }
}

void RecentClickCache::InitFromDB(UserEvictor &evictor, int type) {//
  string tablename;
  if(type == GidType63) {
    tablename = "ad_uid_gid_inpage";
  } else {
    tablename = "ad_uid_gid_brand";
  }
  MCE_INFO("RecentClickCache::InitFromDB-->from DB type:" << tablename);
  Statement sql;
  sql << "SELECT * FROM " << tablename;
  set<Ice::Long> adset;

  mysqlpp::StoreQueryResult res;
  MCE_INFO("UserItemPool::Init");
  try {
    res = QueryRunner("ads_db", CDbRServer, tablename).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN("RecentClickCache::InitFromDB --> error:" << e);
  } catch (std::exception& e) {
    MCE_WARN("RecentClickCache::InitFromDB --> error:" << e.what());
  }

  int maxuserid = 0;
  Ice::Long usercount = 0;

  if (res && res.num_rows() > 0) {

    for (int i = 0; i < res.num_rows(); i++) {
      int userid = boost::lexical_cast<int>(res[i]["userid"]);
      MCE_DEBUG("RecentClickCache::InitFromDB userid:" << userid);
      if (userid <= 0)
        continue;
      if (userid > maxuserid)
        maxuserid = userid;
      usercount++;
      string result = (mysqlpp::Null<string, mysqlpp::NullIsBlank>)(
          res[i]["gids"]);

      vector < string > group_ids;
      boost::algorithm::split_regex(group_ids, result, regex("\\,"));
      if (group_ids.empty()) //没有点击行为不插入
        continue;

      UserPtr upt = new User(); //userid uniq
      upt->userid = userid;
      for (vector<string>::iterator it = group_ids.begin(); it
          != group_ids.end(); it++) {
        MCE_DEBUG("RecentClickCache::InitFromDB-->userid:"
            << userid << ", this groupid = " << *it);
        Ice::Long groupid = 0;
        try {
          groupid = boost::lexical_cast<Ice::Long>(*it);
        } catch (std::exception & e) {
          MCE_WARN("RecentClickCache::InitFromDB --> sting2 cast error :-"
              << e.what());
        }
        if ((groupid > 0)) {
          upt->recent_clicked_[groupid] = time(NULL);
          adset.insert(groupid);
        }
      }

      SeqIndex & sindex = evictor.get<0> ();
      pair<SeqIndex::iterator, bool> p = evictor.push_front(upt); //用户提前
      if (!p.second) {
        sindex.relocate(sindex.begin(), p.first);
      } else {
        if (evictor.size() > kUserSize) {
          evictor.pop_back();
        }
      }

    }
  }

  MCE_INFO("RecentClickCache::InitFromDB-->ad size: " << adset.size() << " usercount:" << usercount);
  //  MCE_DEBUG("UserItemPool::Init-->ad size: " << adset.size() << ",ad indexsize:" << index_cache_.size());
}
UserPtr RecentClickCache::Locate(int userid) {

  {
    IceUtil::Mutex::Lock  lock(mutex_);
    UidIndex & index = evictor_.get<1> ();
    UidIndex::iterator uit = index.find(userid);
    if (uit != index.end()) {
      SeqIndex::iterator sit = evictor_.project<0> (uit);//新查的放到前面
      evictor_.relocate(evictor_.get<0> ().begin(), sit);
      MCE_DEBUG("RecentClickCache::Locate-->userid:" <<userid << " in the cache");
      return *uit;
    }
  }
  MCE_DEBUG("RecentClickCache::Locate-->userid:" <<userid << " not in the cache,need load form DB");
  UserPtr u = Load(userid, GidType63);

  IceUtil::Mutex::Lock  lock(mutex_);
  SeqIndex & sindex = evictor_.get<0> ();
  pair<SeqIndex::iterator, bool> p = evictor_.push_front(u); //用户提前
  if (!p.second) {
    sindex.relocate(sindex.begin(), p.first);
  } else {
    while (evictor_.size() > kUserSize) {
      evictor_.pop_back();
    }

  }
  MCE_INFO("RecentClickCache::Locate-->UserSize:" << evictor_.size());
  return u;
}

UserPtr RecentClickCache::Load(int userid, int type) {
  MCE_DEBUG("RecentClickCache::Load-->userid:" << userid <<" need load from DB");
  UserPtr u = new User();
  u->userid = userid;
  string tablename;
  if(type == GidType63) {
    tablename = "ad_uid_gid_inpage";
  } else {
    tablename = "ad_uid_gid_brand";
  }
  Statement sql;
  sql << "SELECT * FROM " << tablename << " where userid = " << userid;
  mysqlpp::StoreQueryResult res;
  try {
    res = QueryRunner("ads_db", CDbRServer, tablename).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN("RecentClickCache::Load --> error:" << e);
  } catch (std::exception& e) {
    MCE_WARN("RecentClickCache::Load --> error:" << e.what());
  }
  if (!res) {
    MCE_WARN("RecentClickCache::Load --> StoryQueryResult wrong");
  }

  if (res && res.num_rows() > 0) {
    string result = (mysqlpp::Null<string, mysqlpp::NullIsBlank>)(
        res[0]["gids"]);

    vector < string > group_ids;
    boost::algorithm::split_regex(group_ids, result, regex("\\,"));
    for (vector<string>::iterator it = group_ids.begin(); it
        != group_ids.end(); it++) {
      MCE_DEBUG("[INDEXLOG] RecentClickCache::Load-->userid:" << userid
          << ", this groupid = " << *it);
      Ice::Long id = 0;
      try {
        id = boost::lexical_cast<Ice::Long>(*it);
      } catch (std::exception & e) {
        MCE_WARN("RecentClickCache::Load --> sting2 cast error :-"
            << e.what());
      }
      if ((id > 0)) {
        u->recent_clicked_[id] = time(NULL);
      }
    }
  }

  return u;

}


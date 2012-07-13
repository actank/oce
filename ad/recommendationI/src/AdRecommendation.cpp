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
#include "AdCache.h"
#include <fstream>

using namespace xce::ad;
using namespace com::xiaonei::xce;
using namespace boost;

void ItemUsersPool::Init() {//
  ItemUsersEvictor container;
  Statement sql;

  ///////////////////////////////////////////////////////
  InitFromDB(container, GidTypeBrand);
  /****************************************************/
  InitFromDB(container, GidType63);
  RecentClickCache::instance().Init();
  InitFromCategory(container);
  {
    IceUtil::Mutex::Lock  lock(mutex_);
    gidmap_.swap(container);
    MCE_INFO("ItemUsersPool::Init done-->container size:" << gidmap_.size());
  }
  CalculatePropKnn();

  MCE_INFO("ItemUsersPool::Init.UpdateKNNTimer() start");
  TaskManager::instance().schedule(new UpdateKNNTimer());
  TaskManager::instance().schedule(new UpdateKNNPropTimer());
}

void ItemUsersPool::InitFromCategory(ItemUsersEvictor &gidmap) {
  string file_name = "../etc/ad_category.data";
  MCE_INFO("ItemUsersPool::InitFromCategory--> start");
    try {
      ifstream data(file_name.c_str());
      string line;

      if (data) {
        while (data >> line) {
          vector < string > tmp;
          boost::algorithm::split_regex(tmp, line, regex("\\:"));

          Ice::Long gid = boost::lexical_cast<Ice::Long>(tmp[0]);
          map<Ice::Long, ItemUsersPtr>::iterator itg = gidmap.find(gid);
          if(itg == gidmap.end()) {
            continue;
          }

          ItemUsersPtr& pt = itg->second;
          vector <string> categorylist;
          boost::algorithm::split_regex(categorylist, tmp[1], regex("\\;"));
          for(vector<string>::iterator itc = categorylist.begin(); itc != categorylist.end(); ++itc) {
            if((*itc).empty()) {
              continue;
            }
            vector<string> t;
            boost::algorithm::split_regex(t, *itc, regex("\\,"));
            if(t[0].empty() || t[1].empty()) {
              continue;
            }
            int c = boost::lexical_cast<int>(t[0]);
            double v = boost::lexical_cast<double>(t[1]);

          //  pt->category.insert(pair<int,double>(c,v));
            pt->category[c] = v;
          }
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
    MCE_INFO("ItemUsersPool::InitFromCategory--> stop");

}
void ItemUsersPool::CalculatePropKnn() {
  TimeStat ts;
  MCE_INFO("ItemUsersPool::CalculatePropKnn--> start");
  ItemUsersEvictor gidmap;
  {
    IceUtil::Mutex::Lock  lock(mutex_);
    gidmap = gidmap_;
  }
  map<Ice::Long, map<Ice::Long, double> > knn;
  map<Ice::Long, ItemUsersPtr>::iterator itg = gidmap.begin();
  for(; itg != gidmap.end(); ++itg) {
    multimap<double, Ice::Long> knn_tmp;
    map<Ice::Long, double> gidknn;
    Ice::Long gid = itg->first;
    Ice::Long memberid = itg->second->member_id_;
    if(itg->second->category.empty()) {
      continue;
    }
    map<Ice::Long, ItemUsersPtr>::iterator ito = gidmap.begin();

    int count = 0;
    set<Ice::Long> memberid_pool;//用来消除邻居都是同一个memberid的问题
    for(; ito != gidmap.end(); ++ito) {
      if(ito->first == itg->first) {  //自己不算做邻居
        continue;
      }
      if(ito->second->category.empty()) {
        continue;
      }
      if(ito->second->gidtype == GidTypeBrand) {
        continue;
      }

      if(!AdGroupPool::instance().HasAdGroup(ito->first)) { //无效广告不算作邻居
        continue;
      }

      Ice::Long mid = ito->second->member_id_;

      if((count >= 2) && (memberid == mid) ) { //邻居不能有很多同一memberid的广告
        continue;
      }

      if(memberid == mid) {
        count ++;
      }

      set<Ice::Long>::iterator itb = memberid_pool.find(mid);
      if(itb != memberid_pool.end()) {
        continue;
      }
      if(mid != -1) {
        memberid_pool.insert(mid);
      }
      double sim = GetItemPropDistance(itg->second, ito->second);
      if(sim > 0) {
        if(gid == 10000297340005) {
          MCE_DEBUG("ItemUsersPool::CalculatePropKnn-->gid:" << gid <<",memid:"
              << memberid <<",neighbour:"<<ito->first <<",memid:" <<mid << ",sim:" <<sim);
        }
        knn_tmp.insert(pair<double, Ice::Long>(sim * sim, ito->first));
        //knn_tmp[sim] = ito->first;
      }
    }

    int k = 0;
    int knn_count = 5;
    multimap<double,Ice::Long>::reverse_iterator it = knn_tmp.rbegin();
//    set<Ice::Long> memberid_pool;//用来消除邻居都是同一个memberid的问题
    for(; (it != knn_tmp.rend()) && (k < knn_count)  && (it->first > 0.25); ++it, k++) {
//    for(; (it != knn_tmp.rend()) && (it->first > 0.25); ++it, k++) {
//      set<Ice::Long>::iterator itb = memberid_pool.find(it->second);
//      if(itb != memberid_pool.end()) {
//        continue;
//      }
      gidknn.insert(pair<Ice::Long,double>(it->second, it->first));



      MCE_DEBUG("ItemUsersPool::CalculatePropKnn-->gid:" << gid
          <<",neighbour gid:" <<it->second <<",value:" << it->first);
    }
    knn.insert(pair<Ice::Long ,map<Ice::Long,double> >(gid, gidknn));
  }

  {
    IceUtil::Mutex::Lock  lock(mutex_);
    knn_prop_ = knn;
    MCE_INFO("ItemUsersPool::CalculatePropKnn--> stop, used time = " << ts.getTime() << "knn_prop_ size:" << knn_prop_.size());
  }
}

void ItemUsersPool::InitFromDB(ItemUsersEvictor &container, int type) {

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

      ItemUsersPtr upt = new ItemUsers();  //gid uniq
      upt->gid = gid;
      upt->gidtype = type;
///////////////////////////////////////////////////// 获取groupid
//      if(type == GidType63) {
        AdGroupPtr group = AdGroupPool::instance().GetAdGroup(gid);
        if (group) {
          upt->member_id_ = group->member_id();
          if(gid == 10000297340005) {
             MCE_DEBUG("ItemUsersPool::InitFromDB test-->gid:" <<gid <<",memid:" <<upt->member_id_
                 <<",gidtype:" <<type);
           }
        } else {
          upt->member_id_ = -1; //找不到，广告可能无效了,在cache查不到memberid
          MCE_INFO("ItemUsersPool::InitFromDB test-->gid:" <<gid <<",memid:" <<upt->member_id_
              <<",gidtype:"<<type);

        }
//      }
      if(gid == 10000297340005) {
        MCE_DEBUG("ItemUsersPool::InitFromDB-->gid:" <<gid <<",memid:" <<upt->member_id_
            <<",gidtype:" <<type);
      }

////////////////////////////////////////////////////
//      set<int> usergroup;
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
          upt->users.insert(userid);
        //  usergroup.insert(userid);
        //  RecentClickCache::instance().Click(userid,gid);

          userset.insert(userid);
        }
      }
      //upt->users = usergroup;
      //container.push_front(upt); //
      container.insert(pair<Ice::Long, ItemUsersPtr>(gid, upt));

    }
  }
  MCE_INFO("ItemUsersPool::InitFromDB-->usrset size: " << userset.size() << " gidscount:"
      << gidscount <<",gid type:" <<type << ",container size:" << container.size());
}

void UpdateTask::UpdateGidUid(int gidtype) {
  MCE_INFO("UpdateTask::UpdateGidUid execute, gid type:" << gidtype <<",gid:" << upt->gid);
  Statement sql;
  ostringstream os;
  for(set<int>::iterator it = upt->users.begin(); it != upt->users.end(); ++it) {
    if(it != upt->users.begin()){
      os << ",";
    }
    os << *it;
  }
  //sql <<"update ad_uid_gid set gids = " << mysqlpp::quote << os.str() << "where userid = " << upt->userid;
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
}

bool ItemUsersPool::IsGidExist(int groupid) {
  IceUtil::Mutex::Lock  lock(mutex_);
//  GidIndex & index = container_.get<1> ();
//  GidIndex::iterator git = index.find(groupid);
//  if (git != index.end()) {
//  	return false;
//  }
//  return true;
  map<Ice::Long, ItemUsersPtr>::iterator it = gidmap_.find(groupid);
  if (it != gidmap_.end()) {
    return false;
  }
  return true;

}
void ItemUsersPool::UpdateClick(int userid, Ice::Long groupid, bool isbrand) {
//  int pos = GetIndex(container_, userid, adid);//如果不存在，需要新分配index 里面有锁，不能放在锁里
  int gidtype;

  if(isbrand) {
    gidtype = GidTypeBrand;
  } else {
    gidtype = GidType63;
  }
  IceUtil::Mutex::Lock  lock(mutex_);
  ItemUsersPtr upt;
//	GidIndex & index = container_.get<1> ();
//	GidIndex::iterator git = index.find(groupid);
  map<Ice::Long, ItemUsersPtr>::iterator git = gidmap_.find(groupid);
	bool flag = true;
	if (git != gidmap_.end()) {

	  upt = git->second;
	  if(upt->users.count(userid)) {
	    flag = false;
	//    MCE_DEBUG("ItemUsersPool::UpdateClick-->user clicked past" <<userid << ",gid:"<< groupid);
	  } else {
	    upt->users.insert(userid);
	  }
	//  (*git)->users.insert(userid);
	} else {
	//	ItemUsersPtr upt = new ItemUsers();
		upt = new ItemUsers();
		  //  cout << "UserItemEvictor::SetUserItemValue insert pos:" << pos << "userid: " << userid <<"adid: " << adid<< endl;
		upt->gid = groupid;
		upt->gidtype = gidtype;
		upt->users.insert(userid);
		gidmap_.insert(pair<Ice::Long, ItemUsersPtr>(groupid, upt));
//		container_.push_front(upt);
		MCE_INFO("ItemUsersPool::UpdateClick-->new groupid is created, gid:" << groupid <<
		    " clicked by userid:" << userid << ",gidtype:" << gidtype);

	}
  if(flag) {
 //   MCE_DEBUG("ItemUsersPool::UpdateClick-->user:" <<userid  <<",gid:"<< groupid << "need update db");
    TaskManager::instance().execute(new UpdateTask(upt));
  } else {
    MCE_DEBUG("ItemUsersPool::UpdateClick-->user clicked past" <<userid << ",gid:"<< groupid <<"need not update db");
  }



}
double ItemUsersPool::GetItemPropDistance(const ItemUsersPtr& item_ptrx,
    const ItemUsersPtr& item_ptry) {
  map<int,double>& itemx = item_ptrx->category;
  map<int,double>& itemy = item_ptry->category;
  if(itemx.empty() || itemy.empty()) { //如果其中的广告没有标签，不计算相似度
    return 0.0;
  }
  map<int,double>::iterator itx = itemx.begin();
  map<int,double>::iterator ity = itemy.begin();
  double distance = 0;
  double sumxy = 0;
  double sumx = 0;
  double sumy = 0;
  for(; (itx != itemx.end()) && (ity != itemy.end()); ) {
    if(itx->first == ity->first) {
  //    distance += (itx->second - ity->second) * (itx->second - ity->second);
      sumxy += itx->second * ity->second;
      sumx += itx->second * itx->second;
      sumy += ity->second * ity->second;
      ++itx;
      ++ity;
    } else if (itx->first > ity->first) {
  //    distance += ity->second * ity->second;
      sumy += ity->second * ity->second;
      ++ity;
    } else {
 //     distance += itx->second * itx->second;
      sumx += itx->second * itx->second;
      ++itx;
    }

  }
  for(; itx != itemx.end(); ++itx) {
   // distance += itx->second * itx->second;
    sumx += itx->second * itx->second;
  }
  for(; ity != itemy.end(); ++ity) {
   // distance += ity->second * ity->second;
    sumy += ity->second * ity->second;
  }

//  return 1.0 / (distance + 1.0);
  return sumxy / sqrt(sumx * sumy);
}


double ItemUsersPool::GetItemDistance(const ItemUsersPtr& itemptrx, //just for item is 0 or 1
		const ItemUsersPtr& itemptry, int least_com_num) {

	  set<int>& usersx = itemptrx->users;
	  set<int>& usersy = itemptry->users;


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
//  return (double)sumxy / sqrt((double)sumxsq * (double)sumysq);
//  return (double)sumxy / sqrt((double)sumxsq);
   return (double)sumxy / (double)(com);


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
  map<Ice::Long,map<Ice::Long, double> > knn;

  map<Ice::Long,ItemUsersPtr>::iterator git = container.begin();
//  GidIndex & gindex = container.get<1> ();
//  GidIndex::iterator git = gindex.begin();
  for(; git != container.end(); ++git) {
    Ice::Long gid = git->first;
    int gidtype = git->second->gidtype;
    multimap<double, Ice::Long> knn_tmp;
    map<Ice::Long, double> gidknn;
//    GidIndex::iterator otherit = gindex.begin();
    map<Ice::Long,ItemUsersPtr>::iterator otherit = container.begin();
    for(; otherit != container.end(); ++otherit) {
      if(otherit->first == git->first) {  //自己不算做邻居
        continue;
      }
      if(otherit->second->gidtype == GidTypeBrand) { //品牌广告不算作邻居
        continue;
      }
      if(git->second->gidtype == GidTypeBrand) {
        //lease_com_num *= 5;
      }
      ///todo need care:
      if(!AdGroupPool::instance().HasAdGroup(gid)) { //无效广告不计算
        continue;
      }


      double sim = GetItemDistance(git->second, otherit->second, least_com_num);

//      int count = (*otherit)->users.size();
//      if((count > 0)) {
     //   sim *= log2(1000000.0/(double) count);
//      }
      if(sim > 0) {
        knn_tmp.insert(pair<double, Ice::Long>(sim, otherit->first));
      }

    }
    int k = 0;

    multimap<double,Ice::Long>::reverse_iterator it = knn_tmp.rbegin();
    for(; (it != knn_tmp.rend()) && (k < knn_count); ++it, k++) {

      if(it->first < sim_threshold) {
        MCE_INFO("ItemUsersPool::UpdateKNNItem--> less than threshold:" << sim_threshold);
        break;
      }
      gidknn.insert(pair<Ice::Long,double>(it->second, it->first));
      if(gid == 10000297420001) {
        MCE_DEBUG("ItemUsersPool::UpdateKNNItem knn-->gid:"<<gid <<",neighbour:"<<it->second
            <<",value:" <<it->first);
      }


    }

    knn.insert(pair<Ice::Long ,map<Ice::Long,double> >(gid, gidknn));
    MCE_INFO("ItemUsersPool::UpdateKNNItem knn-->gid:" << gid <<",has Knn:" << gidknn.size() <<",gidtype:" << gidtype);
  }
  {
    IceUtil::Mutex::Lock  lock(mutex_);
    knncache_ = knn;
    MCE_INFO("ItemUsersPool::UpdateKNNItem--> stop, used time = " << ts.getTime() << "knncache size:" << knncache_.size());
  }


}
map<Ice::Long,double> ItemUsersPool::GetKNNItem(Ice::Long groupid) {
  MCE_INFO("ItemUsersPool::GetKNNItem-->groupid:" << groupid);
  map<Ice::Long,double> result;

  IceUtil::Mutex::Lock  lock(mutex_);
  map<Ice::Long, map<Ice::Long, double> >::iterator itk = knncache_.find(groupid);
  if(itk != knncache_.end()) {
    result = itk->second;
  }
  return result;
}

map<Ice::Long,double> ItemUsersPool::GetKNNItemProp(Ice::Long groupid) {
//  MCE_INFO("ItemUsersPool::GetKNNItem-->groupid:" << groupid);
  map<Ice::Long,double> result;

  IceUtil::Mutex::Lock  lock(mutex_);
  map<Ice::Long, map<Ice::Long, double> >::iterator itk = knn_prop_.find(groupid);
  if(itk != knn_prop_.end()) {
    result = itk->second;
  }
  return result;
}
int ItemUsersPool::GetGidType(Ice::Long groupid) {

  IceUtil::Mutex::Lock  lock(mutex_);
//  GidIndex & index = container_.get<1> ();
//  GidIndex::iterator git = index.find(groupid);
//  if (git != index.end()) {
//  	return (*git)->gidtype;
//  }
//  return -1;
  map<Ice::Long, ItemUsersPtr>::iterator git = gidmap_.find(groupid);
  if (git != gidmap_.end()) {
    return git->second->gidtype;
  }
  return -1;
}
map<Ice::Long, double> ItemUsersPool::GetRecommendation(int userid) {//该算法没有去除均值（只有0、1项）中，均值在后续会考虑去均值的效果.

//  TimeStat subts;
  TimeStat ts;
  MCE_DEBUG("ItemUsersPool::GetRecommendation-->userid" << userid);
  map<Ice::Long, double> result;
  multimap<double, Ice::Long> result_tmp;

  vector<Ice::Long> clickAd;
  clickAd = RecentClickCache::instance().GetClickedGid(userid);
  if(clickAd.empty()) {
    MCE_INFO("ItemUsersPool::GetRecommendation--> has no click, userid:" << userid);
    return result;
  }
  map<Ice::Long, double> total;
  map<Ice::Long, double> simsum; // 留给点击率用
  double brand_percent = GetBrandPercent();
  vector<Ice::Long>::iterator itc = clickAd.begin();
  for(; itc != clickAd.end(); ++itc) {
   // MCE_INFO("ItemUsersPool::GetRecommendation-->userid:" << userid << ",re depend on clicked gid:" <<*itc);
    Ice::Long gid = *itc;
//    AdGroupPtr group = AdGroupPool::instance().GetAdGroup(gid);
    double ratio = 1;
    int brand_flag = 0;
    int gtype = GetGidType(gid);
    if (gtype == 1){
      ratio = brand_percent;
      brand_flag = 1;
    } else if(gtype == -1) {
      continue;
    }

    MCE_INFO("ItemUsersPool::GetRecommendation-->userid:" << userid << ",re depend on clicked gid:" <<*itc <<",gidtype:" <<brand_flag);
    MCE_INFO("ItemUsersPool::GetRecommendation-->userid:" << userid << ",gid:" <<*itc <<",brandpercent:" << ratio<<",gidtype:" << brand_flag);
    map<Ice::Long, double> knn = GetKNNItem(gid);
    for(map<Ice::Long, double>::iterator itk = knn.begin(); itk != knn.end(); ++itk) {
      map<Ice::Long, double>::iterator itt = total.find(itk->first);
      if(itt != total.end()) {
        itt->second += itk->second * ratio;

      } else {
        total.insert(pair<Ice::Long, double>(itk->first, itk->second * ratio));
        //simsum.insert(pair<Ice::Long, double>);
      }
    }
  }

  map<Ice::Long, double>::iterator itt = total.begin();
  for(; itt != total.end(); ++itt) {
    Ice::Long gid = itt->first;
//    if(!RecentClickCache::instance().Available(userid,gid)) { //最近点击过的广告不计算推荐
//      MCE_INFO("ItemUsersPool::GetRecommendation-->userid:"<<userid<<",dispose just click:" <<gid);
//      continue;
//    }
    if(!AdGroupPool::instance().HasAdGroup(gid)) { //无效广告不计算
      continue;
    }
    result_tmp.insert(pair<double, Ice::Long>(itt->second, itt->first));
  }
  

  double max_re_i_value = 0, min_re_i_value = 1;
  double rec_threshold = GetReThreshold();
  int max_rec_count = GetMaxReCount();
  int num = 0;
  int invalidcount = 0;
  double value;

  string invalidAd;
  for (multimap<double, Ice::Long>::reverse_iterator itm = result_tmp.rbegin(); (itm
      != result_tmp.rend()) && (num < max_rec_count); ++itm) {
    Ice::Long groupid = itm->second;

    value = itm->first;

    if(value <= rec_threshold) {
      MCE_INFO("ItemUsersPool::GetRecommendation--> less than threshold:" << rec_threshold)
      break;
    }
    if(value > max_re_i_value) {
      max_re_i_value = value;
    }
    if(value < min_re_i_value) {
      min_re_i_value = value;
    }
     result.insert(pair<Ice::Long, double> (itm->second, value));
     ++num;
  }

  ////////////////////////////////////////////
  MCE_INFO("ItemUsersPool::GetRecommendation--> userid = " << userid <<",re_i size:"<< result.size() <<",max re_i:" <<max_re_i_value
        <<",min re_i:" <<min_re_i_value );

//  UniFormResult(result, max_re_i_value, 0);

  MCE_INFO("ItemUsersPool::GetRecommendation from re_I-->before merge filt recom size:" << result.size() <<" userid:" <<userid);
  for (map<Ice::Long, double>::iterator it = result.begin(); it != result.end(); ++it) {
    MCE_INFO("ItemUsersPool::GetRecommendation from re_I-->before filt recom userid:"<<userid <<" result Adid:" <<
        it->first << " value:" << it->second);
  }
  double factor = 1;
  if(max_re_i_value > 0) {
    factor = max_re_i_value * 0.8;
  }
  Merge(result, clickAd, factor);
  ////////////////////////////////////////
  MCE_INFO("ItemUsersPool::GetRecommendation from re_I-->after filt recom size:" << result.size() <<" userid:" <<userid);
  for (map<Ice::Long, double>::iterator it = result.begin(); it != result.end(); ++it) {
    MCE_INFO("ItemUsersPool::GetRecommendation from re_I-->after filt recom userid:"<<userid <<" result Adid:" <<
        it->first << " value:" << it->second);
  }




  //merge
  /****************************************************************************/
/*  AdResult result_s;
  map<Ice::Long, double> results_tmp;
  double max_re_s_value = 0, min_re_s_value = 1;
  try {
    result_s = AdRecommendSAdapter::instance().GetAds(userid, 100000000063);
    if (!result_s.empty() && (result_s.begin()->first == -1)) {
      MCE_INFO("AdRecommmendSAdapter::GetAds --> not get result, user_id = "<< userid);
      ///////////////////////////直接返回基于item的推荐值
      MCE_INFO("ItemUsersPool::GetRecommendation--> userid = " << userid <<", used time = " << ts.getTime());
      return result;

    } else {
      MCE_INFO("AdRecommmendSAdapter::GetAds --> succeed, user_id = " << userid);
      for(AdResult::iterator its = result_s.begin(); its != result_s.end(); ++its) {
        results_tmp.insert(pair<Ice::Long, double>(its->first, (double)(its->second)));
        if(its->second > max_re_s_value) {
          max_re_s_value = its->second;
        }
        if(its->second < min_re_s_value) {
          min_re_s_value = its->second;
        }
      }
    }
  } catch (Ice::Exception& e) {
    MCE_WARN("AdRecommmendSAdapter --> Execute err:" << e << ", line:" << __LINE__);
  } catch (std::exception& e) {
    MCE_WARN("AdRecommmendSAdapter --> Execute err:" << e.what() << ", line:"
        << __LINE__);
  } catch (...) {
    MCE_WARN("AdRecommmendSAdapter --> Execute, line:" << __LINE__);
  }*/
  /***************************************************************************************/
/*  MCE_INFO("ItemUsersPool::GetRecommendation--> userid = " << userid <<",re_i size:"<< result.size() <<",max re_i:" <<max_re_i_value
      <<",min re_i:" <<min_re_i_value <<",re_s size"<< results_tmp.size()<<",max re_s:" <<max_re_s_value <<", min re_s:"<<min_re_s_value);

  UniFormResult(result, max_re_i_value, 0);
  UniFormResult(results_tmp, max_re_s_value, 0);
//  UniFormResult(result, max_re_i_value, min_re_i_value);
//  UniFormResult(results_tmp, max_re_s_value, min_re_s_value);
 // result.insert(results_tmp.begin(), results_tmp.end());
 // results_tmp.insert(result.begin(), result.end());
  double percent = GetOccupyPercent();
  MergeResult(result, results_tmp, percent);
  MCE_INFO("ItemUsersPool::GetRecommendation from re_I and re_S-->after filt recom size:" << result.size() <<" userid:" <<userid);
  for (map<Ice::Long, double>::iterator it = result.begin(); it != result.end(); ++it) {
    MCE_INFO("ItemUsersPool::GetRecommendation from re_I and re_S-->after filt recom userid:"<<userid <<" result Adid:" <<
        it->first << " value:" << it->second <<",re_I percent" << percent);
  }*/
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

void ItemUsersPool::Merge(map<Ice::Long,double>& result, vector<Ice::Long>& clickad, double factor) {
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
}
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


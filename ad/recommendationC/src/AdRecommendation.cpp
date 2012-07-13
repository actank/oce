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

void UserUgcPool::Init() {//初始化来自文件，加载用户的ugc行为
  UserUgcEvictor umap;
  map<int,vector<Ice::Long> >gmap;

  InitUserUgcFromFile(umap);
  InitGidMapFromFile(gmap);
  {
    IceUtil::Mutex::Lock  lock(mutex_);
    user_ugc_map_.swap(umap);
    gidmap_.swap(gmap);
    MCE_INFO("UserUgcPool::Init-->container user size:" << user_ugc_map_.size()
        <<",gmap:" <<gidmap_.size());
  }

}

void UserUgcPool::InitUserUgcFromFile(UserUgcEvictor &umap) {
  string file_name = "../etc/user_ugc.data";
  MCE_INFO("UserUgcPool::InitFromFile--> start");
  try {
    ifstream file(file_name.c_str());
    string line;
    if (file) {
      while (getline(file, line)) {
        MCE_DEBUG("UserUgcPool::InitFromFile: content" << line);
        vector<string> tmp;
        boost::algorithm::split_regex(tmp, line, regex(" "));
        int userid = boost::lexical_cast<int>(tmp[0]);
        MCE_DEBUG("UserUgcPool::InitFromFile:->user:" << userid);

        UserUgcPtr upt = new UserUgc();

        upt->userid = userid;
        vector<string>::iterator itc = tmp.begin();
        ++itc;
        int total = boost::lexical_cast<int>(*itc);
        upt->total_ugc = total;
        ++itc;
        int index = 0;
        for (; itc != tmp.end(); ++itc) {
          if ((*itc).empty()) {
            continue;
          }
          int num = boost::lexical_cast<int>(*itc);
          upt->ugc[index] = num;
          upt->pos[index] = 0;
          index ++;
        }
        umap[userid] = upt;
      }
    } else {
      MCE_INFO("no file " << file_name);
    }
    if (file) {
      file.close();
    }
  } catch (Ice::Exception& e) {
    MCE_WARN("UserUgcPool::InitFromFile error : " << e);
  } catch (...) {
    MCE_WARN("UserUgcPool::InitFromFile error : unkown error \"...\"");
  }
  MCE_INFO("UserUgcPool::InitFromFile--> stop");

}

void UserUgcPool::InitGidMapFromFile(map<int, vector<Ice::Long> > &gmap){
  string file_name = "../etc/gid_category.data";
  MCE_INFO("UserUgcPool::InitGidMapFromFile--> start");
  try {
    ifstream file(file_name.c_str());
    string line;
    if (file) {
      int index = 0;
      while (getline(file, line)) {
        MCE_DEBUG("UserUgcPool::InitGidMapFromFile: content" << line);
        vector<string> tmp;
        vector<Ice::Long> gidseq;
        boost::algorithm::split_regex(tmp, line, regex(" "));
        Ice::Long gid = boost::lexical_cast<Ice::Long>(tmp[0]);
        MCE_DEBUG("UserUgcPool::InitGidMapFromFile:->gid:" << gid);

        vector<string>::iterator itc = tmp.begin();
        ++itc;

        for (; itc != tmp.end(); ++itc) {
          if ((*itc).empty()) {
            continue;
          }
          Ice::Long g = boost::lexical_cast<Ice::Long>(*itc);
          gidseq.push_back(g);
        }
        gmap[index++] = gidseq;

        MCE_INFO("UserUgcPool::InitGidMapFromFile:category:" <<index-1 <<",num:" <<gidseq.size());
      }
    } else {
      MCE_INFO("no file " << file_name);
    }
    if (file) {
      file.close();
    }
  } catch (Ice::Exception& e) {
    MCE_WARN("UserUgcPool::InitGidMapFromFile error : " << e);
  } catch (...) {
    MCE_WARN("UserUgcPool::InitGidMapFromFile error : unkown error \"...\"");
  }
  MCE_INFO("UserUgcPool::InitGidMapFromFile--> stop");

}

map<Ice::Long, double> UserUgcPool::GetRecommendation(int userid) {//

  TimeStat ts;
  MCE_DEBUG("UserUgcPool::GetRecommendation-->userid" << userid);
  map<Ice::Long, double> result;


  {
    IceUtil::Mutex::Lock  lock(mutex_);
    map<int,UserUgcPtr>::iterator itu = user_ugc_map_.find(userid);
    if(itu == user_ugc_map_.end()) {
      return result;
    }
    UserUgcPtr upt = itu->second;

    map<int,int> &ugc = upt->ugc;
    map<int,int> &pos = upt->pos;
    for(map<int,int>::iterator itug = ugc.begin(); itug != ugc.end(); ++itug) {
      if(itug->second > 0) {
        map<int, vector<Ice::Long> >::iterator itg = gidmap_.find(itug->first);
        int p = 0;

        if(itg != gidmap_.end()) {
          map<int,int>::iterator itp = pos.find(itug->first);
          if(itp != pos.end()) {
            p = pos[itug->first];
          }

          vector<Ice::Long> & gidseq = itg->second;
          bool flag = true;
          int count = 0;
          if(gidseq.empty()) {
            continue;
          }
          while(flag && (count < gidseq.size())) { //如果得到推荐，跳出，循环一圈还没有得到推荐广告则跳出，可能整个广告组都失效了。
            Ice::Long gid = gidseq[p];
            if(AdGroupPool::instance().HasAdGroup(gid)) { //判断广告是否有效
              result[gid] = 1.0;
              flag = false;
            }
            p = (p+1) % (gidseq.size());    //如果无效，继续往下取，循环取值
            count ++;
          }
        }
        pos[itug->first] = p;  //更新计数位置
      }
    }
  }

    //////////////测试
/*    for(itu = user_ugc_map_.begin(); itu != user_ugc_map_.end(); ++itu) {
      upt = itu->second;
      map<int, int>& tmp = upt->ugc;
      for(map<int, int>::iterator it = tmp.begin(); it != tmp.end(); ++it) {
        MCE_DEBUG("test:userid:" << upt->userid <<",total ugc:" <<upt->total_ugc
            <<",type:" <<it->first <<",value:" << it->second);
      }
    }

    for(map<int, vector<Ice::Long> >::iterator itg = gidmap_.begin();itg!=gidmap_.end();++itg){
      vector<Ice::Long>& gseq = itg->second;
      for(vector<Ice::Long>::iterator its = gseq.begin(); its != gseq.end(); ++its) {
        MCE_DEBUG("test:gid type:" << itg->first <<",gid:" <<*its <<" ");
      }
    }*/


    //////////////////


  //推荐过程,并滤除无效广告


  //

  MCE_INFO("UserUgcPool::GetRecommendation from re_I-->after filt recom size:" << result.size() <<" userid:" <<userid);
  for (map<Ice::Long, double>::iterator it = result.begin(); it != result.end(); ++it) {
    MCE_INFO("UserUgcPool::GetRecommendation from re_I-->after filt recom userid:"<<userid <<" result Adid:" <<
        it->first << " value:" << it->second);
  }

  MCE_INFO("UserUgcPool::GetRecommendation--> userid = " << userid <<", used time = " << ts.getTime());
  return result;
}



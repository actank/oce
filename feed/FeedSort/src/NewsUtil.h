/*
 * NewsUtil.h
 *
 *  Created on: Dec 2, 2010
 *      Author: yejingwei
 */

#ifndef NEWSUTIL_H_
#define NEWSUTIL_H_
#include "LogWrapper.h"
using namespace MyUtil;
namespace xce {
namespace feed {
namespace MergeUtil{

class MergeConfig: public MyUtil::Singleton<MergeConfig> {
public:
  /*
   * sort   主类型 评论类型
   *   0    601  2901    日志
   *   0    701  2902    评论单张照片
   *   1    709  2906    评论多张照片(相册)
   *   1    712  2905    评论终端页中的单张照片
   */
  MergeConfig() {
    main2comment_[601] = 2901;
    main2comment_[701] = 2902;
    main2comment_[709] = 2906;
    main2comment_[712] = 2905;
    for (map<Ice::Int, Ice::Int>::iterator mit = main2comment_.begin(); mit
        != main2comment_.end(); ++mit) {
      mains_.insert(mit->first);
      comments_.insert(mit->second);
      comment2main_[mit->second] = mit->first;
    }
//    MCE_INFO("MergeConfig::Init done --> main2comment_ size = " << main2comment_.size());
  }
  bool IsMain(Ice::Int stype) {
    return mains_.count(stype);
  }
  bool IsComment(Ice::Int stype) {
    return comments_.count(stype);
  }

  Ice::Int Main2Comment(Ice::Int stype) {
    map<Ice::Int, Ice::Int>::const_iterator mit = main2comment_.find(stype);
    if (mit != main2comment_.end()) {
      return mit->second;
    } else {
      return stype;
    }
  }
  Ice::Int Comment2Main(Ice::Int stype) {
    map<Ice::Int, Ice::Int>::const_iterator mit = comment2main_.find(stype);
    if (mit != comment2main_.end()) {
      return mit->second;
    } else {
      return stype;
    }
  }

private:
  set<Ice::Int> mains_;//main stype
  set<Ice::Int> comments_;//comment stype
  map<Ice::Int, Ice::Int> main2comment_;
  map<Ice::Int, Ice::Int> comment2main_;
};
class FeedWrapperHandler{
public:
	static int GetStype(int type){
		return type & 0x0000FFFF;
	}
	static pair<int,long> GetStypeMergeKey(const FeedWrapper & fw){
		return make_pair(GetStype(fw.typeinfo),fw.merge);
	}
	static pair<int,int> GetStypeActorKey(const FeedWrapper & fw){
		return make_pair(GetStype(fw.typeinfo),fw.actor);
	}
	static time_t GetGatherTimeGap(int stype){
		return 60 * 5;
//		return (stype == 2002 || stype == 3707) ? 60 * 60 * 24
//				: 60 * 5;
	}
	static bool IsUpdateTime(const FeedWrapper & fw){
    return fw.weight & 0x02;
	}
	static int GetView(const FeedWrapper & fw){
    int v =  (fw.weight & 0xFF00) >> 8;
//    MCE_INFO("fid:" << fw.feed << " view:" << v);
    return v;
	}
	static bool IsEdm(const FeedWrapper & fw){
		return fw.time < 0 || fw.time > time(NULL);
	}
	static int GetWeightId(const FeedWrapper & fw){
		return (fw.weight) >> 16 & 0xFF;
	}
	static int GetFriendRank(const FeedWrapper & fw){
		return fw.weight >> 24 ;
	}
	static bool IsRandomType(int stype){
		return (stype == 601);
	}
	static int GetClick(int a){
		return (a & 0x00FF0000) >> 16;
	}
	static int GetReply(int a){
		return (a & 0x0000FF00) >> 8;
	}
	static int GetRange(int a){
		return a & 0x000000FF;
	}
};

ostream & operator<<(ostream & os,const FeedWrapper &  fw){
	os << fw.feed;
	return os;
}

class PrintHelper {
public:
  template<typename T>
  static string Print(const T & container) { //将容器内容打印成字符串
    ostringstream ios;
    ios << "(";
    bool first_item = true;
    int i = 0;
    for (typename T::const_iterator itr = container.begin(); itr
        != container.end(); ++itr, ++i) {
      if (first_item == true) {
        first_item = false;
      } else {
        ios << ",";
      }
      ios << *itr;
    }
    ios << ")";
    return ios.str();
  }
  static vector<string> PrintFid3d(vector<vector<vector<long> > > fid3d){
  	vector<string> res;
  	for(size_t i = 0; i < fid3d.size(); ++i){
  		ostringstream o2d;
//  		o2d << " Gather " << i << " ";
  		for(size_t j = 0; j < fid3d.at(i).size(); ++j){
    		ostringstream o1d;
    		o1d << Print(fid3d.at(i).at(j));
    		o2d << o1d.str();
  		}
//  		MCE_INFO("PrintFid3d:" << o2d.str());
  		res.push_back(o2d.str());
  	}
  	return res;
  }
};

class TimeStat{
        timeval _begin, _end;
public:
        TimeStat(){
                reset();
        }
        void reset(){
                gettimeofday(&_begin, NULL);
        }
        float getTime(bool rs = true){
                gettimeofday(&_end, NULL);
                float timeuse=1000000*(_end.tv_sec-_begin.tv_sec)+_end.tv_usec
                                -_begin.tv_usec;
                timeuse/=1000;
                if(rs){
                	reset();
                }
                return timeuse;
        }
};



}
}
}
#endif /* NEWSUTIL_H_ */

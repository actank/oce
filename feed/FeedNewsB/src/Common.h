/*
 * Common.h
 *
 *  Created on: Sep 27, 2011
 *      Author: yejingwei
 */

#ifndef COMMON_H_
#define COMMON_H_
#include <list>
#include <set>
#include <Ice/Object.h>
#include <Singleton.h>
#include <LogWrapper.h>
#include <boost/foreach.hpp>
#include <boost/bimap.hpp>
#include <boost/typeof/typeof.hpp>
#include "Date.h"

#include "RFeed.h"
#include "SharedFeedCache.h"
#include "UserInfoCache.h"
using namespace std;
using namespace MyUtil;

using boost::bimap;

namespace xce {
namespace feed {

class MergeConfig: public MyUtil::Singleton<MergeConfig>{
public:
  /*
   * sort   主类型 评论类型
   *   0    601  2901    日志
   *   0    701  2902    评论单张照片
   *   1    709  2906    评论多张照片(相册)
   *   1    712  2905    评论终端页中的单张照片
   */
	MergeConfig(){
		typedef bimap<int,int>::value_type vt;
		bm_.insert(vt(601 , 2901));
		bm_.insert(vt(701 , 2902));
		bm_.insert(vt(709 , 2906));
		bm_.insert(vt(712 , 2905));
	}

	bool IsMain(int stype){ return bm_.left.count(stype); }
	bool IsComment(int stype){ return bm_.right.count(stype); }

	int Main2Comment(int stype){
		BOOST_AUTO(it,bm_.left.find(stype));
		if(it != bm_.left.end()) return it->second;
		else return stype;
	}
	int Comment2Main(int stype){
		BOOST_AUTO(it,bm_.right.find(stype));
		if(it != bm_.right.end()) return it->second;
		else return stype;
	}

private:
	bimap<int,int> bm_;//main comment bimap
};
class FeedDataUtil{
public:
	static int GetStype(int type){ return type & 0xFFFF; }
	static int SetMark(int weight,bool mark){
//		return mark;
//		weight = 0;
    if (mark)  weight = weight | 0x01;
    else  weight = weight & 0xFFFFFFFE;
    return weight;
	}
	static int SetVersion(int & type,int version){
		type = type | (version << 16);
		return type;
	}
};

class HeavyFeed;
typedef HeavyFeed * HeavyFeedPtr;
typedef vector<HeavyFeedPtr> HFeedPtr1D;
typedef vector<HFeedPtr1D> HFeedPtr2D;
typedef vector<HFeedPtr2D> HFeedPtr3D;




inline static ostream &operator<<(ostream &stream, const FeedItem & i) {
  stream << "{id:" << i.feed << " t:" << (i.type & 0xFFFF)<< " a:" << i.actor << "}";
  return stream;
}

class PrintHelper {
public:
//  template<typename T>
//  static string Print(const T & container) {
//    ostringstream ios;
//    ios << "(";
//    bool first_item = true;
//    int i = 0;
//    for (typename T::const_iterator itr = container.begin(); itr
//        != container.end(); ++itr, ++i) {
//      if (first_item == true) first_item = false;
//      else ios << ",";
//      ios << *itr;
//    }
//    ios << ")";
//    return ios.str();
//  }

  template<typename T>
  static string Print(const T & container) {
    ostringstream ios;

    ios << "(";
    int idx = 0;
    typedef typename T::value_type vt;
    BOOST_FOREACH(const vt & v,container){
    	if(idx++ > 0) ios << ",";
      ios << v;
    }
    ios << ")";

    return ios.str();
  }

  template<typename T>
  static string PrintKeys(const T & container) {
    ostringstream ios;

    ios << "(";
    int idx = 0;
    typedef typename T::value_type vt;
    BOOST_FOREACH(const vt & v,container){
    	if(idx++ > 0) ios << ",";
      ios << v.first;
    }
    ios << ")";

    return ios.str();
  }

//  static void Print( const HFeedPtr3D & item3d, int uid = 0 ){
//  	int totalcnt = 0;
//  	MCE_INFO("uid:" << uid << "item3d--> 2d count:" << item3d.size());
//  	BOOST_FOREACH(const HFeedPtr2D & item2d,item3d){
//  		MCE_INFO("uid:" << uid << "\titem2d--> 1d count:" << item2d.size());
//  		ostringstream os;
//  		BOOST_FOREACH(const HFeedPtr1D & item1d, item2d){
//  			MCE_INFO("uid:" << uid <<"\t\titem1d--> item count:" << item1d.size());
//  			BOOST_FOREACH(const HeavyFeedPtr & item,item1d){
//  					MCE_INFO("uid:" << uid <<"\t\t\tfid:" << item->GetFid()
//  							<< " stype:" << item->GetStype()
//  							<< " actor:" << item->GetActor()
//  							<< " merge:" << item->GetMerge());
//  					++totalcnt;
//  			}
//  		}
//  	}
//  	MCE_INFO("totalcnt:" << totalcnt);
//  }
};

class TimeStat {
public:
  TimeStat() {  reset(); }
  void reset() { gettimeofday(&_begin, NULL); }
  float Get(bool clear = true) {
    gettimeofday(&_end, NULL);
    float timeuse = 1000000 * (_end.tv_sec - _begin.tv_sec)
    								+( _end.tv_usec - _begin.tv_usec);
    if(clear) reset();
    return timeuse / 1000;
  }
private:
  timeval _begin, _end;
};

class Trace{
public:
	Trace(const string & name):name_(name){
		MCE_INFO( "@@<<<< : " << name_  );
	}
	~Trace(){
		MCE_INFO( "@@>>>> : " << name_  );
	}
private:
	string name_;
};
}
}
#endif /* COMMON_H_ */

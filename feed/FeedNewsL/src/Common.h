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
#include <math.h>
#include <boost/tuple/tuple.hpp>

#include "RFeed.h"
#include "UserInfo.h"
#include "SharedFeedCache.h"
using namespace std;
using namespace MyUtil;
using boost::tuple;
using boost::make_tuple;
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
class TypeUtil{
public:
	static int GetStype(int stype){ return stype & 0xFFFF; }
};

//typedef IceUtil::Handle<HeavyItem> HeavyItemPtr;
class HeavyFeed;
typedef HeavyFeed* HeavyFeedPtr;
typedef vector<HeavyFeedPtr> HFeed1D;
typedef vector<HFeed1D> HFeed2D;
typedef vector<HFeed2D> HFeed3D;

typedef map<pair<int, long> , HFeed1D> StypemergeHFeed1DMap;
typedef map<pair<int,long >, double> StypemergeScoreMap;
typedef multimap<double, StypemergeHFeed1DMap::iterator> ScoreIteratorMultiMap;

typedef map<pair<int, long> , SFeed1D> StypemergeSFeed1DMap;

class GetRequest {
public:
	//参数
	int uid_;
	bool sort_;
	bool by_stype_not_btype_;
	int begin_;
	int limit_;
	enum FilterStyle{
		kNone,kInclude,kExclude,kError
	};
//	set<int> types_;
	set<int> include_types_,exclude_types_;

	//排序用临时数据
	StypemergeHFeed1DMap comment_map_; //全部的评论
	StypemergeSFeed1DMap showed_comment_map_; //需要显示的评论
	StypemergeHFeed1DMap stypemerge_item1d_map_;
	ScoreIteratorMultiMap score_iterator_multimap_;

	//计算的结果
	HFeed3D hfeed3d_;//有按照时间折叠的
	HFeed3D showed_hfeed3d_;
	SFeed3D showed_sfeed3d_;
	HeavyFeedPtr random_;
	set<long> showed_fids_;
//	set<long> edm_fids_;
	vector<FeedItem> edm_fitems_;
	long max_fid_;

	bool getshow_;
	time_t nowtime_;
	int older_count_;
	int useless_count_;//由于merge策略，不需要取本体的数量
	FilterStyle filter_style_;
public:
	GetRequest(int uid, const vector<int>& types, bool stype, int begin, int limit, bool hot) {
		uid_ = uid;
		sort_ = hot;
		begin_ = begin;
		limit_ = limit;
		by_stype_not_btype_ = stype;
//		types_.insert(types.begin(),types.end());
		random_ = 0;
		max_fid_ = 0;
		getshow_ = false;
		nowtime_ = time(NULL);
		older_count_ = 0;
		useless_count_ = 0;
		BOOST_FOREACH(int type,types){
			if(type > 0)
				include_types_.insert(type);
			else if(type < 0)
				exclude_types_.insert(type);
			//TODO. if type == 0
		}
		bool include = !include_types_.empty();
		bool exclude = !exclude_types_.empty();
		if(include and exclude){
			filter_style_ = kError;
		} else if(!include and !exclude){
			filter_style_ = kNone;
		} else if(include){
			filter_style_ = kInclude;
		} else{
			filter_style_ = kExclude;
		}
	}
	bool CheckArgs();
	set<int> GetIncludeTypes() const { return include_types_;}
	set<int> GetExcludeTypes() const { return exclude_types_;}
	bool CheckType(int stype){
		int realtype = by_stype_not_btype_ ? stype : (stype / 100 * 100);

		switch (filter_style_){
		case kNone:
			return true;
		case kError:
			return false;
		case kInclude:
			return include_types_.count(realtype);
		case kExclude:
			return !exclude_types_.count(-realtype);
		default:
			return true;
		}

	}
	bool HasEdm() const { return !edm_fitems_.empty(); }

	//第一个true表示是edm，第二个true表示是否删除这条edm
	tuple<bool,bool> SeperateEdm(HeavyFeedPtr & item){
		if(!item->IsEdm())
			return make_tuple(false,false);

		if(!ShowEdm() )
			return make_tuple(true,false);

		if(!edm_fitems_.empty()){ //只显示一条
			MCE_INFO("GetRequest::SeperateEdm. fid:" << item->GetFid()
					<< " type:" << item->GetStype()
					<< " actor:" << item->GetActor()
					<< " time:" << item->GetTime()
					<< " already exist edm. not show.");
			return make_tuple(true,true);
		}else{
			int oldtime = item->GetTime();
			MakeItem2dAndPush(item);
			FeedItem fitem = item->ToFeedItem();
			edm_fitems_.push_back(fitem);
			int newtime = item->ResetEdmTime();
			MCE_INFO("GetRequest::SeperateEdm. fid:" << item->GetFid() << " type:" << item->GetStype()
					<< " actor:" << item->GetActor()
					<< " oldtime:" << oldtime
					<< " newtime:" << newtime
					<< " nowtime:" << time(NULL)
			);
			return make_tuple(true,false);
		}
	}
	bool SeperateComment(HeavyFeedPtr & item){
//		pair<int, long> key = item->GetStypeMerge();
		int stype = item->GetStype();
		//不是评论
		if(!(MergeConfig::instance().IsComment(stype))) return false;
		// 是评论，但是不显示评论
		if(!ShowComment()) return true;
		//显示评论
		int main_stype = MergeConfig::instance().Comment2Main(stype);
		int version = item->GetVersion();
		int main_vstype = (version << 16) | main_stype;
		comment_map_[make_pair(main_vstype,item->GetMerge())].push_back(item);
		return true;
	}
	bool SeperateRandom(HeavyFeedPtr & item){
		if (ShowRandom()
				&& item->IsRandomType() &&
				item->GetView() == 0
				&& !random_) {
			random_ = item;
			return true;
		}
		return false;
	}

	void PushBackRandom();
//	void ReplaceScoreByCommentScore(StypemergeScoreMap & stypemerge_score );

	set<long> & GetShowedFids();
	int GetShowedFidsFromHFeed1D(HFeed1D & hfeed1d,SFeed2D & sfeed2d);
	SFeed3D & GetShowedSharedFeed3D();

//	HFeed3D & GetShowedItem3d();//TODO 无用

	long GetMaxFid();
	time_t GetNowTime() const { return nowtime_; }
	int GetOlderCount();
	int GetUselessCount() const { return useless_count_; }
	bool ShowComment(){return sort_;}
	bool ShowRandom(){return sort_ && begin_ ==0 ;}
	bool ShowEdm(){return begin_ == 0;}
	bool IsSort(){return sort_;};


//	void ThrowUselessFeeds(SFeed1D & sfeed1d);

	void ClearEdms();
	bool GetComments(pair<int,long> stypemerge,HFeed1D & comments);

	void MakeItem2dAndPush(const HeavyFeedPtr & item){
		HFeed1D item1d;
		item1d.push_back(item);
		HFeed2D item2d;
		item2d.push_back(item1d);
		hfeed3d_.push_back(item2d); //放到顶端
	}

	bool InRange(int idx){
		return idx >= begin_ && idx< begin_+limit_;
	}

	string ToString();
	void DumpComments();

	bool MustRaiseUgc(){
		int idx = uid_ % 10;
		return (idx == 5 or uid_ == 220678672 or uid_ == 239556923) and sort_ and 0 == begin_;
	}
	void RaiseUgc();
	bool MustIncViewBelowFid(){ return uid_ % 100 >= 80 and sort_;}
};



inline static ostream &operator<<(ostream &stream, const FeedItem & i) {
  stream << "[id:" << i.feed << " t:" << (i.type & 0xFFFF)<< " a:" << i.actor << " t:" << i.time << " mg:"<< i.merge << "]";
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
  static string Print(const T & container,int limit = 30) {
    ostringstream ios;

    ios << "(";
    int idx = 0;
    typedef typename T::value_type vt;
    BOOST_FOREACH(const vt & v,container){
    	if(idx++ > 0) ios << ",";

    	ios << v;

      if(idx >= limit) break;
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

  static void Print( const HFeed3D & item3d, int uid = 0 ){
  	int totalcnt = 0;
  	MCE_INFO("uid:" << uid << "item3d--> 2d count:" << item3d.size());
  	BOOST_FOREACH(const HFeed2D & item2d,item3d){
  		MCE_INFO("uid:" << uid << "\titem2d--> 1d count:" << item2d.size());
  		ostringstream os;
  		BOOST_FOREACH(const HFeed1D & item1d, item2d){
  			MCE_INFO("uid:" << uid <<"\t\titem1d--> item count:" << item1d.size());
  			BOOST_FOREACH(const HeavyFeedPtr & item,item1d){
  					MCE_INFO("uid:" << uid <<"\t\t\tfid:" << item->GetFid()
  							<< " stype:" << item->GetStype()
  							<< " actor:" << item->GetActor()
  							<< " merge:" << item->GetMerge());
  					++totalcnt;
  			}
  		}
  	}
  	MCE_INFO("totalcnt:" << totalcnt);
  }
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

class TypeFilter:public Singleton<TypeFilter>{
public:
	void Init(){
		int a[] = {1,5,6,7,20,37};
		btypes_.insert(a,a+sizeof(a)/sizeof(a[0]));
		MCE_INFO("TypeFilter::Init. btypes_:" << PrintHelper::Print(btypes_));
	}
	bool Check(int uid,int stype){
		if(uid % 100 == 27 || uid % 100 == 47){
			return btypes_.count(stype / 100);
		}
		return true;
	}
private:
	set<int> btypes_;
};
}
}
#endif /* COMMON_H_ */

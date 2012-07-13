#ifndef USERINFO_H_
#define USERINFO_H_
//#include "Common.h"
#include <list>
#include "RFeed.h"
#include <boost/circular_buffer.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/tag.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/indexed_by.hpp>
#include <TaskManager.h>
#include "SharedFeedCache.h"
#include <Date.h>
using namespace std;
using namespace boost::multi_index;
namespace xce {
namespace feed {

class UserInfo;
typedef IceUtil::Handle<UserInfo> UserInfoPtr;

class MaxFidUploader : public MyUtil::Timer, public MyUtil::Singleton<MaxFidUploader>{
public:
	MaxFidUploader() : Timer(30*1000, 2){}
	void Add(int uid,long fid){
		IceUtil::Mutex::Lock lock(mutex_);
		map_[uid] = fid;
	}
  void handle();
private:
  map<int,long> map_;
	IceUtil::Mutex mutex_;
};

class HeavyFeed /*: public Ice::Object */{
private:
	mutable int viewcnt_;
	mutable int time_;
	mutable int weight_;
	SharedFeedPtr sfeed_;
public:
	HeavyFeed(const FeedItem & item,const SharedFeedPtr & hdl):
			viewcnt_(0),time_(item.time),weight_(item.weight) ,sfeed_(hdl){}

//	virtual ~HeavyItem(){
//		MCE_INFO("Dtor. fid:" << GetFid() );
//	}
	long GetFid()const {return sfeed_->GetFid();}
	int GetStype()const{ return sfeed_->GetStype(); }
	long GetMerge() const { return sfeed_->GetMerge(); }
	int GetVersion() const { return sfeed_->GetVersion(); }
	int GetView()const{ return viewcnt_; }
	void IncView(){ ++viewcnt_;}

	SharedFeedPtr & GetSharedFeedPtr(){ return sfeed_;}

	pair<int,long> GetStypeMerge() const {	return sfeed_->GetStypeMerge(); }
	pair<int,long> GetVersionStypeMerge() const {	return sfeed_->GetVersionStypeMerge(); }
	pair<int,int> GetStypeActor() const { return sfeed_->GetStypeActor(); }
	bool IsGatherManyStype(int stype) const { return stype == 2002 || stype == 3707 ;}
	pair<int,int> GetGatherKey() const {
		pair<int,int> res = sfeed_->GetStypeActor();
		if(IsGatherManyStype(0xFFFF & res.first))
			res.second = 0;
		return res;
	}
	bool SameStypeMerge(int stype,long merge) const { return sfeed_->SameStypeMerge(stype,merge);}
	time_t GetGatherTimeGap(int stype) const {
		if(IsGatherManyStype(0xFFFF & stype))
			return 60 * 60 * 24;
		else
			return 60 * 5;
	}

	bool IsUpdateTime() const { return weight_ & 0x02; }
	void SetUpdateTime(bool up){
    if (up)  weight_ = weight_ | 0x02;
    else  weight_ = weight_ & 0xFFFFFFFD;
	}

	bool IsEdm()const{ return (time_ < 0 ||time_ > time(NULL)); }
	bool IsRandomType()const{ return sfeed_->IsRandomType(); }

	int GetWeightId()const{ return weight_ >> 16 & 0xFF; }
	int GetFriendRank()const{ return (weight_ >> 24) & 0x7F;}
	bool IsFocusSource()const{ return weight_ >> 31;}
	bool IncWid(int uid,int stype) const {
		int btype = stype / 100;
		if(5 == btype || 6 == btype || 7 == btype )
			return (7 == uid % 100 ) ;

		return false;
	}

	double GetScore(int uid, bool sort,time_t now, long pre_max_read_fid){
		int idx = uid % 100;
		if(!sort ) return  max(double(GetTime()),1.0);

		int oldvc = GetView();
//		if(idx < 80){
//			if(GetFid() <= pre_max_read_fid)
//				IncView();
//		}

		time_t tdiff = now - time_;
		double weight_id = GetWeightId();
		if( (idx / 10) == 1 and (GetStype() / 100) == 1)
			weight_id = 17;

		double frank = GetFriendRank();

		double ft = (10000.0f * (1 + weight_id / 100) / pow(tdiff + 1,1.0 / weight_id));

		int viewcount = min(GetView(),1);
		double viewed_factor = (viewcount == 0) ? 800 : 0;

		double viewweightfrank_factor = pow(0.6, viewcount) * (6 * weight_id + 4 * frank) * 1.3;

//		if(idx >= 70 )
//			viewweightfrank_factor *= 1.5;

//		if(idx == 3){
//			viewed_factor = (viewcount == 0) ? 1000 : 0;
//			viewweightfrank_factor = ft+ 1000 + 5* weight_id + 4* frank;
//		}

		double weight = ft + viewed_factor + viewweightfrank_factor;

//		if(idx == 63){
//			viewed_factor = (viewcount == 0) ? 800 : 0;
//			weight = ft + viewed_factor + 6 * weight_id + 4 * frank;
//		}

		double vd = 0;
		if(idx % 10 == 6)
			vd =  GetView() * 30;
		weight -= vd;

		if(idx == 81 || idx == 82 || idx == 91 || idx == 92){
			if(GetFid() > pre_max_read_fid){
				weight += 600;
			}
		}

		if(idx == 93 || idx == 94 || idx == 95 || idx == 96){
			if(GetFid() > pre_max_read_fid || oldvc == 0)
				weight += 800;
		}


		if (uid == 239556923 // zjc
				|| uid == 220678672 //yjw
				|| uid == 347735846
				|| uid == 278427738
				|| uid == 200000032//zhb
				|| uid == 241610785//kwei
				|| uid == 103113664//cyz
				|| uid == 85432256//h sir
				|| uid == 255233137 //yuanfei xiaohao
				) {
			MyUtil::Date date(time_);
			MCE_INFO("GS. u:" << uid
					<< " fd:" << GetFid()
					<< " t:" << GetStype()
					<< " a:"<< GetActor()
					<< " pf:" << frank
					<< " owd:" << GetWeightId()
					<< " wd:" << weight_id
					<< " ft:" << ft
					<< " rvc:" << GetView()
					<< " vc:" << viewcount
					<< " vf:" << viewed_factor
					<< " vwf:" << viewweightfrank_factor
					<< " t:" << date.toDateTime()
					<< " vd:" << vd
					<< " w: " << weight
			);
		}
		//43 前6条只有ugc
		return weight;
	}

	int GetFeedNewsMergeType()const { return sfeed_->GetFeedNewsMergeType();}
	int IsMergeReplace()const {return sfeed_->IsMergeReplace();}
	int IsMergeAppend()const {return sfeed_->IsMergeAppend();}
	int GetTime() const  { return time_;}
	int GetActor() const { return sfeed_->GetActor() ;}

	void SetWeight(int weight){ weight_ = weight;}

	FeedItem ToFeedItem() const {
		FeedItem item;
		item.feed = sfeed_->GetFid();
		item.merge = sfeed_->GetMerge();
		item.type = sfeed_->GetOrigType();
		item.actor = sfeed_->GetActor();
		item.time = time_;
		item.weight = weight_;
		return item;
	}

	string ToString() const {
		ostringstream os;
		MyUtil::Date date(time_);
		os << " fid:" << GetFid()
				<< " t:" << GetStype()
				<< " time_:" << GetTime()
				<< " a:" << GetActor()
				<< " mg:" << GetMerge()
				<< " t:" << date.toDateTime();
		return os.str();
	}
	int ResetEdmTime(){
		if(time_ > 0) return time_;

		if(time_ == -1 || time_ <= - 8 * 60) time_ = time(NULL) + 8 * 60;
		else time_ = time(NULL) - time_;
		return time_;
	}
};

inline static ostream &operator<<(ostream &stream, const HeavyFeed & i) {
	stream << "["<< i.ToString() <<"]";
  return stream;
}

class HeavyFeed;
typedef HeavyFeed* HeavyFeedPtr;
typedef vector<HeavyFeedPtr> HFeed1D;
typedef vector<HFeed1D> HFeed2D;
typedef vector<HFeed2D> HFeed3D;

typedef map<pair<int, long> , HFeed1D> StypemergeHFeed1DMap;
typedef map<pair<int,long >, double> StypemergeScoreMap;
typedef multimap<double, StypemergeHFeed1DMap::iterator> ScoreIteratorMultiMap;

typedef map<pair<int, long> , SFeed1D> StypemergeSFeed1DMap;

class GetRequest;
class UserInfo: public Ice::Object {
private:
	typedef boost::circular_buffer<HeavyFeed> HBuf;
	HBuf hbuf_;//顺序是从大到小
	int uid_;

	long max_fid_;//内存中的最大id,没有起作用，可以考虑删除
	long pre_max_read_fid_;//上次看过的最大id

//	bool is_loading_;
//	bool edm_loaded_;
	bool inited_;
	IceUtil::Mutex mutex_;

public:

	const static int kHbufSize = 250;
	friend class ResetHelper;

	UserInfo(int uid):
			hbuf_(kHbufSize),
			uid_(uid),
			max_fid_(0),
			pre_max_read_fid_(0),
//			is_loading_(false),
//			edm_loaded_(false),
			inited_(false)
	{
//		MCE_INFO("UserInfo ctor. uid:" << uid_);
	//	stamp_ = time(0);
	//	MCE_INFO("UserInfo::UserInfo uid:" << uid << " stamp:" << stamp_);
	}
//	virtual ~UserInfo(){
//		MaxFidUploader::instance().Add(uid_,max_fid_);
//	}
	bool Init(){
		if(inited_){
			MCE_WARN("UserInfo::Init. should be init once. uid:" << uid_);
			return true;
		}
		//读取FeedCache失败，就废弃这个UserInfo
		DownloadMaxRead();

		//必须先加载FeedCache,再加载Edm
		if(!LoadFeedCache()) return false;
		LoadEdms();
		inited_ = true;
		return true;
	}
	int GetUid() const { return uid_;}
	bool LoadEdms();
	bool LoadFeedCache();

	void AddHeavyItem(int uid,HeavyFeed & hitem/*,bool updatemaxfid= false*/);

	void MergeAndSort(GetRequest & req);

	vector<long> SetRead(int stype,long merge);
	vector<long> SetReadById(long fid);
	void SetReadAll();
	void SetReadByIdsExactly(const set<long> & fids,bool remove_feedcache = true);

	long DownloadMaxRead();
	long GetMaxRead() const {return pre_max_read_fid_;}
	bool SetMaxRead(long fid);

	FeedItemSeq GetOrigFeedItems();
	int GetUnviewCount(const set<int> & types);
	map<int,int> GetCountByStype(const set<int> & stypes,bool unviewed);
	map<int,int> GetCountByStypeMerged(const set<int> & stypes,bool unviewed);
	int Size() const;
	string ToString()const;
	void DumpItems()const{
		BOOST_FOREACH(const HBuf::value_type & f,hbuf_){
			MCE_INFO(f.ToString());
		}
	};
	void IncViewBelowFid(long fid);
	void MergeByStypeMergeAndGetScore(GetRequest & req,StypemergeScoreMap & stypemerge_score );
private:
	void MergeFeed(GetRequest & req);
	void GatherFeed(GetRequest & req);
	void GatherFeedSimple(GetRequest & req);

	void ReplaceScoreByCommentScore(GetRequest & req,StypemergeHFeed1DMap & comment_map_,
				StypemergeScoreMap & stypemerge_score );
	void MakeScoreIteratorMultiMap(
			GetRequest & req,StypemergeScoreMap & stypemerge_score);

	template<typename T>
	void SetReadInFeedCache(const T & fids);

	long MakeNewbuf(FeedItemSeq & fitems,HBuf & nbuf);
//	bool GetFromFeedCache(FeedItemSeq & fitems);
	void GetSharedFeedsAndAddBuffer(FeedItemSeq & fitems,bool normalfeed);

	typedef map<pair<int, int> , pair<time_t, int> > StypeactorTimeindexMap;
	bool FindSameStypeActor(HeavyFeedPtr & item,StypeactorTimeindexMap & stypeactor_timeindex,StypeactorTimeindexMap::iterator& ti){
		pair<int,int> gatherkey = item->GetGatherKey();
		ti = stypeactor_timeindex.find(gatherkey);
		return (ti != stypeactor_timeindex.end());
	}

	bool TimeDiffLessThanLimit(time_t a,time_t b){
		static time_t LIMIT =  60 * 5;
		time_t diff = abs(a - b);
		return diff < LIMIT ;//TODO 根据不同类型有区别
	}

	bool CheckUniq();
	bool CheckOrder(FeedItemSeq & fitems);
};

//-------------------------------------------------------------------


class UserInfoPool: public Singleton<UserInfoPool>{
	struct SeqTag{};
	struct UidTag{};
	typedef multi_index_container<
		UserInfoPtr,
		indexed_by<
			sequenced<tag<SeqTag> > ,
			hashed_unique<tag<UidTag>,BOOST_MULTI_INDEX_CONST_MEM_FUN(UserInfo,int,GetUid) >
		>
	>UserInfoMI;
	typedef UserInfoMI::index<SeqTag>::type SeqIndex;
	typedef UserInfoMI::index<UidTag>::type UidIndex;

private:
	const static int kCacheSize = 100;
	const static int kMiCapacity = 3000;

	UserInfoMI mis_[kCacheSize];
	IceUtil::Mutex	mutexs_[kCacheSize];
public:
	void Put(int uid,UserInfoPtr & uinfo){
	  int index = GetIndex(uid);
		IceUtil::Mutex::Lock lock(mutexs_[index]);
	  UserInfoMI & mi = mis_[index];
	  pair<SeqIndex::iterator, bool> p = mi.push_front(uinfo);
	  if (!p.second) {
	  	mi.relocate(mi.begin(), p.first);
	  } else if ((int) mi.size() > kMiCapacity) {
	  	mi.pop_back();
	  }
	}
	UserInfoPtr Find(int uid,bool relocate = false){
	  int index = GetIndex(uid);
		IceUtil::Mutex::Lock lock(mutexs_[index]);
	  UserInfoMI & mi = mis_[index];
	  UidIndex& uid_index = mi.get<UidTag> ();
	  UidIndex::iterator it = uid_index.find(uid);
	  if (it != uid_index.end()) {
	    if (relocate) {
	      SeqIndex& seq_index = mi.get<SeqTag> ();
	      seq_index.relocate(seq_index.begin(), mi.project<SeqTag> (it));
	      it = uid_index.find(uid);
	      if (it != uid_index.end()) {
	        return (*it);
	      }
	    } else {
	      return (*it);
	    }
	  }
	  return NULL;
	}
	bool Erase(int uid){
	  int index = GetIndex(uid);
		IceUtil::Mutex::Lock lock(mutexs_[index]);
	  UserInfoMI & mi = mis_[index];
	  UidIndex& uid_index = mi.get<UidTag> ();
	  UidIndex::iterator it = uid_index.find(uid);
	  if (it != uid_index.end()) {
	    uid_index.erase(it);
	    return true;
	  } else {
	  	return false;
	  }
	}
	string ToString() const {
		ostringstream os;
		os << " UserInfoPool. ";
//		int idx = 0;
		long total = 0;
		for(int i = 0; i < kCacheSize;++i){
			IceUtil::Mutex::Lock lock(mutexs_[i]);
		  const UserInfoMI & mi = mis_[i];
		  total += mi.size();
		}
//		BOOST_FOREACH(const UserInfoMI & mi,mis_){
//			++idx;
//			total += mi.size();
//		}
		os << " total:" << total;
		return os.str();
	}
private:
	int GetIndex(long uid){
		return abs(uid / 100 % kCacheSize);
	}
};


}
}

#endif /* USERINFO_H_ */
